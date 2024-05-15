#include "MyOpenGLWidget.h"
#include <QMimeData>
#include <QPainter>
#include <QDebug>
#include <QDropEvent>
#include <QMouseEvent>
#include <QUrl>
#include <QFileDialog>
#include <QVBoxLayout>

// Constructor: Initialize the OpenGL widget and its components
MyOpenGLWidget::MyOpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {
    setAcceptDrops(true);  // Enable drag-and-drop functionality

    // Initialize the toolbar and connect its actions to the appropriate slots
    toolbar = new ImageToolbar(this);
    toolbar->setVisible(false);
    connect(toolbar, &ImageToolbar::rotateImage, this, &MyOpenGLWidget::rotateSelectedImage);
    connect(toolbar, &ImageToolbar::mirrorImage, this, &MyOpenGLWidget::mirrorSelectedImage);
    connect(toolbar, &ImageToolbar::copyImage, this, &MyOpenGLWidget::copySelectedImage);
    connect(toolbar, &ImageToolbar::deleteImage, this, &MyOpenGLWidget::deleteSelectedImage);
    connect(toolbar, &ImageToolbar::saveImage, this, &MyOpenGLWidget::saveSelectedImage);
    connect(toolbar, &ImageToolbar::toggleEraser, this, &MyOpenGLWidget::toggleEraserMode);

    // Initialize the eraser size slider
    eraserSizeSlider = new QSlider(Qt::Horizontal, this);
    eraserSizeSlider->setRange(1, 100);  // Set the range for the slider
    eraserSizeSlider->setValue(10);  // Set the default value
    eraserSizeSlider->setVisible(false);  // Hide the slider initially
    eraserSizeSlider->setFixedWidth(200);  // Set the width of the slider
    connect(eraserSizeSlider, &QSlider::valueChanged, this, &MyOpenGLWidget::updateEraserSize);

    installEventFilter(this);  // Install an event filter to handle custom events

    // Initialize undo and redo buttons and connect their actions
    undoButton = new QPushButton("Undo", this);
    redoButton = new QPushButton("Redo", this);
    connect(undoButton, &QPushButton::clicked, this, &MyOpenGLWidget::undo);
    connect(redoButton, &QPushButton::clicked, this, &MyOpenGLWidget::redo);
}

// OpenGL initialization
void MyOpenGLWidget::initializeGL() {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);  // Set the background color
}

// Paint event: Render the widget's content
void MyOpenGLWidget::paintGL() {
    QPainter painter(this);
    for (auto& img : images) {
        img.draw(painter, scrollPosition);
    }

    if (selectedImage) {
        // Position the toolbar relative to the selected image
        QPoint toolbarPos = selectedImage->boundingBox.topLeft() + scrollPosition - QPoint(0, toolbar->height());
        toolbar->move(toolbarPos);
        toolbar->setVisible(true);

        // Position the eraser size slider relative to the selected image
        QPoint sliderPos = selectedImage->boundingBox.bottomLeft() + scrollPosition + QPoint((selectedImage->boundingBox.width() - eraserSizeSlider->width()) / 2, 10);
        eraserSizeSlider->move(sliderPos);
        eraserSizeSlider->setVisible(eraserMode);
    } else {
        toolbar->setVisible(false);
        eraserSizeSlider->setVisible(false);
    }

    // Ensure undo and redo buttons are always at the bottom right
    undoButton->move(width() - 180, height() - 40);
    redoButton->move(width() - 90, height() - 40);
}

// Drag event: Handle when a drag enters the widget
void MyOpenGLWidget::dragEnterEvent(QDragEnterEvent* event) {
    qDebug() << "Drag entered with MIME types:" << event->mimeData()->formats();
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();  // Accept the drag event if it contains URLs
    }
}

// Drop event: Handle when an item is dropped onto the widget
void MyOpenGLWidget::dropEvent(QDropEvent* event) {
    saveState();  // Save the current state for undo functionality
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urls = mimeData->urls();
        if (!urls.isEmpty()) {
            QImage image;
            if (image.load(urls.first().toLocalFile())) {
                images.emplace_back(image.scaled(QSize(150, 150), Qt::KeepAspectRatio), event->pos());
                update();
            }
        }
    }
}

// Mouse press event: Handle mouse button press
void MyOpenGLWidget::mousePressEvent(QMouseEvent* event) {
    if (eraserMode && selectedImage) {
        eraseAt(event->pos());  // Erase at the clicked position if eraser mode is enabled
        return;
    }

    QPoint pos = event->pos() - scrollPosition;
    bool imageClicked = false;

    // Check if an image handle or the image itself is clicked
    for (auto& img : images) {
        int handle = img.handleAt(pos, QPoint(0, 0));
        if (handle != 0) {
            currentHandle = handle;
            selectedImage = &img;
            img.isSelected = true;
            imageClicked = true;
            break;
        } else if (img.contains(pos, QPoint(0, 0))) {
            selectedImage = &img;
            img.isSelected = true;
            imageClicked = true;
        } else {
            img.isSelected = false;
        }
    }

    if (!imageClicked && !eraserMode) {
        isDragging = true;
        selectedImage = nullptr;
        qDebug() << "Mouse pressed, selected image set to nullptr";
    }

    lastMousePosition = event->pos();
    update();
}

// Mouse move event: Handle mouse movement
void MyOpenGLWidget::mouseMoveEvent(QMouseEvent* event) {
    if (eraserMode && selectedImage) {
        eraseAt(event->pos());  // Erase at the current mouse position if eraser mode is enabled
        return;
    }

    if (isDragging && !selectedImage) {
        // Scroll the view if dragging and no image is selected
        QPoint delta = event->pos() - lastMousePosition;
        scrollPosition += delta;
        lastMousePosition = event->pos();
        update();
    } else if (event->buttons() & Qt::LeftButton && selectedImage) {
        if (currentHandle != 0) {
            // Resize the image using handles
            QPoint delta = event->pos() - lastMousePosition;
            QRect& rect = selectedImage->boundingBox;
            switch (currentHandle) {
                case 1:
                    rect.setTopLeft(rect.topLeft() + delta);
                    break;
                case 2:
                    rect.setTopRight(rect.topRight() + delta);
                    break;
                case 3:
                    rect.setBottomLeft(rect.bottomLeft() + delta);
                    break;
                case 4:
                    rect.setBottomRight(rect.bottomRight() + delta);
                    break;
            }
            lastMousePosition = event->pos();
            update();
        } else {
            // Move the selected image
            QPoint delta = event->pos() - lastMousePosition;
            selectedImage->boundingBox.translate(delta);
            lastMousePosition = event->pos();
            update();
        }
    }
}

// Mouse release event: Handle mouse button release
void MyOpenGLWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (eraserMode) {
        return;
    }
    isDragging = false;
    currentHandle = 0;
    update();
}

// Rotate the selected image by 90 degrees
void MyOpenGLWidget::rotateSelectedImage() {
    if (selectedImage) {
        saveState();  // Save the current state for undo functionality
        QTransform transform;
        transform.rotate(90);
        selectedImage->image = selectedImage->image.transformed(transform);
        selectedImage->boundingBox.setSize(selectedImage->image.size());
        update();
    } else {
        qDebug() << "No image selected";
    }
}

// Mirror the selected image horizontally
void MyOpenGLWidget::mirrorSelectedImage() {
    if (selectedImage) {
        saveState();  // Save the current state for undo functionality
        selectedImage->image = selectedImage->image.mirrored(true, false);
        update();
    } else {
        qDebug() << "No image selected";
    }
}

// Copy the selected image
void MyOpenGLWidget::copySelectedImage() {
    if (selectedImage) {
        saveState();  // Save the current state for undo functionality
        images.emplace_back(selectedImage->image, selectedImage->boundingBox.center() + QPoint(20, 20));
        update();
    } else {
        qDebug() << "No image selected";
    }
}

// Delete the selected image
void MyOpenGLWidget::deleteSelectedImage() {
    if (selectedImage) {
        saveState();  // Save the current state for undo functionality
        images.erase(std::remove(images.begin(), images.end(), *selectedImage), images.end());
        selectedImage = nullptr;
        update();
    } else {
        qDebug() << "No image selected";
    }
}

// Save the selected image to a file
void MyOpenGLWidget::saveSelectedImage() {
    if (selectedImage) {
        QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "", "PNG Files (*.png);;All Files (*)");
        if (!fileName.isEmpty()) {
            selectedImage->image.save(fileName);
        }
    } else {
        qDebug() << "No image selected";
    }
}

// Toggle the eraser mode on or off
void MyOpenGLWidget::toggleEraserMode(bool enabled) {
    eraserMode = enabled;
    if (enabled) {
        setCursor(Qt::CrossCursor);  // Change cursor to crosshair when eraser mode is enabled
    } else {
        setCursor(Qt::ArrowCursor);  // Change cursor back to arrow when eraser mode is disabled
    }
    update();
}

// Update the eraser size
void MyOpenGLWidget::updateEraserSize(int size) {
    eraserSize = size;
    update();
}

// Erase pixels at the specified position
void MyOpenGLWidget::eraseAt(const QPoint& pos) {
    if (!selectedImage) return;

    QPoint imgPos = pos - scrollPosition - selectedImage->boundingBox.topLeft();
    QPainter painter(&selectedImage->image);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.setBrush(QBrush(Qt::transparent));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(imgPos, eraserSize / 2, eraserSize / 2);
    update();
}

// Event filter to handle custom events, such as mouse move in eraser mode
bool MyOpenGLWidget::eventFilter(QObject* obj, QEvent* event) {
    if (eraserMode && selectedImage && event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        eraseAt(mouseEvent->pos());
        return true;
    }
    return QOpenGLWidget::eventFilter(obj, event);
}

// Save the current state to the undo stack
void MyOpenGLWidget::saveState() {
    undoStack.push(images);
    while (!redoStack.empty()) {
        redoStack.pop();
    }
}

// Undo the last action
void MyOpenGLWidget::undo() {
    if (!undoStack.empty()) {
        redoStack.push(images);
        images = undoStack.top();
        undoStack.pop();
        update();
    }
}

// Redo the last undone action
void MyOpenGLWidget::redo() {
    if (!redoStack.empty()) {
        undoStack.push(images);
        images = redoStack.top();
        redoStack.pop();
        update();
    }
}

// Upload an image from the file system
void MyOpenGLWidget::uploadImage() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.xpm *.jpg)");
    if (!fileName.isEmpty()) {
        QImage image;
        if (image.load(fileName)) {
            saveState();  // Save the current state for undo functionality
            images.emplace_back(image.scaled(QSize(150, 150), Qt::KeepAspectRatio), QPoint(width() / 2, height() / 2));
            update();
        }
    }
}

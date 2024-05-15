#include "MyOpenGLWidget.h"
#include <QMimeData>
#include <QPainter>
#include <QDebug>
#include <QDropEvent>
#include <QMouseEvent>
#include <QUrl>
#include <QFileDialog>
#include <algorithm>

MyOpenGLWidget::MyOpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {
    setAcceptDrops(true);
    toolbar = new ImageToolbar(this);
    toolbar->setVisible(false);
    connect(toolbar, &ImageToolbar::rotateImage, this, &MyOpenGLWidget::rotateSelectedImage);
    connect(toolbar, &ImageToolbar::mirrorImage, this, &MyOpenGLWidget::mirrorSelectedImage);
    connect(toolbar, &ImageToolbar::copyImage, this, &MyOpenGLWidget::copySelectedImage);
    connect(toolbar, &ImageToolbar::deleteImage, this, &MyOpenGLWidget::deleteSelectedImage);
    connect(toolbar, &ImageToolbar::saveImage, this, &MyOpenGLWidget::saveSelectedImage);
    connect(toolbar, &ImageToolbar::toggleEraser, this, &MyOpenGLWidget::toggleEraserMode);

    eraserSizeSlider = new QSlider(Qt::Horizontal, this);
    eraserSizeSlider->setRange(1, 100);
    eraserSizeSlider->setValue(10);
    eraserSizeSlider->setVisible(false);
    eraserSizeSlider->setFixedWidth(200);
    connect(eraserSizeSlider, &QSlider::valueChanged, this, &MyOpenGLWidget::updateEraserSize);

    installEventFilter(this);

    // Add undo and redo buttons
    undoButton = new QPushButton("Undo", this);
    redoButton = new QPushButton("Redo", this);
    undoButton->setGeometry(10, height() - 40, 80, 30);
    redoButton->setGeometry(100, height() - 40, 80, 30);

    connect(undoButton, &QPushButton::clicked, this, &MyOpenGLWidget::undo);
    connect(redoButton, &QPushButton::clicked, this, &MyOpenGLWidget::redo);
}

void MyOpenGLWidget::initializeGL() {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

void MyOpenGLWidget::paintGL() {
    QPainter painter(this);
    for (auto& img : images) {
        img.draw(painter, scrollPosition);
    }

    if (selectedImage) {
        QPoint toolbarPos = selectedImage->boundingBox.topLeft() + scrollPosition - QPoint(0, toolbar->height());
        toolbar->move(toolbarPos);
        toolbar->setVisible(true);

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

void MyOpenGLWidget::dragEnterEvent(QDragEnterEvent* event) {
    qDebug() << "Drag entered with MIME types:" << event->mimeData()->formats();
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MyOpenGLWidget::dropEvent(QDropEvent* event) {
    saveState(); // Save state before making changes
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

void MyOpenGLWidget::mousePressEvent(QMouseEvent* event) {
    QPoint pos = event->pos() - scrollPosition;
    bool imageClicked = false;

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
    } else if (imageClicked && !eraserMode) {
        saveState(); // Save state before making changes
    }

    lastMousePosition = event->pos();
    update();
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent* event) {
    if (eraserMode && selectedImage) {
        eraseAt(event->pos());
        return;
    }

    if (isDragging && !selectedImage) {
        QPoint delta = event->pos() - lastMousePosition;
        scrollPosition += delta;
        lastMousePosition = event->pos();
        update();
    } else if (event->buttons() & Qt::LeftButton && selectedImage) {
        if (currentHandle != 0) {
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
            QPoint delta = event->pos() - lastMousePosition;
            selectedImage->boundingBox.translate(delta);
            lastMousePosition = event->pos();
            update();
        }
    }
}

void MyOpenGLWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (eraserMode) {
        return;
    }
    isDragging = false;
    currentHandle = 0;
    update();
}

void MyOpenGLWidget::rotateSelectedImage() {
    if (selectedImage) {
        saveState(); // Save state before making changes
        QTransform transform;
        transform.rotate(90);
        selectedImage->image = selectedImage->image.transformed(transform);
        selectedImage->boundingBox.setSize(selectedImage->image.size());
        update();
    } else {
        qDebug() << "No image selected";
    }
}

void MyOpenGLWidget::mirrorSelectedImage() {
    if (selectedImage) {
        saveState(); // Save state before making changes
        selectedImage->image = selectedImage->image.mirrored(true, false);
        update();
    } else {
        qDebug() << "No image selected";
    }
}

void MyOpenGLWidget::copySelectedImage() {
    if (selectedImage) {
        saveState(); // Save state before making changes
        images.emplace_back(selectedImage->image, selectedImage->boundingBox.center() + QPoint(20, 20));
        update();
    } else {
        qDebug() << "No image selected";
    }
}

void MyOpenGLWidget::deleteSelectedImage() {
    if (selectedImage) {
        saveState(); // Save state before making changes
        images.erase(std::remove(images.begin(), images.end(), *selectedImage), images.end());
        selectedImage = nullptr;
        update();
    } else {
        qDebug() << "No image selected";
    }
}

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

void MyOpenGLWidget::toggleEraserMode(bool enabled) {
    eraserMode = enabled;
    if (enabled) {
        setCursor(Qt::CrossCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
    update();
}

void MyOpenGLWidget::updateEraserSize(int size) {
    eraserSize = size;
    update();
}

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

bool MyOpenGLWidget::eventFilter(QObject* obj, QEvent* event) {
    if (eraserMode && selectedImage && event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        eraseAt(mouseEvent->pos());
        return true;
    }
    return QOpenGLWidget::eventFilter(obj, event);
}

void MyOpenGLWidget::saveState() {
    undoStack.push(images);
    while (!redoStack.empty()) {
        redoStack.pop();
    }
}

void MyOpenGLWidget::undo() {
    if (!undoStack.empty()) {
        redoStack.push(images);
        images = undoStack.top();
        undoStack.pop();
        update();
    }
}

void MyOpenGLWidget::redo() {
    if (!redoStack.empty()) {
        undoStack.push(images);
        images = redoStack.top();
        redoStack.pop();
        update();
    }
}

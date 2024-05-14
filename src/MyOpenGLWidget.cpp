#include "MyOpenGLWidget.h"
#include <QMimeData>
#include <QPainter>
#include <QDebug>
#include <QDropEvent>
#include <QMouseEvent>
#include <QUrl>
#include <algorithm>

MyOpenGLWidget::MyOpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {
    setAcceptDrops(true);
    toolbar = new ImageToolbar(this);
    toolbar->setVisible(false);
    connect(toolbar, &ImageToolbar::rotateImage, this, &MyOpenGLWidget::rotateSelectedImage);
    connect(toolbar, &ImageToolbar::mirrorImage, this, &MyOpenGLWidget::mirrorSelectedImage);
    connect(toolbar, &ImageToolbar::copyImage, this, &MyOpenGLWidget::copySelectedImage);
    connect(toolbar, &ImageToolbar::deleteImage, this, &MyOpenGLWidget::deleteSelectedImage);
}

void MyOpenGLWidget::initializeGL() {
    // Set the background color to semi-dark gray
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
    } else {
        toolbar->setVisible(false);
    }
}

void MyOpenGLWidget::dragEnterEvent(QDragEnterEvent* event) {
    qDebug() << "Drag entered with MIME types:" << event->mimeData()->formats();
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MyOpenGLWidget::dropEvent(QDropEvent* event) {
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
            qDebug() << "Handle selected";
            break;
        } else if (img.contains(pos, QPoint(0, 0))) {
            selectedImage = &img;
            img.isSelected = true;
            imageClicked = true;
            qDebug() << "Image selected";
        } else {
            img.isSelected = false;
        }
    }

    if (!imageClicked) {
        isDragging = true;
        qDebug() << "No image clicked, starting canvas drag";
    } else {
        qDebug() << "Selected image: " << (selectedImage != nullptr);
    }

    lastMousePosition = event->pos();
    update();
}

void MyOpenGLWidget::mouseReleaseEvent(QMouseEvent* event) {
    isDragging = false;
    currentHandle = 0;
    qDebug() << "Mouse released";
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent* event) {
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

void MyOpenGLWidget::rotateSelectedImage() {
    qDebug() << "rotateSelectedImage called";
    if (selectedImage) {
        qDebug() << "Rotating image";
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
    qDebug() << "mirrorSelectedImage called";
    if (selectedImage) {
        qDebug() << "Mirroring image";
        selectedImage->image = selectedImage->image.mirrored(true, false);
        update();
    } else {
        qDebug() << "No image selected";
    }
}

void MyOpenGLWidget::copySelectedImage() {
    qDebug() << "copySelectedImage called";
    if (selectedImage) {
        qDebug() << "Copying image";
        images.emplace_back(selectedImage->image, selectedImage->boundingBox.center() + QPoint(20, 20));
        update();
    } else {
        qDebug() << "No image selected";
    }
}

void MyOpenGLWidget::deleteSelectedImage() {
    qDebug() << "deleteSelectedImage called";
    if (selectedImage) {
        qDebug() << "Deleting image";
        images.erase(std::remove(images.begin(), images.end(), *selectedImage), images.end());
        selectedImage = nullptr;
        update();
    } else {
        qDebug() << "No image selected";
    }
}

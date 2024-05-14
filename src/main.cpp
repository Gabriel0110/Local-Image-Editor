#include <QApplication>
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QImage>
#include <QUrl>
#include <QList>
#include <QPainter>
#include <QDebug>
#include <QRect>
#include <QToolBar>
#include <QAction>
#include <QHBoxLayout>
#include <QIcon>
#include <iostream>
#include <vector>


// class ImageObject {
// public:
//     QImage image;
//     QRect boundingBox;
//     bool isSelected;
//     static const int HANDLE_SIZE = 10;

//     ImageObject(const QImage& img, const QPoint& pos) : image(img), isSelected(false) {
//         boundingBox.setSize(img.size());
//         boundingBox.moveCenter(pos);
//     }

//     void draw(QPainter& painter, const QPoint& scrollPosition) {
//         QRect adjustedBox = boundingBox.translated(scrollPosition);
//         painter.drawImage(adjustedBox, image);
//         if (isSelected) {
//             painter.setPen(QPen(Qt::magenta, 2, Qt::DashLine));  // Highlight color and style
//             painter.drawRect(adjustedBox);
//             drawHandles(painter, adjustedBox);
//         }
//     }

//     void drawHandles(QPainter& painter, const QRect& rect) {
//         painter.setBrush(Qt::white);
//         painter.setPen(Qt::black);
//         int halfSize = HANDLE_SIZE / 2;
//         painter.drawRect(rect.topLeft().x() - halfSize, rect.topLeft().y() - halfSize, HANDLE_SIZE, HANDLE_SIZE);
//         painter.drawRect(rect.topRight().x() - halfSize, rect.topRight().y() - halfSize, HANDLE_SIZE, HANDLE_SIZE);
//         painter.drawRect(rect.bottomLeft().x() - halfSize, rect.bottomLeft().y() - halfSize, HANDLE_SIZE, HANDLE_SIZE);
//         painter.drawRect(rect.bottomRight().x() - halfSize, rect.bottomRight().y() - halfSize, HANDLE_SIZE, HANDLE_SIZE);
//     }

//     bool contains(const QPoint& pos, const QPoint& scrollPosition) {
//         return boundingBox.translated(scrollPosition).contains(pos);
//     }

//     int handleAt(const QPoint& pos, const QPoint& scrollPosition) {
//         QRect adjustedBox = boundingBox.translated(scrollPosition);
//         if (QRect(adjustedBox.topLeft() - QPoint(HANDLE_SIZE / 2, HANDLE_SIZE / 2), QSize(HANDLE_SIZE, HANDLE_SIZE)).contains(pos))
//             return 1;
//         if (QRect(adjustedBox.topRight() - QPoint(HANDLE_SIZE / 2, HANDLE_SIZE / 2), QSize(HANDLE_SIZE, HANDLE_SIZE)).contains(pos))
//             return 2;
//         if (QRect(adjustedBox.bottomLeft() - QPoint(HANDLE_SIZE / 2, HANDLE_SIZE / 2), QSize(HANDLE_SIZE, HANDLE_SIZE)).contains(pos))
//             return 3;
//         if (QRect(adjustedBox.bottomRight() - QPoint(HANDLE_SIZE / 2, HANDLE_SIZE / 2), QSize(HANDLE_SIZE, HANDLE_SIZE)).contains(pos))
//             return 4;
//         return 0;
//     }

//     // Define the equality operator for ImageObject
//     bool operator==(const ImageObject& other) const {
//         return (this->image == other.image &&
//                 this->boundingBox == other.boundingBox &&
//                 this->isSelected == other.isSelected);
//     }
// };

// class MyOpenGLWidget : public QOpenGLWidget {
//     Q_OBJECT

// private:
//     std::vector<ImageObject> images;
//     QPoint scrollPosition;
//     QPoint lastMousePosition;
//     bool isDragging;
//     ImageObject* selectedImage = nullptr;
//     int currentHandle = 0;
//     ImageToolbar* toolbar;

// public:
//     MyOpenGLWidget(QWidget *parent = nullptr) : QOpenGLWidget(parent) {
//         setAcceptDrops(true);
//         toolbar = new ImageToolbar(this);
//         toolbar->setVisible(false);
//         connect(toolbar, &ImageToolbar::rotateImage, this, &MyOpenGLWidget::rotateSelectedImage);
//         connect(toolbar, &ImageToolbar::mirrorImage, this, &MyOpenGLWidget::mirrorSelectedImage);
//         connect(toolbar, &ImageToolbar::copyImage, this, &MyOpenGLWidget::copySelectedImage);
//         connect(toolbar, &ImageToolbar::deleteImage, this, &MyOpenGLWidget::deleteSelectedImage);
//     }

// protected:
//     void initializeGL() override {
//         // Set the background color to semi-dark gray
//         glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
//     }

//     void paintGL() override {
//         QPainter painter(this);
//         for (auto& img : images) {
//             img.draw(painter, scrollPosition);
//         }

//         if (selectedImage) {
//             QPoint toolbarPos = selectedImage->boundingBox.topLeft() + scrollPosition - QPoint(0, toolbar->height());
//             toolbar->move(toolbarPos);
//             toolbar->setVisible(true);
//         } else {
//             toolbar->setVisible(false);
//         }
//     }

//     void dragEnterEvent(QDragEnterEvent *event) override {
//         qDebug() << "Drag entered with MIME types:" << event->mimeData()->formats();
//         if (event->mimeData()->hasUrls()) {
//             event->acceptProposedAction();
//         }
//     }

//     void dropEvent(QDropEvent *event) override {
//         const QMimeData* mimeData = event->mimeData();
//         if (mimeData->hasUrls()) {
//             QList<QUrl> urls = mimeData->urls();
//             if (!urls.isEmpty()) {
//                 QImage image;
//                 if (image.load(urls.first().toLocalFile())) {
//                     images.emplace_back(image.scaled(QSize(150, 150), Qt::KeepAspectRatio), event->pos());
//                     update();
//                 }
//             }
//         }
//     }

//     void mousePressEvent(QMouseEvent* event) override {
//         QPoint pos = event->pos() - scrollPosition;
//         bool imageClicked = false;

//         for (auto& img : images) {
//             int handle = img.handleAt(pos, QPoint(0, 0));
//             if (handle != 0) {
//                 currentHandle = handle;
//                 selectedImage = &img;
//                 img.isSelected = true;
//                 imageClicked = true;
//                 break;
//             } else if (img.contains(pos, QPoint(0, 0))) {
//                 selectedImage = &img;
//                 img.isSelected = true;
//                 imageClicked = true;
//             } else {
//                 img.isSelected = false;
//             }
//         }

//         if (!imageClicked) {
//             isDragging = true;
//             selectedImage = nullptr;
//         }

//         lastMousePosition = event->pos();
//         update();
//     }

//     void mouseMoveEvent(QMouseEvent* event) override {
//         if (isDragging && !selectedImage) {
//             QPoint delta = event->pos() - lastMousePosition;
//             scrollPosition += delta;
//             lastMousePosition = event->pos();
//             update();
//         } else if (event->buttons() & Qt::LeftButton && selectedImage) {
//             if (currentHandle != 0) {
//                 QPoint delta = event->pos() - lastMousePosition;
//                 QRect& rect = selectedImage->boundingBox;
//                 switch (currentHandle) {
//                     case 1:
//                         rect.setTopLeft(rect.topLeft() + delta);
//                         break;
//                     case 2:
//                         rect.setTopRight(rect.topRight() + delta);
//                         break;
//                     case 3:
//                         rect.setBottomLeft(rect.bottomLeft() + delta);
//                         break;
//                     case 4:
//                         rect.setBottomRight(rect.bottomRight() + delta);
//                         break;
//                 }
//                 lastMousePosition = event->pos();
//                 update();
//             } else {
//                 QPoint delta = event->pos() - lastMousePosition;
//                 selectedImage->boundingBox.translate(delta);
//                 lastMousePosition = event->pos();
//                 update();
//             }
//         }
//     }

//     void mouseReleaseEvent(QMouseEvent* event) override {
//         isDragging = false;
//         selectedImage = nullptr;
//         currentHandle = 0;
//     }

// private slots:
//     void rotateSelectedImage() {
//         if (selectedImage) {
//             QTransform transform;
//             transform.rotate(90);
//             selectedImage->image = selectedImage->image.transformed(transform);
//             selectedImage->boundingBox.setSize(selectedImage->image.size());
//             update();
//         }
//     }


//     void mirrorSelectedImage() {
//         if (selectedImage) {
//             selectedImage->image = selectedImage->image.mirrored(true, false);
//             update();
//         }
//     }

//     void copySelectedImage() {
//         if (selectedImage) {
//             images.emplace_back(selectedImage->image, selectedImage->boundingBox.center() + QPoint(20, 20));
//             update();
//         }
//     }

//     void deleteSelectedImage() {
//         if (selectedImage) {
//             images.erase(std::remove(images.begin(), images.end(), *selectedImage), images.end());
//             selectedImage = nullptr;
//             update();
//         }
//     }
// };


// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);

//     QMainWindow mainWindow;
//     MyOpenGLWidget* openGLWidget = new MyOpenGLWidget;
//     mainWindow.setCentralWidget(openGLWidget);
//     mainWindow.resize(1024, 896);
//     mainWindow.show();

//     return app.exec();
// }


#include <QApplication>
#include <QMainWindow>
#include "MyOpenGLWidget.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QMainWindow mainWindow;
    MyOpenGLWidget* openGLWidget = new MyOpenGLWidget;
    mainWindow.setCentralWidget(openGLWidget);
    mainWindow.resize(1024, 896);
    mainWindow.show();

    return app.exec();
}

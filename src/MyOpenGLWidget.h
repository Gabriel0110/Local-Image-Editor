#ifndef MYOPENGLWIDGET_H
#define MYOPENGLWIDGET_H

#include <QOpenGLWidget>
#include "ImageToolbar.h"
#include "ImageObject.h"
#include <vector>

class MyOpenGLWidget : public QOpenGLWidget {
    Q_OBJECT

private:
    std::vector<ImageObject> images;
    QPoint scrollPosition;
    QPoint lastMousePosition;
    bool isDragging;
    ImageObject* selectedImage = nullptr;
    int currentHandle = 0;
    ImageToolbar* toolbar;

public:
    MyOpenGLWidget(QWidget* parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void rotateSelectedImage();
    void mirrorSelectedImage();
    void copySelectedImage();
    void deleteSelectedImage();
};

#endif // MYOPENGLWIDGET_H

#ifndef MYOPENGLWIDGET_H
#define MYOPENGLWIDGET_H

#include <QOpenGLWidget>
#include "ImageToolbar.h"
#include "ImageObject.h"
#include <vector>
#include <QSlider>
#include <QPushButton>
#include <stack>

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
    QSlider* eraserSizeSlider;
    QPushButton* undoButton;
    QPushButton* redoButton;
    bool eraserMode = false;
    int eraserSize = 10;

    std::stack<std::vector<ImageObject>> undoStack;
    std::stack<std::vector<ImageObject>> redoStack;

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
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void rotateSelectedImage();
    void mirrorSelectedImage();
    void copySelectedImage();
    void deleteSelectedImage();
    void saveSelectedImage();
    void toggleEraserMode(bool enabled);
    void updateEraserSize(int size);
    void undo();
    void redo();

private:
    void eraseAt(const QPoint& pos);
    void saveState();
};

#endif // MYOPENGLWIDGET_H

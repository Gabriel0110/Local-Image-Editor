#ifndef MYOPENGLWIDGET_H
#define MYOPENGLWIDGET_H

#include <QOpenGLWidget>
#include "ImageToolbar.h"
#include "ImageObject.h"
#include <vector>
#include <QSlider>
#include <QPushButton>
#include <stack>

// MyOpenGLWidget class for handling image editing
class MyOpenGLWidget : public QOpenGLWidget {
    Q_OBJECT

private:
    std::vector<ImageObject> images;  // List of images in the widget
    QPoint scrollPosition;  // Current scroll position
    QPoint lastMousePosition;  // Last mouse position
    bool isDragging;  // Flag indicating if dragging is in progress
    ImageObject* selectedImage = nullptr;  // Currently selected image
    int currentHandle = 0;  // Current handle for resizing
    ImageToolbar* toolbar;  // Toolbar for image actions
    QSlider* eraserSizeSlider;  // Slider for eraser size
    bool eraserMode = false;  // Flag indicating if eraser mode is enabled
    int eraserSize = 10;  // Size of the eraser
    QPushButton* undoButton;  // Undo button
    QPushButton* redoButton;  // Redo button
    std::stack<std::vector<ImageObject>> undoStack;  // Stack for undo actions
    std::stack<std::vector<ImageObject>> redoStack;  // Stack for redo actions
    bool cropMode = false;  // Flag indicating if crop mode is enabled
    QRect cropBox;  // Crop box for cropping

public:
    MyOpenGLWidget(QWidget* parent = nullptr);
    void uploadImage();

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
    void bringToFront();
    void pushToBack();
    void toggleCropMode(bool enabled);

private:
    void eraseAt(const QPoint& pos);
    void saveState();
    void undo();
    void redo();
    void adjustCropBox(const QPoint& delta);
    int cropHandleAt(const QPoint& pos) const;
};

#endif // MYOPENGLWIDGET_H

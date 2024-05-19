#ifndef MYOPENGLWIDGET_H
#define MYOPENGLWIDGET_H

#include <QOpenGLWidget>
#include "ImageToolbar.h"
#include "ImageObject.h"
#include <vector>
#include <QSlider>
#include <QPushButton>
#include <QStack>
#include <QLineEdit>
#include <QWidget>
#include <QLabel>
#include <QProcess>
#include <QProgressDialog>

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
    QStack<std::vector<ImageObject>> undoStack;  // Stack for undo actions
    QStack<std::vector<ImageObject>> redoStack;  // Stack for redo actions
    bool cropMode = false;  // Flag indicating if crop mode is enabled
    QRect cropBox;  // Crop box for cropping
    bool inpaintMode = false;  // Flag indicating if inpaint mode is enabled
    QImage maskImage;  // Image for the inpainting mask
    QProgressDialog* progressDialog; // Progress dialog for inpainting
    QProcess* pythonProcess;

    // Inpainting popup elements
    QWidget* inpaintPopup;
    QLabel* inpaintPromptLabel;
    QLineEdit* inpaintTextBox;
    QPushButton* confirmInpaintButton;
    QPushButton* cancelInpaintButton;
    QSlider* inpaintBrushSizeSlider;

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
    void bringToFront();
    void pushToBack();
    void toggleCropMode(bool enabled);
    void toggleInpaintMode(bool enabled);
    void confirmInpaint();
    void handleInpaintResult();
    void handleInpaintError(QProcess::ProcessError error);

private:
    void eraseAt(const QPoint& pos);
    void saveState();
    void undo();
    void redo();
    void adjustCropBox(const QPoint& delta);
    int cropHandleAt(const QPoint& pos) const;
    void drawMaskAt(const QPoint& pos);
};

#endif // MYOPENGLWIDGET_H

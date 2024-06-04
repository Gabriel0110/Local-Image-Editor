#ifndef IMAGETOOLBAR_H
#define IMAGETOOLBAR_H

#include <QWidget>
#include <QToolBar>
#include <QAction>
#include <QHBoxLayout>
#include <QIcon>
#include <QSlider>

// Image toolbar class
class ImageToolbar : public QWidget {
    Q_OBJECT

public:
    explicit ImageToolbar(QWidget* parent = nullptr);
    void checkInpaintAction(bool checked);  // Method to enable/disable inpaint action
    void setMergeActionVisible(bool visible); // Method to set merge action visibility

signals:
    void toggleRotationMode(bool enabled);  // Signal to toggle rotation mode
    void rotateImage(int angle);  // Signal to rotate image
    void mirrorImage();  // Signal to mirror image
    void copyImage();    // Signal to copy image
    void deleteImage();  // Signal to delete image
    void saveImage();    // Signal to save image
    void toggleEraser(bool enabled);  // Signal to toggle eraser mode
    void bringToFront();  // Signal to bring image to front
    void pushToBack();    // Signal to push image to back
    void toggleCropMode(bool enabled); // Signal to toggle crop mode
    void toggleInpaintMode(bool enabled); // Signal to toggle inpaint mode
    void toggleSnipeMode(bool enabled);
    void toggleDepthRemoval(bool enabled); // Signal for depth-wise background removal
    void oneshotRemoval(); // Signal for oneshot background removal
    void mergeImages(); // Signal to merge images

private:
    QAction* rotateAction;
    QAction* eraserAction;
    QAction* cropAction;
    QAction* inpaintAction;
    QSlider* eraserSizeSlider;
    QAction* snipeAction;
    QAction* depthRemovalAction;
    QAction* mergeAction; // Merge action

private slots:
    void onRotateButtonClicked();  // Slot to handle rotate button click
    void onEraserButtonClicked();  // Slot to handle eraser button click
    void onCropButtonClicked();  // Slot to handle crop button click
    void onInpaintButtonClicked(); // Slot to handle inpaint button click
    void onSnipeButtonClicked();
    void onDepthRemovalButtonClicked();
    void onMergeButtonClicked(); // Slot to handle merge button click
};

#endif // IMAGETOOLBAR_H

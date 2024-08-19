#ifndef IMAGETOOLBAR_H
#define IMAGETOOLBAR_H

#include <QWidget>
#include <QToolBar>
#include <QAction>
#include <QHBoxLayout>
#include <QIcon>
#include <QSlider>

class ImageToolbar : public QWidget {
    Q_OBJECT

public:
    explicit ImageToolbar(QWidget* parent = nullptr);
    void checkInpaintAction(bool checked);
    void setMergeActionVisible(bool visible);
    void setUntoggledActions();

signals:
    void toggleRotationMode(bool enabled);
    void rotateImage(int angle);
    void mirrorImage();
    void copyImage();
    void deleteImage();
    void saveImage();
    void toggleEraser(bool enabled);
    void bringToFront();
    void pushToBack();
    void toggleCropMode(bool enabled);
    void toggleInpaintMode(bool enabled);
    void toggleSnipeMode(bool enabled);
    void toggleDepthRemoval(bool enabled);
    void oneshotRemoval();
    void mergeImages();

private:
    QAction* mirrorAction;
    QAction* copyAction;
    QAction* deleteAction;
    QAction* saveAction;
    QAction* bringToFrontAction;
    QAction* pushToBackAction;
    QAction* rotateAction;
    QAction* eraserAction;
    QAction* cropAction;
    QAction* inpaintAction;
    QSlider* eraserSizeSlider;
    QAction* snipeAction;
    QAction* depthRemovalAction;
    QAction* oneshotRemovalAction;
    QAction* mergeAction;

private slots:
    void onRotateButtonClicked();
    void onEraserButtonClicked();
    void onCropButtonClicked();
    void onInpaintButtonClicked();
    void onSnipeButtonClicked();
    void onDepthRemovalButtonClicked();
    void onMergeButtonClicked();
};

#endif // IMAGETOOLBAR_H

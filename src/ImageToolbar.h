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

signals:
    void rotateImage();
    void mirrorImage();
    void copyImage();
    void deleteImage();
    void saveImage();
    void toggleEraser(bool enabled);
    void eraserSizeChanged(int size);

private:
    QAction* eraserAction;
    QSlider* eraserSizeSlider;

private slots:
    void onEraserButtonClicked();
};

#endif // IMAGETOOLBAR_H
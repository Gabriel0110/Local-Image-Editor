#ifndef IMAGETOOLBAR_H
#define IMAGETOOLBAR_H

#include <QWidget>
#include <QToolBar>
#include <QAction>
#include <QHBoxLayout>
#include <QIcon>

class ImageToolbar : public QWidget {
    Q_OBJECT

public:
    explicit ImageToolbar(QWidget* parent = nullptr);

signals:
    void rotateImage();
    void mirrorImage();
    void copyImage();
    void deleteImage();
};

#endif // IMAGETOOLBAR_H

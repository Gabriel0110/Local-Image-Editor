#include "ImageToolbar.h"
#include <QDebug>
#include <QToolBar>

ImageToolbar::ImageToolbar(QWidget* parent) : QWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);

    QToolBar* toolbar = new QToolBar(this);
    QAction* rotateAction = toolbar->addAction(QIcon::fromTheme("object-rotate-right"), "Rotate");
    QAction* mirrorAction = toolbar->addAction(QIcon::fromTheme("object-flip-horizontal"), "Mirror");
    QAction* copyAction = toolbar->addAction(QIcon::fromTheme("edit-copy"), "Copy");
    QAction* deleteAction = toolbar->addAction(QIcon::fromTheme("edit-delete"), "Delete");
    QAction* saveAction = toolbar->addAction(QIcon::fromTheme("document-save"), "Save");
    eraserAction = toolbar->addAction(QIcon::fromTheme("edit-eraser"), "Eraser");
    eraserAction->setCheckable(true);

    connect(rotateAction, &QAction::triggered, this, &ImageToolbar::rotateImage);
    connect(mirrorAction, &QAction::triggered, this, &ImageToolbar::mirrorImage);
    connect(copyAction, &QAction::triggered, this, &ImageToolbar::copyImage);
    connect(deleteAction, &QAction::triggered, this, &ImageToolbar::deleteImage);
    connect(saveAction, &QAction::triggered, this, &ImageToolbar::saveImage);
    connect(eraserAction, &QAction::triggered, this, &ImageToolbar::onEraserButtonClicked);

    layout->addWidget(toolbar);
    setLayout(layout);
}

void ImageToolbar::onEraserButtonClicked() {
    bool toggled = eraserAction->isChecked();
    emit toggleEraser(toggled);
}

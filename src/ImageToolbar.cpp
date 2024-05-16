#include "ImageToolbar.h"
#include <QDebug>
#include <QToolBar>

// Constructor: Initialize the toolbar and its actions
ImageToolbar::ImageToolbar(QWidget* parent) : QWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);

    QToolBar* toolbar = new QToolBar(this);
    QAction* rotateAction = toolbar->addAction(QIcon::fromTheme("object-rotate-right"), "Rotate");
    QAction* mirrorAction = toolbar->addAction(QIcon::fromTheme("object-flip-horizontal"), "Mirror");
    QAction* copyAction = toolbar->addAction(QIcon::fromTheme("edit-copy"), "Copy");
    QAction* deleteAction = toolbar->addAction(QIcon::fromTheme("edit-delete"), "Delete");
    QAction* saveAction = toolbar->addAction(QIcon::fromTheme("document-save"), "Save");
    QAction* bringToFrontAction = toolbar->addAction(QIcon::fromTheme("go-top"), "Bring to Front");
    QAction* pushToBackAction = toolbar->addAction(QIcon::fromTheme("go-bottom"), "Push to Back");
    eraserAction = toolbar->addAction(QIcon::fromTheme("edit-eraser"), "Eraser");
    eraserAction->setCheckable(true);  // Make the eraser action a toggle button
    cropAction = toolbar->addAction(QIcon::fromTheme("crop"), "Crop");
    cropAction->setCheckable(true);  // Make the crop action a toggle button

    // Connect actions to signals
    connect(rotateAction, &QAction::triggered, this, &ImageToolbar::rotateImage);
    connect(mirrorAction, &QAction::triggered, this, &ImageToolbar::mirrorImage);
    connect(copyAction, &QAction::triggered, this, &ImageToolbar::copyImage);
    connect(deleteAction, &QAction::triggered, this, &ImageToolbar::deleteImage);
    connect(saveAction, &QAction::triggered, this, &ImageToolbar::saveImage);
    connect(bringToFrontAction, &QAction::triggered, this, &ImageToolbar::bringToFront);
    connect(pushToBackAction, &QAction::triggered, this, &ImageToolbar::pushToBack);
    connect(eraserAction, &QAction::triggered, this, &ImageToolbar::onEraserButtonClicked);
    connect(cropAction, &QAction::triggered, this, &ImageToolbar::onCropButtonClicked);

    layout->addWidget(toolbar);
    setLayout(layout);
}

// Slot for handling the eraser button click
void ImageToolbar::onEraserButtonClicked() {
    bool toggled = eraserAction->isChecked();
    emit toggleEraser(toggled);  // Emit a signal indicating the eraser mode status
}

// Slot to handle crop button click
void ImageToolbar::onCropButtonClicked() {
    bool toggled = cropAction->isChecked();
    emit toggleCropMode(toggled);  // Emit a signal indicating the crop mode status
}
#include "ImageToolbar.h"
#include <QDebug>

ImageToolbar::ImageToolbar(QWidget* parent) : QWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);

    QToolBar* toolbar = new QToolBar(this);
    QAction* rotateAction = toolbar->addAction(QIcon::fromTheme("object-rotate-right"), "Rotate");
    QAction* mirrorAction = toolbar->addAction(QIcon::fromTheme("object-flip-horizontal"), "Mirror");
    QAction* copyAction = toolbar->addAction(QIcon::fromTheme("edit-copy"), "Copy");
    QAction* deleteAction = toolbar->addAction(QIcon::fromTheme("edit-delete"), "Delete");

    connect(rotateAction, &QAction::triggered, this, [this]() {
        qDebug() << "Rotate button clicked";
        emit rotateImage();
    });
    connect(mirrorAction, &QAction::triggered, this, [this]() {
        qDebug() << "Mirror button clicked";
        emit mirrorImage();
    });
    connect(copyAction, &QAction::triggered, this, [this]() {
        qDebug() << "Copy button clicked";
        emit copyImage();
    });
    connect(deleteAction, &QAction::triggered, this, [this]() {
        qDebug() << "Delete button clicked";
        emit deleteImage();
    });

    layout->addWidget(toolbar);
    setLayout(layout);
}



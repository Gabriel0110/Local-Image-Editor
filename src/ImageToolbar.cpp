#include "ImageToolbar.h"
#include <QDebug>
#include <QToolBar>
#include <QDir>

ImageToolbar::ImageToolbar(QWidget* parent) : QWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);

    QToolBar* toolbar = new QToolBar(this);

    // Set the stylesheet for the toolbar and actions
    toolbar->setStyleSheet(
        "QToolBar { background-color: white; border: 1px solid lightgray; }"
        "QToolButton { background-color: transparent; border: none; }"
        "QToolButton:hover { background-color: lightgray; }"
        "QToolButton:pressed { background-color: darkgray; }"
        "QToolButton:checked { background-color: lightblue; }"
    );
    toolbar->setIconSize(QSize(24, 24));  // Set a consistent icon size
    toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);  // Display only the icon
    toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);  // Allow horizontal expansion
    toolbar->setMinimumWidth(400);  // Set a minimum width for the toolbar

    QString iconPath = QDir::currentPath() + "/resources/images/icons/";

    // Icon Attribution: https://www.flaticon.com/free-icons/rotate title="rotate icons" Rotate icons created by adriansyah - Flaticon
    rotateAction = toolbar->addAction(QIcon(iconPath + "rotate.png"), "Rotate");
    rotateAction->setToolTip("Rotate (Currently disabled)");
    rotateAction->setCheckable(true);
    //rotateAction->setEnabled(false);

    // Icon Attribution: https://www.flaticon.com/free-icons/flip title="flip icons" Flip icons created by Freepik - Flaticon
    mirrorAction = toolbar->addAction(QIcon(iconPath + "flip.png"), "Mirror");
    mirrorAction->setToolTip("Mirror");

    // Icon Attribution: "https://www.flaticon.com/free-icons/photo" title="photo icons" Photo icons created by Freepik - Flaticon
    copyAction = toolbar->addAction(QIcon(iconPath + "copy.png"), "Copy");
    copyAction->setToolTip("Copy");

    // Icon Attribution: "https://www.flaticon.com/free-icons/edit-tools" title="edit tools icons" Edit tools icons created by srip - Flaticon
    deleteAction = toolbar->addAction(QIcon(iconPath + "delete.png"), "Delete");
    deleteAction->setToolTip("Delete");

    // Icon Attribution: "https://www.flaticon.com/free-icons/save" title="save icons" Save icons created by Bharat Icons - Flaticon
    saveAction = toolbar->addAction(QIcon(iconPath + "save.png"), "Save");
    saveAction->setToolTip("Save");

    // Icon Attribution: "https://www.flaticon.com/free-icons/bring" title="bring icons" Bring icons created by Icon Mela - Flaticon
    bringToFrontAction = toolbar->addAction(QIcon(iconPath + "bring-to-front.png"), "Bring to Front");
    bringToFrontAction->setToolTip("Bring to Front");

    // Icon Attribution: "https://www.flaticon.com/free-icons/higher" title="higher icons" Higher icons created by icon wind - Flaticon
    pushToBackAction = toolbar->addAction(QIcon(iconPath + "push-to-back.png"), "Push to Back");
    pushToBackAction->setToolTip("Push to Back");

    // Icon Attribution: "https://www.flaticon.com/free-icons/design-tools" title="design tools icons" Design tools icons created by Culmbio - Flaticon
    eraserAction = toolbar->addAction(QIcon(iconPath + "eraser.png"), "Eraser");
    eraserAction->setToolTip("Eraser");
    eraserAction->setCheckable(true);  // Make the eraser action a toggle button

    // Icon Attribution: "https://www.flaticon.com/free-icons/photo-editing" title="photo editing icons" Photo editing icons created by M.Z Vector - Flaticon
    cropAction = toolbar->addAction(QIcon(iconPath + "crop.png"), "Crop");
    cropAction->setToolTip("Crop");
    cropAction->setCheckable(true);  // Make the crop action a toggle button

    // Icon Attribution: "https://www.flaticon.com/free-icons/effect" title="effect icons" Effect icons created by Those Icons - Flaticon
    inpaintAction = toolbar->addAction(QIcon(iconPath + "inpaint.png"), "Inpaint");
    inpaintAction->setToolTip("Inpaint");
    inpaintAction->setCheckable(true);

    // Icon Attribution: "https://www.flaticon.com/free-icons/gun" title="gun icons" Gun icons created by photo3idea_studio - Flaticon
    snipeAction = toolbar->addAction(QIcon(iconPath + "snipe.png"), "Snipe (Segment)");
    snipeAction->setToolTip("Snipe (Segment)");
    snipeAction->setCheckable(true);

    // Icon Attribution: "https://www.flaticon.com/free-icons/depth-perception" title="depth perception icons" Depth perception icons created by Freepik - Flaticon
    depthRemovalAction = toolbar->addAction(QIcon(iconPath + "depthwise-background-removal.png"), "Depthwise Background Removal"); // Add depth removal button
    depthRemovalAction->setToolTip("Depthwise Background Removal (Currently unavailable)");
    depthRemovalAction->setCheckable(true);

    // Icon Attribution: "https://www.flaticon.com/free-icons/remove-user" title="remove user icons" Remove user icons created by Tempo_doloe - Flaticon
    oneshotRemovalAction = toolbar->addAction(QIcon(iconPath + "oneshot-background-removal.png"), "Oneshot Background Removal"); // Add oneshot removal button
    oneshotRemovalAction->setToolTip("Oneshot Background Removal");

    // Icon Attribution: "https://www.flaticon.com/free-icons/consolidate" title="consolidate icons" Consolidate icons created by Freepik - Flaticon
    mergeAction = toolbar->addAction(QIcon(iconPath + "merge.png"), "Merge Images");
    mergeAction->setToolTip("Merge");
    mergeAction->setVisible(false);  // Initially invisible

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
    connect(inpaintAction, &QAction::triggered, this, &ImageToolbar::onInpaintButtonClicked);
    connect(snipeAction, &QAction::triggered, this, &ImageToolbar::onSnipeButtonClicked);
    connect(depthRemovalAction, &QAction::triggered, this, &ImageToolbar::onDepthRemovalButtonClicked); // Connect depth removal button
    connect(oneshotRemovalAction, &QAction::triggered, this, &ImageToolbar::oneshotRemoval); // Connect oneshot removal button
    connect(mergeAction, &QAction::triggered, this, &ImageToolbar::onMergeButtonClicked); // Connect merge button

    layout->addWidget(toolbar);
    setLayout(layout);
}

void ImageToolbar::onRotateButtonClicked() {
    bool toggled = rotateAction->isChecked();
    emit toggleRotationMode(toggled);
}

void ImageToolbar::onEraserButtonClicked() {
    bool toggled = eraserAction->isChecked();
    emit toggleEraser(toggled);
}

void ImageToolbar::onCropButtonClicked() {
    bool toggled = cropAction->isChecked();
    emit toggleCropMode(toggled);
}

void ImageToolbar::onInpaintButtonClicked() {
    bool toggled = inpaintAction->isChecked();
    emit toggleInpaintMode(toggled);
}

void ImageToolbar::checkInpaintAction(bool checked) {
    inpaintAction->setChecked(checked);
}

void ImageToolbar::onSnipeButtonClicked() {
    bool toggled = snipeAction->isChecked();
    emit toggleSnipeMode(toggled);
}

void ImageToolbar::onDepthRemovalButtonClicked() {
    bool toggled = depthRemovalAction->isChecked();
    emit toggleDepthRemoval(toggled);
}

void ImageToolbar::onMergeButtonClicked() {
    emit mergeImages();
}

void ImageToolbar::setMergeActionVisible(bool visible) {
    mergeAction->setVisible(visible);
}

void ImageToolbar::setUntoggledActions() {
    if (rotateAction) {
        rotateAction->setChecked(false);
    }

    if (eraserAction) {
        eraserAction->setChecked(false);
    }

    if (cropAction) {
        cropAction->setChecked(false);
    }

    if (inpaintAction) {
        inpaintAction->setChecked(false);
    }

    if (snipeAction) {
        snipeAction->setChecked(false);
    }

    if (depthRemovalAction) {
        depthRemovalAction->setChecked(false);
    }
}



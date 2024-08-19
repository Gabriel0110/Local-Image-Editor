#include "MyOpenGLWidget.h"
#include "CustomConfirmationDialog.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include <QMimeData>
#include <QPainter>
#include <QDebug>
#include <QDropEvent>
#include <QMouseEvent>
#include <QUrl>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProcess>
#include <QBuffer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QPixmap>
#include <QComboBox>
#include <QPushButton>
#include <QColorDialog>

MyOpenGLWidget::MyOpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {
    setAcceptDrops(true);
    setFocusPolicy(Qt::StrongFocus); // Ensure the widget can receive keyboard focus

    projectRoot = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("../..");

    // Ensure the project root directory is "Local-Image-Editor"
    if (!QDir(projectRoot).exists()) {
        QMessageBox::critical(nullptr, "Error", "The application must be run from the 'Local-Image-Editor' project root directory. Project root: " + projectRoot);
        return;
    }

    // Ensure we are in the proper directory and look for the script directory
    QString scriptDir = QDir(projectRoot).absoluteFilePath("resources/scripts/inference");
    qDebug() << "Script directory path: " << scriptDir;

    if (!QDir(scriptDir).exists()) {
        qDebug() << "Directory does not exist: " << scriptDir;
        progressDialog->hide();
        delete progressDialog;
        return;
    }

    // Initialize the toolbar
    toolbar = new ImageToolbar(this);
    toolbar->setVisible(false);
    connect(toolbar, &ImageToolbar::rotateImage, this, &MyOpenGLWidget::toggleRotationMode);
    connect(toolbar, &ImageToolbar::mirrorImage, this, &MyOpenGLWidget::mirrorSelectedImage);
    connect(toolbar, &ImageToolbar::copyImage, this, &MyOpenGLWidget::copySelectedImage);
    connect(toolbar, &ImageToolbar::deleteImage, this, &MyOpenGLWidget::deleteSelectedImage);
    connect(toolbar, &ImageToolbar::saveImage, this, &MyOpenGLWidget::saveSelectedImage);
    connect(toolbar, &ImageToolbar::toggleEraser, this, &MyOpenGLWidget::toggleEraserMode);
    connect(toolbar, &ImageToolbar::toggleCropMode, this, &MyOpenGLWidget::toggleCropMode);
    connect(toolbar, &ImageToolbar::bringToFront, this, &MyOpenGLWidget::bringToFront);
    connect(toolbar, &ImageToolbar::pushToBack, this, &MyOpenGLWidget::pushToBack);
    connect(toolbar, &ImageToolbar::toggleInpaintMode, this, &MyOpenGLWidget::toggleInpaintMode);
    connect(toolbar, &ImageToolbar::toggleSnipeMode, this, &MyOpenGLWidget::toggleSnipeMode);
    connect(toolbar, &ImageToolbar::toggleDepthRemoval, this, &MyOpenGLWidget::toggleDepthRemovalMode);
    connect(toolbar, &ImageToolbar::oneshotRemoval, this, &MyOpenGLWidget::oneshotRemoval);
    connect(toolbar, &ImageToolbar::mergeImages, this, &MyOpenGLWidget::mergeSelectedImages); // Connect merge action

    // Initialize the rotation slider
    rotationSlider = new QSlider(Qt::Horizontal, this);
    rotationSlider->setRange(0, 360);
    rotationSlider->setValue(0);
    rotationSlider->setVisible(false);
    rotationSlider->setFixedWidth(200);
    connect(rotationSlider, &QSlider::valueChanged, this, &MyOpenGLWidget::rotateSelectedImage); // Connect rotation slider

    // Initialize the eraser size slider
    eraserSizeSlider = new QSlider(Qt::Horizontal, this);
    eraserSizeSlider->setRange(1, 100);
    eraserSizeSlider->setValue(20);
    eraserSizeSlider->setVisible(false);
    eraserSizeSlider->setFixedWidth(200);

    // Initialize inpaint popup
    inpaintPopup = new QWidget(this);
    inpaintPopup->setVisible(false);
    QVBoxLayout* inpaintLayout = new QVBoxLayout(inpaintPopup);
    inpaintPromptLabel = new QLabel("Prompt:", inpaintPopup);
    inpaintTextBox = new QLineEdit(inpaintPopup);
    confirmInpaintButton = new QPushButton("Confirm", inpaintPopup);
    cancelInpaintButton = new QPushButton("Cancel", inpaintPopup);
    inpaintBrushSizeSlider = new QSlider(Qt::Horizontal, inpaintPopup);
    inpaintBrushSizeSlider->setRange(1, 100);
    inpaintBrushSizeSlider->setValue(20);

    inpaintLayout->addWidget(inpaintPromptLabel);
    inpaintLayout->addWidget(inpaintTextBox);
    inpaintLayout->addWidget(confirmInpaintButton);
    inpaintLayout->addWidget(cancelInpaintButton);
    inpaintLayout->addWidget(inpaintBrushSizeSlider);

    connect(confirmInpaintButton, &QPushButton::clicked, this, &MyOpenGLWidget::confirmInpaint);
    connect(cancelInpaintButton, &QPushButton::clicked, this, [this]() { toggleInpaintMode(false); });

    // Initialize the snipe popup
    snipePopup = new QWidget(this);
    snipePopup->setVisible(false);
    QVBoxLayout* snipeLayout = new QVBoxLayout(snipePopup);
    confirmSnipeButton = new QPushButton("Confirm Points", snipePopup);
    clearSnipeButton = new QPushButton("Clear Points", snipePopup);
    snipeLayout->addWidget(confirmSnipeButton);
    snipeLayout->addWidget(clearSnipeButton);
    snipePopup->setLayout(snipeLayout);

    connect(confirmSnipeButton, &QPushButton::clicked, this, &MyOpenGLWidget::confirmSnipe);
    connect(clearSnipeButton, &QPushButton::clicked, this, &MyOpenGLWidget::clearSnipePoints);

    // Initialize depth removal slider
    depthRemovalSlider = new QSlider(Qt::Horizontal, this);
    depthRemovalSlider->setRange(0, 1000);
    depthRemovalSlider->setValue(0);
    depthRemovalSlider->setVisible(false);
    depthRemovalSlider->setFixedWidth(200);
    connect(depthRemovalSlider, &QSlider::valueChanged, this, &MyOpenGLWidget::adjustImage); // Connect depth removal slider

    // Initialize shape menu
    shapeMenu = new QDialog(this);
    shapeMenu->setWindowTitle("Add Shape");
    shapeMenu->setFixedSize(200, 200);
    shapeMenu->setStyleSheet("background-color: black;");
    QVBoxLayout* shapeLayout = new QVBoxLayout(shapeMenu);

    shapeLabel = new QLabel("Select Shape", shapeMenu);
    shapeLabel->setFixedHeight(15);
    shapeLabel->setStyleSheet("color: white;");
    shapeLayout->addWidget(shapeLabel, 0, Qt::AlignHCenter);

    shapeSelector = new QComboBox(shapeMenu);
    shapeSelector->setFixedSize(100, 25);
    shapeSelector->addItem("Square");
    shapeSelector->setStyleSheet("background-color: gray;");
    shapeLayout->addWidget(shapeSelector, 0, Qt::AlignHCenter);

    colorButton = new QPushButton("Select Color", shapeMenu);
    colorButton->setFixedWidth(100);
    colorButton->setStyleSheet("background-color: gray;");
    shapeLayout->addWidget(colorButton, 0, Qt::AlignHCenter);
    connect(colorButton, &QPushButton::clicked, this, &MyOpenGLWidget::openColorDialog);

    createShapeButton = new QPushButton("Create", shapeMenu);
    createShapeButton->setFixedWidth(50);
    createShapeButton->setStyleSheet("background-color: gray;");
    shapeLayout->addWidget(createShapeButton, 0, Qt::AlignHCenter);
    connect(createShapeButton, &QPushButton::clicked, this, &MyOpenGLWidget::createShape);

    addShapeButton = new QPushButton("Add Shape", this);
    addShapeButton->move(width() - 60, 10);
    addShapeButton->resize(125, 30);
    connect(addShapeButton, &QPushButton::clicked, this, &MyOpenGLWidget::openShapeMenu);

    colorDialog = new QColorDialog(this);

    // Initialize Generate AI Image button
    generateAIImageButton = new QPushButton("Generate AI Image", this);
    generateAIImageButton->move(addShapeButton->x() - 10, 10); // Position it just to the left of the "Add Shape" button
    generateAIImageButton->resize(150, 30);
    connect(generateAIImageButton, &QPushButton::clicked, this, &MyOpenGLWidget::openGenerateAIMenu); // Connect to the function that will open the menu

    // Initialize Generate AI popup
    generateAIPopup = new QWidget(this);
    generateAIPopup->setVisible(false);
    QVBoxLayout* aiLayout = new QVBoxLayout(generateAIPopup);

    useOpenAICheckBox = new QCheckBox("Use OpenAI DALL-E API", generateAIPopup);
    connect(useOpenAICheckBox, &QCheckBox::toggled, this, &MyOpenGLWidget::toggleAPIKeyInput);
    aiLayout->addWidget(useOpenAICheckBox);

    useEnvVarCheckBox = new QCheckBox("Use OpenAI API Key from Environment Variable", generateAIPopup);
    aiLayout->addWidget(useEnvVarCheckBox);

    apiKeyTextBox = new QLineEdit(generateAIPopup);
    apiKeyTextBox->setPlaceholderText("Enter your OpenAI API key");
    apiKeyTextBox->setEnabled(false); // Initially disabled
    aiLayout->addWidget(apiKeyTextBox);

    QLabel* promptLabel = new QLabel("Prompt:", generateAIPopup);
    aiLayout->addWidget(promptLabel);

    promptTextBox = new QLineEdit(generateAIPopup);
    aiLayout->addWidget(promptTextBox);

    confirmGenerateAIButton = new QPushButton("Generate", generateAIPopup);
    connect(confirmGenerateAIButton, &QPushButton::clicked, this, &MyOpenGLWidget::confirmGenerateAIImage);
    aiLayout->addWidget(confirmGenerateAIButton);

    cancelGenerateAIButton = new QPushButton("Cancel", generateAIPopup);
    connect(cancelGenerateAIButton, &QPushButton::clicked, generateAIPopup, &QWidget::hide);
    aiLayout->addWidget(cancelGenerateAIButton);

    installEventFilter(this);

    // Initialize undo and redo buttons
    undoButton = new QPushButton("Undo", this);
    redoButton = new QPushButton("Redo", this);
    connect(undoButton, &QPushButton::clicked, this, &MyOpenGLWidget::undo);
    connect(redoButton, &QPushButton::clicked, this, &MyOpenGLWidget::redo);
}

void MyOpenGLWidget::initializeGL() {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

void MyOpenGLWidget::paintGL() {
    QPainter painter(this);

    // Anti-aliasing for smoother rendering
    painter.setRenderHint(QPainter::Antialiasing);

    // Smooth scaling for better image quality
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    for (auto& img : images) {
        img.draw(painter, scrollPosition);
    }

    if (!selectedImages.empty()) {
        // Disable the bounding box for the selected images before drawing the combined bounding box
        for (auto& img : selectedImages) {
            img->disableBoundingBox();
        }

        // Compute combined bounding box for all selected images
        QRect combinedBoundingBox = computeBoundingBoxForSelectedImages();
        QPoint toolbarPos = combinedBoundingBox.topLeft() + scrollPosition - QPoint(0, toolbar->height());
        toolbar->move(toolbarPos);
        toolbar->setVisible(true);

        QPoint rotationSliderPos = combinedBoundingBox.bottomLeft() + scrollPosition + QPoint((combinedBoundingBox.width() - rotationSlider->width()) / 2, 10);
        rotationSlider->move(rotationSliderPos);
        rotationSlider->setVisible(rotationMode);

        QPoint eraserSliderPos = combinedBoundingBox.bottomLeft() + scrollPosition + QPoint((combinedBoundingBox.width() - eraserSizeSlider->width()) / 2, 10);
        eraserSizeSlider->move(eraserSliderPos);
        eraserSizeSlider->setVisible(eraserMode);

        QPoint depthSliderPos = combinedBoundingBox.bottomLeft() + scrollPosition + QPoint((combinedBoundingBox.width() - depthRemovalSlider->width()) / 2, 10);
        depthRemovalSlider->move(depthSliderPos);
        depthRemovalSlider->setVisible(depthRemovalMode);

        if (cropMode) {
            QRect translatedCropBox = cropBox.translated(scrollPosition);
            painter.setPen(QPen(Qt::blue, 2, Qt::DashLine));
            painter.drawRect(translatedCropBox);

            // Draw the crop handles
            const int handleSize = 6;
            painter.setBrush(Qt::white);
            painter.setPen(Qt::black);
            QRect cropHandles[] = {
                QRect(translatedCropBox.topLeft() - QPoint(handleSize / 2, handleSize / 2), QSize(handleSize, handleSize)),
                QRect(translatedCropBox.topRight() - QPoint(handleSize / 2, handleSize / 2), QSize(handleSize, handleSize)),
                QRect(translatedCropBox.bottomLeft() - QPoint(handleSize / 2, handleSize / 2), QSize(handleSize, handleSize)),
                QRect(translatedCropBox.bottomRight() - QPoint(handleSize / 2, handleSize / 2), QSize(handleSize, handleSize)),
                QRect(translatedCropBox.left() + translatedCropBox.width() / 2 - handleSize / 2, translatedCropBox.top() - handleSize / 2, handleSize, handleSize),
                QRect(translatedCropBox.left() + translatedCropBox.width() / 2 - handleSize / 2, translatedCropBox.bottom() - handleSize / 2, handleSize, handleSize),
                QRect(translatedCropBox.left() - handleSize / 2, translatedCropBox.top() + translatedCropBox.height() / 2 - handleSize / 2, handleSize, handleSize),
                QRect(translatedCropBox.right() - handleSize / 2, translatedCropBox.top() + translatedCropBox.height() / 2 - handleSize / 2, handleSize, handleSize)
            };
            for (const QRect& handle : cropHandles) {
                painter.drawRect(handle);
            }
        }

        if (inpaintMode) {
            painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
            painter.drawImage(combinedBoundingBox.topLeft() + scrollPosition, maskImage);
        }

        if (snipeMode) {
            drawSnipePoints(painter, scrollPosition);
            QPoint popupPos = combinedBoundingBox.topRight() + scrollPosition + QPoint(10, 0);
            snipePopup->move(popupPos);
            snipePopup->setVisible(true);
        } else {
            snipePopup->setVisible(false);
        }

        // Disable the bounding box for the selected images before drawing the combined bounding box
        for (auto& img : selectedImages) {
            img->disableBoundingBox();
        }

        // Draw the combined bounding box around all selected images
        painter.setPen(QPen(Qt::magenta, 2, Qt::DashLine));
        painter.drawRect(combinedBoundingBox.translated(scrollPosition));

        // Show merge button if multiple images are selected
        toolbar->setMergeActionVisible(selectedImages.size() > 1);

    } else if (selectedImage) {
        QRect boundingBox = selectedImage->boundingBox;
        QPoint toolbarPos = boundingBox.topLeft() + scrollPosition - QPoint(0, toolbar->height());
        toolbar->move(toolbarPos);
        toolbar->setVisible(true);

        QPoint rotationSliderPos = boundingBox.bottomLeft() + scrollPosition + QPoint((boundingBox.width() - rotationSlider->width()) / 2, 10);
        rotationSlider->move(rotationSliderPos);
        rotationSlider->setVisible(rotationMode);

        QPoint eraserSliderPos = boundingBox.bottomLeft() + scrollPosition + QPoint((boundingBox.width() - eraserSizeSlider->width()) / 2, 10);
        eraserSizeSlider->move(eraserSliderPos);
        eraserSizeSlider->setVisible(eraserMode);

        QPoint depthSliderPos = boundingBox.bottomLeft() + scrollPosition + QPoint((boundingBox.width() - depthRemovalSlider->width()) / 2, 10);
        depthRemovalSlider->move(depthSliderPos);
        depthRemovalSlider->setVisible(depthRemovalMode);

        if (cropMode) {
            QRect translatedCropBox = cropBox.translated(scrollPosition);
            painter.setPen(QPen(Qt::blue, 2, Qt::DashLine));
            painter.drawRect(translatedCropBox);

            // Draw the crop handles
            const int handleSize = 6;
            painter.setBrush(Qt::white);
            painter.setPen(Qt::black);
            QRect cropHandles[] = {
                QRect(translatedCropBox.topLeft() - QPoint(handleSize / 2, handleSize / 2), QSize(handleSize, handleSize)),
                QRect(translatedCropBox.topRight() - QPoint(handleSize / 2, handleSize / 2), QSize(handleSize, handleSize)),
                QRect(translatedCropBox.bottomLeft() - QPoint(handleSize / 2, handleSize / 2), QSize(handleSize, handleSize)),
                QRect(translatedCropBox.bottomRight() - QPoint(handleSize / 2, handleSize / 2), QSize(handleSize, handleSize)),
                QRect(translatedCropBox.left() + translatedCropBox.width() / 2 - handleSize / 2, translatedCropBox.top() - handleSize / 2, handleSize, handleSize),
                QRect(translatedCropBox.left() + translatedCropBox.width() / 2 - handleSize / 2, translatedCropBox.bottom() - handleSize / 2, handleSize, handleSize),
                QRect(translatedCropBox.left() - handleSize / 2, translatedCropBox.top() + translatedCropBox.height() / 2 - handleSize / 2, handleSize, handleSize),
                QRect(translatedCropBox.right() - handleSize / 2, translatedCropBox.top() + translatedCropBox.height() / 2 - handleSize / 2, handleSize, handleSize)
            };
            for (const QRect& handle : cropHandles) {
                painter.drawRect(handle);
            }
        }

        if (inpaintMode) {
            painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
            painter.drawImage(boundingBox.topLeft() + scrollPosition, maskImage);
        }

        if (snipeMode) {
            drawSnipePoints(painter, scrollPosition);
            QPoint popupPos = boundingBox.topRight() + scrollPosition + QPoint(10, 0);
            snipePopup->move(popupPos);
            snipePopup->setVisible(true);
        } else {
            snipePopup->setVisible(false);
        }
    } else {
        toolbar->setVisible(false);
        rotationSlider->setVisible(false);
        eraserSizeSlider->setVisible(false);
        snipePopup->setVisible(false);
        depthRemovalSlider->setVisible(false);
    }

    // Draw the selection box
    if (isSelecting) {
        painter.setPen(QPen(Qt::blue, 2, Qt::DashLine));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(QRect(selectionStartPoint + scrollPosition, selectionEndPoint + scrollPosition));
    }

    // Ensure undo and redo buttons are always at the bottom right
    undoButton->move(width() - 180, height() - 40);
    redoButton->move(width() - 90, height() - 40);

    // Ensure addShapeButton and shapeMenu are always at the top right
    addShapeButton->move(width() - 180, 10);
    if (shapeMenu->isVisible()) {
        shapeMenu->move(addShapeButton->pos() + QPoint(0, addShapeButton->height()));
    }

    // Ensure generateAIImageButton and generateAIPopup are always at the top right
    generateAIImageButton->move(addShapeButton->x() - 200, 10);
    if (generateAIPopup->isVisible()) {
        generateAIPopup->move(generateAIImageButton->pos() + QPoint(0, generateAIImageButton->height()));
    }
}

void MyOpenGLWidget::dragEnterEvent(QDragEnterEvent* event) {
    //qDebug() << "Drag entered with MIME types:" << event->mimeData()->formats();
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void scaleImage(QImage& image, int maxWidth, int maxHeight) {
    QSize originalSize = image.size();
    QSize scaledSize = originalSize;

    if (originalSize.width() > maxWidth || originalSize.height() > maxHeight) {
        scaledSize = originalSize.scaled(maxWidth, maxHeight, Qt::KeepAspectRatio);
    }

    image = image.scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void MyOpenGLWidget::dropEvent(QDropEvent* event) {
    saveState(); // Save state before making changes
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urls = mimeData->urls();
        if (!urls.isEmpty()) {
            QImage image;
            if (image.load(urls.first().toLocalFile())) {
                scaleImage(image, MAX_IMAGE_WIDTH, MAX_IMAGE_HEIGHT);
                images.emplace_back(image, event->pos() - scrollPosition);
                update();
            }
        }
    }
}

void MyOpenGLWidget::contextMenuEvent(QContextMenuEvent* event) {
    if (!snipeMode) {
        QMenu contextMenu(this);
        QAction pasteAction("Paste", this);
        connect(&pasteAction, &QAction::triggered, this, &MyOpenGLWidget::pasteImageFromClipboard);
        contextMenu.addAction(&pasteAction);

        contextMenu.exec(event->globalPos());
    } else {
        event->ignore();
    }
}

void MyOpenGLWidget::openShapeMenu() {
    shapeMenu->exec();
}

void MyOpenGLWidget::openColorDialog() {
    colorDialog->setCurrentColor(selectedColor);
    connect(colorDialog, &QColorDialog::colorSelected, this, [this](const QColor& color) {
        selectedColor = color;
    });
    colorDialog->open();
}

void MyOpenGLWidget::createShape() {
    QString selectedShape = shapeSelector->currentText();
    QColor fillColor = selectedColor;

    if (selectedShape == "Square") {
        int defaultSize = 100;
        QImage shapeImage(defaultSize, defaultSize, QImage::Format_ARGB32);
        shapeImage.fill(fillColor);

        saveState();
        images.emplace_back(shapeImage, QPoint(width() / 2, height() / 2));
        update();
    }

    shapeMenu->close();
    colorDialog->close();
}

void MyOpenGLWidget::keyPressEvent(QKeyEvent* event) {
    if (event->matches(QKeySequence::Copy)) {
        copyImageToClipboard();
    } else if (event->matches(QKeySequence::Paste)) {
        pasteImageFromClipboard();
    } else {
        QOpenGLWidget::keyPressEvent(event);
    }
}

void MyOpenGLWidget::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Control || event->key() == Qt::Key_Meta) {
        clearFocus();
    }
    QOpenGLWidget::keyReleaseEvent(event);
}

void MyOpenGLWidget::mousePressEvent(QMouseEvent* event) {
    if (eraserMode && selectedImage) {
        eraseAt(event->pos());
        return;
    }

    if (inpaintMode && selectedImage) {
        drawMaskAt(event->pos());
        return;
    }

    if (snipeMode && selectedImage) {
        QPoint pos = event->pos() - scrollPosition;
        QPoint relativePos = pos - selectedImage->boundingBox.topLeft();

        // Calculate the relative position in terms of the original image size
        qreal relativeX = static_cast<qreal>(relativePos.x()) / selectedImage->boundingBox.width() * selectedImage->image.width();
        qreal relativeY = static_cast<qreal>(relativePos.y()) / selectedImage->boundingBox.height() * selectedImage->image.height();
        QPointF scaledPos(relativeX, relativeY);

        // Only add points within the image bounds
        if (scaledPos.x() >= 0 && scaledPos.x() < selectedImage->image.width() && scaledPos.y() >= 0 && scaledPos.y() < selectedImage->image.height()) {
            if (event->button() == Qt::LeftButton) {
                positivePoints.push_back(scaledPos);
            } else if (event->button() == Qt::RightButton) {
                negativePoints.push_back(scaledPos);
            }
        }

        update();
        return;
    }

    QPoint pos = event->pos() - scrollPosition; // Adjust for scroll position
    bool imageClicked = false;

    if (cropMode && selectedImage) {
        // Check if a crop handle was clicked
        int handle = cropHandleAt(pos);
        if (handle != 0) {
            currentHandle = handle;
            lastMousePosition = event->pos();
            return;
        }
    }

    if (event->button() == Qt::LeftButton) {
        if (event->modifiers() & Qt::ControlModifier || event->modifiers() & Qt::MetaModifier) {
            // Handle multi-select with ctrl/cmd click
            for (auto& img : images) {
                if (img.contains(pos, QPoint(0, 0))) {
                    auto it = std::find(selectedImages.begin(), selectedImages.end(), &img);
                    if (it != selectedImages.end()) {
                        // Image is already selected, unselect it
                        qDebug() << "Image unselected";
                        selectedImages.erase(it);
                        img.isSelected = false;

                        // If it was the last selected image, set selectedImage to nullptr
                        if (selectedImage == &img) {
                            qDebug() << "Selected image set to nullptr";
                            selectedImage = nullptr;
                        }
                    } else {
                        // Image is not selected, add it to selectedImages
                        if (selectedImage && std::find(selectedImages.begin(), selectedImages.end(), selectedImage) == selectedImages.end()) {
                            // If there is already a selected image, add it to selectedImages first
                            selectedImages.push_back(selectedImage);
                            selectedImage->isSelected = true;
                            selectedImage->disableBoundingBox();
                        }
                        selectedImages.push_back(&img);
                        img.isSelected = true;
                        img.disableBoundingBox();

                        // If the clicked image is the only image selected, set selectedImage to it
                        if (selectedImages.size() == 1) {
                            selectedImage = &img;

                            // Disable the bounding box for the selected image since any image added to selectedImages will have its bounding box drawn
                            selectedImage->disableBoundingBox();
                        }
                    }
                    imageClicked = true;
                    break;
                }
            }

            if (!imageClicked) {
                // If no image was clicked, start a selection box
                isSelecting = true;
                selectionStartPoint = event->pos() - scrollPosition; // Set the start point of the selection box considering scroll position
                selectionEndPoint = selectionStartPoint;
                isDragging = false; // Ensure dragging is reset
                currentHandle = 0; // Ensure handle is reset
            } else if (!selectedImages.empty() && selectedImages.size() > 1) {
                // Compute bounding box for selected images
                QRect combinedBoundingBox = computeBoundingBoxForSelectedImages();
                if (combinedBoundingBox.contains(pos)) {
                    isDragging = true;
                    selectedImage = nullptr;
                }
            }
            
        } else {
            // Iterate over images in reverse order to select the topmost image
            for (auto it = images.rbegin(); it != images.rend(); ++it) {
                ImageObject& img = *it;
                int handle = img.handleAt(pos, QPoint(0, 0));
                if (handle != 0) {
                    currentHandle = handle;
                    if (selectedImage != &img) {
                        if (selectedImage) {
                            selectedImage->isSelected = false;
                        }
                        selectedImage = &img;
                    }
                    img.isSelected = true;
                    imageClicked = true;
                    qDebug() << "Image topLeft:" << img.boundingBox.topLeft() << "pos:" << pos << "scrollPosition:" << scrollPosition;
                    qDebug() << "Image bottomRight:" << img.boundingBox.bottomRight() << "pos:" << pos << "scrollPosition:" << scrollPosition;
                    qDebug() << "Image bounding box size:" << selectedImage->boundingBox.size();
                    qDebug() << "Image size (not bounding box):" << selectedImage->image.size();
                    break;
                } else if (img.contains(pos, QPoint(0, 0))) {
                    if (selectedImage != &img) {
                        if (selectedImage) {
                            selectedImage->isSelected = false;
                        }
                        selectedImage = &img;
                    }
                    img.isSelected = true;
                    imageClicked = true;
                    qDebug() << "Image topLeft:" << img.boundingBox.topLeft() << "pos:" << pos << "scrollPosition:" << scrollPosition;
                    qDebug() << "Image bottomRight:" << img.boundingBox.bottomRight() << "pos:" << pos << "scrollPosition:" << scrollPosition;
                    qDebug() << "Image bounding box size:" << selectedImage->boundingBox.size();
                    qDebug() << "Image size (not bounding box):" << selectedImage->image.size();
                    break;
                } else {
                    img.isSelected = false;
                }
            }
            // for (auto& img : images){
            //     int handle = img.handleAt(pos, QPoint(0, 0));
            //     if (handle != 0) {
            //         currentHandle = handle;
            //         selectedImage = &img;
            //         img.isSelected = true;
            //         imageClicked = true;
            //         break;
            //     } else if (img.contains(pos, QPoint(0, 0))) {
            //         selectedImage = &img;
            //         img.isSelected = true;
            //         imageClicked = true;
            //         break;
            //     } else {
            //         img.isSelected = false;
            //     }
            // }

            // if (!imageClicked && !eraserMode && !cropMode && !inpaintMode && !snipeMode && !depthRemovalMode && !rotationMode) {
            if (!imageClicked) {
                // Check if click is inside combined bounding box of selected images
                QRect combinedBoundingBox = computeBoundingBoxForSelectedImages();
                if (combinedBoundingBox.contains(pos)) {
                    isDragging = true;
                } else {
                    isDragging = true;
                    selectedImage = nullptr;
                    clearSelection();
                    qDebug() << "Mouse pressed, selected image set to nullptr";
                }
            } else if (imageClicked && !selectedImages.empty()) {
                // Start dragging the combined selection
                isDragging = true;
                selectedImage = nullptr;
            }
        }
    }

    lastMousePosition = event->pos();
    update();
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent* event) {
    if (eraserMode && selectedImage) {
        eraseAt(event->pos());
        return;
    }

    if (inpaintMode && selectedImage) {
        drawMaskAt(event->pos());
        return;
    }

    if (isDragging && !selectedImage) {
        QPoint delta = event->pos() - lastMousePosition;
        if (!selectedImages.empty()) {
            for (auto& img : selectedImages) {
                img->boundingBox.translate(delta);
            }
        } else {
            scrollPosition += delta;
        }
        lastMousePosition = event->pos();
        update();
    } else if (event->buttons() & Qt::LeftButton && selectedImage) {
        if (cropMode && currentHandle != 0) {
            QPoint delta = event->pos() - lastMousePosition;
            adjustCropBox(delta);
            lastMousePosition = event->pos();
            update();
        } else if (currentHandle != 0) {
            QPoint delta = event->pos() - lastMousePosition;
            QRect rect = selectedImage->boundingBox;
            QRectF normalizedRect = rect.normalized();

            switch (currentHandle) {
                case 1:
                    normalizedRect.setTopLeft(normalizedRect.topLeft() + delta);
                    break;
                case 2:
                    normalizedRect.setTopRight(normalizedRect.topRight() + delta);
                    break;
                case 3:
                    normalizedRect.setBottomLeft(normalizedRect.bottomLeft() + delta);
                    break;
                case 4:
                    normalizedRect.setBottomRight(normalizedRect.bottomRight() + delta);
                    break;
                case 5:
                    normalizedRect.setTop(normalizedRect.top() + delta.y());
                    break;
                case 6:
                    normalizedRect.setBottom(normalizedRect.bottom() + delta.y());
                    break;
                case 7:
                    normalizedRect.setLeft(normalizedRect.left() + delta.x());
                    break;
                case 8:
                    normalizedRect.setRight(normalizedRect.right() + delta.x());
                    break;
            }

            selectedImage->boundingBox = normalizedRect.toRect();
            selectedImage->image = selectedImage->originalImage.scaled(selectedImage->boundingBox.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

            lastMousePosition = event->pos();
            update();
        } else if (!cropMode && !snipeMode) {
            QPoint delta = event->pos() - lastMousePosition;
            selectedImage->boundingBox.translate(delta);
            lastMousePosition = event->pos();
            update();
        }
    } else if (isSelecting) {
        selectionEndPoint = event->pos() - scrollPosition; // Update the end point relative to the scroll position
        update();
    }
}

void MyOpenGLWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (eraserMode || inpaintMode || snipeMode) {
        return;
    }
    isDragging = false;
    currentHandle = 0;

    if (isSelecting) {
        isSelecting = false;
        QRect selectionBox = QRect(selectionStartPoint, selectionEndPoint).normalized();
        selectImagesInBox(selectionBox.translated(scrollPosition)); // Translate selection box coordinates back to original
    }

    // if (selectedImage) {
    //     // Scale the image to the size of the bounding box
    //     selectedImage->image = selectedImage->originalImage.scaled(selectedImage->boundingBox.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    // }
    
    update();
}

void MyOpenGLWidget::copyImageToClipboard() {
    if (selectedImage) {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setImage(selectedImage->image);
    } else {
        qDebug() << "No image selected to copy";
    }
}

void MyOpenGLWidget::pasteImageFromClipboard() {
    QClipboard* clipboard = QApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData();

    //qDebug() << "Available MIME types in clipboard:" << mimeData->formats();

    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        //qDebug() << "Clipboard contains URLs:" << urlList;
        for (const QUrl &url : urlList) {
            if (url.isLocalFile()) {
                QImage image(url.toLocalFile());
                if (!image.isNull()) {
                    //qDebug() << "Loading image from URL:" << url.toLocalFile();
                    saveState(); // Save state before making changes
                    scaleImage(image, MAX_IMAGE_WIDTH, MAX_IMAGE_HEIGHT); // Scale the image to default smaller size
                    images.emplace_back(image, QPoint(width() / 2, height() / 2)); // Paste image at the center
                    update();
                    return;
                } else {
                    qDebug() << "Failed to load image from file URL";
                }
            }
        }
    }

    if (mimeData->hasFormat(QString("application/x-qt-image"))) {
        QByteArray imageData = mimeData->data("application/x-qt-image");
        QImage image = QImage::fromData(imageData);
        if (!image.isNull()) {
            //qDebug() << "Clipboard contains application/x-qt-image and successfully retrieved the image";
            saveState(); // Save state before making changes
            scaleImage(image, MAX_IMAGE_WIDTH, MAX_IMAGE_HEIGHT); // Scale the image to default smaller size
            images.emplace_back(image, QPoint(width() / 2, height() / 2)); // Paste image at the center
            update();
            return;
        } else {
            qDebug() << "Failed to retrieve image from clipboard for application/x-qt-image";
        }
    }

    if (mimeData->hasImage()) {
        QImage image = qvariant_cast<QImage>(mimeData->imageData());
        if (!image.isNull()) {
            //qDebug() << "Clipboard contains valid image data";
            saveState(); // Save state before making changes
            scaleImage(image, MAX_IMAGE_WIDTH, MAX_IMAGE_HEIGHT); // Scale the image to default smaller size
            images.emplace_back(image, QPoint(width() / 2, height() / 2)); // Paste image at the center
            update();
            return;
        } else {
            qDebug() << "Failed to retrieve valid image from clipboard despite hasImage()";
        }
    }

    if (mimeData->hasFormat("text/uri-list")) {
        QList<QUrl> urlList = mimeData->urls();
        //qDebug() << "Clipboard contains text/uri-list URLs:" << urlList;
        for (const QUrl &url : urlList) {
            if (url.isLocalFile()) {
                QImage image(url.toLocalFile());
                if (!image.isNull()) {
                    //qDebug() << "Loading image from URL:" << url.toLocalFile();
                    saveState(); // Save state before making changes
                    scaleImage(image, MAX_IMAGE_WIDTH, MAX_IMAGE_HEIGHT); // Scale the image to default smaller size
                    images.emplace_back(image, QPoint(width() / 2, height() / 2)); // Paste image at the center
                    update();
                    return;
                } else {
                    qDebug() << "Failed to load image from file URL";
                }
            }
        }
    }

    qDebug() << "No valid image data found in clipboard";
}

void MyOpenGLWidget::rotateSelectedImage(int angle) {
    if (!selectedImages.empty()) {
        saveState(); // Save state before making changes
        for (auto& img : selectedImages) {
            // Calculate the center of the image
            QPoint center = img->boundingBox.center();

            // Apply the rotation transformation around the center for image
            QTransform transform;
            transform.translate(center.x(), center.y());
            transform.rotate(angle);
            transform.translate(-center.x(), -center.y());

            // Transform the image
            QImage rotatedImage = img->originalImage.transformed(transform, Qt::SmoothTransformation);

            // Calculate the new bounding box to fit the rotated image
            QRect newBoundingBox = QRect(center - QPoint(rotatedImage.width() / 2, rotatedImage.height() / 2), rotatedImage.size());

            // Update the selected image and bounding box
            img->image = rotatedImage;
            img->boundingBox = newBoundingBox;
        }
        update();
    } else if (selectedImage) {
        qDebug() << "Rotating image by" << angle;
        
        saveState(); // Save state before making changes

        if (originalImageBeforeRotation.isNull()) {
            originalImageBeforeRotation = selectedImage->image;
        }

        // Calculate the center of the image
        QPoint center = selectedImage->boundingBox.center();

        // Apply the rotation transformation around the center for image
        QTransform transform;
        transform.translate(center.x(), center.y());
        transform.rotate(angle);
        transform.translate(-center.x(), -center.y());

        // Transform the image
        QImage rotatedImage = originalImageBeforeRotation.transformed(transform, Qt::SmoothTransformation);

        // Calculate the new bounding box to fit the rotated image
        QRect newBoundingBox = QRect(center - QPoint(rotatedImage.width() / 2, rotatedImage.height() / 2), rotatedImage.size());

        // Update the selected image and bounding box
        selectedImage->image = rotatedImage;
        selectedImage->boundingBox = newBoundingBox;

        selectedImage->originalImage = selectedImage->image;

        update();
    } else {
        qDebug() << "No image selected";
    }
}

void MyOpenGLWidget::toggleRotationMode(bool enabled) {
    rotationMode = enabled;
    if (enabled) {
        rotationSlider->setVisible(true);
    } else {
        rotationSlider->setVisible(false);
    }
    update();
}

void MyOpenGLWidget::mirrorSelectedImage() {
    if (!selectedImages.empty()) {
        saveState(); // Save state before making changes
        for (auto& img : selectedImages) {
            img->image = img->image.mirrored(true, false);
        }
        update();
    } else if (selectedImage) {
        saveState(); // Save state before making changes
        selectedImage->image = selectedImage->image.mirrored(true, false);
        selectedImage->originalImage = selectedImage->image;
        update();
    } else {
        qDebug() << "No image selected";
    }
}

void MyOpenGLWidget::copySelectedImage() {
    if (selectedImage) {
        saveState(); // Save state before making changes
        images.emplace_back(selectedImage->image, selectedImage->boundingBox.center() + QPoint(20, 20));
        update();
    } else {
        qDebug() << "No image selected";
    }
}

void MyOpenGLWidget::deleteSelectedImage() {
    if (selectedImage) {
        saveState(); // Save state before making changes
        images.erase(std::remove(images.begin(), images.end(), *selectedImage), images.end());
        selectedImage = nullptr;
        update();
    } else {
        qDebug() << "No image selected";
    }
}

void MyOpenGLWidget::saveSelectedImage() {
    if (selectedImage) {
        QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "", "PNG Files (*.png);;All Files (*)");
        if (!fileName.isEmpty()) {
            selectedImage->image.save(fileName);
        }
    } else {
        qDebug() << "No image selected";
    }
}

void MyOpenGLWidget::toggleEraserMode(bool enabled) {
    eraserMode = enabled;
    if (enabled) {
        setCursor(Qt::CrossCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
    update();
}

void MyOpenGLWidget::eraseAt(const QPoint& pos) {
    if (!selectedImage) return;

    QPoint imgPos = pos - scrollPosition - selectedImage->boundingBox.topLeft();
    QPainter painter(&selectedImage->image);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.setBrush(QBrush(Qt::transparent));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(imgPos, eraserSizeSlider->value() / 2, eraserSizeSlider->value() / 2);

    selectedImage->originalImage = selectedImage->image;
    update();
}

bool MyOpenGLWidget::eventFilter(QObject* obj, QEvent* event) {
    if (eraserMode && selectedImage && event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        eraseAt(mouseEvent->pos());
        return true;
    }
    return QOpenGLWidget::eventFilter(obj, event);
}

void MyOpenGLWidget::saveState() {
    // Helper function to save the current state of the images for undo/redo
    undoStack.push(images);
    while (!redoStack.empty()) {
        redoStack.pop();
    }
}

void MyOpenGLWidget::undo() {
    if (!undoStack.empty()) {
        redoStack.push(images);
        images = undoStack.top();
        undoStack.pop();

        if (selectedImage && std::find(images.begin(), images.end(), *selectedImage) == images.end()) {
            selectedImage = nullptr;
            toolbar->setVisible(false);
        }

        update();
    }
}

void MyOpenGLWidget::redo() {
    if (!redoStack.empty()) {
        undoStack.push(images);
        images = redoStack.top();
        redoStack.pop();

        if (selectedImage && std::find(images.begin(), images.end(), *selectedImage) == images.end()) {
            selectedImage = nullptr;
            toolbar->setVisible(false);
        }
        
        update();
    }
}

void MyOpenGLWidget::uploadImage() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.xpm *.jpg)");
    if (!fileName.isEmpty()) {
        QImage image;
        if (image.load(fileName)) {
            saveState(); // Save state before making changes
            scaleImage(image, MAX_IMAGE_WIDTH, MAX_IMAGE_HEIGHT);
            images.emplace_back(image, QPoint(0, 0));
            update();
        }
    }
}

void MyOpenGLWidget::bringToFront() {
    if (selectedImage) {
        saveState(); // Save state before making changes
        auto it = std::find(images.begin(), images.end(), *selectedImage);
        if (it != images.end()) {
            std::rotate(it, it + 1, images.end());
            selectedImage = &(images.back());
        }
        update();
    }
}

void MyOpenGLWidget::pushToBack() {
    if (selectedImage) {
        saveState(); // Save state before making changes
        auto it = std::find(images.begin(), images.end(), *selectedImage);
        if (it != images.end() && it != images.begin()) {
            std::rotate(images.begin(), it, it + 1);
            selectedImage = &(images.front());
        }
        update();
    }
}

void MyOpenGLWidget::toggleCropMode(bool enabled) {
    cropMode = enabled;
    if (enabled && selectedImage) {
        selectedImage->disableBoundingBox();
        cropBox = selectedImage->boundingBox;
        setCursor(Qt::CrossCursor);
    } else {
        setCursor(Qt::ArrowCursor);
        if (selectedImage) {
            selectedImage->enableBoundingBox();
            if (cropBox != selectedImage->boundingBox) {
                saveState(); // Save state before making changes
                selectedImage->image = selectedImage->image.copy(cropBox.translated(-selectedImage->boundingBox.topLeft()));
                selectedImage->boundingBox.setSize(cropBox.size());
            }
        }
    }
    update();
}

void MyOpenGLWidget::toggleInpaintMode(bool enabled) {
    inpaintMode = enabled;
    if (enabled && selectedImage) {
        selectedImage->originalImage = selectedImage->image;
        maskImage = QImage(selectedImage->image.size(), QImage::Format_ARGB32);
        maskImage.fill(Qt::transparent);
        selectedImage->disableBoundingBox();
        setCursor(Qt::CrossCursor);
        QPoint popupPos = selectedImage->boundingBox.bottomLeft() + scrollPosition + QPoint(10, 10);
        inpaintPopup->move(popupPos);
        inpaintPopup->setVisible(true);
    } else {
        toolbar->checkInpaintAction(false);
        setCursor(Qt::ArrowCursor);
        if (selectedImage) {
            selectedImage->enableBoundingBox();
        }
        inpaintPopup->setVisible(false);
    }
    update();
}

void MyOpenGLWidget::openGenerateAIMenu() {
    generateAIPopup->setVisible(true);
    generateAIPopup->raise();
    generateAIPopup->move(generateAIImageButton->pos() + QPoint(0, generateAIImageButton->height()));
}

void MyOpenGLWidget::toggleAPIKeyInput(bool enabled) {
    apiKeyTextBox->setEnabled(enabled && !useEnvVarCheckBox->isChecked());
}

void MyOpenGLWidget::confirmGenerateAIImage() {
    QString apiKey = apiKeyTextBox->text();
    QString prompt = promptTextBox->text();

    if (useEnvVarCheckBox->isChecked()) {
        apiKey = qgetenv("OPENAI_API_KEY");
        if (apiKey.isEmpty()) {
            QMessageBox::warning(this, "Error", "Environment variable OPENAI_API_KEY is not set.");
            return;
        }
    }

    if (useOpenAICheckBox->isChecked() && apiKey.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please provide a valid API key.");
        return;
    }

    if (prompt.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please provide a prompt.");
        return;
    }

    generateAIPopup->setVisible(false);

    // Show progress dialog
    progressDialog = new QProgressDialog("Generating AI Image...", "Cancel", 0, 0, this);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setCancelButton(nullptr);
    progressDialog->show();

    // Prepare JSON data for Python script
    QJsonObject json;
    json["api_key"] = apiKey;
    json["prompt"] = prompt;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    pythonProcess = new QProcess(this);
    connect(pythonProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MyOpenGLWidget::handleGeneratedAIImage);
    connect(pythonProcess, &QProcess::errorOccurred, this, &MyOpenGLWidget::handleProcessError);
    connect(pythonProcess, &QProcess::readyReadStandardOutput, this, &MyOpenGLWidget::handlePythonOutput);
    connect(pythonProcess, &QProcess::readyReadStandardError, this, &MyOpenGLWidget::handlePythonError);

    QString pythonExecutable = QDir(projectRoot).absoluteFilePath("local-image-editor-venv/Scripts/python.exe");
    QString scriptDir = QDir(projectRoot).absoluteFilePath("resources/scripts/inference");

    if (!QDir(scriptDir).exists()) {
        qDebug() << "Directory does not exist: " << scriptDir;
        progressDialog->hide();
        delete progressDialog;
        return;
    }

    pythonProcess->setWorkingDirectory(scriptDir);

    pythonProcess->start(pythonExecutable, QStringList() << "generate_ai_image.py");

    if (!pythonProcess->waitForStarted()) {
        qDebug() << "Failed to start Python process.";
        progressDialog->hide();
        delete progressDialog;
        return;
    }

    pythonProcess->write(data);
    pythonProcess->closeWriteChannel();

    if (!pythonProcess->waitForFinished(60000 * 5)) {
        qDebug() << "Python process did not finish within the expected time.";
        progressDialog->hide();
        delete progressDialog;
    }


}

void MyOpenGLWidget::handleGeneratedAIImage() {
    QString filePath = projectRoot + "/resources/scripts/inference/generated_ai_image.txt";
    
    // Open the file containing the Base64 encoded image string
    std::ifstream file(filePath.toStdString(), std::ios::binary);
    if (!file.is_open()) {
        qDebug() << "Failed to open generated AI image file.";
        QMessageBox::critical(this, "Error", "Failed to open the generated AI image file.");
        return;
    }

    // Read the entire file into a stringstream
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string resultBase64 = buffer.str();
    file.close();

    // Convert the Base64 string to a QByteArray
    QByteArray resultByteArray = QByteArray::fromBase64(QString::fromStdString(resultBase64).toUtf8());

    // Load the image from the QByteArray
    QPixmap resultImage;
    if (!resultImage.loadFromData(resultByteArray)) {
        qDebug() << "Failed to decode the image.";
        QMessageBox::critical(this, "Error", "Failed to decode the generated AI image.");
        return;
    }

    // Convert the QPixmap to QImage
    QImage resultQImage = resultImage.toImage();

    // Resize the image to fit the canvas
    scaleImage(resultQImage, 512, 512);

    // Create a new ImageObject and add it to the canvas
    saveState(); // Save state before making changes
    images.emplace_back(resultQImage, QPoint(width() / 2, height() / 2)); // Add the image to the center
    selectedImage = &images.back(); // Set the new image as the selected image
    update(); // Refresh the canvas

    progressDialog->hide();
    delete progressDialog;

    // Delete the result file to clean up
    std::remove(filePath.toStdString().c_str());
}


// void MyOpenGLWidget::confirmInpaint() {
//     if (!selectedImage) return;

//     QImage originalImage = selectedImage->image;

//     // Create a mask image where the mask area is white and the unmasked area is black
//     QImage binaryMask(maskImage.size(), QImage::Format_RGB32);
//     binaryMask.fill(Qt::black);

//     for (int y = 0; y < maskImage.height(); ++y) {
//         for (int x = 0; x < maskImage.width(); ++x) {
//             QColor color = maskImage.pixelColor(x, y);
//             if (color == Qt::magenta) {
//                 binaryMask.setPixelColor(x, y, Qt::white);
//             }
//         }
//     }

//     QByteArray originalByteArray;
//     QBuffer originalBuffer(&originalByteArray);
//     originalImage.save(&originalBuffer, "PNG");
//     QString originalBase64 = originalByteArray.toBase64();

//     QByteArray maskByteArray;
//     QBuffer maskBuffer(&maskByteArray);
//     binaryMask.save(&maskBuffer, "PNG");
//     QString maskBase64 = maskByteArray.toBase64();

//     // Convert images to base64
//     // QByteArray originalImageBase64;
//     // QByteArray maskImageBase64;
//     // QBuffer originalImageBuffer(&originalImageBase64);
//     // QBuffer maskImageBuffer(&maskImageBase64);
//     // originalImageBuffer.open(QIODevice::WriteOnly);
//     // maskImageBuffer.open(QIODevice::WriteOnly);
//     // selectedImage->image.save(&originalImageBuffer, "PNG");
//     // binaryMask.save(&maskImageBuffer, "PNG");

//     // Call the Python function with the base64 images
//     QString promptText = inpaintTextBox->text();
//     // QString resultImageBase64 = callPythonInpaintFunction(originalImageBase64.toBase64(), maskImageBase64.toBase64(), inpaintText);

//     // Convert the result base64 image back to QImage
//     // QByteArray resultImageBytes = QByteArray::fromBase64(resultImageBase64.toUtf8());
//     // QImage resultImage;
//     // resultImage.loadFromData(resultImageBytes, "PNG");

//     // Replace the selected image with the result image
//     // saveState(); // Save state before making changes
//     // selectedImage->image = resultImage;
//     // selectedImage->boundingBox.setSize(selectedImage->image.size());

//     // std::string resultImageBase64 = process_images(originalImageBase64.toBase64().toStdString(), maskImageBase64.toBase64().toStdString(), promptText.toStdString());
//     // qDebug() << "Result image base64: " << QString::fromStdString(resultImageBase64);

//     // QByteArray resultImageBytes = QByteArray::fromBase64(resultImageBase64.c_str());
//     // QImage resultImage;
//     // resultImage.loadFromData(resultImageBytes, "PNG");

//     // qDebug() << "Result image loaded";

//     // Disable UI elements and show progress dialog
//     inpaintPopup->setVisible(false);
//     progressDialog = new QProgressDialog("Inpainting...", "Cancel", 0, 0, this);
//     progressDialog->setWindowModality(Qt::WindowModal);
//     progressDialog->setCancelButton(nullptr);
//     progressDialog->show();

//     QJsonObject json;
//     json["init_image_base64"] = QString::fromStdString(originalBase64.toStdString());
//     json["mask_image_base64"] = QString::fromStdString(maskBase64.toStdString());
//     json["user_prompt"] = promptText;

//     QJsonDocument doc(json);
//     QByteArray data = doc.toJson(QJsonDocument::Compact);

//     pythonProcess = new QProcess(this);
//     connect(pythonProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MyOpenGLWidget::handleInpaintResult);
//     connect(pythonProcess, QOverload<QProcess::ProcessError>::of(&QProcess::errorOccurred), this, &MyOpenGLWidget::handleInpaintError);
    
//     // Set working directory if needed
//     pythonProcess->setWorkingDirectory(projectRoot + "/resources/scripts/inference");

//     pythonProcess->start("python3.10", QStringList() << "inpainting.py");
//     pythonProcess->write(data);
//     pythonProcess->closeWriteChannel();
// }

void MyOpenGLWidget::confirmInpaint() {
    if (!selectedImage) return;

    saveState();

    QImage originalImage = selectedImage->image;

    // Create a mask image where the mask area is white and the unmasked area is black
    QImage binaryMask(maskImage.size(), QImage::Format_RGB32);
    binaryMask.fill(Qt::black);

    for (int y = 0; y < maskImage.height(); ++y) {
        for (int x = 0; x < maskImage.width(); ++x) {
            QColor color = maskImage.pixelColor(x, y);
            if (color == Qt::magenta) {
                binaryMask.setPixelColor(x, y, Qt::white);
            }
        }
    }

    QByteArray originalByteArray;
    QBuffer originalBuffer(&originalByteArray);
    originalImage.save(&originalBuffer, "PNG");
    QString originalBase64 = originalByteArray.toBase64();

    QByteArray maskByteArray;
    QBuffer maskBuffer(&maskByteArray);
    binaryMask.save(&maskBuffer, "PNG");
    QString maskBase64 = maskByteArray.toBase64();

    QString promptText = inpaintTextBox->text();

    // Disable UI elements and show progress dialog
    inpaintPopup->setVisible(false);
    progressDialog = new QProgressDialog("Inpainting...", "Cancel", 0, 0, this);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setCancelButton(nullptr);
    progressDialog->show();

    QJsonObject json;
    json["init_image_base64"] = QString::fromStdString(originalBase64.toStdString());
    json["mask_image_base64"] = QString::fromStdString(maskBase64.toStdString());
    json["user_prompt"] = promptText;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    pythonProcess = new QProcess(this);
    connect(pythonProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MyOpenGLWidget::handleInpaintResult);
    connect(pythonProcess, &QProcess::errorOccurred, this, &MyOpenGLWidget::handleProcessError);
    connect(pythonProcess, &QProcess::readyReadStandardOutput, this, &MyOpenGLWidget::handlePythonOutput);
    connect(pythonProcess, &QProcess::readyReadStandardError, this, &MyOpenGLWidget::handlePythonError);

    QString pythonExecutable = QDir(projectRoot).absoluteFilePath("local-image-editor-venv/Scripts/python.exe");  // Adjust path as needed

    QString scriptDir = QDir(projectRoot).absoluteFilePath("resources/scripts/inference");
    QString outputDir = QDir(projectRoot).absoluteFilePath("resources/scripts/inference");

    if (!QDir(scriptDir).exists()) {
        qDebug() << "Directory does not exist: " << scriptDir;
        progressDialog->hide();
        delete progressDialog;
        return;
    }

    pythonProcess->setWorkingDirectory(scriptDir);

    pythonProcess->start(pythonExecutable, QStringList() << "inpainting.py" << outputDir);

    if (!pythonProcess->waitForStarted()) {
        qDebug() << "Failed to start Python process.";
        progressDialog->hide();
        delete progressDialog;
        return;
    }

    pythonProcess->write(data);
    pythonProcess->closeWriteChannel();

    qDebug() << "*** Inpainting can take a bit longer, especially on less powerful machines, due to float32 operations instead of float16. Mac M1 took roughly 20-30 seconds for a 512x512 image using FP32. ***";

    if (!pythonProcess->waitForFinished(60000*5)) {
        qDebug() << "Python process did not finish within the expected time.";
        progressDialog->hide();
        delete progressDialog;
    }
}


void MyOpenGLWidget::handleInpaintResult() {
    if (!selectedImage) return;

    // Get the size of the selected image
    QSize originalSize = selectedImage->image.size();

    std::ifstream file((projectRoot + "/resources/scripts/inference/inpainting_result.txt").toStdString(), std::ios::binary);
    if (!file.is_open()) {
        qDebug() << "Failed to open inpainting result file.";
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string resultBase64 = buffer.str();
    file.close();

    QByteArray resultByteArray = QByteArray::fromBase64(QString::fromStdString(resultBase64).toUtf8());

    // Load the image from the QByteArray
    QPixmap resultImage;
    if (!resultImage.loadFromData(resultByteArray)) {
        qDebug() << "Failed to decode the image.";
        QMessageBox::critical(this, "Error", "Failed to decode the inpainted image.");
        return;
    }

    // Convert the QPixmap to QImage
    QImage resultQImage = resultImage.toImage();

    // Set the size of the inpainted image to the original size
    resultQImage = resultQImage.scaled(originalSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    // Replace the selected image with the inpainted image
    selectedImage->image = resultQImage;
    selectedImage->boundingBox.setSize(resultQImage.size());

    selectedImage->originalImage = selectedImage->image;

    toggleInpaintMode(false);
    progressDialog->hide();
    delete progressDialog;

    update();

    // Delete the result file
    std::remove((projectRoot + "/resources/scripts/inference/inpainting_result.txt").toStdString().c_str());
}

void MyOpenGLWidget::handleInpaintError(QProcess::ProcessError error) {
    QString errorOutput = pythonProcess->readAllStandardError();
    QMessageBox::critical(this, "Error", "Inpainting process failed: " + errorOutput);
    toggleInpaintMode(false);
    progressDialog->hide();
    delete progressDialog;
}

void MyOpenGLWidget::toggleSnipeMode(bool enabled) {
    snipeMode = enabled;
    if (enabled) {
        positivePoints.clear();
        negativePoints.clear();
        setCursor(Qt::CrossCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
    update();
}

// void MyOpenGLWidget::confirmSnipe() {
//     if (!selectedImage || positivePoints.empty()) return;

//     QImage originalImage = selectedImage->image;
//     QByteArray originalByteArray;
//     QBuffer originalBuffer(&originalByteArray);
//     originalImage.save(&originalBuffer, "PNG");
//     QString originalBase64 = originalByteArray.toBase64();

//     // Create JSON object to send to Python script
//     QJsonObject json;
//     QJsonArray positiveArray;
//     QJsonArray negativeArray;

//     json["original_image"] = QString::fromStdString(originalBase64.toStdString());

//     for (const auto& point : positivePoints) {
//         QJsonObject pointJson;
//         pointJson["x"] = point.x();
//         pointJson["y"] = point.y();
//         positiveArray.append(pointJson);
//     }

//     for (const auto& point : negativePoints) {
//         QJsonObject pointJson;
//         pointJson["x"] = point.x();
//         pointJson["y"] = point.y();
//         negativeArray.append(pointJson);
//     }

//     qDebug() << "Positive points: " << positiveArray;
//     qDebug() << "Negative points: " << negativeArray;

//     json["positive_points"] = positiveArray;
//     json["negative_points"] = negativeArray;

//     progressDialog = new QProgressDialog("Sniping...", "Cancel", 0, 0, this);
//     progressDialog->setWindowModality(Qt::WindowModal);
//     progressDialog->setCancelButton(nullptr);
//     progressDialog->show();

//     QJsonDocument doc(json);
//     QByteArray data = doc.toJson(QJsonDocument::Compact);

//     pythonProcess = new QProcess(this);
//     connect(pythonProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MyOpenGLWidget::handleSnipeResult);
//     //connect(pythonProcess, QOverload<QProcess::ProcessError>::of(&QProcess::errorOccurred), this, &MyOpenGLWidget::handleSnipeError);

//     // Set working directory if needed
//     pythonProcess->setWorkingDirectory(projectRoot + "/resources/scripts/inference");

//     pythonProcess->start("python3.10", QStringList() << "sam.py");
//     pythonProcess->write(data);
//     pythonProcess->closeWriteChannel();
// }

void MyOpenGLWidget::confirmSnipe() {
    if (!selectedImage || positivePoints.empty()) return;

    saveState();

    QImage originalImage = selectedImage->image;
    QByteArray originalByteArray;
    QBuffer originalBuffer(&originalByteArray);
    originalImage.save(&originalBuffer, "PNG");
    QString originalBase64 = originalByteArray.toBase64();

    // Create JSON object to send to Python script
    QJsonObject json;
    QJsonArray positiveArray;
    QJsonArray negativeArray;

    json["original_image"] = QString::fromStdString(originalBase64.toStdString());

    for (const auto& point : positivePoints) {
        QJsonObject pointJson;
        pointJson["x"] = point.x();
        pointJson["y"] = point.y();
        positiveArray.append(pointJson);
    }

    for (const auto& point : negativePoints) {
        QJsonObject pointJson;
        pointJson["x"] = point.x();
        pointJson["y"] = point.y();
        negativeArray.append(pointJson);
    }

    qDebug() << "Positive points: " << positiveArray;
    qDebug() << "Negative points: " << negativeArray;

    json["positive_points"] = positiveArray;
    json["negative_points"] = negativeArray;

    progressDialog = new QProgressDialog("Sniping...", "Cancel", 0, 0, this);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setCancelButton(nullptr);
    progressDialog->show();

    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    pythonProcess = new QProcess(this);
    connect(pythonProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MyOpenGLWidget::handleSnipeResult);
    connect(pythonProcess, &QProcess::errorOccurred, this, &MyOpenGLWidget::handleProcessError);
    connect(pythonProcess, &QProcess::readyReadStandardOutput, this, &MyOpenGLWidget::handlePythonOutput);
    connect(pythonProcess, &QProcess::readyReadStandardError, this, &MyOpenGLWidget::handlePythonError);

    QString pythonExecutable = QDir(projectRoot).absoluteFilePath("local-image-editor-venv/Scripts/python.exe");  // Adjust path as needed

    QString scriptDir = QDir(projectRoot).absoluteFilePath("resources/scripts/inference");
    QString outputDir = QDir(projectRoot).absoluteFilePath("resources/scripts/inference");

    if (!QDir(scriptDir).exists()) {
        qDebug() << "Directory does not exist: " << scriptDir;
        progressDialog->hide();
        delete progressDialog;
        return;
    }

    pythonProcess->setWorkingDirectory(scriptDir);

    pythonProcess->start(pythonExecutable, QStringList() << "sam.py" << outputDir);

    if (!pythonProcess->waitForStarted()) {
        qDebug() << "Failed to start Python process.";
        progressDialog->hide();
        delete progressDialog;
        return;
    }

    pythonProcess->write(data);
    pythonProcess->closeWriteChannel();

    if (!pythonProcess->waitForFinished(60000*5)) {
        qDebug() << "Python process did not finish within the expected time.";
        progressDialog->hide();
        delete progressDialog;
    }
}

void MyOpenGLWidget::handleSnipeResult() {
    if (!selectedImage) return;

    // Three files we need to convert to QImages: image_hole.txt, image_object.txt, image_with_mask.txt

    std::ifstream fileHole((projectRoot + "/resources/scripts/inference/image_hole.txt").toStdString(), std::ios::binary);
    if (!fileHole.is_open()) {
        qDebug() << "Failed to open image hole file.";
        return;
    }

    std::stringstream bufferHole;
    bufferHole << fileHole.rdbuf();
    std::string resultBase64 = bufferHole.str();
    fileHole.close();

    std::ifstream fileObject((projectRoot + "/resources/scripts/inference/image_object.txt").toStdString(), std::ios::binary);
    if (!fileObject.is_open()) {
        qDebug() << "Failed to open image object file.";
        return;
    }

    std::stringstream bufferObject;
    bufferObject << fileObject.rdbuf();
    std::string resultObjectBase64 = bufferObject.str();
    fileObject.close();

    std::ifstream fileMask((projectRoot + "/resources/scripts/inference/image_with_mask.txt").toStdString(), std::ios::binary);
    if (!fileMask.is_open()) {
        qDebug() << "Failed to open image with mask file.";
        return;
    }

    std::stringstream bufferMask;
    bufferMask << fileMask.rdbuf();
    std::string resultMaskBase64 = bufferMask.str();
    fileMask.close();

    QByteArray imageHoleByteArray = QByteArray::fromBase64(QString::fromStdString(resultBase64).toUtf8());
    QByteArray imageObjectByteArray = QByteArray::fromBase64(QString::fromStdString(resultObjectBase64).toUtf8());
    QByteArray imageWithMaskByteArray = QByteArray::fromBase64(QString::fromStdString(resultMaskBase64).toUtf8());

    QPixmap imageHole;
    QPixmap imageObject;
    QPixmap imageWithMask;

    if (!imageHole.loadFromData(imageHoleByteArray) || !imageObject.loadFromData(imageObjectByteArray) || !imageWithMask.loadFromData(imageWithMaskByteArray)) {
        qDebug() << "Failed to decode the images.";
        QMessageBox::critical(this, "Error", "Failed to decode the snipe images.");
        return;
    }

    // Convert the QPixmap to QImage
    QImage imageHoleQImage = imageHole.toImage();
    QImage imageObjectQImage = imageObject.toImage();
    QImage imageWithMaskQImage = imageWithMask.toImage();

    // Scale
    // scaleImage(imageHoleQImage, MAX_IMAGE_WIDTH, MAX_IMAGE_HEIGHT);
    // scaleImage(imageObjectQImage, MAX_IMAGE_WIDTH, MAX_IMAGE_HEIGHT);
    // scaleImage(imageWithMaskQImage, MAX_IMAGE_WIDTH, MAX_IMAGE_HEIGHT);

    // Store the original image before replacing it with the mask image
    QImage originalImage = selectedImage->image;

    progressDialog->hide();
    delete progressDialog;

    // Replace the selected image with the image with mask and popup a confirmation dialog to confirm or deny the selected mask
    selectedImage->image = imageWithMaskQImage;
    selectedImage->boundingBox.setSize(imageWithMaskQImage.size());

    // Create and show the custom confirmation dialog
    confirmationDialog = new CustomConfirmationDialog(this);
    confirmationDialog->setImage(imageWithMaskQImage);
    connect(confirmationDialog, &CustomConfirmationDialog::confirmed, this, [this, imageHoleQImage, imageObjectQImage]() {
        // Replace the image with the hole and add the object image
        selectedImage->image = imageHoleQImage;
        selectedImage->boundingBox.setSize(imageHoleQImage.size());

        selectedImage->originalImage = selectedImage->image;

        ImageObject newObjectImage(imageObjectQImage, selectedImage->boundingBox.topLeft());
        newObjectImage.isSelected = true;
        images.push_back(newObjectImage);
        selectedImage = &images.back();

        toggleSnipeMode(false);
        update();
    });
    connect(confirmationDialog, &CustomConfirmationDialog::denied, this, [this, originalImage]() {
        // Revert to the original image
        selectedImage->image = originalImage;
        selectedImage->boundingBox.setSize(originalImage.size());

        toggleSnipeMode(false);
        update();
    });
    confirmationDialog->show();

    // Delete the result files
    std::remove((projectRoot + "/resources/scripts/inference/image_hole.txt").toStdString().c_str());
    std::remove((projectRoot + "/resources/scripts/inference/image_object.txt").toStdString().c_str());
    std::remove((projectRoot + "/resources/scripts/inference/image_with_mask.txt").toStdString().c_str());
    std::remove((projectRoot + "/resources/scripts/inference/snipe_result.txt").toStdString().c_str());
}

void MyOpenGLWidget::clearSnipePoints() {
    positivePoints.clear();
    negativePoints.clear();
    update();
}

void MyOpenGLWidget::drawSnipePoints(QPainter& painter, const QPoint& scrollPosition) {
    painter.setPen(QPen(Qt::white, 2));
    for (const auto& point : positivePoints) {
        painter.setBrush(Qt::green);
        QPoint drawPoint = point.toPoint() + selectedImage->boundingBox.topLeft() + scrollPosition;
        painter.drawEllipse(drawPoint, 5, 5);
    }
    for (const auto& point : negativePoints) {
        painter.setBrush(Qt::red);
        QPoint drawPoint = point.toPoint() + selectedImage->boundingBox.topLeft() + scrollPosition;
        painter.drawEllipse(drawPoint, 5, 5);
    }
}

void MyOpenGLWidget::adjustCropBox(const QPoint& delta) {
    switch (currentHandle) {
        case 1:
            cropBox.setTopLeft(cropBox.topLeft() + delta);
            break;
        case 2:
            cropBox.setTopRight(cropBox.topRight() + delta);
            break;
        case 3:
            cropBox.setBottomLeft(cropBox.bottomLeft() + delta);
            break;
        case 4:
            cropBox.setBottomRight(cropBox.bottomRight() + delta);
            break;
        case 5:
            cropBox.setTop(cropBox.top() + delta.y());
            break;
        case 6:
            cropBox.setBottom(cropBox.bottom() + delta.y());
            break;
        case 7:
            cropBox.setLeft(cropBox.left() + delta.x());
            break;
        case 8:
            cropBox.setRight(cropBox.right() + delta.x());
            break;
    }

    // Ensure the crop box stays within the bounds of the image
    cropBox = cropBox.intersected(selectedImage->boundingBox);

    // Ensure the crop box maintains a minimum size
    if (cropBox.width() < 10) {
        cropBox.setWidth(10);
    }
    if (cropBox.height() < 10) {
        cropBox.setHeight(10);
    }
}

int MyOpenGLWidget::cropHandleAt(const QPoint& pos) const {
    const int handleSize = 6;
    QRect cropHandles[] = {
        QRect(cropBox.topLeft() - QPoint(handleSize / 2, handleSize / 2), QSize(handleSize, handleSize)),
        QRect(cropBox.topRight() - QPoint(handleSize / 2, handleSize / 2), QSize(handleSize, handleSize)),
        QRect(cropBox.bottomLeft() - QPoint(handleSize / 2, handleSize / 2), QSize(handleSize, handleSize)),
        QRect(cropBox.bottomRight() - QPoint(handleSize / 2, handleSize / 2), QSize(handleSize, handleSize)),
        QRect(cropBox.left() + cropBox.width() / 2 - handleSize / 2, cropBox.top() - handleSize / 2, handleSize, handleSize),
        QRect(cropBox.left() + cropBox.width() / 2 - handleSize / 2, cropBox.bottom() - handleSize / 2, handleSize, handleSize),
        QRect(cropBox.left() - handleSize / 2, cropBox.top() + cropBox.height() / 2 - handleSize / 2, handleSize, handleSize),
        QRect(cropBox.right() - handleSize / 2, cropBox.top() + cropBox.height() / 2 - handleSize / 2, handleSize, handleSize)
    };
    for (int i = 0; i < 8; ++i) {
        if (cropHandles[i].contains(pos)) {
            return i + 1;
        }
    }
    return 0;
}

void MyOpenGLWidget::drawMaskAt(const QPoint& pos) {
    if (!selectedImage) return;

    QPoint imgPos = pos - scrollPosition - selectedImage->boundingBox.topLeft();

    // Ensure imgPos is within the bounds of the image
    imgPos.setX(qBound(0, imgPos.x(), selectedImage->image.width() - 1));
    imgPos.setY(qBound(0, imgPos.y(), selectedImage->image.height() - 1));
    QPainter painter(&maskImage);
    painter.setBrush(QBrush(Qt::magenta));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(imgPos, inpaintBrushSizeSlider->value() / 2, inpaintBrushSizeSlider->value() / 2);
    update();
}

void MyOpenGLWidget::toggleDepthRemovalMode(bool enabled) {
    depthRemovalMode = enabled;
    if (enabled) {
        setCursor(Qt::CrossCursor);
        if (selectedImage) {
            originalImage = selectedImage->image;  // Save the original image
        }
    } else {
        setCursor(Qt::ArrowCursor);
    }
    update();
}

// For depth background removal
void MyOpenGLWidget::adjustImage(int value) {
    if (!depthRemovalMode || !selectedImage) return;

    saveState();

    QImage grayscale = originalImage.convertToFormat(QImage::Format_Grayscale8);
    std::vector<std::pair<int, int>> pixels;

    for (int y = 0; y < grayscale.height(); ++y) {
        for (int x = 0; x < grayscale.width(); ++x) {
            int brightness = qGray(grayscale.pixel(x, y));
            pixels.emplace_back(brightness, y * grayscale.width() + x);
        }
    }

    std::sort(pixels.begin(), pixels.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        return a.first < b.first;
    });

    int numPixelsToRemove = static_cast<int>(pixels.size() * (value / 1000.0));
    QImage mask(originalImage.size(), QImage::Format_ARGB32);
    mask.fill(Qt::black);

    for (int i = 0; i < numPixelsToRemove; ++i) {
        int index = pixels[i].second;
        int x = index % grayscale.width();
        int y = index / grayscale.width();
        mask.setPixelColor(x, y, Qt::white);
    }

    QImage tempImage = originalImage;
    for (int y = 0; y < tempImage.height(); ++y) {
        for (int x = 0; x < tempImage.width(); ++x) {
            if (mask.pixelColor(x, y) == Qt::white) {
                tempImage.setPixelColor(x, y, Qt::transparent);
            }
        }
    }

    selectedImage->image = tempImage;
    selectedImage->boundingBox.setSize(tempImage.size());

    selectedImage->originalImage = selectedImage->image;
    update();
}

// void MyOpenGLWidget::oneshotRemoval() {
//     if (!selectedImage) return;

//     QImage originalImage = selectedImage->image;
//     QByteArray originalByteArray;
//     QBuffer originalBuffer(&originalByteArray);
//     originalImage.save(&originalBuffer, "PNG");
//     QString originalBase64 = originalByteArray.toBase64();

//     // Create JSON object to send to Python script
//     QJsonObject json;
//     json["original_image"] = QString::fromStdString(originalBase64.toStdString());

//     progressDialog = new QProgressDialog("Removing background...", "Cancel", 0, 0, this);
//     progressDialog->setWindowModality(Qt::WindowModal);
//     progressDialog->setCancelButton(nullptr);
//     progressDialog->show();

//     QJsonDocument doc(json);
//     QByteArray data = doc.toJson(QJsonDocument::Compact);

//     pythonProcess = new QProcess(this);
//     connect(pythonProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MyOpenGLWidget::handleOneshotRemovalResult);

//     // Set working directory if needed
//     try {
//         pythonProcess->setWorkingDirectory(projectRoot + "/resources/scripts/inference");
//     } catch (std::exception& e) {
//         qDebug() << "Failed to set working directory: " << e.what();
//     }
//     // pythonProcess->setWorkingDirectory("/resources/scripts/inference");

//     pythonProcess->start("python3.10", QStringList() << "oneshot-background-removal.py");
//     pythonProcess->write(data);
//     pythonProcess->closeWriteChannel();
// }

void MyOpenGLWidget::oneshotRemoval() {
    if (!selectedImage) return;

    saveState();

    QImage originalImage = selectedImage->image;
    QByteArray originalByteArray;
    QBuffer originalBuffer(&originalByteArray);
    originalImage.save(&originalBuffer, "PNG");
    QString originalBase64 = originalByteArray.toBase64();

    // Create JSON object to send to Python script
    QJsonObject json;
    json["original_image"] = QString::fromStdString(originalBase64.toStdString());

    progressDialog = new QProgressDialog("Removing background...", "Cancel", 0, 0, this);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setCancelButton(nullptr);
    progressDialog->show();

    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    pythonProcess = new QProcess(this);
    connect(pythonProcess, &QProcess::errorOccurred, this, &MyOpenGLWidget::handleProcessError);
    connect(pythonProcess, &QProcess::readyReadStandardOutput, this, &MyOpenGLWidget::handlePythonOutput);
    connect(pythonProcess, &QProcess::readyReadStandardError, this, &MyOpenGLWidget::handlePythonError);
    connect(pythonProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MyOpenGLWidget::handleOneshotRemovalResult);

    QString pythonExecutable = QDir(projectRoot).absoluteFilePath("local-image-editor-venv/Scripts/python.exe");

    QString scriptDir = QDir(projectRoot).absoluteFilePath("resources/scripts/inference");
    QString outputDir = QDir(projectRoot).absoluteFilePath("resources/scripts/inference");

    if (!QDir(scriptDir).exists()) {
        qDebug() << "Directory does not exist: " << scriptDir;
        progressDialog->hide();
        delete progressDialog;
        return;
    }

    pythonProcess->setWorkingDirectory(scriptDir);

    pythonProcess->start(pythonExecutable, QStringList() << "oneshot-background-removal.py" << outputDir);

    if (!pythonProcess->waitForStarted()) {
        qDebug() << "Failed to start Python process.";
        progressDialog->hide();
        delete progressDialog;
        return;
    }

    pythonProcess->write(data);
    pythonProcess->closeWriteChannel();

    qDebug() << "\n*** IF THIS IS YOUR FIRST TIME RUNNING ONE-SHOT REMOVAL, THE MODEL NEEDS TO BE DOWNLOADED. THIS MAY TAKE A FEW MINUTES. ***\n";

    if (!pythonProcess->waitForFinished(60000*5)) {
        qDebug() << "Python process did not finish within the expected time.";
        progressDialog->hide();
        delete progressDialog;
    }
}


void MyOpenGLWidget::handleOneshotRemovalResult() {
    if (!selectedImage) return;

    // Get the size of the selected image
    QSize originalSize = selectedImage->image.size();

    std::ifstream file((projectRoot + "/resources/scripts/inference/oneshot_removal_result.txt").toStdString(), std::ios::binary);
    if (!file.is_open()) {
        qDebug() << "Failed to open oneshot removal result file.";
        QMessageBox::critical(this, "Error", "Failed to open the oneshot removal result file.");
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string resultBase64 = buffer.str();
    file.close();

    QByteArray resultByteArray = QByteArray::fromBase64(QString::fromStdString(resultBase64).toUtf8());

    // Load the image from the QByteArray
    QPixmap resultImage;
    if (!resultImage.loadFromData(resultByteArray)) {
        qDebug() << "Failed to decode the image.";
        QMessageBox::critical(this, "Error", "Failed to decode the oneshot removal image.");
        return;
    }

    // Convert the QPixmap to QImage
    QImage resultQImage = resultImage.toImage();

    // Set the size of the inpainted image to the original size
    resultQImage = resultQImage.scaled(originalSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Replace the selected image with the result image
    selectedImage->image = resultQImage;
    selectedImage->boundingBox.setSize(resultQImage.size());

    selectedImage->originalImage = selectedImage->image;

    progressDialog->hide();
    delete progressDialog;

    update();

    // Delete the result file
    std::remove((projectRoot + "/resources/scripts/inference/oneshot_removal_result.txt").toStdString().c_str());
}

void MyOpenGLWidget::handlePythonOutput() {
    QByteArray output = pythonProcess->readAllStandardOutput();
    qDebug() << "Python Output:" << output;
}

void MyOpenGLWidget::handlePythonError() {
    QByteArray error = pythonProcess->readAllStandardError();
    qDebug() << "Python Error:" << error;
}

void MyOpenGLWidget::handleProcessError(QProcess::ProcessError error) {
    qDebug() << "Process error occurred:" << pythonProcess->errorString();
    QMessageBox::critical(this, "Error", "Python process failed: " + pythonProcess->errorString());
    progressDialog->hide();
    delete progressDialog;
}

QRect MyOpenGLWidget::computeBoundingBoxForSelectedImages() {
    if (selectedImages.empty()) return QRect();

    QRect combinedBoundingBox = selectedImages.front()->boundingBox;
    for (const auto& img : selectedImages) {
        combinedBoundingBox = combinedBoundingBox.united(img->boundingBox);
    }
    return combinedBoundingBox;
}

void MyOpenGLWidget::clearSelection() {
    // Disable any active modes before clearing selection
    toggleCropMode(false);
    toggleRotationMode(false);
    toggleEraserMode(false);
    toggleInpaintMode(false);
    toggleSnipeMode(false);
    toggleDepthRemovalMode(false);

    // Untoggle toolbar buttons without emitting signals
    if (toolbar) {
        toolbar->setUntoggledActions();
    }

    // Clear the selection of all images
    for (auto& img : images) {
        img.isSelected = false;
        img.enableBoundingBox();
    }
    selectedImages.clear();
    selectedImage = nullptr;

    update();
}

void MyOpenGLWidget::selectImagesInBox(const QRect& box) {
    selectedImages.clear();
    for (auto& img : images) {
        if (box.intersects(img.boundingBox)) {
            img.isSelected = true;
            selectedImages.push_back(&img);
        } else {
            img.isSelected = false;
        }
    }
    if (!selectedImages.empty()) {
        selectedImage = nullptr;
        for (auto& img : selectedImages) {
            img->disableBoundingBox();
        }
    } else {
        selectedImage = nullptr;
    }
    update();
}


void MyOpenGLWidget::mergeSelectedImages() {
    if (selectedImages.size() < 2) return;

    // Save the current state before making changes
    saveState();

    // Compute the bounding box that encompasses all selected images
    QRect boundingBox = computeBoundingBoxForSelectedImages();
    QImage mergedImage(boundingBox.size(), QImage::Format_ARGB32);
    mergedImage.fill(Qt::transparent);

    // Sort selected images based on their order in the `images` list, which represents layer order
    std::vector<ImageObject*> sortedSelectedImages = selectedImages;
    std::sort(sortedSelectedImages.begin(), sortedSelectedImages.end(), [&](ImageObject* a, ImageObject* b) {
        return std::find(images.begin(), images.end(), *a) < std::find(images.begin(), images.end(), *b);
    });

    // Draw the selected images onto the merged image, adjusted to their relative positions
    QPainter painter(&mergedImage);
    for (auto& img : sortedSelectedImages) {
        QRect targetRect = img->boundingBox.translated(-boundingBox.topLeft());
        painter.drawImage(targetRect, img->image);
    }

    // Create a new ImageObject for the merged image
    ImageObject newMergedImage(mergedImage, boundingBox.topLeft() + QPoint(boundingBox.width() / 2, boundingBox.height() / 2));

    // Remove the selected images from the images list
    for (auto it = images.begin(); it != images.end(); ) {
        if (std::find(sortedSelectedImages.begin(), sortedSelectedImages.end(), &(*it)) != sortedSelectedImages.end()) {
            it = images.erase(it);
        } else {
            ++it;
        }
    }

    // Add the new merged image to the images list and select it
    images.push_back(newMergedImage);
    clearSelection();
    selectedImage = &images.back();
    selectedImage->isSelected = true;

    selectedImage->originalImage = selectedImage->image;

    // Update the widget to reflect the changes
    update();
}


#ifndef MYOPENGLWIDGET_H
#define MYOPENGLWIDGET_H

#include <QOpenGLWidget>
#include "ImageToolbar.h"
#include "ImageObject.h"
#include "CustomConfirmationDialog.h"
#include <vector>
#include <QSlider>
#include <QPushButton>
#include <QStack>
#include <QLineEdit>
#include <QWidget>
#include <QLabel>
#include <QProcess>
#include <QProgressDialog>
#include <QPointF>
#include <QClipboard>
#include <QApplication>
#include <QMenu>
#include <QComboBox>
#include <QColorDialog>
#include <QDialog>
#include <QCheckBox>

class MyOpenGLWidget : public QOpenGLWidget {
    Q_OBJECT

private:
    QString projectRoot;  // Root directory of the project

    const int MAX_IMAGE_WIDTH = 512;
    const int MAX_IMAGE_HEIGHT = 512;
    std::vector<ImageObject> images;  // List of images in the widget
    QPoint scrollPosition;  // Current scroll position
    QPoint lastMousePosition;  // Last mouse position
    bool isDragging;  // Flag indicating if dragging is in progress
    ImageObject* selectedImage = nullptr;  // Currently selected image
    std::vector<ImageObject*> selectedImages; // Multi-selected images
    QRect selectionBox; // Selection box for click-and-drag multi-select
    bool isSelecting = false; // Flag indicating if selection box is being drawn
    QPoint selectionStartPoint; // Start point for selection box
    QPoint selectionEndPoint; // End point for selection box
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
    QImage originalImage;
    QImage originalImageBeforeRotation;
    CustomConfirmationDialog* confirmationDialog;
    bool snipeMode = false;  // Flag indicating if snipe mode is enabled
    std::vector<QPointF> positivePoints;  // Positive points for snipe mode
    std::vector<QPointF> negativePoints;  // Negative points for snipe mode
    QWidget* snipePopup;  // Popup widget for snipe mode
    QPushButton* confirmSnipeButton;  // Confirm snipe button
    QPushButton* clearSnipeButton;  // Clear snipe button
    bool depthRemovalMode = false;
    QSlider* depthRemovalSlider;
    bool rotationMode = false;  // Flag indicating if rotation mode is enabled
    QSlider* rotationSlider;  // Slider for rotation

    // Inpainting popup elements
    QWidget* inpaintPopup;
    QLabel* inpaintPromptLabel;
    QLineEdit* inpaintTextBox;
    QPushButton* confirmInpaintButton;
    QPushButton* cancelInpaintButton;
    QSlider* inpaintBrushSizeSlider;

    // Shape menu elements
    QDialog* shapeMenu;
    QPushButton* addShapeButton;
    QLabel* shapeLabel;
    QComboBox* shapeSelector;
    QPushButton* colorButton;
    QColorDialog* colorDialog;
    QPushButton* createShapeButton;
    QColor selectedColor;

    // AI image generation elements
    QPushButton* generateAIImageButton;
    QWidget* generateAIPopup;
    QLineEdit* apiKeyTextBox;
    QLineEdit* promptTextBox;
    QCheckBox* useEnvVarCheckBox;
    QCheckBox* useOpenAICheckBox;
    QPushButton* confirmGenerateAIButton;
    QPushButton* cancelGenerateAIButton;

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
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    void toggleRotationMode(bool enabled);
    void rotateSelectedImage(int angle);
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
    void toggleSnipeMode(bool enabled);
    void confirmSnipe();
    void clearSnipePoints();
    void handleSnipeResult();
    void toggleDepthRemovalMode(bool enabled);
    void adjustImage(int value);
    void oneshotRemoval();
    void handleOneshotRemovalResult();
    void copyImageToClipboard();
    void pasteImageFromClipboard();
    void mergeSelectedImages();

    // Slots for shape menu
    void openShapeMenu();
    void openColorDialog();
    void createShape();

    // Slots for AI Image Generation
    void openGenerateAIMenu();
    void toggleAPIKeyInput(bool enabled);
    void confirmGenerateAIImage();
    void handleGeneratedAIImage();

private:
    void eraseAt(const QPoint& pos);
    void saveState();
    void undo();
    void redo();
    void adjustCropBox(const QPoint& delta);
    int cropHandleAt(const QPoint& pos) const;
    void drawMaskAt(const QPoint& pos);
    void drawSnipePoints(QPainter& painter, const QPoint& scrollPosition);
    QRect computeBoundingBoxForSelectedImages();
    void selectImagesInBox(const QRect& box);
    void clearSelection();
    void handleProcessError(QProcess::ProcessError error);
    void handlePythonOutput();
    void handlePythonError();
};

#endif // MYOPENGLWIDGET_H

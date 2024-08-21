#ifndef IMAGEOBJECT_H
#define IMAGEOBJECT_H

#include <QImage>
#include <QRect>
#include <QPainter>

class ImageObject {
public:
    QImage image;
    QImage originalImage; // Store the original image to prevent loss of quality during resizing
    QImage originalImageBeforeRotation;
    QRect boundingBox;
    bool isSelected;
    bool boundingBoxEnabled;
    int currentRotationAngle;
    static const int HANDLE_SIZE = 10;

    ImageObject(const QImage& img, const QPoint& pos) : image(img), originalImage(img), currentRotationAngle(0), isSelected(false), boundingBoxEnabled(true) {
        boundingBox.setSize(img.size());
        boundingBox.moveCenter(pos);
    }

    // Draw the image and its handles if selected and boundingBoxEnabled
    void draw(QPainter& painter, const QPoint& scrollPosition) {
        QRect adjustedBox = boundingBox.translated(scrollPosition);
        painter.drawImage(adjustedBox, image);
        if (isSelected && boundingBoxEnabled) {
            painter.setPen(QPen(Qt::magenta, 2, Qt::DashLine));  // Highlight color and style
            painter.drawRect(adjustedBox);
            drawHandles(painter, adjustedBox);
        }
    }

    // Draw resize handles on the image
    void drawHandles(QPainter& painter, const QRect& rect) {
        painter.setBrush(Qt::white);
        painter.setPen(Qt::black);
        int halfSize = HANDLE_SIZE / 2;
        painter.drawRect(QRect(rect.topLeft() - QPoint(halfSize, halfSize), QSize(HANDLE_SIZE, HANDLE_SIZE)));
        painter.drawRect(QRect(rect.topRight() - QPoint(halfSize, halfSize), QSize(HANDLE_SIZE, HANDLE_SIZE)));
        painter.drawRect(QRect(rect.bottomLeft() - QPoint(halfSize, halfSize), QSize(HANDLE_SIZE, HANDLE_SIZE)));
        painter.drawRect(QRect(rect.bottomRight() - QPoint(halfSize, halfSize), QSize(HANDLE_SIZE, HANDLE_SIZE)));
    }

    // Check if the image contains a specific point
    bool contains(const QPoint& pos, const QPoint& scrollPosition) const {
        return boundingBox.translated(scrollPosition).contains(pos);
    }

    // Check if a handle contains a specific point and return the handle index
    int handleAt(const QPoint& pos, const QPoint& scrollPosition) const {
        if (!boundingBoxEnabled) return 0;
        QRect adjustedBox = boundingBox.translated(scrollPosition);
        if (QRect(adjustedBox.topLeft() - QPoint(HANDLE_SIZE / 2, HANDLE_SIZE / 2), QSize(HANDLE_SIZE, HANDLE_SIZE)).contains(pos))
            return 1;
        if (QRect(adjustedBox.topRight() - QPoint(HANDLE_SIZE / 2, HANDLE_SIZE / 2), QSize(HANDLE_SIZE, HANDLE_SIZE)).contains(pos))
            return 2;
        if (QRect(adjustedBox.bottomLeft() - QPoint(HANDLE_SIZE / 2, HANDLE_SIZE / 2), QSize(HANDLE_SIZE, HANDLE_SIZE)).contains(pos))
            return 3;
        if (QRect(adjustedBox.bottomRight() - QPoint(HANDLE_SIZE / 2, HANDLE_SIZE / 2), QSize(HANDLE_SIZE, HANDLE_SIZE)).contains(pos))
            return 4;
        return 0;
    }

    // Disable the bounding box, making it invisible and the handles non-interactive
    void disableBoundingBox() {
        boundingBoxEnabled = false;
    }

    // Enable the bounding box, making it visible and the handles interactive
    void enableBoundingBox() {
        boundingBoxEnabled = true;
    }

    // Equality operator for comparing image objects
    bool operator==(const ImageObject& other) const {
        return (this->image == other.image &&
                this->boundingBox == other.boundingBox &&
                this->isSelected == other.isSelected);
    }
};

#endif // IMAGEOBJECT_H

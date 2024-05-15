// #ifndef IMAGEOBJECT_H
// #define IMAGEOBJECT_H

// #include <QImage>
// #include <QRect>
// #include <QPainter>

// class ImageObject {
// public:
//     QImage image;
//     QRect boundingBox;
//     bool isSelected;
//     static const int HANDLE_SIZE = 10;

//     ImageObject(const QImage& img, const QPoint& pos) : image(img), isSelected(false) {
//         boundingBox.setSize(img.size());
//         boundingBox.moveCenter(pos);
//     }

//     void draw(QPainter& painter, const QPoint& scrollPosition) {
//         QRect adjustedBox = boundingBox.translated(scrollPosition);
//         painter.drawImage(adjustedBox, image);
//         if (isSelected) {
//             painter.setPen(QPen(Qt::magenta, 2, Qt::DashLine));  // Highlight color and style
//             painter.drawRect(adjustedBox);
//             drawHandles(painter, adjustedBox);
//         }
//     }

//     void drawHandles(QPainter& painter, const QRect& rect) {
//         painter.setBrush(Qt::white);
//         painter.setPen(Qt::black);
//         int halfSize = HANDLE_SIZE / 2;
//         painter.drawRect(rect.topLeft().x() - halfSize, rect.topLeft().y() - halfSize, HANDLE_SIZE, HANDLE_SIZE);
//         painter.drawRect(rect.topRight().x() - halfSize, rect.topRight().y() - halfSize, HANDLE_SIZE, HANDLE_SIZE);
//         painter.drawRect(rect.bottomLeft().x() - halfSize, rect.bottomLeft().y() - halfSize, HANDLE_SIZE, HANDLE_SIZE);
//         painter.drawRect(rect.bottomRight().x() - halfSize, rect.bottomRight().y() - halfSize, HANDLE_SIZE, HANDLE_SIZE);
//     }

//     bool contains(const QPoint& pos, const QPoint& scrollPosition) {
//         return boundingBox.translated(scrollPosition).contains(pos);
//     }

//     int handleAt(const QPoint& pos, const QPoint& scrollPosition) {
//         QRect adjustedBox = boundingBox.translated(scrollPosition);
//         if (QRect(adjustedBox.topLeft() - QPoint(HANDLE_SIZE / 2, HANDLE_SIZE / 2), QSize(HANDLE_SIZE, HANDLE_SIZE)).contains(pos))
//             return 1;
//         if (QRect(adjustedBox.topRight() - QPoint(HANDLE_SIZE / 2, HANDLE_SIZE / 2), QSize(HANDLE_SIZE, HANDLE_SIZE)).contains(pos))
//             return 2;
//         if (QRect(adjustedBox.bottomLeft() - QPoint(HANDLE_SIZE / 2, HANDLE_SIZE / 2), QSize(HANDLE_SIZE, HANDLE_SIZE)).contains(pos))
//             return 3;
//         if (QRect(adjustedBox.bottomRight() - QPoint(HANDLE_SIZE / 2, HANDLE_SIZE / 2), QSize(HANDLE_SIZE, HANDLE_SIZE)).contains(pos))
//             return 4;
//         return 0;
//     }

//     bool operator==(const ImageObject& other) const {
//         return (this->image == other.image &&
//                 this->boundingBox == other.boundingBox &&
//                 this->isSelected == other.isSelected);
//     }
// };

// #endif // IMAGEOBJECT_H

#ifndef IMAGEOBJECT_H
#define IMAGEOBJECT_H

#include <QImage>
#include <QRect>
#include <QPainter>

// Class representing an image object with handles for resizing and moving
class ImageObject {
public:
    QImage image;  // The image data
    QRect boundingBox;  // The bounding box of the image
    bool isSelected;  // Whether the image is currently selected
    static const int HANDLE_SIZE = 10;  // Size of the handles for resizing

    // Constructor: Initialize the image object
    ImageObject(const QImage& img, const QPoint& pos) : image(img), isSelected(false) {
        boundingBox.setSize(img.size());
        boundingBox.moveCenter(pos);
    }

    // Draw the image and its handles if selected
    void draw(QPainter& painter, const QPoint& scrollPosition) {
        QRect adjustedBox = boundingBox.translated(scrollPosition);
        painter.drawImage(adjustedBox, image);
        if (isSelected) {
            painter.setPen(QPen(Qt::magenta, 2, Qt::DashLine));  // Highlight color and style
            painter.drawRect(adjustedBox);
            drawHandles(painter, adjustedBox);
        }
    }

    // Draw the handles for resizing the image
    void drawHandles(QPainter& painter, const QRect& rect) {
        painter.setBrush(Qt::white);
        painter.setPen(Qt::black);
        int halfSize = HANDLE_SIZE / 2;
        painter.drawRect(QRect(rect.topLeft() - QPoint(halfSize, halfSize), QSize(HANDLE_SIZE, HANDLE_SIZE)));
        painter.drawRect(QRect(rect.topRight() - QPoint(halfSize, halfSize), QSize(HANDLE_SIZE, HANDLE_SIZE)));
        painter.drawRect(QRect(rect.bottomLeft() - QPoint(halfSize, halfSize), QSize(HANDLE_SIZE, HANDLE_SIZE)));
        painter.drawRect(QRect(rect.bottomRight() - QPoint(halfSize, halfSize), QSize(HANDLE_SIZE, HANDLE_SIZE)));
    }

    // Check if a point is within the bounding box of the image
    bool contains(const QPoint& pos, const QPoint& scrollPosition) const {
        return boundingBox.translated(scrollPosition).contains(pos);
    }

    // Check if a point is within any of the handles for resizing the image
    int handleAt(const QPoint& pos, const QPoint& scrollPosition) const {
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

    // Overload the equality operator to compare two ImageObject instances
    bool operator==(const ImageObject& other) const {
        return (this->image == other.image &&
                this->boundingBox == other.boundingBox &&
                this->isSelected == other.isSelected);
    }
};

#endif // IMAGEOBJECT_H


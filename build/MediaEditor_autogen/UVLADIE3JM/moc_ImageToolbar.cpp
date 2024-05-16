/****************************************************************************
** Meta object code from reading C++ file 'ImageToolbar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ImageToolbar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ImageToolbar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ImageToolbar_t {
    QByteArrayData data[14];
    char stringdata0[160];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ImageToolbar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ImageToolbar_t qt_meta_stringdata_ImageToolbar = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ImageToolbar"
QT_MOC_LITERAL(1, 13, 11), // "rotateImage"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 11), // "mirrorImage"
QT_MOC_LITERAL(4, 38, 9), // "copyImage"
QT_MOC_LITERAL(5, 48, 11), // "deleteImage"
QT_MOC_LITERAL(6, 60, 9), // "saveImage"
QT_MOC_LITERAL(7, 70, 12), // "toggleEraser"
QT_MOC_LITERAL(8, 83, 7), // "enabled"
QT_MOC_LITERAL(9, 91, 17), // "eraserSizeChanged"
QT_MOC_LITERAL(10, 109, 4), // "size"
QT_MOC_LITERAL(11, 114, 12), // "bringToFront"
QT_MOC_LITERAL(12, 127, 10), // "pushToBack"
QT_MOC_LITERAL(13, 138, 21) // "onEraserButtonClicked"

    },
    "ImageToolbar\0rotateImage\0\0mirrorImage\0"
    "copyImage\0deleteImage\0saveImage\0"
    "toggleEraser\0enabled\0eraserSizeChanged\0"
    "size\0bringToFront\0pushToBack\0"
    "onEraserButtonClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ImageToolbar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,
       3,    0,   65,    2, 0x06 /* Public */,
       4,    0,   66,    2, 0x06 /* Public */,
       5,    0,   67,    2, 0x06 /* Public */,
       6,    0,   68,    2, 0x06 /* Public */,
       7,    1,   69,    2, 0x06 /* Public */,
       9,    1,   72,    2, 0x06 /* Public */,
      11,    0,   75,    2, 0x06 /* Public */,
      12,    0,   76,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    0,   77,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void ImageToolbar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ImageToolbar *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->rotateImage(); break;
        case 1: _t->mirrorImage(); break;
        case 2: _t->copyImage(); break;
        case 3: _t->deleteImage(); break;
        case 4: _t->saveImage(); break;
        case 5: _t->toggleEraser((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->eraserSizeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->bringToFront(); break;
        case 8: _t->pushToBack(); break;
        case 9: _t->onEraserButtonClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ImageToolbar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageToolbar::rotateImage)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ImageToolbar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageToolbar::mirrorImage)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ImageToolbar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageToolbar::copyImage)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ImageToolbar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageToolbar::deleteImage)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ImageToolbar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageToolbar::saveImage)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ImageToolbar::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageToolbar::toggleEraser)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ImageToolbar::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageToolbar::eraserSizeChanged)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ImageToolbar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageToolbar::bringToFront)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (ImageToolbar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageToolbar::pushToBack)) {
                *result = 8;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ImageToolbar::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ImageToolbar.data,
    qt_meta_data_ImageToolbar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ImageToolbar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ImageToolbar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ImageToolbar.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ImageToolbar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void ImageToolbar::rotateImage()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ImageToolbar::mirrorImage()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ImageToolbar::copyImage()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ImageToolbar::deleteImage()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ImageToolbar::saveImage()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ImageToolbar::toggleEraser(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ImageToolbar::eraserSizeChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ImageToolbar::bringToFront()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void ImageToolbar::pushToBack()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

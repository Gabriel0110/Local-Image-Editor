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
    QByteArrayData data[16];
    char stringdata0[213];
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
QT_MOC_LITERAL(9, 91, 12), // "bringToFront"
QT_MOC_LITERAL(10, 104, 10), // "pushToBack"
QT_MOC_LITERAL(11, 115, 14), // "toggleCropMode"
QT_MOC_LITERAL(12, 130, 17), // "toggleInpaintMode"
QT_MOC_LITERAL(13, 148, 21), // "onEraserButtonClicked"
QT_MOC_LITERAL(14, 170, 19), // "onCropButtonClicked"
QT_MOC_LITERAL(15, 190, 22) // "onInpaintButtonClicked"

    },
    "ImageToolbar\0rotateImage\0\0mirrorImage\0"
    "copyImage\0deleteImage\0saveImage\0"
    "toggleEraser\0enabled\0bringToFront\0"
    "pushToBack\0toggleCropMode\0toggleInpaintMode\0"
    "onEraserButtonClicked\0onCropButtonClicked\0"
    "onInpaintButtonClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ImageToolbar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x06 /* Public */,
       3,    0,   80,    2, 0x06 /* Public */,
       4,    0,   81,    2, 0x06 /* Public */,
       5,    0,   82,    2, 0x06 /* Public */,
       6,    0,   83,    2, 0x06 /* Public */,
       7,    1,   84,    2, 0x06 /* Public */,
       9,    0,   87,    2, 0x06 /* Public */,
      10,    0,   88,    2, 0x06 /* Public */,
      11,    1,   89,    2, 0x06 /* Public */,
      12,    1,   92,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    0,   95,    2, 0x08 /* Private */,
      14,    0,   96,    2, 0x08 /* Private */,
      15,    0,   97,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::Bool,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
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
        case 6: _t->bringToFront(); break;
        case 7: _t->pushToBack(); break;
        case 8: _t->toggleCropMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->toggleInpaintMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->onEraserButtonClicked(); break;
        case 11: _t->onCropButtonClicked(); break;
        case 12: _t->onInpaintButtonClicked(); break;
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
            using _t = void (ImageToolbar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageToolbar::bringToFront)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ImageToolbar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageToolbar::pushToBack)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (ImageToolbar::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageToolbar::toggleCropMode)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (ImageToolbar::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageToolbar::toggleInpaintMode)) {
                *result = 9;
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
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
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
void ImageToolbar::bringToFront()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void ImageToolbar::pushToBack()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void ImageToolbar::toggleCropMode(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void ImageToolbar::toggleInpaintMode(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

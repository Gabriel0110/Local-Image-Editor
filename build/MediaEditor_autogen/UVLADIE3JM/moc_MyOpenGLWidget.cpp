/****************************************************************************
** Meta object code from reading C++ file 'MyOpenGLWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/MyOpenGLWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MyOpenGLWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MyOpenGLWidget_t {
    QByteArrayData data[18];
    char stringdata0[277];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MyOpenGLWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MyOpenGLWidget_t qt_meta_stringdata_MyOpenGLWidget = {
    {
QT_MOC_LITERAL(0, 0, 14), // "MyOpenGLWidget"
QT_MOC_LITERAL(1, 15, 19), // "rotateSelectedImage"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 19), // "mirrorSelectedImage"
QT_MOC_LITERAL(4, 56, 17), // "copySelectedImage"
QT_MOC_LITERAL(5, 74, 19), // "deleteSelectedImage"
QT_MOC_LITERAL(6, 94, 17), // "saveSelectedImage"
QT_MOC_LITERAL(7, 112, 16), // "toggleEraserMode"
QT_MOC_LITERAL(8, 129, 7), // "enabled"
QT_MOC_LITERAL(9, 137, 12), // "bringToFront"
QT_MOC_LITERAL(10, 150, 10), // "pushToBack"
QT_MOC_LITERAL(11, 161, 14), // "toggleCropMode"
QT_MOC_LITERAL(12, 176, 17), // "toggleInpaintMode"
QT_MOC_LITERAL(13, 194, 14), // "confirmInpaint"
QT_MOC_LITERAL(14, 209, 19), // "handleInpaintResult"
QT_MOC_LITERAL(15, 229, 18), // "handleInpaintError"
QT_MOC_LITERAL(16, 248, 22), // "QProcess::ProcessError"
QT_MOC_LITERAL(17, 271, 5) // "error"

    },
    "MyOpenGLWidget\0rotateSelectedImage\0\0"
    "mirrorSelectedImage\0copySelectedImage\0"
    "deleteSelectedImage\0saveSelectedImage\0"
    "toggleEraserMode\0enabled\0bringToFront\0"
    "pushToBack\0toggleCropMode\0toggleInpaintMode\0"
    "confirmInpaint\0handleInpaintResult\0"
    "handleInpaintError\0QProcess::ProcessError\0"
    "error"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MyOpenGLWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x08 /* Private */,
       3,    0,   80,    2, 0x08 /* Private */,
       4,    0,   81,    2, 0x08 /* Private */,
       5,    0,   82,    2, 0x08 /* Private */,
       6,    0,   83,    2, 0x08 /* Private */,
       7,    1,   84,    2, 0x08 /* Private */,
       9,    0,   87,    2, 0x08 /* Private */,
      10,    0,   88,    2, 0x08 /* Private */,
      11,    1,   89,    2, 0x08 /* Private */,
      12,    1,   92,    2, 0x08 /* Private */,
      13,    0,   95,    2, 0x08 /* Private */,
      14,    0,   96,    2, 0x08 /* Private */,
      15,    1,   97,    2, 0x08 /* Private */,

 // slots: parameters
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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16,   17,

       0        // eod
};

void MyOpenGLWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MyOpenGLWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->rotateSelectedImage(); break;
        case 1: _t->mirrorSelectedImage(); break;
        case 2: _t->copySelectedImage(); break;
        case 3: _t->deleteSelectedImage(); break;
        case 4: _t->saveSelectedImage(); break;
        case 5: _t->toggleEraserMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->bringToFront(); break;
        case 7: _t->pushToBack(); break;
        case 8: _t->toggleCropMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->toggleInpaintMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->confirmInpaint(); break;
        case 11: _t->handleInpaintResult(); break;
        case 12: _t->handleInpaintError((*reinterpret_cast< QProcess::ProcessError(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MyOpenGLWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QOpenGLWidget::staticMetaObject>(),
    qt_meta_stringdata_MyOpenGLWidget.data,
    qt_meta_data_MyOpenGLWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MyOpenGLWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MyOpenGLWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MyOpenGLWidget.stringdata0))
        return static_cast<void*>(this);
    return QOpenGLWidget::qt_metacast(_clname);
}

int MyOpenGLWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QOpenGLWidget::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE

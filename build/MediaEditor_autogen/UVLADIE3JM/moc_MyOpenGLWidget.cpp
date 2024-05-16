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
    QByteArrayData data[13];
    char stringdata0[183];
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
QT_MOC_LITERAL(9, 137, 16), // "updateEraserSize"
QT_MOC_LITERAL(10, 154, 4), // "size"
QT_MOC_LITERAL(11, 159, 12), // "bringToFront"
QT_MOC_LITERAL(12, 172, 10) // "pushToBack"

    },
    "MyOpenGLWidget\0rotateSelectedImage\0\0"
    "mirrorSelectedImage\0copySelectedImage\0"
    "deleteSelectedImage\0saveSelectedImage\0"
    "toggleEraserMode\0enabled\0updateEraserSize\0"
    "size\0bringToFront\0pushToBack"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MyOpenGLWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x08 /* Private */,
       3,    0,   60,    2, 0x08 /* Private */,
       4,    0,   61,    2, 0x08 /* Private */,
       5,    0,   62,    2, 0x08 /* Private */,
       6,    0,   63,    2, 0x08 /* Private */,
       7,    1,   64,    2, 0x08 /* Private */,
       9,    1,   67,    2, 0x08 /* Private */,
      11,    0,   70,    2, 0x08 /* Private */,
      12,    0,   71,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void,
    QMetaType::Void,

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
        case 6: _t->updateEraserSize((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->bringToFront(); break;
        case 8: _t->pushToBack(); break;
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
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[20];
    char stringdata0[178];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 6), // "newTab"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 4), // "path"
QT_MOC_LITERAL(4, 24, 10), // "newTabHere"
QT_MOC_LITERAL(5, 35, 8), // "closeTab"
QT_MOC_LITERAL(6, 44, 5), // "index"
QT_MOC_LITERAL(7, 50, 9), // "goAddress"
QT_MOC_LITERAL(8, 60, 11), // "onNewFolder"
QT_MOC_LITERAL(9, 72, 8), // "onRename"
QT_MOC_LITERAL(10, 81, 8), // "onDelete"
QT_MOC_LITERAL(11, 90, 6), // "onCopy"
QT_MOC_LITERAL(12, 97, 5), // "onCut"
QT_MOC_LITERAL(13, 103, 7), // "onPaste"
QT_MOC_LITERAL(14, 111, 9), // "onRefresh"
QT_MOC_LITERAL(15, 121, 4), // "onUp"
QT_MOC_LITERAL(16, 126, 6), // "onBack"
QT_MOC_LITERAL(17, 133, 9), // "onForward"
QT_MOC_LITERAL(18, 143, 10), // "showConfig"
QT_MOC_LITERAL(19, 154, 23) // "updateNavigationButtons"

    },
    "MainWindow\0newTab\0\0path\0newTabHere\0"
    "closeTab\0index\0goAddress\0onNewFolder\0"
    "onRename\0onDelete\0onCopy\0onCut\0onPaste\0"
    "onRefresh\0onUp\0onBack\0onForward\0"
    "showConfig\0updateNavigationButtons"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   99,    2, 0x08 /* Private */,
       1,    0,  102,    2, 0x28 /* Private | MethodCloned */,
       4,    0,  103,    2, 0x08 /* Private */,
       5,    1,  104,    2, 0x08 /* Private */,
       7,    0,  107,    2, 0x08 /* Private */,
       8,    0,  108,    2, 0x08 /* Private */,
       9,    0,  109,    2, 0x08 /* Private */,
      10,    0,  110,    2, 0x08 /* Private */,
      11,    0,  111,    2, 0x08 /* Private */,
      12,    0,  112,    2, 0x08 /* Private */,
      13,    0,  113,    2, 0x08 /* Private */,
      14,    0,  114,    2, 0x08 /* Private */,
      15,    0,  115,    2, 0x08 /* Private */,
      16,    0,  116,    2, 0x08 /* Private */,
      17,    0,  117,    2, 0x08 /* Private */,
      18,    0,  118,    2, 0x08 /* Private */,
      19,    0,  119,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->newTab((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->newTab(); break;
        case 2: _t->newTabHere(); break;
        case 3: _t->closeTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->goAddress(); break;
        case 5: _t->onNewFolder(); break;
        case 6: _t->onRename(); break;
        case 7: _t->onDelete(); break;
        case 8: _t->onCopy(); break;
        case 9: _t->onCut(); break;
        case 10: _t->onPaste(); break;
        case 11: _t->onRefresh(); break;
        case 12: _t->onUp(); break;
        case 13: _t->onBack(); break;
        case 14: _t->onForward(); break;
        case 15: _t->showConfig(); break;
        case 16: _t->updateNavigationButtons(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'filetab.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/filetab.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filetab.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FileTab_t {
    QByteArrayData data[20];
    char stringdata0[213];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FileTab_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FileTab_t qt_meta_stringdata_FileTab = {
    {
QT_MOC_LITERAL(0, 0, 7), // "FileTab"
QT_MOC_LITERAL(1, 8, 15), // "requestOpenPath"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 4), // "path"
QT_MOC_LITERAL(4, 30, 13), // "statusMessage"
QT_MOC_LITERAL(5, 44, 3), // "msg"
QT_MOC_LITERAL(6, 48, 17), // "onTreeItemClicked"
QT_MOC_LITERAL(7, 66, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(8, 83, 4), // "item"
QT_MOC_LITERAL(9, 88, 6), // "column"
QT_MOC_LITERAL(10, 95, 18), // "onTreeItemExpanded"
QT_MOC_LITERAL(11, 114, 17), // "onTreeContextMenu"
QT_MOC_LITERAL(12, 132, 3), // "pos"
QT_MOC_LITERAL(13, 136, 13), // "onDoubleClick"
QT_MOC_LITERAL(14, 150, 11), // "QModelIndex"
QT_MOC_LITERAL(15, 162, 5), // "index"
QT_MOC_LITERAL(16, 168, 11), // "contextMenu"
QT_MOC_LITERAL(17, 180, 11), // "startSearch"
QT_MOC_LITERAL(18, 192, 14), // "changeIconSize"
QT_MOC_LITERAL(19, 207, 5) // "value"

    },
    "FileTab\0requestOpenPath\0\0path\0"
    "statusMessage\0msg\0onTreeItemClicked\0"
    "QTreeWidgetItem*\0item\0column\0"
    "onTreeItemExpanded\0onTreeContextMenu\0"
    "pos\0onDoubleClick\0QModelIndex\0index\0"
    "contextMenu\0startSearch\0changeIconSize\0"
    "value"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FileTab[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       4,    1,   62,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    2,   65,    2, 0x08 /* Private */,
      10,    1,   70,    2, 0x08 /* Private */,
      11,    1,   73,    2, 0x08 /* Private */,
      13,    1,   76,    2, 0x08 /* Private */,
      16,    1,   79,    2, 0x08 /* Private */,
      17,    0,   82,    2, 0x08 /* Private */,
      18,    1,   83,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 7, QMetaType::Int,    8,    9,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, QMetaType::QPoint,   12,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, QMetaType::QPoint,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   19,

       0        // eod
};

void FileTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileTab *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->requestOpenPath((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->statusMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->onTreeItemClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->onTreeItemExpanded((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 4: _t->onTreeContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 5: _t->onDoubleClick((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 6: _t->contextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 7: _t->startSearch(); break;
        case 8: _t->changeIconSize((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileTab::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileTab::requestOpenPath)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FileTab::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileTab::statusMessage)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject FileTab::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_FileTab.data,
    qt_meta_data_FileTab,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *FileTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileTab::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FileTab.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int FileTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void FileTab::requestOpenPath(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FileTab::statusMessage(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

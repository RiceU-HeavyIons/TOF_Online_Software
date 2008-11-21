/****************************************************************************
** Meta object code from reading C++ file 'MyFrame.h'
**
** Created: Thu Nov 20 20:40:21 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MyFrame.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MyFrame.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MyFrame[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      11,    9,    8,    8, 0x08,
      24,    9,    8,    8, 0x08,
      42,    9,    8,    8, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MyFrame[] = {
    "MyFrame\0\0i\0setMode(int)\0setProgress1(int)\0"
    "setProgress2(int)\0"
};

const QMetaObject MyFrame::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_MyFrame,
      qt_meta_data_MyFrame, 0 }
};

const QMetaObject *MyFrame::metaObject() const
{
    return &staticMetaObject;
}

void *MyFrame::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MyFrame))
        return static_cast<void*>(const_cast< MyFrame*>(this));
    return QFrame::qt_metacast(_clname);
}

int MyFrame::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: setProgress1((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: setProgress2((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'KMainWindow.h'
**
** Created: Fri Nov 21 04:34:18 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "KMainWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KMainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KMainWindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x0a,
      23,   12,   12,   12, 0x0a,
      32,   12,   12,   12, 0x0a,
      52,   50,   12,   12, 0x0a,
      65,   50,   12,   12, 0x0a,
      83,   50,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_KMainWindow[] = {
    "KMainWindow\0\0doReset()\0doSync()\0"
    "doToggleToolbar()\0i\0setMode(int)\0"
    "setProgress1(int)\0setProgress2(int)\0"
};

const QMetaObject KMainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_KMainWindow,
      qt_meta_data_KMainWindow, 0 }
};

const QMetaObject *KMainWindow::metaObject() const
{
    return &staticMetaObject;
}

void *KMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KMainWindow))
        return static_cast<void*>(const_cast< KMainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int KMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: doReset(); break;
        case 1: doSync(); break;
        case 2: doToggleToolbar(); break;
        case 3: setMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: setProgress1((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: setProgress2((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

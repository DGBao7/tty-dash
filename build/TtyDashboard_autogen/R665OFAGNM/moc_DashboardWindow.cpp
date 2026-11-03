/****************************************************************************
** Meta object code from reading C++ file 'DashboardWindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.12.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/window/DashboardWindow.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DashboardWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.12.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN7ttydash15DashboardWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto ttydash::DashboardWindow::qt_create_metaobjectdata<qt_meta_tag_ZN7ttydash15DashboardWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ttydash::DashboardWindow",
        "widget_geometry_changed",
        "",
        "display_name",
        "widget_id",
        "QRect",
        "geometry",
        "terbon_toggle_requested",
        "browser_open_requested",
        "vscode_open_requested",
        "explorer_open_requested",
        "github_desktop_open_requested"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'widget_geometry_changed'
        QtMocHelpers::SignalData<void(const QString &, const QString &, const QRect &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 4 }, { 0x80000000 | 5, 6 },
        }}),
        // Signal 'terbon_toggle_requested'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'browser_open_requested'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'vscode_open_requested'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'explorer_open_requested'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'github_desktop_open_requested'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DashboardWindow, qt_meta_tag_ZN7ttydash15DashboardWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ttydash::DashboardWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7ttydash15DashboardWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7ttydash15DashboardWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN7ttydash15DashboardWindowE_t>.metaTypes,
    nullptr
} };

void ttydash::DashboardWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DashboardWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->widget_geometry_changed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QRect>>(_a[3]))); break;
        case 1: _t->terbon_toggle_requested(); break;
        case 2: _t->browser_open_requested(); break;
        case 3: _t->vscode_open_requested(); break;
        case 4: _t->explorer_open_requested(); break;
        case 5: _t->github_desktop_open_requested(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DashboardWindow::*)(const QString & , const QString & , const QRect & )>(_a, &DashboardWindow::widget_geometry_changed, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DashboardWindow::*)()>(_a, &DashboardWindow::terbon_toggle_requested, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DashboardWindow::*)()>(_a, &DashboardWindow::browser_open_requested, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DashboardWindow::*)()>(_a, &DashboardWindow::vscode_open_requested, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (DashboardWindow::*)()>(_a, &DashboardWindow::explorer_open_requested, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (DashboardWindow::*)()>(_a, &DashboardWindow::github_desktop_open_requested, 5))
            return;
    }
}

const QMetaObject *ttydash::DashboardWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ttydash::DashboardWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7ttydash15DashboardWindowE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ttydash::DashboardWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ttydash::DashboardWindow::widget_geometry_changed(const QString & _t1, const QString & _t2, const QRect & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3);
}

// SIGNAL 1
void ttydash::DashboardWindow::terbon_toggle_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ttydash::DashboardWindow::browser_open_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ttydash::DashboardWindow::vscode_open_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ttydash::DashboardWindow::explorer_open_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ttydash::DashboardWindow::github_desktop_open_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP

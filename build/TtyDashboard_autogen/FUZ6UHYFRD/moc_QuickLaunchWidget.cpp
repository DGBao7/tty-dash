/****************************************************************************
** Meta object code from reading C++ file 'QuickLaunchWidget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.12.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/widgets/launch/QuickLaunchWidget.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QuickLaunchWidget.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN7ttydash17QuickLaunchWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto ttydash::QuickLaunchWidget::qt_create_metaobjectdata<qt_meta_tag_ZN7ttydash17QuickLaunchWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ttydash::QuickLaunchWidget",
        "terbon_toggle_requested",
        "",
        "browser_open_requested",
        "vscode_open_requested",
        "explorer_open_requested",
        "github_desktop_open_requested"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'terbon_toggle_requested'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'browser_open_requested'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'vscode_open_requested'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'explorer_open_requested'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'github_desktop_open_requested'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QuickLaunchWidget, qt_meta_tag_ZN7ttydash17QuickLaunchWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ttydash::QuickLaunchWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<DashboardWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7ttydash17QuickLaunchWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7ttydash17QuickLaunchWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN7ttydash17QuickLaunchWidgetE_t>.metaTypes,
    nullptr
} };

void ttydash::QuickLaunchWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QuickLaunchWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->terbon_toggle_requested(); break;
        case 1: _t->browser_open_requested(); break;
        case 2: _t->vscode_open_requested(); break;
        case 3: _t->explorer_open_requested(); break;
        case 4: _t->github_desktop_open_requested(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QuickLaunchWidget::*)()>(_a, &QuickLaunchWidget::terbon_toggle_requested, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (QuickLaunchWidget::*)()>(_a, &QuickLaunchWidget::browser_open_requested, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (QuickLaunchWidget::*)()>(_a, &QuickLaunchWidget::vscode_open_requested, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (QuickLaunchWidget::*)()>(_a, &QuickLaunchWidget::explorer_open_requested, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (QuickLaunchWidget::*)()>(_a, &QuickLaunchWidget::github_desktop_open_requested, 4))
            return;
    }
}

const QMetaObject *ttydash::QuickLaunchWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ttydash::QuickLaunchWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7ttydash17QuickLaunchWidgetE_t>.strings))
        return static_cast<void*>(this);
    return DashboardWidget::qt_metacast(_clname);
}

int ttydash::QuickLaunchWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DashboardWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ttydash::QuickLaunchWidget::terbon_toggle_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ttydash::QuickLaunchWidget::browser_open_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ttydash::QuickLaunchWidget::vscode_open_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ttydash::QuickLaunchWidget::explorer_open_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ttydash::QuickLaunchWidget::github_desktop_open_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP

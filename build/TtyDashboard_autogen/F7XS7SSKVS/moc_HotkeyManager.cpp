/****************************************************************************
** Meta object code from reading C++ file 'HotkeyManager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.12.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/hotkeys/HotkeyManager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HotkeyManager.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN7ttydash13HotkeyManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto ttydash::HotkeyManager::qt_create_metaobjectdata<qt_meta_tag_ZN7ttydash13HotkeyManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ttydash::HotkeyManager",
        "exit_requested",
        "",
        "visibility_toggle_requested",
        "configuration_reload_requested",
        "interaction_toggle_requested",
        "terbon_visibility_toggle_requested"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'exit_requested'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'visibility_toggle_requested'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'configuration_reload_requested'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'interaction_toggle_requested'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'terbon_visibility_toggle_requested'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<HotkeyManager, qt_meta_tag_ZN7ttydash13HotkeyManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ttydash::HotkeyManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7ttydash13HotkeyManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7ttydash13HotkeyManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN7ttydash13HotkeyManagerE_t>.metaTypes,
    nullptr
} };

void ttydash::HotkeyManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<HotkeyManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->exit_requested(); break;
        case 1: _t->visibility_toggle_requested(); break;
        case 2: _t->configuration_reload_requested(); break;
        case 3: _t->interaction_toggle_requested(); break;
        case 4: _t->terbon_visibility_toggle_requested(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (HotkeyManager::*)()>(_a, &HotkeyManager::exit_requested, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotkeyManager::*)()>(_a, &HotkeyManager::visibility_toggle_requested, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotkeyManager::*)()>(_a, &HotkeyManager::configuration_reload_requested, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotkeyManager::*)()>(_a, &HotkeyManager::interaction_toggle_requested, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotkeyManager::*)()>(_a, &HotkeyManager::terbon_visibility_toggle_requested, 4))
            return;
    }
}

const QMetaObject *ttydash::HotkeyManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ttydash::HotkeyManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7ttydash13HotkeyManagerE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QAbstractNativeEventFilter"))
        return static_cast< QAbstractNativeEventFilter*>(this);
    return QObject::qt_metacast(_clname);
}

int ttydash::HotkeyManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void ttydash::HotkeyManager::exit_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ttydash::HotkeyManager::visibility_toggle_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ttydash::HotkeyManager::configuration_reload_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ttydash::HotkeyManager::interaction_toggle_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ttydash::HotkeyManager::terbon_visibility_toggle_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP

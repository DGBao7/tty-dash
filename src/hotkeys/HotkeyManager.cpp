#include "hotkeys/HotkeyManager.h"

#include "utils/Logger.h"

#include <QCoreApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace ttydash {
namespace {

constexpr int _exit_hotkey = 0x711;
constexpr int _visibility_hotkey = 0x712;
constexpr int _reload_hotkey = 0x713;
constexpr int _interaction_hotkey = 0x714;
constexpr int _terbon_visibility_hotkey = 0x715;

}

HotkeyManager::HotkeyManager() = default;

HotkeyManager::~HotkeyManager() {
    _stop();
}

void HotkeyManager::_start() {
    if (_started) {
        return;
    }

#ifdef Q_OS_WIN
    const UINT modifiers = MOD_CONTROL | MOD_ALT | MOD_NOREPEAT;
    struct Registration {
        int id;
        UINT key;
        const wchar_t *label;
    };
    const Registration registrations[] = {
        {_exit_hotkey , 'Q' , L"Ctrl+Alt+Q"} ,
        {_visibility_hotkey , 'H' , L"Ctrl+Alt+H"} ,
        {_reload_hotkey , 'R' , L"Ctrl+Alt+R"} ,
        {_interaction_hotkey , 'I' , L"Ctrl+Alt+I"} ,
        {_terbon_visibility_hotkey , 'T' , L"Ctrl+Alt+T"}
    };

    QCoreApplication::instance()->installNativeEventFilter(this);
    for (const Registration &registration : registrations) {
        if (!RegisterHotKey(nullptr , registration.id , modifiers , registration.key)) {
            Logger::_warning(QStringLiteral("Global hotkey registration failed for %1 (error %2)")
                .arg(QString::fromWCharArray(registration.label))
                .arg(GetLastError()));
        } else {
            ++_registered_hotkeys;
        }
    }
    _started = true;
#else
    Logger::_warning(QStringLiteral("Global hotkeys are only available on Windows"));
#endif
}

void HotkeyManager::_stop() {
    if (!_started) {
        return;
    }

#ifdef Q_OS_WIN
    QCoreApplication::instance()->removeNativeEventFilter(this);
    UnregisterHotKey(nullptr , _exit_hotkey);
    UnregisterHotKey(nullptr , _visibility_hotkey);
    UnregisterHotKey(nullptr , _reload_hotkey);
    UnregisterHotKey(nullptr , _interaction_hotkey);
    UnregisterHotKey(nullptr , _terbon_visibility_hotkey);
#endif
    _started = false;
    _registered_hotkeys = 0;
}

int HotkeyManager::_registered_count() const {
    return _registered_hotkeys;
}

bool HotkeyManager::nativeEventFilter(const QByteArray &event_type , void *message , qintptr *result) {
    Q_UNUSED(event_type)
    Q_UNUSED(result)
#ifdef Q_OS_WIN
    const MSG *native_message = static_cast<const MSG *>(message);
    if (!native_message || native_message->message != WM_HOTKEY) {
        return false;
    }

    switch (native_message->wParam) {
    case _exit_hotkey:
        emit exit_requested();
        return true;
    case _visibility_hotkey:
        emit visibility_toggle_requested();
        return true;
    case _reload_hotkey:
        emit configuration_reload_requested();
        return true;
    case _interaction_hotkey:
        emit interaction_toggle_requested();
        return true;
    case _terbon_visibility_hotkey:
        emit terbon_visibility_toggle_requested();
        return true;
    default:
        break;
    }
#else
    Q_UNUSED(message)
#endif

    return false;
}

}

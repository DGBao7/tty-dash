#pragma once

#include <QAbstractNativeEventFilter>
#include <QObject>

namespace ttydash {

class HotkeyManager : public QObject , public QAbstractNativeEventFilter {
    Q_OBJECT

public:
    HotkeyManager();
    ~HotkeyManager() override;

    void _start();
    void _stop();
    int _registered_count() const;
    bool nativeEventFilter(const QByteArray &event_type , void *message , qintptr *result) override;

signals:
    void exit_requested();
    void visibility_toggle_requested();
    void configuration_reload_requested();
    void interaction_toggle_requested();

private:
    bool _started = false;
    int _registered_hotkeys = 0;
};

}

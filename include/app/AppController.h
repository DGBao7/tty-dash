#pragma once

#include "config/ConfigManager.h"
#include "display/DisplayManager.h"
#include "hotkeys/HotkeyManager.h"
#include "layout/LayoutManager.h"
#include "services/SystemMonitor.h"
#include "widgets/WidgetManager.h"

#include <QApplication>
#include <QVector>

class QSystemTrayIcon;
class QMenu;

namespace ttydash {

class DashboardWindow;

class AppController {
public:
    AppController(QApplication &application);
    ~AppController();

    int _run();

private:
    void _initialize();
    void _rebuild_dashboards();
    void _reload_configuration();
    void _toggle_visibility();
    void _toggle_interaction();
    void _create_tray_icon();
    void _stop();

    QApplication &_application;
    ConfigManager _config_manager;
    DisplayManager _display_manager;
    LayoutManager _layout_manager;
    SystemMonitor _system_monitor;
    WidgetManager _widget_manager;
    HotkeyManager _hotkey_manager;
    DashboardConfig _config;
    QVector<DashboardWindow *> _windows;
    QSystemTrayIcon *_tray_icon = nullptr;
    QMenu *_tray_menu = nullptr;
    bool _visible = true;
    bool _click_through_enabled = true;
    bool _initialized = false;
    bool _rebuilding = false;
};

}

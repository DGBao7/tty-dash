#include "app/AppController.h"

#include "utils/Logger.h"
#include "window/DashboardWindow.h"

#include <QCoreApplication>
#include <QAction>
#include <QFile>
#include <QHash>
#include <QMenu>
#include <QScreen>
#include <QStyle>
#include <QSystemTrayIcon>

namespace ttydash {

AppController::AppController(QApplication &application)
    : _application(application) {
}

AppController::~AppController() {
    _stop();
}

int AppController::_run() {
    _initialize();

    return _application.exec();
}

void AppController::_initialize() {
    if (_initialized) {
        return;
    }

    Logger::_initialize();
    QApplication::setApplicationName(QStringLiteral("TTY Dashboard"));
    QApplication::setOrganizationName(QStringLiteral("TtyDashboard"));
    QApplication::setApplicationVersion(QStringLiteral("0.1.0"));
    _application.setQuitOnLastWindowClosed(false);
    QFile theme(QStringLiteral(":/themes/Dark.qss"));
    if (theme.open(QIODevice::ReadOnly)) {
        _application.setStyleSheet(QString::fromUtf8(theme.readAll()));
    }
    _config = _config_manager._load();
    _click_through_enabled = _config.application.click_through;

    QObject::connect(&_display_manager , &DisplayManager::displays_changed , &_application , [this]() {
        Logger::_info(QStringLiteral("Display configuration changed; restoring dashboards"));
        _rebuild_dashboards();
    });
    QObject::connect(&_hotkey_manager , &HotkeyManager::exit_requested , &_application , [this]() {
        _application.quit();
    });
    QObject::connect(&_hotkey_manager , &HotkeyManager::visibility_toggle_requested , &_application , [this]() {
        _toggle_visibility();
    });
    QObject::connect(&_hotkey_manager , &HotkeyManager::configuration_reload_requested , &_application , [this]() {
        _reload_configuration();
    });
    QObject::connect(&_hotkey_manager , &HotkeyManager::interaction_toggle_requested , &_application , [this]() {
        _toggle_interaction();
    });

    _system_monitor._start(_config.application.monitoring_interval_ms);
    _hotkey_manager._start();
    _create_tray_icon();
    _rebuild_dashboards();
    Logger::_info(QStringLiteral("Configuration loaded from %1").arg(_config_manager._path()));
    _initialized = true;
}

void AppController::_rebuild_dashboards() {
    if (_rebuilding) {
        return;
    }

    _rebuilding = true;
    qDeleteAll(_windows);
    _windows.clear();
    QHash<QScreen * , bool> assigned_screens;
    const QVector<DisplayInfo> connected_screens = _display_manager._screens();

    for (const DisplayConfig &display : _config.displays) {
        QScreen *screen = nullptr;
        if (display.name.compare(QStringLiteral("primary") , Qt::CaseInsensitive) == 0) {
            screen = _display_manager._primary_screen();
        } else {
            for (const DisplayInfo &info : connected_screens) {
                if (info.name.compare(display.name , Qt::CaseInsensitive) == 0) {
                    screen = info.screen;
                    break;
                }
            }

            if (!screen) {
                screen = _display_manager._primary_screen();
                Logger::_warning(QStringLiteral("Configured display '%1' is unavailable; using the primary display").arg(display.name));
            }
        }

        if (!screen) {
            Logger::_error(QStringLiteral("No display is available for dashboard '%1'").arg(display.name));

            continue;
        }
        if (assigned_screens.contains(screen)) {
            Logger::_warning(QStringLiteral("More than one dashboard selected display '%1'; skipping the duplicate").arg(screen->name()));

            continue;
        }
        assigned_screens.insert(screen , true);

        DashboardWindow *window = new DashboardWindow(display , _config , screen , _layout_manager , _widget_manager , &_system_monitor);
        QObject::connect(window , &DashboardWindow::widget_geometry_changed , &_application , [this](const QString &display_name , const QString &widget_id , const QRect &geometry) {
            if (!_config_manager._update_widget_geometry(display_name , widget_id , geometry)) {
                Logger::_warning(QStringLiteral("Could not persist widget position for %1").arg(widget_id));
            }
            for (DisplayConfig &display : _config.displays) {
                if (display.name != display_name) {
                    continue;
                }
                for (WidgetConfig &widget : display.widgets) {
                    if (widget.id == widget_id) {
                        widget.position_mode = QStringLiteral("custom");
                        widget.x = geometry.x();
                        widget.y = geometry.y();
                        widget.width = geometry.width();
                        widget.height = geometry.height();
                    }
                }
            }
        });
        _windows.append(window);
        if (_visible) {
            window->show();
        } else {
            window->hide();
        }
    }

    _rebuilding = false;
}

void AppController::_reload_configuration() {
    Logger::_info(QStringLiteral("Reloading dashboard configuration"));
    _config = _config_manager._load();
    _click_through_enabled = _config.application.click_through;
    _system_monitor._set_interval(_config.application.monitoring_interval_ms);
    _rebuild_dashboards();
}

void AppController::_toggle_visibility() {
    _visible = !_visible;
    for (DashboardWindow *window : _windows) {
        if (_visible) {
            window->show();
        } else {
            window->hide();
        }
    }
}

void AppController::_toggle_interaction() {
    _click_through_enabled = !_click_through_enabled;
    _config.application.click_through = _click_through_enabled;
    for (DashboardWindow *window : _windows) {
        window->_set_click_through(_click_through_enabled);
    }
    _config_manager._set_click_through(_click_through_enabled);
    Logger::_info(_click_through_enabled
        ? QStringLiteral("Dashboard interaction disabled; click-through is active")
        : QStringLiteral("Dashboard interaction enabled; drag widgets to move and resize"));
}

void AppController::_create_tray_icon() {
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        Logger::_warning(QStringLiteral("System tray is unavailable; application controls depend on global hotkeys"));

        return;
    }

    QMenu *menu = new QMenu();
    _tray_menu = menu;
    QAction *visibility_action = menu->addAction(QStringLiteral("Hide dashboards"));
    QAction *interaction_action = menu->addAction(QStringLiteral("Enable interaction"));
    QAction *reload_action = menu->addAction(QStringLiteral("Reload configuration"));
    menu->addSeparator();
    QAction *exit_action = menu->addAction(QStringLiteral("Exit"));
    QObject::connect(menu , &QMenu::aboutToShow , menu , [this , visibility_action , interaction_action]() {
        visibility_action->setText(_visible ? QStringLiteral("Hide dashboards") : QStringLiteral("Show dashboards"));
        interaction_action->setText(_click_through_enabled ? QStringLiteral("Enable interaction") : QStringLiteral("Disable interaction"));
    });

    QObject::connect(visibility_action , &QAction::triggered , &_application , [this , visibility_action]() {
        _toggle_visibility();
        visibility_action->setText(_visible ? QStringLiteral("Hide dashboards") : QStringLiteral("Show dashboards"));
    });
    QObject::connect(interaction_action , &QAction::triggered , &_application , [this , interaction_action]() {
        _toggle_interaction();
        interaction_action->setText(_click_through_enabled ? QStringLiteral("Enable interaction") : QStringLiteral("Disable interaction"));
    });
    QObject::connect(reload_action , &QAction::triggered , &_application , [this]() {
        _reload_configuration();
    });
    QObject::connect(exit_action , &QAction::triggered , &_application , [this]() {
        _application.quit();
    });

    _tray_icon = new QSystemTrayIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon) , &_application);
    _tray_icon->setToolTip(QStringLiteral("TTY Dashboard"));
    _tray_icon->setContextMenu(menu);
    QObject::connect(_tray_icon , &QSystemTrayIcon::activated , &_application , [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick || reason == QSystemTrayIcon::Trigger) {
            _toggle_visibility();
        }
    });
    _tray_icon->show();

    if (_hotkey_manager._registered_count() < 4) {
        Logger::_warning(QStringLiteral("One or more global shortcuts were unavailable; use the system tray menu for controls"));
    }
}

void AppController::_stop() {
    _hotkey_manager._stop();
    qDeleteAll(_windows);
    _windows.clear();
    if (_tray_icon) {
        _tray_icon->hide();
        delete _tray_icon;
        _tray_icon = nullptr;
    }
    delete _tray_menu;
    _tray_menu = nullptr;
}

}

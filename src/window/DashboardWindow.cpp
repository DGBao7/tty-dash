#include "window/DashboardWindow.h"

#include "layout/LayoutManager.h"

#include "utils/Logger.h"

#include "widgets/DashboardWidget.h"
#include "widgets/WidgetManager.h"

#include "services/SystemMonitor.h"

#include <QScreen>
#include <QShowEvent>

#include <algorithm>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace ttydash {

DashboardWindow::DashboardWindow(const DisplayConfig &display_config , const DashboardConfig &application_config , QScreen *screen , LayoutManager &layout_manager , WidgetManager &widget_manager , SystemMonitor *system_monitor , QWidget *parent)
    : QWidget(parent)
    , _display_config(display_config)
    , _application_settings(application_config.application)
    , _effects_settings(application_config.effects)
    , _display_name(display_config.name)
    , _click_through_enabled(application_config.application.click_through) {
    setWindowTitle(QStringLiteral("TTY Dashboard"));
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    setAutoFillBackground(false);
    setMinimumSize(120 , 80);
    _apply_flags();

    if (screen) {
        setGeometry(layout_manager._window_geometry(_display_config , screen->availableGeometry()));
    }

    _apply_widget_layout(layout_manager , widget_manager , system_monitor);
    setWindowOpacity(1.0);
    winId();
    _apply_native_styles();
}

void DashboardWindow::_set_click_through(bool enabled) {
    if (_click_through_enabled == enabled) {
        return;
    }

    const bool was_visible = isVisible();
    const QRect previous_geometry = geometry();
    _click_through_enabled = enabled;
    _apply_flags();
    setGeometry(previous_geometry);
    for (DashboardWidget *widget : findChildren<DashboardWidget *>()) {
        widget->_set_interaction_enabled(false);
    }

    if (was_visible) {
        show();
    }
    winId();
    _apply_native_styles();
    update();
}

bool DashboardWindow::_click_through() const {
    return _click_through_enabled;
}

void DashboardWindow::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    _apply_native_styles();
}

void DashboardWindow::_apply_flags() {
    Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::Tool;
    if (_click_through_enabled) {
        flags |= Qt::WindowDoesNotAcceptFocus;
    }
    if (_application_settings.always_on_top) {
        flags |= Qt::WindowStaysOnTopHint;
    }

    setWindowFlags(flags);
    setAttribute(Qt::WA_ShowWithoutActivating , _click_through_enabled);
}

void DashboardWindow::_apply_native_styles() {
#ifdef Q_OS_WIN
    const HWND hwnd = reinterpret_cast<HWND>(winId());
    if (!hwnd) {
        return;
    }

    LONG_PTR extended_style = GetWindowLongPtrW(hwnd , GWL_EXSTYLE);
    extended_style |= WS_EX_TOOLWINDOW | WS_EX_LAYERED;
    extended_style &= ~static_cast<LONG_PTR>(WS_EX_APPWINDOW);
    if (_click_through_enabled) {
        extended_style |= WS_EX_TRANSPARENT | WS_EX_NOACTIVATE;
    } else {
        extended_style &= ~static_cast<LONG_PTR>(WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
    }

    SetLastError(ERROR_SUCCESS);
    const LONG_PTR previous_style = SetWindowLongPtrW(hwnd , GWL_EXSTYLE , extended_style);
    if (previous_style == 0 && GetLastError() != ERROR_SUCCESS) {
        Logger::_warning(QStringLiteral("Could not apply native dashboard window styles"));
    }

    const HWND z_order = _application_settings.always_on_top ? HWND_TOPMOST : HWND_NOTOPMOST;
    SetWindowPos(hwnd , z_order , 0 , 0 , 0 , 0 , SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
    _window_effects._clear(hwnd);
    const QString backdrop = _display_config.backdrop.isEmpty() ? _effects_settings.backdrop : _display_config.backdrop;
    _native_backdrop_applied = _effects_settings.blur && backdrop != QStringLiteral("none")
        ? _window_effects._apply(hwnd , _effects_settings , _display_config.backdrop , _display_config.background_color)
        : false;
#else
    _native_backdrop_applied = false;
#endif
    update();
}

void DashboardWindow::_apply_widget_layout(LayoutManager &layout_manager , WidgetManager &widget_manager , SystemMonitor *system_monitor) {
    const QVector<DashboardWidget *> widgets = widget_manager._create_widgets(_display_config.widgets , system_monitor , this);

    for (DashboardWidget *widget : widgets) {

        if (auto *quick_launch = qobject_cast<QuickLaunchWidget *>(widget)) {
            connect(
                quick_launch,
                &QuickLaunchWidget::terbon_toggle_requested,
                this,
                &DashboardWindow::terbon_toggle_requested
            );

            connect(
                quick_launch,
                &QuickLaunchWidget::browser_open_requested,
                this,
                &DashboardWindow::browser_open_requested
            );

            connect(
                quick_launch,
                &QuickLaunchWidget::vscode_open_requested,
                this,
                &DashboardWindow::vscode_open_requested
            );

            connect(
                quick_launch,
                &QuickLaunchWidget::explorer_open_requested,
                this,
                &DashboardWindow::explorer_open_requested
            );

            connect(
                quick_launch,
                &QuickLaunchWidget::github_desktop_open_requested,
                this,
                &DashboardWindow::github_desktop_open_requested
            );
        }

        const auto config = std::find_if(_display_config.widgets.cbegin() , _display_config.widgets.cend() , [widget](const WidgetConfig &candidate) {
            return candidate.id == widget->_widget_id();
        });

        if (config != _display_config.widgets.cend()) {
            widget->setGeometry(layout_manager._widget_geometry(*config , QRect(0 , 0 , width() , height())));
        }

        widget->_set_interaction_enabled(false);

        connect(widget , &DashboardWidget::geometry_changed , this , [this](const QString &widget_id , const QRect &geometry) {
            emit widget_geometry_changed(_display_name , widget_id , geometry);
        });

        widget->show();
    }
}

}

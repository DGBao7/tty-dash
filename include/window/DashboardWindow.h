#pragma once

#include "config/ConfigManager.h"
#include "window/WindowEffects.h"

#include <QWidget>

class QScreen;
class QShowEvent;

namespace ttydash {

class DashboardWidget;
class LayoutManager;
class SystemMonitor;
class WidgetManager;

class DashboardWindow : public QWidget {
    Q_OBJECT

public:
    DashboardWindow(const DisplayConfig &display_config , const DashboardConfig &application_config , QScreen *screen , LayoutManager &layout_manager , WidgetManager &widget_manager , SystemMonitor *system_monitor , QWidget *parent = nullptr);

    void _set_click_through(bool enabled);
    bool _click_through() const;

signals:
    void widget_geometry_changed(const QString &display_name , const QString &widget_id , const QRect &geometry);

protected:
    void showEvent(QShowEvent *event) override;

private:
    void _apply_flags();
    void _apply_native_styles();
    void _apply_widget_layout(LayoutManager &layout_manager , WidgetManager &widget_manager , SystemMonitor *system_monitor);

    DisplayConfig _display_config;
    ApplicationSettings _application_settings;
    EffectsSettings _effects_settings;
    QString _display_name;
    bool _click_through_enabled = true;
    bool _native_backdrop_applied = false;
    WindowEffects _window_effects;
};

}

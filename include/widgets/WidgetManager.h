#pragma once

#include "config/ConfigManager.h"

#include <QHash>
#include <QVector>
#include <QWidget>

#include <functional>

namespace ttydash {

class DashboardWidget;
class SystemMonitor;

class WidgetManager {
public:
    using Factory = std::function<DashboardWidget *(const WidgetConfig &config , SystemMonitor *monitor , QWidget *parent)>;

    WidgetManager();

    bool _register_widget(const QString &type , Factory factory);
    QVector<DashboardWidget *> _create_widgets(const QVector<WidgetConfig> &configs , SystemMonitor *monitor , QWidget *parent) const;

private:
    QHash<QString , Factory> _factories;
};

}

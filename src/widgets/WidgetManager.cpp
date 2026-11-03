#include "widgets/WidgetManager.h"

#include "utils/Logger.h"

#include "widgets/DashboardWidget.h"

#include "widgets/ascii/MatrixWidget.h"

#include "widgets/clock/ClockWidget.h"

#include "widgets/system/AudioVisualizerWidget.h"
#include "widgets/system/CpuWidget.h"
#include "widgets/system/DiskWidget.h"
#include "widgets/system/NetworkWidget.h"
#include "widgets/system/RamWidget.h"

#include "widgets/launch/QuickLaunchWidget.h"

#include "widgets/live/LiveWindowsWidget.h"

namespace ttydash {

WidgetManager::WidgetManager() {
    _register_widget(QStringLiteral("clock") , [](const WidgetConfig &config , SystemMonitor * , QWidget *parent) {
        return new ClockWidget(config , parent);
    });
    _register_widget(QStringLiteral("matrix") , [](const WidgetConfig &config , SystemMonitor * , QWidget *parent) {
        return new MatrixWidget(config , parent);
    });
    _register_widget(QStringLiteral("audio_visualizer") , [](const WidgetConfig &config , SystemMonitor * , QWidget *parent) {
        return new AudioVisualizerWidget(config , parent);
    });
    _register_widget(QStringLiteral("cpu") , [](const WidgetConfig &config , SystemMonitor *monitor , QWidget *parent) {
        return new CpuWidget(config , monitor , parent);
    });
    _register_widget(QStringLiteral("ram") , [](const WidgetConfig &config , SystemMonitor *monitor , QWidget *parent) {
        return new RamWidget(config , monitor , parent);
    });
    _register_widget(QStringLiteral("network") , [](const WidgetConfig &config , SystemMonitor *monitor , QWidget *parent) {
        return new NetworkWidget(config , monitor , parent);
    });
    _register_widget(QStringLiteral("disk") , [](const WidgetConfig &config , SystemMonitor * , QWidget *parent) {
        return new DiskWidget(config , parent);
    });
    _register_widget(QStringLiteral("quick_launch") , [](const WidgetConfig &config , SystemMonitor * , QWidget *parent) {
        return new QuickLaunchWidget(config , parent);
    });
    _register_widget(QStringLiteral("live_windows") , [](const WidgetConfig &config , SystemMonitor * , QWidget *parent) {
        return new LiveWindowsWidget(config , parent);
    });
}

bool WidgetManager::_register_widget(const QString &type , Factory factory) {
    const QString key = type.trimmed().toLower();
    if (key.isEmpty() || !factory) {
        return false;
    }

    _factories.insert(key , std::move(factory));

    return true;
}

QVector<DashboardWidget *> WidgetManager::_create_widgets(const QVector<WidgetConfig> &configs , SystemMonitor *monitor , QWidget *parent) const {
    QVector<DashboardWidget *> widgets;
    for (const WidgetConfig &config : configs) {
        if (!config.enabled) {
            continue;
        }

        const auto factory = _factories.constFind(config.type.toLower());
        if (factory == _factories.cend()) {
            Logger::_warning(QStringLiteral("No widget factory is registered for type '%1'").arg(config.type));

            continue;
        }

        DashboardWidget *widget = factory.value()(config , monitor , parent);
        if (widget) {
            widgets.append(widget);
        }
    }

    return widgets;
}

}

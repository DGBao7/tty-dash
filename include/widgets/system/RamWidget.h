#pragma once

#include "widgets/DashboardWidget.h"

namespace ttydash {

class SystemMonitor;

class RamWidget : public DashboardWidget {
public:
    RamWidget(const WidgetConfig &config , SystemMonitor *monitor , QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    quint64 _total_bytes = 0;
    quint64 _used_bytes = 0;
    bool _show_graph = true;
    QVector<qreal> _history;
};

}

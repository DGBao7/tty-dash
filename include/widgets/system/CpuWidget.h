#pragma once

#include "widgets/DashboardWidget.h"

#include <QVector>

namespace ttydash {

class SystemMonitor;

class CpuWidget : public DashboardWidget {
public:
    CpuWidget(const WidgetConfig &config , SystemMonitor *monitor , QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    qreal _usage = 0.0;
    bool _show_graph = true;
    QVector<qreal> _history;
};

}

#pragma once

#include "widgets/DashboardWidget.h"

#include <QTimer>

namespace ttydash {

class DiskWidget : public DashboardWidget {
public:
    DiskWidget(const WidgetConfig &config , QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void _refresh();

    QTimer _timer;
    QString _volume;
    qreal _used = 0.0;
    qreal _total_gib = 0.0;
};

}

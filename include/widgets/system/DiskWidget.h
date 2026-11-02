#pragma once

#include "widgets/DashboardWidget.h"

#include <QTimer>
#include <QVector>
#include <QString>

namespace ttydash {

class DiskWidget : public DashboardWidget {
public:
    DiskWidget(const WidgetConfig &config , QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void _refresh();

    QTimer _timer;
    
    struct DiskInfo {
        QString volume;
        qreal used = 0.0;
        qreal total_gib = 0.0;
        qreal used_gib = 0.0;
    };

    QVector<DiskInfo> _disks;
};

}

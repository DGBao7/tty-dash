#pragma once

#include "widgets/DashboardWidget.h"

namespace ttydash {

class SystemMonitor;

class NetworkWidget : public DashboardWidget {
public:
    NetworkWidget(const WidgetConfig &config , SystemMonitor *monitor , QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString _format_speed(quint64 bytes_per_second) const;

    quint64 _upload_bytes_per_second = 0;
    quint64 _download_bytes_per_second = 0;
    QString _unit = QStringLiteral("auto");
};

}

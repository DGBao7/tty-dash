#pragma once

#include "widgets/DashboardWidget.h"

#include <QTimer>

namespace ttydash {

class ClockWidget : public DashboardWidget {
public:
    ClockWidget(const WidgetConfig &config , QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void _refresh();

    QTimer _timer;
    QString _time_format;
    QString _date_format;
    QString _time_text;
    QString _date_text;
    int _display_font_size = 34;
};

}

#include "widgets/system/CpuWidget.h"

#include "services/SystemMonitor.h"

#include <QPainter>

namespace ttydash {

CpuWidget::CpuWidget(const WidgetConfig &config , SystemMonitor *monitor , QWidget *parent)
    : DashboardWidget(config , parent) {
    _show_graph = config.options.value(QStringLiteral("show_graph")).toBool(true);
    if (monitor) {
        connect(monitor , &SystemMonitor::cpu_updated , this , [this](qreal usage) {
            _usage = usage;
            _history.append(usage);
            if (_history.size() > 80) {
                _history.remove(0 , _history.size() - 80);
            }
            update();
        });
    }
}

void CpuWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    _paint_panel(painter , _title);
    painter.setFont(_font);
    painter.setPen(_text_color);
    painter.drawText(QRect(16 , 34 , width() - 30 , 30) , Qt::AlignLeft | Qt::AlignVCenter , QStringLiteral("%1 %").arg(_usage , 0 , 'f' , 1));
    painter.setPen(_accent_color);
    painter.drawText(QRect(width() - 74 , 36 , 56 , 24) , Qt::AlignRight | Qt::AlignVCenter , QStringLiteral("CPU"));

    if (_show_graph) {
        _paint_graph(painter , QRectF(18.0 , 70.0 , width() - 34.0 , qMax(18 , height() - 82)) , _history , 100.0);
    }
}

}

#include "widgets/system/RamWidget.h"

#include "services/SystemMonitor.h"

#include <QPainter>

namespace ttydash {
namespace {

QString _format_memory(quint64 bytes) {
    constexpr qreal mebibyte = 1024.0 * 1024.0;
    constexpr qreal gibibyte = mebibyte * 1024.0;
    if (bytes >= static_cast<quint64>(gibibyte)) {
        return QStringLiteral("%1 GiB").arg(static_cast<qreal>(bytes) / gibibyte , 0 , 'f' , 1);
    }

    return QStringLiteral("%1 MiB").arg(static_cast<qreal>(bytes) / mebibyte , 0 , 'f' , 0);
}

}

RamWidget::RamWidget(const WidgetConfig &config , SystemMonitor *monitor , QWidget *parent)
    : DashboardWidget(config , parent) {
    _show_graph = config.options.value(QStringLiteral("show_graph")).toBool(true);
    if (monitor) {
        connect(monitor , &SystemMonitor::memory_updated , this , [this](quint64 total , quint64 used) {
            _total_bytes = total;
            _used_bytes = used;
            const qreal percentage = total == 0 ? 0.0 : 100.0 * static_cast<qreal>(used) / static_cast<qreal>(total);
            _history.append(percentage);
            if (_history.size() > 80) {
                _history.remove(0 , _history.size() - 80);
            }
            update();
        });
    }
}

void RamWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    _paint_panel(painter , _title);

    const qreal percentage = _total_bytes == 0 ? 0.0 : 100.0 * static_cast<qreal>(_used_bytes) / static_cast<qreal>(_total_bytes);
    painter.setFont(_font);
    painter.setPen(_text_color);
    painter.drawText(QRect(16 , 34 , width() - 30 , 28) , Qt::AlignLeft | Qt::AlignVCenter , QStringLiteral("%1 / %2").arg(_format_memory(_used_bytes) , _format_memory(_total_bytes)));
    painter.setPen(_accent_color);
    painter.drawText(QRect(16 , 61 , width() - 32 , 20) , Qt::AlignLeft | Qt::AlignVCenter , QStringLiteral("%1% USED").arg(percentage , 0 , 'f' , 1));

    if (_show_graph) {
        _paint_graph(painter , QRectF(18.0 , 83.0 , width() - 34.0 , qMax(16 , height() - 95)) , _history , 100.0);
    }
}

}

#include "widgets/system/NetworkWidget.h"

#include "services/SystemMonitor.h"

#include <QPainter>

namespace ttydash {

NetworkWidget::NetworkWidget(const WidgetConfig &config , SystemMonitor *monitor , QWidget *parent)
    : DashboardWidget(config , parent) {
    _unit = config.options.value(QStringLiteral("speed_unit")).toString(QStringLiteral("auto")).toLower();
    if (monitor) {
        connect(monitor , &SystemMonitor::network_updated , this , [this](quint64 upload , quint64 download) {
            _upload_bytes_per_second = upload;
            _download_bytes_per_second = download;
            update();
        });
    }
}

void NetworkWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    _paint_panel(painter , _title);
    painter.setFont(_font);
    painter.setPen(_text_color);
    const int line_height = qMax(18 , (height() - 36) / 2);
    painter.drawText(QRect(16 , 34 , width() - 32 , line_height) , Qt::AlignLeft | Qt::AlignVCenter , QStringLiteral("DOWN  %1").arg(_format_speed(_download_bytes_per_second)));
    painter.drawText(QRect(16 , 34 + line_height , width() - 32 , line_height) , Qt::AlignLeft | Qt::AlignVCenter , QStringLiteral("UP      %1").arg(_format_speed(_upload_bytes_per_second)));
    painter.setPen(_accent_color);
    painter.drawText(QRect(width() - 74 , 36 , 56 , 24) , Qt::AlignRight | Qt::AlignVCenter , QStringLiteral("NET"));
}

QString NetworkWidget::_format_speed(quint64 bytes_per_second) const {
    const bool bits = _unit == QStringLiteral("bits") || (_unit == QStringLiteral("auto") && bytes_per_second >= 1024 * 1024);
    qreal value = static_cast<qreal>(bytes_per_second) * (bits ? 8.0 : 1.0);
    QString suffix = bits ? QStringLiteral("b/s") : QStringLiteral("B/s");
    const qreal base = bits ? 1000.0 : 1024.0;

    if (value >= base * base) {
        value /= base * base;
        suffix = bits ? QStringLiteral("Mb/s") : QStringLiteral("MiB/s");
    } else if (value >= base) {
        value /= base;
        suffix = bits ? QStringLiteral("Kb/s") : QStringLiteral("KiB/s");
    }

    return QStringLiteral("%1 %2").arg(value , 0 , 'f' , value >= 100.0 ? 0 : 1).arg(suffix);
}

}

#include "widgets/system/DiskWidget.h"

#include <QPainter>
#include <QStorageInfo>

namespace ttydash {

DiskWidget::DiskWidget(const WidgetConfig &config , QWidget *parent)
    : DashboardWidget(config , parent) {
    connect(&_timer , &QTimer::timeout , this , [this]() {
        _refresh();
    });
    _refresh();
    _timer.start(10000);
}

void DiskWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    _paint_panel(painter , _title);
    painter.setFont(_font);
    painter.setPen(_text_color);
    painter.drawText(QRect(16 , 34 , width() - 32 , 26) , Qt::AlignLeft | Qt::AlignVCenter ,
        QStringLiteral("%1  %2% USED").arg(_volume).arg(qRound(_used * 100.0)));
    painter.setPen(Qt::NoPen);
    painter.setBrush(_panel_color.lighter(145));
    const QRect track(16 , 68 , width() - 32 , 12);
    painter.drawRoundedRect(track , 5 , 5);
    painter.setBrush(_accent_color);
    painter.drawRoundedRect(QRect(track.x() , track.y() , qRound(track.width() * _used) , track.height()) , 5 , 5);
    painter.setPen(_accent_color);
    painter.drawText(QRect(16 , 82 , width() - 32 , 18) , Qt::AlignLeft | Qt::AlignVCenter ,
        QStringLiteral("%1 GiB TOTAL").arg(_total_gib , 0 , 'f' , 1));
}

void DiskWidget::_refresh() {
    const QStorageInfo storage = QStorageInfo::root();
    if (storage.isReady() && storage.bytesTotal() > 0) {
        const quint64 total = storage.bytesTotal();
        const quint64 free = storage.bytesAvailable();
        _used = qBound(0.0 , 1.0 - static_cast<qreal>(free) / static_cast<qreal>(total) , 1.0);
        _total_gib = static_cast<qreal>(total) / (1024.0 * 1024.0 * 1024.0);
        _volume = storage.rootPath();
    } else {
        _used = 0.0;
        _total_gib = 0.0;
        _volume = QStringLiteral("N/A");
    }
    update();
}

}

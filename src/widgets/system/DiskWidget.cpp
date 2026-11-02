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

    if (_disks.isEmpty()) {
        painter.drawText(
            rect(),
            Qt::AlignCenter,
            QStringLiteral("NO DRIVE FOUND")
        );

        return;
    }

    const int row_height = 30;

    for (int i = 0; i < _disks.size(); ++i) {
        const DiskInfo &disk = _disks[i];

        const int y = 34 + i * row_height;

        painter.setPen(_text_color);

        painter.drawText(
            QRect(16 , y , width() - 32 , 16),
            Qt::AlignLeft | Qt::AlignVCenter,
            QStringLiteral("%1   %2%   %3 / %4 GiB")
                .arg(disk.volume)
                .arg(qRound(disk.used * 100.0))
                .arg(disk.used_gib , 0 , 'f' , 1)
                .arg(disk.total_gib , 0 , 'f' , 1)
        );

        const QRect track(16 , y + 19 , width() - 32 , 7);

        painter.setPen(Qt::NoPen);

        painter.setBrush(_panel_color.lighter(145));
        painter.drawRoundedRect(track , 3 , 3);

        painter.setBrush(_accent_color);

        painter.drawRoundedRect(
            QRect(
                track.x(),
                track.y(),
                qRound(track.width() * disk.used),
                track.height()
            ),
            3,
            3
        );
    }
}

void DiskWidget::_refresh() {
    QVector<DiskInfo> disks;

    const QList<QStorageInfo> volumes = QStorageInfo::mountedVolumes();

    for (const QStorageInfo &storage : volumes) {
        if (!storage.isValid() || !storage.isReady()
            || storage.bytesTotal() == 0) {
            continue;
        }

        const quint64 total = storage.bytesTotal();
        const quint64 free = storage.bytesFree();

        DiskInfo disk;

        disk.volume = storage.rootPath();

        if (disk.volume.endsWith('/') || disk.volume.endsWith('\\')) {
            disk.volume.chop(1);
        }

        disk.used = qBound(
            0.0,
            1.0 - static_cast<qreal>(free) / static_cast<qreal>(total),
            1.0
        );

        disk.total_gib = static_cast<qreal>(total)
            / (1024.0 * 1024.0 * 1024.0);

        disk.used_gib = disk.used * disk.total_gib;

        disks.append(disk);
    }

    std::sort(
        disks.begin(),
        disks.end(),
        [](const DiskInfo &a , const DiskInfo &b) {
            return a.volume < b.volume;
        }
    );

    _disks = disks;

    update();
}

}

#pragma once

#include <QObject>
#include <QRect>
#include <QScreen>
#include <QSet>
#include <QString>
#include <QVector>

namespace ttydash {

struct DisplayInfo {
    QString name;
    QRect geometry;
    QRect available_geometry;
    qreal device_pixel_ratio = 1.0;
    bool primary = false;
    QScreen *screen = nullptr;
};

class DisplayManager : public QObject {
    Q_OBJECT

public:
    DisplayManager();

    QVector<DisplayInfo> _screens() const;
    QScreen *_screen_for_name(const QString &name) const;
    QScreen *_primary_screen() const;

signals:
    void displays_changed();

private:
    void _connect_screen(QScreen *screen);
    QSet<QScreen *> _connected_screens;
};

}

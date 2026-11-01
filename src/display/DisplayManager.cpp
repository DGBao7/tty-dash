#include "display/DisplayManager.h"

#include <QGuiApplication>
#include <QSet>

namespace ttydash {

DisplayManager::DisplayManager() {
    const QList<QScreen *> screens = QGuiApplication::screens();
    for (QScreen *screen : screens) {
        _connect_screen(screen);
    }

    QGuiApplication *application = qobject_cast<QGuiApplication *>(QCoreApplication::instance());
    connect(application , &QGuiApplication::screenAdded , this , [this](QScreen *screen) {
        _connect_screen(screen);
        emit displays_changed();
    });
    connect(application , &QGuiApplication::screenRemoved , this , [this](QScreen *screen) {
        _connected_screens.remove(screen);
        emit displays_changed();
    });
}

QVector<DisplayInfo> DisplayManager::_screens() const {
    QVector<DisplayInfo> result;
    const QScreen *primary_screen = QGuiApplication::primaryScreen();

    for (QScreen *screen : QGuiApplication::screens()) {
        DisplayInfo info;
        info.name = screen->name();
        info.geometry = screen->geometry();
        info.available_geometry = screen->availableGeometry();
        info.device_pixel_ratio = screen->devicePixelRatio();
        info.primary = screen == primary_screen;
        info.screen = screen;
        result.append(info);
    }

    return result;
}

QScreen *DisplayManager::_screen_for_name(const QString &name) const {
    if (name.isEmpty() || name.compare(QStringLiteral("primary") , Qt::CaseInsensitive) == 0) {
        return _primary_screen();
    }

    const QList<QScreen *> screens = QGuiApplication::screens();
    for (QScreen *screen : screens) {
        if (screen->name().compare(name , Qt::CaseInsensitive) == 0) {
            return screen;
        }
    }

    return _primary_screen();
}

QScreen *DisplayManager::_primary_screen() const {
    return QGuiApplication::primaryScreen();
}

void DisplayManager::_connect_screen(QScreen *screen) {
    if (!screen || _connected_screens.contains(screen)) {
        return;
    }

    _connected_screens.insert(screen);
    connect(screen , &QScreen::geometryChanged , this , [this](const QRect &) {
        emit displays_changed();
    });
    connect(screen , &QScreen::availableGeometryChanged , this , [this](const QRect &) {
        emit displays_changed();
    });
}

}

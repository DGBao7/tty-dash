#pragma once

#include "config/ConfigManager.h"

#include <QColor>
#include <QString>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace ttydash {

class WindowEffects {
public:
#ifdef Q_OS_WIN
    bool _apply(HWND hwnd , const EffectsSettings &settings , const QString &backdrop_override , const QString &color_override) const;
    void _clear(HWND hwnd) const;
#endif
};

}

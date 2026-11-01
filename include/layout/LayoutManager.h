#pragma once

#include "config/ConfigManager.h"

#include <QRect>

namespace ttydash {

class LayoutManager {
public:
    QRect _window_geometry(const DisplayConfig &config , const QRect &screen_geometry) const;
    QRect _widget_geometry(const WidgetConfig &config , const QRect &window_geometry) const;

private:
    QRect _calculate(const QString &mode , int x , int y , int width , int height , const QMargins &margins , const QRect &bounds , bool fill_empty) const;
};

}

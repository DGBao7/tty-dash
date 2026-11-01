#include "layout/LayoutManager.h"

#include <algorithm>

namespace ttydash {

QRect LayoutManager::_window_geometry(const DisplayConfig &config , const QRect &screen_geometry) const {
    return _calculate(config.position_mode , config.x , config.y , config.width , config.height , config.margins , screen_geometry , true);
}

QRect LayoutManager::_widget_geometry(const WidgetConfig &config , const QRect &window_geometry) const {
    const QRect local_bounds(0 , 0 , window_geometry.width() , window_geometry.height());

    return _calculate(config.position_mode , config.x , config.y , config.width , config.height , config.margins , local_bounds , false);
}

QRect LayoutManager::_calculate(const QString &mode , int x , int y , int width , int height , const QMargins &margins , const QRect &bounds , bool fill_empty) const {
    if (bounds.isEmpty()) {
        return QRect();
    }

    const int usable_width = std::max(1 , bounds.width() - margins.left() - margins.right());
    const int usable_height = std::max(1 , bounds.height() - margins.top() - margins.bottom());
    const int resolved_width = std::clamp(width > 0 ? width : (fill_empty ? usable_width : 300) , 1 , usable_width);
    const int resolved_height = std::clamp(height > 0 ? height : (fill_empty ? usable_height : 120) , 1 , usable_height);
    int resolved_x = bounds.x() + margins.left();
    int resolved_y = bounds.y() + margins.top();

    if (mode == QStringLiteral("top_right")) {
        resolved_x = bounds.x() + bounds.width() - resolved_width - margins.right();
    } else if (mode == QStringLiteral("bottom_left")) {
        resolved_y = bounds.y() + bounds.height() - resolved_height - margins.bottom();
    } else if (mode == QStringLiteral("bottom_right")) {
        resolved_x = bounds.x() + bounds.width() - resolved_width - margins.right();
        resolved_y = bounds.y() + bounds.height() - resolved_height - margins.bottom();
    } else if (mode == QStringLiteral("center")) {
        resolved_x = bounds.x() + (bounds.width() - resolved_width) / 2;
        resolved_y = bounds.y() + (bounds.height() - resolved_height) / 2;
    } else if (mode == QStringLiteral("custom")) {
        resolved_x = bounds.x() + x;
        resolved_y = bounds.y() + y;
    }

    return QRect(resolved_x , resolved_y , resolved_width , resolved_height);
}

}

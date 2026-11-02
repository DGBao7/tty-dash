#include "layout/LayoutManager.h"

#include <algorithm>
namespace {
constexpr qreal empty_left_column_share = 0.30;
constexpr qreal center_column_share = 0.55;
constexpr qreal center_row_shares[]{0.32 , 0.24 , 0.18 , 0.26};
constexpr qreal right_row_shares[]{0.32 , 0.33 , 0.35};

int _empty_left_column_width(int screen_width) {
    return qRound(screen_width * empty_left_column_share);
}
}

namespace ttydash {

QRect LayoutManager::_window_geometry(const DisplayConfig &config , const QRect &screen_geometry) const {
    return _calculate(config.position_mode , config.x , config.y , config.width , config.height , config.margins , screen_geometry , true);
}

QRect LayoutManager::_widget_geometry(const WidgetConfig &config , const QRect &window_geometry) const {
    const QRect local_bounds(0 , 0 , window_geometry.width() , window_geometry.height());

    if (config.position_mode == QStringLiteral("adaptive_right")) {
        const QString type = config.type.toLower();
        const QStringList center_types{QStringLiteral("cpu") , QStringLiteral("ram") , QStringLiteral("network") , QStringLiteral("disk")};
        const QStringList right_types{QStringLiteral("clock") , QStringLiteral("matrix") , QStringLiteral("audio_visualizer")};
        const int row = center_types.indexOf(type) >= 0 ? center_types.indexOf(type) : right_types.indexOf(type);
        if (row < 0) return _calculate(QStringLiteral("custom") , config.x , config.y , config.width , config.height , config.margins , local_bounds , false);

        const int left_width = _empty_left_column_width(local_bounds.width());
        const int center_width = qRound((local_bounds.width() - left_width) * center_column_share);
        const int gap = 4;
        const bool center = center_types.contains(type);
        const int column_x = center ? left_width : left_width + center_width;
        const int column_width = center ? center_width : local_bounds.width() - left_width - center_width;
        const int column_gap = std::min(gap , std::max(0 , column_width - 1));
        const int inner_x = column_x + column_gap / 2;
        const int inner_width = std::max(1 , column_width - column_gap);
        const int row_count = center ? 4 : 3;
        const int row_gap = center ? 2 : gap;
        const int content_height = std::max(1 , local_bounds.height() - row_gap * (row_count - 1));
        const qreal *row_shares = center ? center_row_shares : right_row_shares;
        int y = 0;
        for (int previous_row = 0; previous_row < row; ++previous_row) {
            y += qRound(content_height * row_shares[previous_row]) + row_gap;
        }
        const int row_height = std::max(32 , qRound(content_height * row_shares[row]));
        const int height = row == row_count - 1 ? std::max(32 , local_bounds.height() - y) : row_height;
        return QRect(inner_x , y , inner_width , height);

    }

    return _calculate(config.position_mode , config.x , config.y , config.width , config.height , config.margins , local_bounds , false);
}

QRect LayoutManager::_terbon_geometry(const TerBonSettings &config , const QRect &screen_geometry) const {
    if (screen_geometry.isEmpty()) return {};
    constexpr int width = 700;
    constexpr int height = 350;
    QRect geometry;
    if (config.remember_geometry && config.has_saved_geometry) {
        geometry = QRect(config.x , config.y , width , height);
    } else if (config.position == QStringLiteral("custom")) {
        geometry = QRect(config.x , config.y , width , height);
    } else {
        geometry = QRect(screen_geometry.right() - width - 24 , screen_geometry.top() + (screen_geometry.height() - height) / 2 , width , height);
    }
    geometry.setSize(QSize(width , height));
    const int max_x = std::max(screen_geometry.left() , screen_geometry.right() - width + 1);
    const int max_y = std::max(screen_geometry.top() , screen_geometry.bottom() - height + 1);
    geometry.moveLeft(std::clamp(geometry.left() , screen_geometry.left() , max_x));
    geometry.moveTop(std::clamp(geometry.top() , screen_geometry.top() , max_y));
    return geometry;
}

QRect LayoutManager::_calculate(const QString &mode , int x , int y , int width , int height , const QMargins &margins , const QRect &bounds , bool fill_empty) const {
    if (bounds.isEmpty()) {
        return QRect();
    }

    const int left_margin = std::clamp(margins.left() , 0 , std::max(0 , bounds.width() - 1));
    const int right_margin = std::clamp(margins.right() , 0 , std::max(0 , bounds.width() - left_margin - 1));
    const int top_margin = std::clamp(margins.top() , 0 , std::max(0 , bounds.height() - 1));
    const int bottom_margin = std::clamp(margins.bottom() , 0 , std::max(0 , bounds.height() - top_margin - 1));
    const int usable_width = std::max(1 , bounds.width() - left_margin - right_margin);
    const int usable_height = std::max(1 , bounds.height() - top_margin - bottom_margin);
    const int resolved_width = std::clamp(width > 0 ? width : (fill_empty ? usable_width : 300) , 1 , usable_width);
    const int resolved_height = std::clamp(height > 0 ? height : (fill_empty ? usable_height : 120) , 1 , usable_height);
    int resolved_x = bounds.x() + left_margin;
    int resolved_y = bounds.y() + top_margin;

    if (mode == QStringLiteral("top_right")) {
        resolved_x = bounds.x() + bounds.width() - resolved_width - right_margin;
    } else if (mode == QStringLiteral("bottom_left")) {
        resolved_y = bounds.y() + bounds.height() - resolved_height - bottom_margin;
    } else if (mode == QStringLiteral("bottom_right")) {
        resolved_x = bounds.x() + bounds.width() - resolved_width - right_margin;
        resolved_y = bounds.y() + bounds.height() - resolved_height - bottom_margin;
    } else if (mode == QStringLiteral("center")) {
        resolved_x = bounds.x() + (bounds.width() - resolved_width) / 2;
        resolved_y = bounds.y() + (bounds.height() - resolved_height) / 2;
    } else if (mode == QStringLiteral("custom")) {
        resolved_x = std::clamp(bounds.x() + x , bounds.x() + left_margin , bounds.x() + bounds.width() - right_margin - resolved_width);
        resolved_y = std::clamp(bounds.y() + y , bounds.y() + top_margin , bounds.y() + bounds.height() - bottom_margin - resolved_height);
    }

    return QRect(resolved_x , resolved_y , resolved_width , resolved_height);
}

}

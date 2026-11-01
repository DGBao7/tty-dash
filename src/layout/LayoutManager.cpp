#include "layout/LayoutManager.h"

#include <algorithm>
#include <QStringList>
#include <QVector>

namespace ttydash {

QRect LayoutManager::_window_geometry(const DisplayConfig &config , const QRect &screen_geometry) const {
    return _calculate(config.position_mode , config.x , config.y , config.width , config.height , config.margins , screen_geometry , true);
}

QRect LayoutManager::_widget_geometry(const WidgetConfig &config , const QRect &window_geometry) const {
    const QRect local_bounds(0 , 0 , window_geometry.width() , window_geometry.height());

    if (config.position_mode == QStringLiteral("adaptive_right")) {
        const int margin = 28;
        const int gap = 14;
        const int usable_width = std::max(1 , local_bounds.width() - margin * 2);
        const int usable_height = std::max(1 , local_bounds.height() - margin * 2);
        const int right_edge = local_bounds.right() + 1 - margin;
        const bool narrow = local_bounds.width() < 1180;

        if (narrow) {
            const QStringList types{
                QStringLiteral("clock") , QStringLiteral("cpu") , QStringLiteral("ram") ,
                QStringLiteral("network") , QStringLiteral("matrix") , QStringLiteral("pipes")
            };
            const QVector<int> preferred{112 , 104 , 104 , 104 , 170 , 150};
            const int index = types.indexOf(config.type.toLower());
            if (index < 0) return _calculate(QStringLiteral("custom") , config.x , config.y , config.width , config.height , config.margins , local_bounds , false);
            const int total_gaps = gap * (types.size() - 1);
            const int target_total = std::max(static_cast<int>(types.size()) * 32 , usable_height - total_gaps);
            int preferred_total = 0;
            for (const int value : preferred) preferred_total += value;
            const qreal scale = std::min(1.0 , static_cast<qreal>(target_total) / preferred_total);
            int y = margin;
            for (int i = 0; i < index; ++i) y += std::max(32 , static_cast<int>(preferred.at(i) * scale)) + gap;
            const int height = std::max(32 , static_cast<int>(preferred.at(index) * scale));
            const int width = std::clamp(config.width , std::min(160 , usable_width) , usable_width);
            return QRect(right_edge - width + 1 , y , width , height);
        }

        const int right_column_width = std::clamp(config.width , std::min(160 , usable_width) , std::min(usable_width , std::max(240 , static_cast<int>(usable_width * 0.36))));
        const int animation_width = std::clamp(config.type.compare(QStringLiteral("pipes") , Qt::CaseInsensitive) == 0 ? config.width : 420 , 240 , std::max(240 , static_cast<int>(usable_width * 0.38)));
        const int column_gap = 28;
        const int right_column_left = right_edge - right_column_width + 1;
        const int animation_right = right_column_left - column_gap - 1;
        const QString type = config.type.toLower();

        if (type == QStringLiteral("clock") || type == QStringLiteral("cpu") || type == QStringLiteral("ram") || type == QStringLiteral("network")) {
            const QStringList types{QStringLiteral("clock") , QStringLiteral("cpu") , QStringLiteral("ram") , QStringLiteral("network")};
            const QVector<int> preferred{112 , 112 , 112 , 112};
            const int index = types.indexOf(type);
            const int row_gap = 14;
            const int total = static_cast<int>(preferred.size()) * 112 + (static_cast<int>(preferred.size()) - 1) * row_gap;
            const int start_y = std::max(margin , (local_bounds.height() - total) / 2);
            const int height = std::min(config.height , std::max(48 , (usable_height - (static_cast<int>(preferred.size()) - 1) * row_gap) / static_cast<int>(preferred.size())));
            int y = start_y;
            for (int i = 0; i < index; ++i) y += height + row_gap;
            const int width = std::min(right_column_width , usable_width);
            return QRect(right_edge - width + 1 , y , width , height);
        }

        const bool matrix = type == QStringLiteral("matrix");
        const bool pipes = type == QStringLiteral("pipes");
        if (matrix || pipes) {
            const int requested_matrix = 500;
            const int requested_pipes = 220;
            const int animation_gap = 16;
            const int available = std::max(64 , usable_height - animation_gap);
            const qreal scale = std::min(1.0 , static_cast<qreal>(available) / (requested_matrix + requested_pipes));
            const int matrix_height = std::max(32 , static_cast<int>(requested_matrix * scale));
            const int pipes_height = std::max(32 , std::min(static_cast<int>(requested_pipes * scale) , available - matrix_height));
            const int stack_height = matrix_height + animation_gap + pipes_height;
            const int y = std::max(margin , (local_bounds.height() - stack_height) / 2);
            const int width = std::min(animation_width , std::max(1 , animation_right + 1 - margin));
            const int x = animation_right - width + 1;
            return QRect(x , matrix ? y : y + matrix_height + animation_gap , width , matrix ? matrix_height : pipes_height);
        }

        return _calculate(QStringLiteral("custom") , config.x , config.y , config.width , config.height , config.margins , local_bounds , false);
    }

    return _calculate(config.position_mode , config.x , config.y , config.width , config.height , config.margins , local_bounds , false);
}

QRect LayoutManager::_terbon_geometry(const TerBonSettings &config , const QRect &screen_geometry) const {
    if (screen_geometry.isEmpty()) return {};
    const int max_width = std::max(1 , screen_geometry.width() - 32);
    const int max_height = std::max(1 , screen_geometry.height() - 32);
    const int width = std::clamp(config.width , std::min(420 , max_width) , max_width);
    const int height = std::clamp(config.height , std::min(260 , max_height) , max_height);
    QRect geometry;
    if (config.remember_geometry && config.has_saved_geometry) {
        geometry = QRect(config.x , config.y , width , height);
    } else if (config.position == QStringLiteral("custom")) {
        geometry = QRect(config.x , config.y , width , height);
    } else {
        geometry = QRect(screen_geometry.right() - width - 24 , screen_geometry.top() + (screen_geometry.height() - height) / 2 , width , height);
    }
    geometry.setSize(QSize(width , height));
    if (geometry.left() < screen_geometry.left()) geometry.moveLeft(screen_geometry.left());
    if (geometry.top() < screen_geometry.top()) geometry.moveTop(screen_geometry.top());
    if (geometry.right() > screen_geometry.right()) geometry.moveRight(screen_geometry.right());
    if (geometry.bottom() > screen_geometry.bottom()) geometry.moveBottom(screen_geometry.bottom());
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

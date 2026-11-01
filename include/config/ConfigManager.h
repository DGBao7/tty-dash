#pragma once

#include <QColor>
#include <QMargins>
#include <QRect>
#include <QVector>
#include <QJsonObject>
#include <QString>

namespace ttydash {

struct WidgetConfig {
    QString id;
    QString type;
    bool enabled = true;
    QString position_mode = QStringLiteral("custom");
    int x = 0;
    int y = 0;
    int width = 300;
    int height = 120;
    QMargins margins;
    QJsonObject options;
};

struct DisplayConfig {
    QString name = QStringLiteral("primary");
    QString position_mode = QStringLiteral("top_left");
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    QMargins margins;
    qreal opacity = -1.0;
    QString backdrop;
    QString background_color;
    QVector<WidgetConfig> widgets;
};

struct ApplicationSettings {
    bool always_on_top = false;
    bool click_through = true;
    qreal opacity = 0.92;
    int monitoring_interval_ms = 1500;
};

struct EffectsSettings {
    bool blur = true;
    QString backdrop = QStringLiteral("acrylic");
    QString background_color = QStringLiteral("#66071016");
};

struct DashboardConfig {
    ApplicationSettings application;
    EffectsSettings effects;
    QVector<DisplayConfig> displays;
};

class ConfigManager {
public:
    ConfigManager();

    DashboardConfig _load();
    bool _save();
    bool _set_click_through(bool enabled);
    bool _update_widget_geometry(const QString &display_name , const QString &widget_id , const QRect &geometry);
    QString _path() const;

private:
    DashboardConfig _default_config() const;
    bool _parse(const QByteArray &json_data , DashboardConfig &config) const;
    QByteArray _serialize() const;
    QMargins _parse_margins(const QJsonObject &object) const;
    QString _config_path;
    DashboardConfig _config;
};

}

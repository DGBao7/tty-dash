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
    QJsonObject extra;
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
    QJsonObject extra;
};

struct ApplicationSettings {
    bool always_on_top = false;
    bool click_through = true;
    qreal opacity = 1.0;
    int monitoring_interval_ms = 1500;
    QJsonObject extra;
};

struct EffectsSettings {
    bool blur = false;
    QString backdrop = QStringLiteral("none");
    QString background_color = QStringLiteral("#00000000");
    QJsonObject extra;
};

struct TerBonSettings {
    bool enabled = true;
    bool visible_on_startup = true;
    QString monitor = QStringLiteral("primary");
    QString position = QStringLiteral("right");
    int width = 700;
    int height = 350;
    QString initial_working_directory = QStringLiteral("D:/");
    bool remember_geometry = true;
    bool has_saved_geometry = false;
    int x = 0;
    int y = 0;
    QJsonObject extra;
};

struct DashboardConfig {
    int schema_version = 5;
    ApplicationSettings application;
    EffectsSettings effects;
    TerBonSettings terbon;
    QVector<DisplayConfig> displays;
    QJsonObject extra;
};

class ConfigManager {
public:
    ConfigManager();

    DashboardConfig _load();
    bool _save();
    bool _set_click_through(bool enabled);
    bool _update_widget_geometry(const QString &display_name , const QString &widget_id , const QRect &geometry);
    bool _update_terbon_settings(const TerBonSettings &settings);
    bool _set_terbon_visible_on_startup(bool visible);
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

#include "config/ConfigManager.h"

#include "utils/Logger.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QSaveFile>
#include <QSet>
#include <QStandardPaths>
#include <QCoreApplication>

#include <algorithm>

namespace ttydash {
namespace {

int _integer(const QJsonObject &object , const QString &key , int fallback) {
    const QJsonValue value = object.value(key);

    if (!value.isDouble()) {
        return fallback;
    }

    return value.toInt(fallback);
}

qreal _bounded_opacity(const QJsonValue &value , qreal fallback) {
    if (!value.isDouble()) {
        return fallback;
    }

    return std::clamp(value.toDouble() , 0.0 , 1.0);
}

QString _valid_backdrop(const QString &backdrop) {
    if (backdrop == QStringLiteral("acrylic") || backdrop == QStringLiteral("mica")) {
        return backdrop;
    }

    return QStringLiteral("none");
}

QString _valid_position_mode(const QString &mode , const QString &fallback) {
    static const QSet<QString> modes{
        QStringLiteral("top_left") ,
        QStringLiteral("top_right") ,
        QStringLiteral("bottom_left") ,
        QStringLiteral("bottom_right") ,
        QStringLiteral("center") ,
        QStringLiteral("custom") ,
        QStringLiteral("adaptive_right")
    };
    const QString normalized = mode.toLower();

    return modes.contains(normalized) ? normalized : fallback;
}

QJsonObject _without_keys(const QJsonObject &source , const QStringList &keys) {
    QJsonObject result = source;
    for (const QString &key : keys) {
        result.remove(key);
    }
    return result;
}

}

ConfigManager::ConfigManager() {
    _config_path = qEnvironmentVariable("TTY_DASH_CONFIG");
    if (_config_path.isEmpty()) {
        _config_path = QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation))
            .filePath(QStringLiteral("dashboard.json"));
    }
}

DashboardConfig ConfigManager::_load() {
    QFile file(_config_path);
    QByteArray contents;

    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        contents = file.readAll();
        file.close();
    } else {
        const QString portable_path = QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("dashboard.json"));
        QFile portable(portable_path);
        if (portable.open(QIODevice::ReadOnly)) {
            _config_path = portable_path;
            contents = portable.readAll();
        }

        QFile defaults(QStringLiteral(":/configs/dashboard.json"));

        if (contents.isEmpty() && defaults.open(QIODevice::ReadOnly)) {
            contents = defaults.readAll();
        }
    }

    DashboardConfig loaded;
    if (contents.isEmpty() || !_parse(contents , loaded)) {
        if (!contents.isEmpty()) {
            Logger::_warning(QStringLiteral("Configuration is invalid; restoring defaults from %1").arg(_config_path));
        }

        _config = _default_config();
        _save();

        return _config;
    }

    _config = loaded;
    _save();

    return _config;
}

bool ConfigManager::_save() {
    const QByteArray data = _serialize();
    QString failure;
    const auto write_to_path = [&data , &failure](const QString &path) {
        const QFileInfo info(path);
        if (!QDir().mkpath(info.absolutePath())) {
            failure = QStringLiteral("could not create directory %1").arg(info.absolutePath());

            return false;
        }

        QSaveFile file(path);
        if (!file.open(QIODevice::WriteOnly)) {
            failure = file.errorString();

            return false;
        }
        if (file.write(data) != data.size() || !file.commit()) {
            failure = file.errorString();

            return false;
        }

        return true;
    };

    if (write_to_path(_config_path)) {
        return true;
    }

    Logger::_warning(QStringLiteral("Could not save configuration to %1: %2; trying a portable location")
        .arg(_config_path , failure));
    const QString portable_path = QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("dashboard.json"));
    if (portable_path != _config_path && write_to_path(portable_path)) {
        _config_path = portable_path;
        Logger::_info(QStringLiteral("Using portable configuration at %1").arg(_config_path));

        return true;
    }

    Logger::_error(QStringLiteral("Could not save configuration to a writable location: %1").arg(failure));

    return false;
}

bool ConfigManager::_set_click_through(bool enabled) {
    _config.application.click_through = enabled;

    return _save();
}

bool ConfigManager::_update_widget_geometry(const QString &display_name , const QString &widget_id , const QRect &geometry) {
    for (DisplayConfig &display : _config.displays) {
        if (display.name != display_name) {
            continue;
        }

        for (WidgetConfig &widget : display.widgets) {
            if (widget.id != widget_id) {
                continue;
            }

            widget.position_mode = QStringLiteral("custom");
            widget.x = geometry.x();
            widget.y = geometry.y();
            widget.width = geometry.width();
            widget.height = geometry.height();

            return _save();
        }
    }

    return false;
}

bool ConfigManager::_update_terbon_settings(const TerBonSettings &settings) {
    _config.terbon = settings;
    return _save();
}

bool ConfigManager::_set_terbon_visible_on_startup(bool visible) {
    _config.terbon.visible_on_startup = visible;
    return _save();
}

QString ConfigManager::_path() const {
    return _config_path;
}

DashboardConfig ConfigManager::_default_config() const {
    DashboardConfig config;
    DisplayConfig display;
    display.name = QStringLiteral("primary");

    const auto add_widget = [&display](const QString &id , const QString &type , int x , int y , int width , int height , const QString &mode = QStringLiteral("custom")) {
        WidgetConfig widget;
        widget.id = id;
        widget.type = type;
        widget.x = x;
        widget.y = y;
        widget.width = width;
        widget.height = height;
        widget.position_mode = mode;
        display.widgets.append(widget);
    };

    add_widget(QStringLiteral("clock") , QStringLiteral("clock") , 0 , 0 , 340 , 112 , QStringLiteral("adaptive_right"));
    add_widget(QStringLiteral("cpu") , QStringLiteral("cpu") , 0 , 0 , 290 , 112 , QStringLiteral("adaptive_right"));
    add_widget(QStringLiteral("ram") , QStringLiteral("ram") , 0 , 0 , 290 , 112 , QStringLiteral("adaptive_right"));
    add_widget(QStringLiteral("network") , QStringLiteral("network") , 0 , 0 , 290 , 112 , QStringLiteral("adaptive_right"));
    add_widget(QStringLiteral("matrix") , QStringLiteral("matrix") , 0 , 0 , 420 , 500 , QStringLiteral("adaptive_right"));
    add_widget(QStringLiteral("pipes") , QStringLiteral("pipes") , 0 , 0 , 400 , 220 , QStringLiteral("adaptive_right"));
    config.displays.append(display);

    return config;
}

bool ConfigManager::_parse(const QByteArray &json_data , DashboardConfig &config) const {
    QJsonParseError parse_error;
    const QJsonDocument document = QJsonDocument::fromJson(json_data , &parse_error);
    if (parse_error.error != QJsonParseError::NoError || !document.isObject()) {
        Logger::_error(QStringLiteral("JSON parse error at byte %1: %2").arg(parse_error.offset).arg(parse_error.errorString()));

        return false;
    }

    const QJsonObject root = document.object();
    if (!root.value(QStringLiteral("application")).isObject() || !root.value(QStringLiteral("displays")).isArray()) {
        Logger::_error(QStringLiteral("Configuration must contain application and displays objects"));

        return false;
    }

    const int schema_version = _integer(root , QStringLiteral("schema_version") , 1);
    config.schema_version = 5;
    config.extra = _without_keys(root , {QStringLiteral("schema_version") , QStringLiteral("application") , QStringLiteral("effects") , QStringLiteral("displays") , QStringLiteral("terbon")});

    const QJsonObject application = root.value(QStringLiteral("application")).toObject();
    config.application.always_on_top = application.value(QStringLiteral("always_on_top")).toBool(false);
    config.application.click_through = application.value(QStringLiteral("click_through")).toBool(true);
    config.application.opacity = _bounded_opacity(application.value(QStringLiteral("opacity")) , 1.0);
    config.application.monitoring_interval_ms = std::clamp(_integer(application , QStringLiteral("monitoring_interval_ms") , 1500) , 500 , 10000);
    config.application.extra = _without_keys(application , {QStringLiteral("always_on_top") , QStringLiteral("click_through") , QStringLiteral("opacity") , QStringLiteral("monitoring_interval_ms")});

    const QJsonObject effects = root.value(QStringLiteral("effects")).toObject();
    config.effects.blur = effects.value(QStringLiteral("blur")).toBool(false);
    config.effects.backdrop = _valid_backdrop(effects.value(QStringLiteral("backdrop")).toString(QStringLiteral("none")));
    const QString background = effects.value(QStringLiteral("background_color")).toString(QStringLiteral("#00000000"));
    if (QColor(background).isValid()) {
        config.effects.background_color = background;
    }
    config.effects.extra = _without_keys(effects , {QStringLiteral("blur") , QStringLiteral("backdrop") , QStringLiteral("background_color")});

    const QJsonObject terminal = root.value(QStringLiteral("terbon")).toObject();
    config.terbon.extra = _without_keys(terminal , {QStringLiteral("enabled") , QStringLiteral("visible_on_startup") , QStringLiteral("monitor") , QStringLiteral("position") , QStringLiteral("width") , QStringLiteral("height") , QStringLiteral("initial_working_directory") , QStringLiteral("remember_geometry") , QStringLiteral("has_saved_geometry") , QStringLiteral("x") , QStringLiteral("y")});
    config.terbon.enabled = terminal.value(QStringLiteral("enabled")).toBool(true);
    config.terbon.visible_on_startup = terminal.value(QStringLiteral("visible_on_startup")).toBool(true);
    config.terbon.monitor = terminal.value(QStringLiteral("monitor")).toString(QStringLiteral("primary")).trimmed();
    if (config.terbon.monitor.isEmpty()) config.terbon.monitor = QStringLiteral("primary");
    config.terbon.position = terminal.value(QStringLiteral("position")).toString(QStringLiteral("right")).toLower();
    if (config.terbon.position != QStringLiteral("right") && config.terbon.position != QStringLiteral("custom")) config.terbon.position = QStringLiteral("right");
    config.terbon.width = std::clamp(_integer(terminal , QStringLiteral("width") , 700) , 420 , 4000);
    config.terbon.height = std::clamp(_integer(terminal , QStringLiteral("height") , 350) , 260 , 3000);
    config.terbon.initial_working_directory = terminal.value(QStringLiteral("initial_working_directory")).toString(QStringLiteral("D:/")).trimmed();
    if (config.terbon.initial_working_directory.isEmpty()) config.terbon.initial_working_directory = QStringLiteral("D:/");
    config.terbon.remember_geometry = terminal.value(QStringLiteral("remember_geometry")).toBool(true);
    config.terbon.has_saved_geometry = terminal.value(QStringLiteral("has_saved_geometry")).toBool(terminal.contains(QStringLiteral("x")) && terminal.contains(QStringLiteral("y")));
    config.terbon.x = _integer(terminal , QStringLiteral("x") , 0);
    config.terbon.y = _integer(terminal , QStringLiteral("y") , 0);

    QSet<QString> display_names;
    const QJsonArray display_array = root.value(QStringLiteral("displays")).toArray();
    for (const QJsonValue &display_value : display_array) {
        if (!display_value.isObject()) {
            continue;
        }

        const QJsonObject object = display_value.toObject();
        if (!object.value(QStringLiteral("widgets")).isArray()) {
            continue;
        }

        DisplayConfig display;
        display.name = object.value(QStringLiteral("name")).toString(QStringLiteral("primary"));
        display.position_mode = _valid_position_mode(object.value(QStringLiteral("position_mode")).toString(QStringLiteral("top_left")) , QStringLiteral("top_left"));
        display.x = _integer(object , QStringLiteral("x") , 0);
        display.y = _integer(object , QStringLiteral("y") , 0);
        display.width = std::max(0 , _integer(object , QStringLiteral("width") , 0));
        display.height = std::max(0 , _integer(object , QStringLiteral("height") , 0));
        display.margins = _parse_margins(object.value(QStringLiteral("margins")).toObject());
        display.opacity = _bounded_opacity(object.value(QStringLiteral("opacity")) , -1.0);
        display.backdrop = object.value(QStringLiteral("backdrop")).toString();
        if (!display.backdrop.isEmpty()) {
            display.backdrop = _valid_backdrop(display.backdrop);
        }
        display.background_color = object.value(QStringLiteral("background_color")).toString();
        if (!QColor(display.background_color).isValid()) {
            display.background_color.clear();
        }

        if (display_names.contains(display.name)) {
            Logger::_warning(QStringLiteral("Ignoring duplicate display configuration %1").arg(display.name));

            continue;
        }
        display.extra = _without_keys(object , {QStringLiteral("name") , QStringLiteral("position_mode") , QStringLiteral("x") , QStringLiteral("y") , QStringLiteral("width") , QStringLiteral("height") , QStringLiteral("margins") , QStringLiteral("opacity") , QStringLiteral("backdrop") , QStringLiteral("background_color") , QStringLiteral("widgets")});
        display_names.insert(display.name);

        QSet<QString> widget_ids;
        const QJsonArray widget_array = object.value(QStringLiteral("widgets")).toArray();
        for (qsizetype index = 0; index < widget_array.size(); ++index) {
            if (!widget_array.at(index).isObject()) {
                continue;
            }

            const QJsonObject widget_object = widget_array.at(index).toObject();
            WidgetConfig widget;
            widget.type = widget_object.value(QStringLiteral("type")).toString().trimmed().toLower();
            if (widget.type.isEmpty()) {
                continue;
            }

            widget.id = widget_object.value(QStringLiteral("id")).toString(widget.type);
            if (widget_ids.contains(widget.id)) {
                widget.id = QStringLiteral("%1-%2").arg(widget.type).arg(index);
            }
            widget_ids.insert(widget.id);
            widget.enabled = widget_object.value(QStringLiteral("enabled")).toBool(true);
            const QJsonObject position = widget_object.value(QStringLiteral("position")).toObject();
            const QJsonObject size = widget_object.value(QStringLiteral("size")).toObject();
            widget.position_mode = _valid_position_mode(position.value(QStringLiteral("mode")).toString(widget_object.value(QStringLiteral("position_mode")).toString(QStringLiteral("custom"))) , QStringLiteral("custom"));
            widget.x = _integer(position , QStringLiteral("x") , _integer(widget_object , QStringLiteral("x") , 0));
            widget.y = _integer(position , QStringLiteral("y") , _integer(widget_object , QStringLiteral("y") , 0));
            widget.width = std::clamp(_integer(size , QStringLiteral("width") , _integer(widget_object , QStringLiteral("width") , 300)) , 40 , 10000);
            widget.height = std::clamp(_integer(size , QStringLiteral("height") , _integer(widget_object , QStringLiteral("height") , 120)) , 32 , 10000);
            widget.margins = _parse_margins(position.value(QStringLiteral("margins")).toObject());
            widget.options = widget_object.value(QStringLiteral("options")).toObject();
            widget.extra = _without_keys(widget_object , {QStringLiteral("id") , QStringLiteral("type") , QStringLiteral("enabled") , QStringLiteral("position") , QStringLiteral("position_mode") , QStringLiteral("x") , QStringLiteral("y") , QStringLiteral("size") , QStringLiteral("width") , QStringLiteral("height") , QStringLiteral("options") , QStringLiteral("color") , QStringLiteral("font_family") , QStringLiteral("font_size") , QStringLiteral("display_font_size") , QStringLiteral("panel_color") , QStringLiteral("text_color") , QStringLiteral("show_graph") , QStringLiteral("animation_speed") , QStringLiteral("interval_ms") , QStringLiteral("density") , QStringLiteral("time_format") , QStringLiteral("date_format") , QStringLiteral("speed_unit")});

            const auto copy_option = [&widget , &widget_object](const QString &key) {
                if (widget_object.contains(key)) {
                    widget.options.insert(key , widget_object.value(key));
                }
            };
            copy_option(QStringLiteral("color"));
            copy_option(QStringLiteral("font_family"));
            copy_option(QStringLiteral("font_size"));
            copy_option(QStringLiteral("display_font_size"));
            copy_option(QStringLiteral("panel_color"));
            copy_option(QStringLiteral("text_color"));
            copy_option(QStringLiteral("show_graph"));
            copy_option(QStringLiteral("animation_speed"));
            copy_option(QStringLiteral("interval_ms"));
            copy_option(QStringLiteral("density"));
            copy_option(QStringLiteral("time_format"));
            copy_option(QStringLiteral("date_format"));
            copy_option(QStringLiteral("speed_unit"));
            display.widgets.append(widget);
        }

        config.displays.append(display);
    }

    if (config.displays.isEmpty()) {
        Logger::_warning(QStringLiteral("No valid displays were configured; using the primary display"));
        config.displays = _default_config().displays;
    }

    if (schema_version < 3) {
        config.application.opacity = 1.0;
        config.effects.blur = false;
        config.effects.backdrop = QStringLiteral("none");
        config.effects.background_color = QStringLiteral("#00000000");
        for (DisplayConfig &display : config.displays) {
            display.opacity = -1.0;
            display.backdrop = QStringLiteral("none");
            display.background_color = QStringLiteral("#00000000");
            for (WidgetConfig &widget : display.widgets) {
                const bool old_default =
                    (widget.id == QStringLiteral("clock") && widget.x == 32 && widget.y == 30) ||
                    (widget.id == QStringLiteral("cpu") && widget.x == 32 && widget.y == 160) ||
                    (widget.id == QStringLiteral("ram") && widget.x == 32 && widget.y == 292) ||
                    (widget.id == QStringLiteral("network") && widget.x == 32 && widget.y == 424) ||
                    (widget.id == QStringLiteral("matrix") && widget.position_mode == QStringLiteral("top_right") && widget.x == 32 && widget.y == 32) ||
                    (widget.id == QStringLiteral("pipes") && widget.position_mode == QStringLiteral("bottom_right") && widget.x == 32 && widget.y == 32);
                if (old_default) widget.position_mode = QStringLiteral("adaptive_right");
            }
        }
    }

    if (schema_version < 5 && config.terbon.width == 700 && config.terbon.height == 420) {
        config.terbon.height = 350;
    }

    return true;
}

QByteArray ConfigManager::_serialize() const {
    QJsonObject application = _config.application.extra;
    application.insert(QStringLiteral("always_on_top") , _config.application.always_on_top);
    application.insert(QStringLiteral("click_through") , _config.application.click_through);
    application.insert(QStringLiteral("opacity") , _config.application.opacity);
    application.insert(QStringLiteral("monitoring_interval_ms") , _config.application.monitoring_interval_ms);

    QJsonObject effects = _config.effects.extra;
    effects.insert(QStringLiteral("blur") , _config.effects.blur);
    effects.insert(QStringLiteral("backdrop") , _config.effects.backdrop);
    effects.insert(QStringLiteral("background_color") , _config.effects.background_color);

    QJsonArray displays;
    for (const DisplayConfig &display : _config.displays) {
        QJsonObject object = display.extra;
        object.insert(QStringLiteral("name") , display.name);
        object.insert(QStringLiteral("position_mode") , display.position_mode);
        object.insert(QStringLiteral("x") , display.x);
        object.insert(QStringLiteral("y") , display.y);
        object.insert(QStringLiteral("width") , display.width);
        object.insert(QStringLiteral("height") , display.height);
        object.insert(QStringLiteral("margins") , QJsonObject{
            {QStringLiteral("left") , display.margins.left()} ,
            {QStringLiteral("top") , display.margins.top()} ,
            {QStringLiteral("right") , display.margins.right()} ,
            {QStringLiteral("bottom") , display.margins.bottom()}
        });
        if (display.opacity >= 0.0) {
            object.insert(QStringLiteral("opacity") , display.opacity);
        }
        if (!display.backdrop.isEmpty()) {
            object.insert(QStringLiteral("backdrop") , display.backdrop);
        }
        if (!display.background_color.isEmpty()) {
            object.insert(QStringLiteral("background_color") , display.background_color);
        }

        QJsonArray widgets;
        for (const WidgetConfig &widget : display.widgets) {
            QJsonObject widget_object = widget.extra;
            widget_object.insert(QStringLiteral("id") , widget.id);
            widget_object.insert(QStringLiteral("type") , widget.type);
            widget_object.insert(QStringLiteral("enabled") , widget.enabled);
            widget_object.insert(QStringLiteral("position") , QJsonObject{
                {QStringLiteral("mode") , widget.position_mode} ,
                {QStringLiteral("x") , widget.x} ,
                {QStringLiteral("y") , widget.y} ,
                {QStringLiteral("margins") , QJsonObject{
                    {QStringLiteral("left") , widget.margins.left()} ,
                    {QStringLiteral("top") , widget.margins.top()} ,
                    {QStringLiteral("right") , widget.margins.right()} ,
                    {QStringLiteral("bottom") , widget.margins.bottom()}
                }}
            });
            widget_object.insert(QStringLiteral("size") , QJsonObject{
                {QStringLiteral("width") , widget.width} ,
                {QStringLiteral("height") , widget.height}
            });
            widget_object.insert(QStringLiteral("options") , widget.options);
            widgets.append(widget_object);
        }

        object.insert(QStringLiteral("widgets") , widgets);
        displays.append(object);
    }

    QJsonObject root = _config.extra;
    root.insert(QStringLiteral("schema_version") , 5);
    root.insert(QStringLiteral("application") , application);
    root.insert(QStringLiteral("effects") , effects);
    root.insert(QStringLiteral("displays") , displays);
    const TerBonSettings &terminal = _config.terbon;
    QJsonObject terbon = terminal.extra;
    terbon.insert(QStringLiteral("enabled") , terminal.enabled);
    terbon.insert(QStringLiteral("visible_on_startup") , terminal.visible_on_startup);
    terbon.insert(QStringLiteral("monitor") , terminal.monitor);
    terbon.insert(QStringLiteral("position") , terminal.position);
    terbon.insert(QStringLiteral("width") , terminal.width);
    terbon.insert(QStringLiteral("height") , terminal.height);
    terbon.insert(QStringLiteral("initial_working_directory") , terminal.initial_working_directory);
    terbon.insert(QStringLiteral("remember_geometry") , terminal.remember_geometry);
    terbon.insert(QStringLiteral("has_saved_geometry") , terminal.has_saved_geometry);
    if (terminal.has_saved_geometry || terminal.position == QStringLiteral("custom")) {
        terbon.insert(QStringLiteral("x") , terminal.x);
        terbon.insert(QStringLiteral("y") , terminal.y);
    }
    root.insert(QStringLiteral("terbon") , terbon);

    return QJsonDocument(root).toJson(QJsonDocument::Indented);
}

QMargins ConfigManager::_parse_margins(const QJsonObject &object) const {
    return QMargins(
        std::max(0 , _integer(object , QStringLiteral("left") , 0)) ,
        std::max(0 , _integer(object , QStringLiteral("top") , 0)) ,
        std::max(0 , _integer(object , QStringLiteral("right") , 0)) ,
        std::max(0 , _integer(object , QStringLiteral("bottom") , 0))
    );
}

}

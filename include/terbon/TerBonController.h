#pragma once

#include "config/ConfigManager.h"

#include <QObject>
#include <vector>

namespace ttydash {
class DisplayManager;
class LayoutManager;
class ConfigManager;
}

namespace terbon {

class TerBonWindow;

class TerBonController : public QObject {
    Q_OBJECT

public:
    TerBonController(const ttydash::TerBonSettings &settings , ttydash::DisplayManager &display_manager , ttydash::LayoutManager &layout_manager , ttydash::ConfigManager &config_manager , QObject *parent = nullptr);
    ~TerBonController() override;

    void _toggle_visibility();
    void _apply_settings(const ttydash::TerBonSettings &settings);
    void _screen_configuration_changed();
    bool _visible() const;

private:
    void _set_visible(bool visible , bool persist_startup);
    void _position_window();
    void _handle_command(const QString &command);

    ttydash::TerBonSettings _settings;
    ttydash::DisplayManager &_display_manager;
    ttydash::LayoutManager &_layout_manager;
    ttydash::ConfigManager &_config_manager;
    TerBonWindow *_window = nullptr;
    std::vector<std::string> _pending_command;
};

}

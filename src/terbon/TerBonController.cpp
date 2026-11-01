#include "terbon/TerBonController.h"

#include "display/DisplayManager.h"
#include "layout/LayoutManager.h"
#include "terbon/TerBonWindow.h"
#include "terbon/pas/Command.h"
#include "terbon/str/AnalizeStr.h"
#include "terbon/str/AnalyzePath.h"
#include "terbon/ui/StatusBar.h"
#include "terbon/ui/TerminalWidget.h"

#include <QScreen>
#include <QWindow>
#include <QDir>
#include <QCoreApplication>

namespace terbon {

TerBonController::TerBonController(const ttydash::TerBonSettings &settings , ttydash::DisplayManager &display_manager , ttydash::LayoutManager &layout_manager , ttydash::ConfigManager &config_manager , QObject *parent)
    : QObject(parent)
    , _settings(settings)
    , _display_manager(display_manager)
    , _layout_manager(layout_manager)
    , _config_manager(config_manager) {
    QString startup_warning;
    QString error;
    const QString initial_directory = _settings.initial_working_directory.trimmed().isEmpty()
        ? QStringLiteral("D:/")
        : _settings.initial_working_directory.trimmed();
    if (!Path::setWorkingDirectory(initial_directory , &error)) {
        QString fallback = QDir::homePath();
        QString fallback_error;
        bool fallback_ready = Path::setWorkingDirectory(fallback , &fallback_error);
        if (!fallback_ready) {
            fallback = QCoreApplication::applicationDirPath();
            fallback_ready = Path::setWorkingDirectory(fallback , &fallback_error);
        }
        if (!fallback_ready) {
            fallback = QDir::currentPath();
            fallback = QDir::toNativeSeparators(QDir::cleanPath(fallback));
            if (!fallback.endsWith(QDir::separator())) fallback.append(QDir::separator());
            Path::path = fallback.toStdString();
        }
        startup_warning = QStringLiteral("Warning: initial working directory '%1' is unavailable (%2). Using '%3'.")
            .arg(initial_directory , error , QString::fromStdString(Path::path));
    }

    _window = new TerBonWindow();
    connect(_window , &TerBonWindow::closed_by_user , this , [this]() {
        _set_visible(false , true);
    });
    connect(_window , &TerBonWindow::geometry_changed , this , [this](const QRect &geometry) {
        if (!_settings.remember_geometry) return;
        _settings.position = QStringLiteral("custom");
        _settings.has_saved_geometry = true;
        _settings.x = geometry.x();
        _settings.y = geometry.y();
        _settings.width = geometry.width();
        _settings.height = geometry.height();
        _config_manager._update_terbon_settings(_settings);
    });
    connect(_window->_terminal() , &TerminalWidget::CommandEntered , this , [this](const QString &command) {
        _handle_command(command);
    });
    if (!startup_warning.isEmpty()) {
        _window->_terminal()->ClearTerminal();
        _window->_terminal()->Print(startup_warning);
        _window->_terminal()->NewPrompt();
    }
    _position_window();
    if (_settings.enabled && _settings.visible_on_startup) _set_visible(true , false);
}

TerBonController::~TerBonController() {
    delete _window;
    _window = nullptr;
}

void TerBonController::_toggle_visibility() {
    if (!_settings.enabled) return;
    _set_visible(!_window->isVisible() , true);
}

void TerBonController::_apply_settings(const ttydash::TerBonSettings &settings) {
    _settings = settings;
    _position_window();
    if (!_settings.enabled) {
        _window->hide();
    } else if (_settings.visible_on_startup) {
        _set_visible(true , false);
    } else {
        _window->hide();
    }
}

void TerBonController::_screen_configuration_changed() {
    _position_window();
}

bool TerBonController::_visible() const {
    return _window && _window->isVisible();
}

void TerBonController::_set_visible(bool visible , bool persist_startup) {
    if (!_window || (visible && !_settings.enabled)) return;
    if (persist_startup) {
        _settings.visible_on_startup = visible;
        _config_manager._set_terbon_visible_on_startup(visible);
    }
    if (visible) {
        _position_window();
        _window->show();
        _window->raise();
        _window->activateWindow();
        _window->_terminal()->setFocus(Qt::OtherFocusReason);
    } else {
        _window->hide();
    }
}

void TerBonController::_position_window() {
    if (!_window) return;
    QScreen *screen = _display_manager._screen_for_name(_settings.monitor);
    if (!screen) screen = _display_manager._primary_screen();
    if (!screen) return;
    _window->winId();
    if (_window->windowHandle() && _window->windowHandle()->screen() != screen) _window->windowHandle()->setScreen(screen);
    _window->_set_fixed_geometry(_layout_manager._terbon_geometry(_settings , screen->availableGeometry()));
}

void TerBonController::_handle_command(const QString &command) {
    const std::vector<std::string> split_command = split(command.toStdString());
    if (split_command.empty()) return;
    TerminalWidget *terminal = _window->_terminal();

    if (!_pending_command.empty()) {
        if (split_command.size() == 1 && split_command.front() == "y") {
            execute(_pending_command , terminal , _window->_status_bar());
        } else {
            terminal->Print(QStringLiteral("Deletion cancelled"));
        }
        _pending_command.clear();
        return;
    }

    const std::string permission = check(split_command);
    if (permission == "allow") {
        execute(split_command , terminal , _window->_status_bar());
    } else if (permission == "confirm" && split_command.size() >= 3 && split_command.front() == "del") {
        _pending_command = split_command;
        terminal->Print(QStringLiteral("Are you sure del %1, master?").arg(QString::fromStdString(split_command[2])));
        terminal->Print(QStringLiteral("Confirm? (y/n)"));
    } else {
        terminal->Print(QStringLiteral("There is no executable command or it is forbidden"));
    }
}

}

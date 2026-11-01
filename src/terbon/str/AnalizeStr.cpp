#include "str/AnalizeStr.h"

#include "nlohmann/json.hpp"

#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <cctype>
#include <string>

namespace terbon {
namespace {

bool _is_number(const std::string &target) {
    if (target.empty()) return false;
    for (const unsigned char character : target) {
        if (!std::isdigit(character)) return false;
    }
    return true;
}

bool _read_command_config(QByteArray &contents) {
    const QStringList paths{
        QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("configs/commands.json")) ,
        QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("../configs/commands.json")) ,
        QStringLiteral(":/configs/commands.json")
    };
    for (const QString &path : paths) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            contents = file.readAll();
            return !contents.isEmpty();
        }
    }
    return false;
}

}

std::vector<std::string> split(std::string command) {
    std::vector<std::string> tokens;
    std::string token;
    char quote = '\0';
    for (const char character : command) {
        if ((character == '"' || character == '\'') && (quote == '\0' || quote == character)) {
            quote = quote == '\0' ? character : '\0';
            continue;
        }
        if (std::isspace(static_cast<unsigned char>(character)) && quote == '\0') {
            if (!token.empty()) {
                tokens.push_back(std::move(token));
                token.clear();
            }
            continue;
        }
        token.push_back(character);
    }
    if (!token.empty()) tokens.push_back(std::move(token));
    if (!tokens.empty() && tokens.front() == "master$") tokens.erase(tokens.begin());
    return tokens;
}

std::vector<std::string> buildSources(std::vector<std::string> split_command) {
    std::vector<std::string> sources;
    bool after_separator = false;
    for (const std::string &token : split_command) {
        if (token == "--") {
            after_separator = true;
            continue;
        }
        if (after_separator) sources.push_back(token);
    }
    return sources;
}

std::string check(std::vector<std::string> split_command) {
    if (split_command.empty()) return "null";

    QByteArray contents;
    if (!_read_command_config(contents)) return "null";

    try {
        const nlohmann::json data = nlohmann::json::parse(contents.constData() , contents.constData() + contents.size());
        if (!data.is_object()) return "null";
        const std::string &action = split_command.front();
        const auto command_it = data.find(action);
        if (command_it == data.end() || !command_it->is_object()) return "null";
        const nlohmann::json &command = *command_it;
        const auto length_it = command.find("len");
        if (length_it == command.end() || !length_it->is_number_integer()) return "null";
        const auto minimum_length = length_it->get<long long>();
        if (minimum_length < 1 || split_command.size() < static_cast<std::size_t>(minimum_length)) return "null";

        if (action == "cle" || action == "pwd" || action == "cd" || action == "dir" || (action == "help" && split_command.size() == 1)) return "allow";

        const std::string &target = split_command[1];
        if ((action == "tre" || action == "the") && _is_number(target)) return "allow";
        if (action == "help") return data.contains(target) ? "allow" : "null";

        const auto permission = command.find(target);
        if (permission == command.end() || !permission->is_string()) return "null";
        const std::string result = permission->get<std::string>();
        if (result == "allow" || result == "confirm") return result;
        return "null";
    } catch (const nlohmann::json::exception &) {
        return "null";
    }
}

}

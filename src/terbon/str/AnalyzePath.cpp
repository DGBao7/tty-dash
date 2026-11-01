#include "str/AnalyzePath.h"

#include <QDir>
#include <QStringList>

namespace terbon {
namespace Path {
std::string path = QDir::toNativeSeparators(QDir::homePath() + QStringLiteral("/" )).toStdString();
}

std::string analyzePath(std::vector<std::string> split_command) {
    if (split_command.size() < 2) return {};
    QStringList parts;
    for (std::size_t index = 1; index < split_command.size(); ++index) {
        parts.push_back(QString::fromStdString(split_command[index]));
    }
    const QString requested = parts.join(QDir::separator());
    const QString path_value = QDir::isAbsolutePath(requested)
        ? QDir::cleanPath(requested)
        : QDir::cleanPath(QDir(QString::fromStdString(Path::path)).filePath(requested));
    return QDir::toNativeSeparators(path_value).toStdString();
}

std::string takeTrueParentsPath(std::string path_value) {
    if (path_value.empty()) return path_value;
    QDir directory(QString::fromStdString(path_value));
    if (directory.cdUp()) path_value = QDir::toNativeSeparators(directory.absolutePath()).toStdString();
    if (!path_value.empty() && path_value.back() != '\\' && path_value.back() != '/') path_value.push_back(QDir::separator().toLatin1());
    return path_value;
}

}

#include "str/AnalyzePath.h"

#include <QDir>
#include <QFileInfo>
#include <QStorageInfo>
#include <QStringList>

namespace terbon {
namespace {

bool _is_drive_letter(QChar value) {
    const ushort code = value.toUpper().unicode();
    return code >= 'A' && code <= 'Z';
}

}

namespace Path {

std::string path = QStringLiteral("D:/").toStdString();

namespace {

QString _drive_root(QChar drive) {
    return QString(drive.toUpper()) + QStringLiteral(":/");
}

QString _resolve_candidate(const QString &requested_path , bool allow_single_drive_letter = false) {
    const QString requested = QDir::fromNativeSeparators(requested_path.trimmed());
    if (requested.isEmpty()) return {};

    if (requested.size() == 1 && allow_single_drive_letter && _is_drive_letter(requested.front())) {
        return _drive_root(requested.front());
    }

    if (requested.size() >= 2 && _is_drive_letter(requested.front())) {
        if (requested.at(1) == QLatin1Char(':')) {
            const QString root = _drive_root(requested.front());
            if (requested.size() == 2 || (requested.size() == 3 && requested.at(2) == QLatin1Char('/'))) return root;
            if (requested.at(2) == QLatin1Char('/')) return QDir::cleanPath(requested);
            return QDir::cleanPath(QDir(root).filePath(requested.mid(2)));
        }
    }

    if (QDir::isAbsolutePath(requested)) return QDir::cleanPath(requested);
    const QString current = QString::fromStdString(path);
    const QDir base(current.isEmpty() ? QDir::homePath() : current);
    return QDir::cleanPath(base.filePath(requested));
}

QString _drive_root_for_path(const QString &candidate) {
    if (candidate.size() >= 2 && _is_drive_letter(candidate.at(0)) && candidate.at(1) == QLatin1Char(':')) {
        return _drive_root(candidate.at(0));
    }
    return {};
}

QString _directory_error(const QString &candidate) {
    const QString drive_root = _drive_root_for_path(candidate);
    if (!drive_root.isEmpty()) {
        QStorageInfo storage(drive_root);
        storage.refresh();
        if (!storage.isValid() || !storage.isReady()) return QStringLiteral("Drive not found");
    }

    const QFileInfo info(candidate);
    if (!info.exists()) return QStringLiteral("Path not found");
    if (!info.isDir()) return QStringLiteral("Not a directory");
    if (!info.isReadable() || !QDir(candidate).isReadable()) return QStringLiteral("Permission denied");
    return {};
}

}

QString resolve(const QString &requested_path) {
    const QString candidate = _resolve_candidate(requested_path);
    return candidate.isEmpty() ? QString() : QDir::toNativeSeparators(candidate);
}

bool setWorkingDirectory(const QString &requested_path , QString *error) {
    const QString candidate = _resolve_candidate(requested_path , true);
    if (candidate.isEmpty()) {
        if (error) *error = QStringLiteral("Path not found");
        return false;
    }

    const QString failure = _directory_error(candidate);
    if (!failure.isEmpty()) {
        if (error) *error = failure;
        return false;
    }

    QString normalized = QDir::cleanPath(QFileInfo(candidate).absoluteFilePath());
    const QString drive_root = _drive_root_for_path(normalized);
    if (!drive_root.isEmpty() && normalized.compare(drive_root.left(2), Qt::CaseInsensitive) == 0) {
        normalized = drive_root;
    } else if (!normalized.endsWith(QLatin1Char('/'))) {
        normalized.append(QLatin1Char('/'));
    }

    path = QDir::toNativeSeparators(normalized).toStdString();
    if (error) error->clear();
    return true;
}

}

std::string analyzePath(std::vector<std::string> split_command) {
    if (split_command.size() < 2) return {};

    QString combined = QDir::fromNativeSeparators(QString::fromStdString(split_command[1]).trimmed());
    if (combined.isEmpty()) return {};
    if (combined.size() == 1 && _is_drive_letter(combined.front())) {
        combined = combined.toUpper() + QStringLiteral(":/");
    }

    for (std::size_t index = 2; index < split_command.size(); ++index) {
        QString segment = QDir::fromNativeSeparators(QString::fromStdString(split_command[index]).trimmed());
        while (segment.startsWith(QLatin1Char('/'))) segment.remove(0 , 1);
        if (segment.isEmpty()) continue;
        if (!combined.endsWith(QLatin1Char('/'))) combined.append(QLatin1Char('/'));
        combined.append(segment);
    }

    return Path::resolve(combined).toStdString();
}

std::string takeTrueParentsPath(std::string path_value) {
    if (path_value.empty()) return path_value;
    QDir directory(QString::fromStdString(path_value));
    if (directory.cdUp()) path_value = QDir::toNativeSeparators(directory.absolutePath()).toStdString();
    if (!path_value.empty() && path_value.back() != '\\' && path_value.back() != '/') path_value.push_back(QDir::separator().toLatin1());
    return path_value;
}

}

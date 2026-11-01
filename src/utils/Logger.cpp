#include "utils/Logger.h"

#include <QDir>
#include <QDateTime>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QStandardPaths>
#include <QTextStream>

namespace ttydash {
namespace {

QMutex _logger_mutex;
QString _logger_path;

}

void Logger::_initialize() {
    const QString directory = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(directory);
    _logger_path = QDir(directory).filePath(QStringLiteral("dashboard.log"));
    qInstallMessageHandler(&_message_handler);
    _info(QStringLiteral("TTY Dashboard logger initialized"));
}

void Logger::_info(const QString &message) {
    _write(QStringLiteral("INFO") , message);
}

void Logger::_warning(const QString &message) {
    _write(QStringLiteral("WARN") , message);
}

void Logger::_error(const QString &message) {
    _write(QStringLiteral("ERROR") , message);
}

void Logger::_message_handler(QtMsgType type , const QMessageLogContext &context , const QString &message) {
    Q_UNUSED(context)

    QString level = QStringLiteral("INFO");
    if (type == QtWarningMsg) {
        level = QStringLiteral("WARN");
    } else if (type == QtCriticalMsg || type == QtFatalMsg) {
        level = QStringLiteral("ERROR");
    } else if (type == QtDebugMsg) {
        level = QStringLiteral("DEBUG");
    }

    _write(level , message);
}

void Logger::_write(const QString &level , const QString &message) {
    QMutexLocker lock(&_logger_mutex);
    if (_logger_path.isEmpty()) {
        return;
    }

    QFile file(_logger_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        return;
    }

    QTextStream stream(&file);
    stream << QDateTime::currentDateTime().toString(Qt::ISODateWithMs)
           << " [" << level << "] " << message << Qt::endl;
}

}

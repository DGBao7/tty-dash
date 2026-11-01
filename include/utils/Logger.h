#pragma once

#include <QString>
#include <QLoggingCategory>

namespace ttydash {

class Logger {
public:
    static void _initialize();
    static void _info(const QString &message);
    static void _warning(const QString &message);
    static void _error(const QString &message);

private:
    static void _message_handler(QtMsgType type , const QMessageLogContext &context , const QString &message);
    static void _write(const QString &level , const QString &message);
};

}

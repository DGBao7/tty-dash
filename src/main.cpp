#include <QApplication>
#include <QDebug>

#include "app/AppController.h"

int main(int argc , char *argv[]) {
    QApplication application(argc , argv);
qDebug() << "TTY Dashboard started";
    QApplication::setApplicationName(QStringLiteral("TTY Dashboard"));
    QApplication::setOrganizationName(QStringLiteral("TtyDashboard"));
    QApplication::setApplicationVersion(QStringLiteral("0.1.0"));

    ttydash::AppController controller(application);

    return controller._run();
}

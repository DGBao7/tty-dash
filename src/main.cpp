#include <QApplication>

#include "app/AppController.h"

int main(int argc , char *argv[]) {
    QApplication application(argc , argv);
    QApplication::setApplicationName(QStringLiteral("TTY Dashboard"));
    QApplication::setOrganizationName(QStringLiteral("TtyDashboard"));
    QApplication::setApplicationVersion(QStringLiteral("0.1.0"));

    ttydash::AppController controller(application);

    return controller._run();
}

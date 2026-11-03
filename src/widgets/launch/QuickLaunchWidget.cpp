#include "widgets/launch/QuickLaunchWidget.h"

#include <QPainter>

#include <QDebug>

namespace ttydash {

QuickLaunchWidget::QuickLaunchWidget(const WidgetConfig &config , QWidget *parent)
    : DashboardWidget(config , parent) {
        qDebug() << "QuickLaunch created:" << size();
}

void QuickLaunchWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    qDebug() << "QuickLaunch painted:" << size();

    QPainter painter(this);

    _paint_panel(painter , _title);
}

}
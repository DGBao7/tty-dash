#pragma once

#include "widgets/DashboardWidget.h"

#include <QPaintEvent>

namespace ttydash {

class QuickLaunchWidget : public DashboardWidget {
public:
    QuickLaunchWidget(const WidgetConfig &config , QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

}
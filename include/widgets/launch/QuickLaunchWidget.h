
#pragma once

#include "widgets/DashboardWidget.h"

#include <QPaintEvent>

class QPushButton;

namespace ttydash {

class QuickLaunchWidget : public DashboardWidget {
    Q_OBJECT

public:
    QuickLaunchWidget(const WidgetConfig &config , QWidget *parent = nullptr);

signals:
    void terbon_toggle_requested();
    void browser_open_requested();
    void vscode_open_requested();
    void explorer_open_requested();
    void github_desktop_open_requested();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPushButton *_terbon_button = nullptr;
};

}

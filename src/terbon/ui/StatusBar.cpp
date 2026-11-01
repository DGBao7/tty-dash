#include <QtWidgets/QHBoxLayout>

#include "ui/StatusBar.h"
#include "str/AnalyzePath.h"
#include "theme/Theme.h"

namespace terbon {

StatusBar::StatusBar(QWidget* parent)
    : QWidget(parent)
{
    path_label = new QLabel(this);
    info_label = new QLabel("● Ready" , this);
    path_label->setObjectName(QStringLiteral("terbonPath"));
    info_label->setObjectName(QStringLiteral("terbonInfo"));

    QHBoxLayout* layout = new QHBoxLayout(this);

    layout->setContentsMargins(10 , 0 , 10 , 0);
    layout->setSpacing(0);

    layout->addWidget(path_label);
    layout->addStretch();
    layout->addWidget(info_label);

    setFixedHeight(24);

    setObjectName(QStringLiteral("terbonStatusBar"));
    setStyleSheet(ttydash::Theme::_terminal_stylesheet());

    UpdatePath();
}

void StatusBar::UpdatePath()
{
    path_label -> setText(
        QString::fromStdString(Path::path)
    );
}

void StatusBar::UpdateStatus(QString text)
{
    info_label -> setText(text);
}

}

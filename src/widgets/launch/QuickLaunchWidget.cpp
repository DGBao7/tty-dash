
#include "widgets/launch/QuickLaunchWidget.h"

#include "theme/Theme.h"

#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>

namespace ttydash {

QuickLaunchWidget::QuickLaunchWidget(const WidgetConfig &config , QWidget *parent)
    : DashboardWidget(config , parent) {

    auto *layout = new QVBoxLayout(this);

    layout->setContentsMargins(8 , 34 , 8 , 8);
    layout->setSpacing(0);

    QFont item_font = kultum_font;
    item_font.setPointSize(16);

    const auto create_item = [this , layout , &item_font](const QString &text) {
        auto *button = new QPushButton(text , this);

        button->setFont(item_font);
        button->setCursor(Qt::PointingHandCursor);
        button->setFlat(false);
        button->setFocusPolicy(Qt::NoFocus);

        button->setStyleSheet(QStringLiteral(
            "QPushButton {"
            "font-family: \"%1\";"
            "font-size: 16pt;"
            "color: %2;"
            "background: transparent;"
            "border: none;"
            "border-bottom: 1px solid %3;"
            "text-align: center;"
            "padding: 4px;"
            "}"
            "QPushButton:hover {"
            "color: %4;"
            "background-color: %5;"
            "}"
            "QPushButton:pressed {"
            "background-color: %6;"
            "}"
        ).arg(
            kultum_font.family(),
            _text_color.name(),
            Theme::border.name(),
            Theme::highlight.name(),
            Theme::secondary_background.name(),
            Theme::secondary.name()
        ));

        layout->addWidget(button , 1);

        return button;
    };

    _terbon_button = create_item(QStringLiteral("TERBON"));

    QPushButton *browser_button = create_item(QStringLiteral("BROWSER"));
    QPushButton *vscode_button = create_item(QStringLiteral("VS CODE"));
    QPushButton *explorer_button = create_item(QStringLiteral("EXPLORER"));
    QPushButton *github_button = create_item(QStringLiteral("GITHUB DESKTOP"));

    connect(
        _terbon_button,
        &QPushButton::clicked,
        this,
        &QuickLaunchWidget::terbon_toggle_requested
    );

    connect(
        browser_button,
        &QPushButton::clicked,
        this,
        &QuickLaunchWidget::browser_open_requested
    );

    connect(
        vscode_button,
        &QPushButton::clicked,
        this,
        &QuickLaunchWidget::vscode_open_requested
    );

    connect(
        explorer_button,
        &QPushButton::clicked,
        this,
        &QuickLaunchWidget::explorer_open_requested
    );

    connect(
        github_button,
        &QPushButton::clicked,
        this,
        &QuickLaunchWidget::github_desktop_open_requested
    );
}

void QuickLaunchWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QPainter painter(this);

    _paint_panel(painter , _title);
}

}

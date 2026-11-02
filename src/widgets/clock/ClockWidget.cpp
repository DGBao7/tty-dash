#include "widgets/clock/ClockWidget.h"

#include <QDateTime>
#include <QPainter>

namespace ttydash {

ClockWidget::ClockWidget(const WidgetConfig &config , QWidget *parent)
    : DashboardWidget(config , parent) {
    _time_format = config.options.value(QStringLiteral("time_format")).toString(QStringLiteral("HH:mm:ss"));
    _date_format = config.options.value(QStringLiteral("date_format")).toString(QStringLiteral("dddd, dd MMMM yyyy"));
    _display_font_size = qBound(16 , config.options.value(QStringLiteral("display_font_size")).toInt(34) , 72);
    connect(&_timer , &QTimer::timeout , this , [this]() {
        _refresh();
    });
    _timer.start(1000);
    _refresh();
}

void ClockWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    _paint_panel(painter , _title);

    QFont display_font = _font;
    display_font.setPixelSize(_display_font_size);
    display_font.setBold(true);
    painter.setFont(display_font);
    painter.setPen(_text_color);
    painter.drawText(QRect(10 , 34 , width() - 20 , 58) , Qt::AlignHCenter | Qt::AlignVCenter , _time_text);

    QFont date_font = _font;
    date_font.setPixelSize(qMax(10 , _display_font_size / 3));
    painter.setFont(date_font);
    painter.setPen(_accent_color);
    painter.drawText(QRect(10 , height() - 35 , width() - 20 , 22) , Qt::AlignHCenter | Qt::AlignVCenter , _date_text);
}

void ClockWidget::_refresh() {
    const QDateTime now = QDateTime::currentDateTime();
    _time_text = now.toString(_time_format);
    _date_text = now.toString(_date_format);
    update();
}

}

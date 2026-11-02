#include "widgets/ascii/MatrixWidget.h"

#include <QPainter>
#include <QRandomGenerator>
#include <QResizeEvent>

#include <algorithm>

namespace ttydash {

MatrixWidget::MatrixWidget(const WidgetConfig &config , QWidget *parent)
    : DashboardWidget(config , parent) {
    _speed = qBound(0.2 , config.options.value(QStringLiteral("animation_speed")).toDouble(1.0) , 4.0);

    _density = qBound(
        0.3 , 
        config.options.value(
            QStringLiteral("density")).toDouble(1.0) , 
            2.5
        );
    _cell_width = qMax(9 , qRound(16.0 / _density));
    
    const int interval = qBound(25 , config.options.value(QStringLiteral("interval_ms")).toInt(75) , 500);
    connect(&_timer , &QTimer::timeout , this , [this]() {
        _advance();
    });
    _reset_columns();
    _timer.start(interval);
}

void MatrixWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    _paint_panel(painter , _title);
    painter.setFont(_font);
    QFont matrix_font(QStringLiteral("Consolas"));
    matrix_font.setPixelSize(qMax(7 , _cell_height - 1));
    painter.setFont(matrix_font);

    const int visible_rows = qMax(1 , (height() - 38) / _cell_height);
    for (qsizetype column = 0; column < _drops.size(); ++column) {
        const int head = qRound(_drops.at(column));
        const int first = qMax(0 , head - _trail_lengths.at(column));
        const int last = qMin(visible_rows - 1 , head);
        for (int row = first; row <= last; ++row) {
            const int glyph_index = QRandomGenerator::global()->bounded(_glyphs.size());
            QColor color = _accent_color;
            if (row == head) {
                color = color.lighter(175);
            } else {
                color.setAlpha(qBound(25 , 255 * (row - first + 1) / qMax(1 , last - first + 1) , 230));
            }
            painter.setPen(color);
            painter.drawText(QPointF(12 + column * _cell_width , 42 + row * _cell_height) , _glyphs.at(glyph_index));
        }
    }
}

void MatrixWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    _reset_columns();
}

void MatrixWidget::_reset_columns() {
    const int count = qMax(1 , (width() - 24) / _cell_width);
    _drops.resize(count);
    _trail_lengths.resize(count);
    const int rows = qMax(1 , (height() - 38) / _cell_height);
    for (int index = 0; index < count; ++index) {
        _drops[index] = QRandomGenerator::global()->bounded(qMax(1 , rows)) - rows;
        _trail_lengths[index] = QRandomGenerator::global()->bounded(5 , 18);
    }
}

void MatrixWidget::_advance() {
    const int rows = qMax(1 , (height() - 38) / _cell_height);
    for (qsizetype index = 0; index < _drops.size(); ++index) {
        _drops[index] += _speed * (0.65 + QRandomGenerator::global()->generateDouble() * 0.7);
        if (_drops.at(index) - _trail_lengths.at(index) > rows) {
            _drops[index] = -QRandomGenerator::global()->bounded(1 , rows + 1);
            _trail_lengths[index] = QRandomGenerator::global()->bounded(5 , 18);
        }
    }

    update();
}

}

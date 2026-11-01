#include "widgets/ascii/PipesWidget.h"

#include <QFontMetrics>
#include <QPainter>
#include <QRandomGenerator>
#include <QResizeEvent>

#include <array>

namespace ttydash {
namespace {

constexpr std::array<int , 4> _move_x{1 , 0 , -1 , 0};
constexpr std::array<int , 4> _move_y{0 , 1 , 0 , -1};

}

PipesWidget::PipesWidget(const WidgetConfig &config , QWidget *parent)
    : DashboardWidget(config , parent) {
    _speed = qBound(0.25 , config.options.value(QStringLiteral("animation_speed")).toDouble(1.0) , 4.0);
    _interval_ms = qBound(35 , config.options.value(QStringLiteral("interval_ms")).toInt(105) , 600);
    connect(&_timer , &QTimer::timeout , this , [this]() {
        _advance();
    });
    _reset_grid();
    _timer.start(qMax(30 , qRound(_interval_ms / _speed)));
}

void PipesWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    _paint_panel(painter , _title);
    painter.setFont(QFont(QStringLiteral("Consolas") , qMax(9 , _font.pointSize())));

    for (int row = 0; row < _rows; ++row) {
        for (int column = 0; column < _columns; ++column) {
            const int index = _index(column , row);
            if (_cells.at(index).isNull()) {
                continue;
            }

            QColor color = _accent_color;
            color.setAlpha(qBound(25 , 255 * _ages.at(index) / 180 , 255));
            painter.setPen(color);
            painter.drawText(QPointF(13 + column * _cell_width , 43 + row * _cell_height) , _cells.at(index));
        }
    }
}

void PipesWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    _reset_grid();
}

void PipesWidget::_reset_grid() {
    const QFont font(QStringLiteral("Consolas") , qMax(9 , _font.pointSize()));
    const QFontMetrics metrics(font);
    _cell_width = qMax(8 , metrics.horizontalAdvance(QLatin1Char('M')));
    _cell_height = qMax(12 , metrics.height());
    _columns = qMax(1 , (width() - 26) / _cell_width);
    _rows = qMax(1 , (height() - 42) / _cell_height);
    _cells.fill(QChar() , _columns * _rows);
    _ages.fill(0 , _columns * _rows);
    _x = _columns / 2;
    _y = _rows / 2;
    _direction = QRandomGenerator::global()->bounded(4);
}

void PipesWidget::_advance() {
    for (int index = 0; index < _ages.size(); ++index) {
        if (_ages[index] > 0) {
            --_ages[index];
            if (_ages.at(index) == 0) {
                _cells[index] = QChar();
            }
        }
    }

    std::array<int , 4> candidates{};
    int candidate_count = 0;
    for (int turn = -1; turn <= 1; ++turn) {
        const int direction = (_direction + turn + 4) % 4;
        const int next_x = _x + _move_x.at(direction);
        const int next_y = _y + _move_y.at(direction);
        if (next_x >= 0 && next_x < _columns && next_y >= 0 && next_y < _rows) {
            candidates.at(candidate_count++) = direction;
        }
    }

    if (candidate_count == 0) {
        for (int direction = 0; direction < 4; ++direction) {
            const int next_x = _x + _move_x.at(direction);
            const int next_y = _y + _move_y.at(direction);
            if (next_x >= 0 && next_x < _columns && next_y >= 0 && next_y < _rows) {
                candidates.at(candidate_count++) = direction;
            }
        }
    }

    if (candidate_count > 0) {
        const int next_direction = candidates.at(QRandomGenerator::global()->bounded(candidate_count));
        const int cell = _index(_x , _y);
        _cells[cell] = QChar(_glyph_for(_direction , next_direction));
        _ages[cell] = 180;
        _x += _move_x.at(next_direction);
        _y += _move_y.at(next_direction);
        _direction = next_direction;
    }

    update();
}

int PipesWidget::_index(int x , int y) const {
    return y * _columns + x;
}

int PipesWidget::_glyph_for(int incoming , int outgoing) const {
    const bool incoming_horizontal = incoming == 0 || incoming == 2;
    const bool outgoing_horizontal = outgoing == 0 || outgoing == 2;
    if (incoming_horizontal && outgoing_horizontal) {
        return '-';
    }
    if (!incoming_horizontal && !outgoing_horizontal) {
        return '|';
    }

    return '+';
}

}

#pragma once

#include "widgets/DashboardWidget.h"

#include <QTimer>

namespace ttydash {

class PipesWidget : public DashboardWidget {
public:
    PipesWidget(const WidgetConfig &config , QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void _reset_grid();
    void _advance();
    int _index(int x , int y) const;
    int _glyph_for(int incoming , int outgoing) const;

    QTimer _timer;
    QVector<QChar> _cells;
    QVector<int> _ages;
    int _columns = 1;
    int _rows = 1;
    int _cell_width = 14;
    int _cell_height = 18;
    int _x = 0;
    int _y = 0;
    int _direction = 0;
    qreal _speed = 1.0;
    int _interval_ms = 105;
};

}

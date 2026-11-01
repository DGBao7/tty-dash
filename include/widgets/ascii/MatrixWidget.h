#pragma once

#include "widgets/DashboardWidget.h"

#include <QTimer>

namespace ttydash {

class MatrixWidget : public DashboardWidget {
public:
    MatrixWidget(const WidgetConfig &config , QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void _reset_columns();
    void _advance();

    QTimer _timer;
    QVector<qreal> _drops;
    QVector<int> _trail_lengths;
    int _cell_width = 13;
    int _cell_height = 17;
    qreal _speed = 1.0;
    qreal _density = 1.0;
    QString _glyphs = QStringLiteral("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ#$%&*+-<>");
};

}

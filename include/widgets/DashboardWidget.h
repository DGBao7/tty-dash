#pragma once

#include "config/ConfigManager.h"

#include <QColor>
#include <QFont>
#include <QVector>
#include <QWidget>

class QMouseEvent;
class QPainter;

namespace ttydash {

class DashboardWidget : public QWidget {
    Q_OBJECT

public:
    DashboardWidget(const WidgetConfig &config , QWidget *parent = nullptr);

    QString _widget_id() const;
    void _set_interaction_enabled(bool enabled);

signals:
    void geometry_changed(const QString &widget_id , const QRect &geometry);

protected:
    void _paint_panel(QPainter &painter , const QString &title) const;
    void _paint_graph(QPainter &painter , const QRectF &area , const QVector<qreal> &values , qreal max_value) const;
    QFont _font;
    QColor _text_color;
    QColor _accent_color;
    QColor _panel_color;
    QString _id;
    QString _title;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool _interaction_enabled = false;
    bool _dragging = false;
    bool _resizing = false;
    QPoint _drag_origin;
    QRect _geometry_origin;
};

}

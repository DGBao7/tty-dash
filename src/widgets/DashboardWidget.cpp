#include "widgets/DashboardWidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

namespace ttydash {
namespace {

QColor _option_color(const QJsonObject &options , const QString &key , const QColor &fallback) {
    const QColor value(options.value(key).toString());

    return value.isValid() ? value : fallback;
}

}

DashboardWidget::DashboardWidget(const WidgetConfig &config , QWidget *parent)
    : QWidget(parent)
    , _id(config.id)
    , _title(config.options.value(QStringLiteral("title")).toString(config.type.toUpper())) {
    _accent_color = _option_color(config.options , QStringLiteral("color") , QColor(QStringLiteral("#62F5A5")));
    _text_color = _option_color(config.options , QStringLiteral("text_color") , QColor(QStringLiteral("#D8F5E5")));
    _panel_color = _option_color(config.options , QStringLiteral("panel_color") , QColor(QStringLiteral("#D906161B")));

    const QString family = config.options.value(QStringLiteral("font_family")).toString(QStringLiteral("Consolas"));
    const int size = qBound(8 , config.options.value(QStringLiteral("font_size")).toInt(11) , 48);
    _font = QFont(family , size);
    _font.setStyleHint(QFont::Monospace);

    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
    setMinimumSize(40 , 32);
}

QString DashboardWidget::_widget_id() const {
    return _id;
}

void DashboardWidget::_set_interaction_enabled(bool enabled) {
    _interaction_enabled = enabled;
    unsetCursor();
}

void DashboardWidget::_paint_panel(QPainter &painter , const QString &title) const {
    const QRectF panel = QRectF(rect()).adjusted(1.0 , 1.0 , -1.0 , -1.0);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(_accent_color.darker(150) , 1.0));
    painter.setBrush(_panel_color);
    painter.drawRoundedRect(panel , 8.0 , 8.0);
    painter.fillRect(QRectF(2.0 , 10.0 , 3.0 , height() - 20.0) , _accent_color);

    painter.setFont(_font);
    painter.setPen(_accent_color);
    painter.drawText(QPointF(16.0 , 24.0) , title.toUpper());

    if (_interaction_enabled) {
        painter.setPen(QPen(_accent_color , 1.0 , Qt::DashLine));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(panel.adjusted(3.0 , 3.0 , -3.0 , -3.0) , 6.0 , 6.0);
    }
}

void DashboardWidget::_paint_graph(QPainter &painter , const QRectF &area , const QVector<qreal> &values , qreal max_value) const {
    if (values.size() < 2 || max_value <= 0.0) {
        return;
    }

    QPainterPath path;
    for (qsizetype index = 0; index < values.size(); ++index) {
        const qreal x = area.left() + static_cast<qreal>(index) * area.width() / static_cast<qreal>(values.size() - 1);
        const qreal normalized = qBound(0.0 , values.at(index) / max_value , 1.0);
        const qreal y = area.bottom() - normalized * area.height();
        if (index == 0) {
            path.moveTo(x , y);
        } else {
            path.lineTo(x , y);
        }
    }

    painter.setPen(QPen(_accent_color , 1.5));
    painter.drawPath(path);
}

void DashboardWidget::mousePressEvent(QMouseEvent *event) {
    if (!_interaction_enabled || event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);

        return;
    }

    _dragging = true;
    _resizing = event->position().x() >= width() - 22 && event->position().y() >= height() - 22;
    _drag_origin = event->globalPosition().toPoint();
    _geometry_origin = geometry();
    event->accept();
}

void DashboardWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!_interaction_enabled) {
        QWidget::mouseMoveEvent(event);

        return;
    }

    if (!_dragging) {
        const bool resize_corner = event->position().x() >= width() - 22 && event->position().y() >= height() - 22;
        setCursor(resize_corner ? Qt::SizeFDiagCursor : Qt::SizeAllCursor);

        return;
    }

    const QPoint delta = event->globalPosition().toPoint() - _drag_origin;
    if (_resizing) {
        setGeometry(_geometry_origin.x() , _geometry_origin.y() , qMax(80 , _geometry_origin.width() + delta.x()) , qMax(48 , _geometry_origin.height() + delta.y()));
    } else {
        move(_geometry_origin.topLeft() + delta);
    }
    event->accept();
}

void DashboardWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (_interaction_enabled && _dragging && event->button() == Qt::LeftButton) {
        _dragging = false;
        emit geometry_changed(_id , geometry());
        event->accept();

        return;
    }

    QWidget::mouseReleaseEvent(event);
}

}

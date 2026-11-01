#include "terbon/TerBonWindow.h"

#include "theme/Theme.h"
#include "terbon/ui/StatusBar.h"
#include "terbon/ui/TerminalWidget.h"

#include <QCloseEvent>
#include <QMouseEvent>
#include <QIcon>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QShowEvent>
#include <QVBoxLayout>
#include <QWidget>

#include <functional>
#include <utility>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace terbon {
namespace {

class DragHandle final : public QWidget {
public:
    DragHandle(QWidget *window , std::function<void(const QRect &)> moved , QWidget *parent)
        : QWidget(parent)
        , _window(window)
        , _moved(std::move(moved)) {
        setObjectName(QStringLiteral("terbonDragHandle"));
        setFixedHeight(16);
        setCursor(Qt::OpenHandCursor);
        setToolTip(QStringLiteral("Drag to move TerBon"));
    }

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            _dragging = true;
            _drag_origin = event->globalPosition().toPoint();
            _window_origin = _window->pos();
            setCursor(Qt::ClosedHandCursor);
            event->accept();
            return;
        }
        QWidget::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (_dragging && (event->buttons() & Qt::LeftButton)) {
            const QPoint delta = event->globalPosition().toPoint() - _drag_origin;
            _window->move(_window_origin + delta);
            event->accept();
            return;
        }
        QWidget::mouseMoveEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        if (_dragging && event->button() == Qt::LeftButton) {
            _dragging = false;
            setCursor(Qt::OpenHandCursor);
            if (_moved && _window->pos() != _window_origin) _moved(_window->geometry());
            event->accept();
            return;
        }
        QWidget::mouseReleaseEvent(event);
    }

    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event)
        QPainter painter(this);
        painter.fillRect(rect() , ttydash::Theme::background);
        painter.setPen(Qt::NoPen);
        painter.setBrush(ttydash::Theme::primary);
        painter.drawRoundedRect(QRectF((width() - 34) / 2.0 , (height() - 3) / 2.0 , 34 , 3) , 1.5 , 1.5);
    }

private:
    QWidget *_window = nullptr;
    std::function<void(const QRect &)> _moved;
    QPoint _drag_origin;
    QPoint _window_origin;
    bool _dragging = false;
};

}

TerBonWindow::TerBonWindow(QWidget *parent)
    : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("TerBon Terminal"));
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    const QIcon source_icon(QStringLiteral(":/terbon/bon.ico"));
    QPixmap logo = source_icon.pixmap(QSize(64 , 64));
    if (!logo.isNull()) {
        QPainter painter(&logo);
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.fillRect(logo.rect() , ttydash::Theme::primary);
        setWindowIcon(QIcon(logo));
    }
    setFixedSize(700 , 350);
    setAttribute(Qt::WA_DeleteOnClose , false);
    setStyleSheet(ttydash::Theme::_terminal_stylesheet());

    auto *central = new QWidget(this);
    central->setObjectName(QStringLiteral("terbonCentral"));
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(0 , 0 , 0 , 0);
    layout->setSpacing(0);
    auto *drag_handle = new DragHandle(this , [this](const QRect &geometry) {
        emit geometry_changed(geometry);
    } , central);
    _terminal_widget = new TerminalWidget(central);
    _status = new StatusBar(central);
    layout->addWidget(drag_handle , 0);
    layout->addWidget(_terminal_widget , 1);
    layout->addWidget(_status , 0);
    setCentralWidget(central);
}

TerminalWidget *TerBonWindow::_terminal() const {
    return _terminal_widget;
}

StatusBar *TerBonWindow::_status_bar() const {
    return _status;
}

void TerBonWindow::_set_fixed_geometry(const QRect &geometry) {
    if (geometry.isEmpty()) return;
    _fixed_geometry = geometry;
    _applying_geometry = true;
    setFixedSize(geometry.size());
    setGeometry(geometry);
    _applying_geometry = false;
}

void TerBonWindow::closeEvent(QCloseEvent *event) {
    emit closed_by_user();
    event->accept();
}

void TerBonWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    _apply_native_styles();
}

bool TerBonWindow::nativeEvent(const QByteArray &event_type , void *message , qintptr *result) {
#ifdef Q_OS_WIN
    Q_UNUSED(event_type)
    MSG *native_message = static_cast<MSG *>(message);
    if (!native_message || !result) return QMainWindow::nativeEvent(event_type , message , result);

    if (native_message->message == WM_NCHITTEST) {
        *result = HTCLIENT;
        return true;
    }
    if (native_message->message == WM_SYSCOMMAND) {
        const UINT command = static_cast<UINT>(native_message->wParam) & 0xFFF0U;
        if (command == SC_SIZE || command == SC_MAXIMIZE || command == SC_MINIMIZE) {
            *result = 0;
            return true;
        }
    }
    if (native_message->message == WM_WINDOWPOSCHANGING && !_applying_geometry && !_fixed_geometry.isEmpty()) {
        auto *position = reinterpret_cast<WINDOWPOS *>(native_message->lParam);
        if (position && !(position->flags & SWP_NOSIZE)) {
            const qreal scale = devicePixelRatioF();
            position->cx = qRound(_fixed_geometry.width() * scale);
            position->cy = qRound(_fixed_geometry.height() * scale);
        }
    }
#else
    Q_UNUSED(event_type)
    Q_UNUSED(message)
    Q_UNUSED(result)
#endif
    return QMainWindow::nativeEvent(event_type , message , result);
}

void TerBonWindow::_apply_native_styles() {
#ifdef Q_OS_WIN
    const HWND hwnd = reinterpret_cast<HWND>(winId());
    if (!hwnd) return;
    LONG_PTR style = GetWindowLongPtrW(hwnd , GWL_STYLE);
    style &= ~static_cast<LONG_PTR>(WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
    SetWindowLongPtrW(hwnd , GWL_STYLE , style);
    SetWindowPos(hwnd , nullptr , 0 , 0 , 0 , 0 , SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
#endif
}

}

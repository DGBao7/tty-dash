#include "terbon/TerBonWindow.h"

#include "theme/Theme.h"
#include "terbon/ui/StatusBar.h"
#include "terbon/ui/TerminalWidget.h"

#include <QCloseEvent>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QWidget>

namespace terbon {

TerBonWindow::TerBonWindow(QWidget *parent)
    : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("TerBon Terminal"));
    const QIcon source_icon(QStringLiteral(":/terbon/bon.ico"));
    QPixmap logo = source_icon.pixmap(QSize(64 , 64));
    if (!logo.isNull()) {
        QPainter painter(&logo);
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.fillRect(logo.rect() , ttydash::Theme::primary);
        setWindowIcon(QIcon(logo));
    }
    setMinimumSize(320 , 220);
    resize(700 , 420);
    setAttribute(Qt::WA_DeleteOnClose , false);
    setStyleSheet(ttydash::Theme::_terminal_stylesheet());

    auto *central = new QWidget(this);
    central->setObjectName(QStringLiteral("terbonCentral"));
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(0 , 0 , 0 , 0);
    layout->setSpacing(0);
    _terminal_widget = new TerminalWidget(central);
    _status = new StatusBar(central);
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

void TerBonWindow::closeEvent(QCloseEvent *event) {
    emit closed_by_user();
    event->accept();
}

void TerBonWindow::moveEvent(QMoveEvent *event) {
    QMainWindow::moveEvent(event);
    emit geometry_changed(geometry());
}

void TerBonWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    emit geometry_changed(geometry());
}

}

#pragma once

#include <QColor>
#include <QString>

namespace ttydash::Theme {

inline const QColor background(QStringLiteral("#0B0712"));
inline const QColor secondary_background(QStringLiteral("#110B1C"));
inline const QColor panel(QStringLiteral("#E6191125"));
inline const QColor primary(QStringLiteral("#A970FF"));
inline const QColor bright(QStringLiteral("#BC8CFF"));
inline const QColor secondary(QStringLiteral("#8250DF"));
inline const QColor highlight(QStringLiteral("#D8B9FF"));
inline const QColor text(QStringLiteral("#F0E8FF"));
inline const QColor secondary_text(QStringLiteral("#A99BBE"));
inline const QColor border(QStringLiteral("#49315F"));
inline const QColor transparent(0 , 0 , 0 , 0);

inline QString _application_stylesheet() {
    return QStringLiteral(
        "QWidget { color: %1; font-family: Consolas, monospace; background: transparent; }"
        "QToolTip, QMenu { color: %1; background: %2; border: 1px solid %3; }"
    ).arg(text.name() , secondary_background.name() , border.name());
}

inline QString _terminal_stylesheet() {
    return QStringLiteral(
        "QMainWindow, QWidget#terbonCentral { background-color: %1; color: %2; }"
        "QPlainTextEdit { background-color: %1; color: %2; border: none; selection-background-color: %3; }"
        "QStatusBar, QWidget#terbonStatusBar { background-color: %4; color: %5; border-top: 1px solid %3; }"
        "QLabel#terbonPath { color: %6; }"
        "QLabel#terbonInfo { color: %7; }"
        "QScrollBar:vertical { background: %4; width: 10px; margin: 0; }"
        "QScrollBar::handle:vertical { background: %3; min-height: 24px; border-radius: 4px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }"
    ).arg(background.name() , text.name() , border.name() , secondary_background.name() , secondary_text.name() , primary.name() , bright.name());
}

}

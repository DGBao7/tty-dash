#pragma once

#include <QMainWindow>
#include <QRect>

class QCloseEvent;
class QMoveEvent;
class QResizeEvent;

namespace terbon {

class StatusBar;
class TerminalWidget;

class TerBonWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit TerBonWindow(QWidget *parent = nullptr);

    TerminalWidget *_terminal() const;
    StatusBar *_status_bar() const;

signals:
    void geometry_changed(const QRect &geometry);
    void closed_by_user();

protected:
    void closeEvent(QCloseEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    TerminalWidget *_terminal_widget = nullptr;
    StatusBar *_status = nullptr;
};

}

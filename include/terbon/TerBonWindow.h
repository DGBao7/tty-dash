#pragma once

#include <QMainWindow>
#include <QRect>

class QCloseEvent;
class QShowEvent;

namespace terbon {

class StatusBar;
class TerminalWidget;

class TerBonWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit TerBonWindow(QWidget *parent = nullptr);

    TerminalWidget *_terminal() const;
    StatusBar *_status_bar() const;
    void _set_fixed_geometry(const QRect &geometry);

signals:
    void geometry_changed(const QRect &geometry);
    void closed_by_user();

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    bool nativeEvent(const QByteArray &event_type , void *message , qintptr *result) override;

private:
    void _apply_native_styles();

    TerminalWidget *_terminal_widget = nullptr;
    StatusBar *_status = nullptr;
    QRect _fixed_geometry;
    bool _applying_geometry = false;
};

}

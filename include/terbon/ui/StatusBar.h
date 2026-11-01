#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>

namespace terbon {

class StatusBar : public QWidget
{
    Q_OBJECT

public:
    explicit StatusBar(QWidget* parent = nullptr);

    void UpdatePath();

    void UpdateStatus(QString text);
private:
    QLabel* path_label;
    QLabel* info_label;
};

}

#endif

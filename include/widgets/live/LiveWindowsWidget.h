
#pragma once

#include "widgets/DashboardWidget.h"

#include <QHash>
#include <QGridLayout>
#include <QScrollBar>
#include <QGridLayout>
#include <QScrollBar>
#include <QSizePolicy>
#include <QVector>

class QFrame;
class QLabel;
class QPushButton;
class QScrollArea;
class QTimer;
class QGridLayout;
class QResizeEvent;

namespace ttydash {

struct LiveWindowPreview;

class LiveWindowsWidget : public DashboardWidget {
    Q_OBJECT

public:
    LiveWindowsWidget(const WidgetConfig &config , QWidget *parent = nullptr);
    ~LiveWindowsWidget() override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void _refresh_windows();
    void _update_preview_sizes();
    void _update_thumbnails();
    void _release_preview(LiveWindowPreview *preview);
    void _activate_window(quintptr handle);

    QScrollArea *_scroll_area = nullptr;
    QWidget *_content = nullptr;
    QGridLayout *_content_layout = nullptr;
    QTimer *_refresh_timer = nullptr;
    QLabel *_empty_label = nullptr;

    QHash<quintptr , LiveWindowPreview *> _previews;

    quintptr _destination_handle = 0;
};

}


#include "widgets/live/LiveWindowsWidget.h"

#include "theme/Theme.h"

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
#include <QFontMetrics>
#include <QResizeEvent>
#include <QSet>
#include <QPainter>

#include <windows.h>
#include <dwmapi.h>

#include <algorithm>
#include <vector>

namespace ttydash {

struct LiveWindowPreview {
    HWND source = nullptr;
    HTHUMBNAIL thumbnail = nullptr;

    QFrame *card = nullptr;
    QLabel *title = nullptr;
    QPushButton *surface = nullptr;
};

namespace {

struct WindowInfo {
    HWND handle = nullptr;
    QString title;
};

struct WindowEnumeration {
    QVector<WindowInfo> *windows = nullptr;
    DWORD own_process_id = 0;
};

BOOL CALLBACK _enum_windows_callback(HWND hwnd , LPARAM parameter) {
    auto *context = reinterpret_cast<WindowEnumeration *>(parameter);

    if (!IsWindowVisible(hwnd)) {
        return TRUE;
    }

    const LONG_PTR style = GetWindowLongPtrW(hwnd , GWL_EXSTYLE);

    if (style & WS_EX_TOOLWINDOW) {
        return TRUE;
    }

    if (GetWindow(hwnd , GW_OWNER) && !(style & WS_EX_APPWINDOW)) {
        return TRUE;
    }

    DWORD process_id = 0;

    GetWindowThreadProcessId(hwnd , &process_id);

    if (process_id == 0 || process_id == context->own_process_id) {
        return TRUE;
    }

    DWORD cloaked = 0;

    if (SUCCEEDED(DwmGetWindowAttribute(
        hwnd,
        DWMWA_CLOAKED,
        &cloaked,
        sizeof(cloaked)
    )) && cloaked) {
        return TRUE;
    }

    const int title_length = GetWindowTextLengthW(hwnd);

    if (title_length <= 0) {
        return TRUE;
    }

    std::vector<wchar_t> buffer(
        static_cast<size_t>(title_length) + 1
    );

    const int copied = GetWindowTextW(
        hwnd,
        buffer.data(),
        static_cast<int>(buffer.size())
    );

    if (copied <= 0) {
        return TRUE;
    }

    WindowInfo info;

    info.handle = hwnd;
    info.title = QString::fromWCharArray(
        buffer.data(),
        copied
    ).trimmed();

    if (!info.title.isEmpty()) {
        context->windows->append(info);
    }

    return TRUE;
}

}

LiveWindowsWidget::LiveWindowsWidget(const WidgetConfig &config , QWidget *parent)
    : DashboardWidget(config , parent) {

    auto *layout = new QVBoxLayout(this);

    layout->setContentsMargins(8 , 34 , 8 , 8);
    layout->setSpacing(0);

    _scroll_area = new QScrollArea(this);

    _scroll_area->setWidgetResizable(true);
    _scroll_area->setFrameShape(QFrame::NoFrame);
    _scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    _scroll_area->setStyleSheet(
        "QScrollArea { background: transparent; border: none; }"
    );

    _content = new QWidget();

    _content_layout = new QGridLayout(_content);

    _content_layout->setContentsMargins(2 , 2 , 6 , 2);
    _content_layout->setHorizontalSpacing(5);
    _content_layout->setVerticalSpacing(5);

    for (int column = 0; column < 3; ++column) {
        _content_layout->setColumnStretch(column , 1);
    }

    _scroll_area->setWidget(_content);

    layout->addWidget(_scroll_area);

    _refresh_timer = new QTimer(this);

    _refresh_timer->setInterval(1000);

    connect(
        _refresh_timer,
        &QTimer::timeout,
        this,
        &LiveWindowsWidget::_refresh_windows
    );

    _refresh_timer->start();

    QTimer::singleShot(
        0,
        this,
        &LiveWindowsWidget::_refresh_windows
    );

    connect(
        _scroll_area->verticalScrollBar(),
        &QScrollBar::valueChanged,
        this,
        [this](int) {
            _update_thumbnails();
        }
    );
}

LiveWindowsWidget::~LiveWindowsWidget() {
    _refresh_timer->stop();

    for (LiveWindowPreview *preview : _previews) {
        _release_preview(preview);
    }

    _previews.clear();
}

void LiveWindowsWidget::_refresh_windows() {
    QWidget *destination_window = window();

    if (!destination_window) {
        return;
    }

    const HWND destination = reinterpret_cast<HWND>(
        destination_window->winId()
    );

    if (!destination) {
        return;
    }

    const quintptr destination_handle = reinterpret_cast<quintptr>(destination);

    if (_destination_handle != destination_handle) {
        for (LiveWindowPreview *preview : _previews) {
            if (preview->thumbnail) {
                DwmUnregisterThumbnail(preview->thumbnail);
                preview->thumbnail = nullptr;
            }
        }

        _destination_handle = destination_handle;
    }

    QVector<WindowInfo> windows;

    WindowEnumeration context;

    context.windows = &windows;
    context.own_process_id = GetCurrentProcessId();

    EnumWindows(
        _enum_windows_callback,
        reinterpret_cast<LPARAM>(&context)
    );

    std::sort(
        windows.begin(),
        windows.end(),
        [](const WindowInfo &left , const WindowInfo &right) {
            return left.title.compare(right.title , Qt::CaseInsensitive) < 0;
        }
    );

    QSet<quintptr> active_handles;

    for (const WindowInfo &info : windows) {
        active_handles.insert(
            reinterpret_cast<quintptr>(info.handle)
        );
    }

    QVector<quintptr> stale_handles;

    for (auto it = _previews.cbegin(); it != _previews.cend(); ++it) {
        if (!active_handles.contains(it.key())) {
            stale_handles.append(it.key());
        }
    }

    for (quintptr handle : stale_handles) {
        LiveWindowPreview *preview = _previews.take(handle);
        _release_preview(preview);
    }

    if (windows.isEmpty()) {
        if (!_empty_label) {
            _empty_label = new QLabel(
                QStringLiteral("No open windows"),
                _content
            );

            _empty_label->setAlignment(Qt::AlignCenter);
            _empty_label->setFont(_font);
            _empty_label->setStyleSheet(
                QStringLiteral("color: %1; background: transparent;")
                    .arg(_text_color.name())
            );

            _content_layout->addWidget(_empty_label , 0 , 0 , 1 , 3);
        }

        _update_preview_sizes();

        return;
    }

    if (_empty_label) {
        _content_layout->removeWidget(_empty_label);
        delete _empty_label;
        _empty_label = nullptr;
    }

    for (LiveWindowPreview *preview : _previews) {
        _content_layout->removeWidget(preview->card);
    }

    for (int index = 0; index < windows.size(); ++index) {
        const WindowInfo &info = windows.at(index);

        const quintptr handle = reinterpret_cast<quintptr>(info.handle);

        LiveWindowPreview *preview = _previews.value(handle , nullptr);

        if (!preview) {
            preview = new LiveWindowPreview();

            preview->source = info.handle;

            preview->card = new QFrame(_content);
            preview->card->setObjectName(QStringLiteral("liveWindowCard"));

            preview->card->setStyleSheet(QStringLiteral(
                "QFrame#liveWindowCard {"
                "background-color: %1;"
                "border: 1px solid %2;"
                "border-radius: 4px;"
                "}"
                "QLabel {"
                "color: %3;"
                "background: transparent;"
                "border: none;"
                "}"
                "QPushButton#liveWindowPreview {"
                "background-color: #111111;"
                "color: %3;"
                "border: 1px solid %2;"
                "border-radius: 3px;"
                "padding: 0px;"
                "}"
                "QPushButton#liveWindowPreview:hover {"
                "border: 1px solid %4;"
                "}"
            ).arg(
                Theme::panel.name(),
                Theme::border.name(),
                Theme::text.name(),
                Theme::highlight.name()
            ));

            auto *card_layout = new QVBoxLayout(preview->card);

            card_layout->setContentsMargins(4 , 4 , 4 , 4);
            card_layout->setSpacing(3);

            preview->title = new QLabel(preview->card);

            preview->title->setFont(_font);
            preview->title->setFixedHeight(18);

            preview->surface = new QPushButton(preview->card);

            preview->card->setMinimumWidth(0);
            preview->title->setMinimumWidth(0);
            preview->surface->setMinimumWidth(0);

            preview->card->setSizePolicy(
                QSizePolicy::Expanding,
                QSizePolicy::Fixed
            );

            preview->surface->setSizePolicy(
                QSizePolicy::Expanding,
                QSizePolicy::Fixed
            );

            preview->surface->setObjectName(
                QStringLiteral("liveWindowPreview")
            );

            preview->surface->setCursor(Qt::PointingHandCursor);
            preview->surface->setFocusPolicy(Qt::NoFocus);
            preview->surface->setFlat(true);

            card_layout->addWidget(preview->title);
            card_layout->addWidget(preview->surface);

            connect(
                preview->surface,
                &QPushButton::clicked,
                this,
                [this , handle]() {
                    _activate_window(handle);
                }
            );

            _previews.insert(handle , preview);
        }

        preview->source = info.handle;

        preview->title->setToolTip(info.title);

        const int row = index / 3;
        const int column = index % 3;

        _content_layout->addWidget(
            preview->card,
            row,
            column
        );
    }

    _update_preview_sizes();
    _update_thumbnails();
}

void LiveWindowsWidget::_update_preview_sizes() {
    if (_previews.isEmpty() || !_scroll_area) {
        return;
    }

    const int available_width = _scroll_area->viewport()->width();

    const int grid_width = std::max(
        1,
        available_width - 8 - 10
    );

    const int card_width = std::max(
        1,
        grid_width / 3
    );

    const int preview_width = std::max(
        1,
        card_width - 8
    );

    const int preview_height = std::min(
        320,
        std::max(1 , qRound(preview_width * 9.0 / 16.0))
    );

    const int title_width = std::max(
        20,
        card_width - 16
    );

    const QFontMetrics metrics(_font);

    for (LiveWindowPreview *preview : _previews) {
        preview->surface->setFixedHeight(preview_height);

        preview->card->setFixedHeight(preview_height + 29);

        preview->title->setText(
            metrics.elidedText(
                preview->title->toolTip(),
                Qt::ElideMiddle,
                title_width
            )
        );
    }

    _content_layout->activate();
}

void LiveWindowsWidget::_update_thumbnails() {
    QWidget *destination_window = window();

    if (!destination_window || !_destination_handle) {
        return;
    }

    const HWND destination = reinterpret_cast<HWND>(_destination_handle);

    const QRect viewport_rect = _scroll_area->viewport()->rect();

    const qreal device_scale = destination_window->devicePixelRatioF();

    for (LiveWindowPreview *preview : _previews) {
        if (!IsWindow(preview->source)) {
            if (preview->thumbnail) {
                DwmUnregisterThumbnail(preview->thumbnail);
                preview->thumbnail = nullptr;
            }

            preview->surface->setText(
                QStringLiteral("Window unavailable")
            );

            continue;
        }

        if (IsIconic(preview->source)) {
            if (preview->thumbnail) {
                DwmUnregisterThumbnail(preview->thumbnail);
                preview->thumbnail = nullptr;
            }

            preview->surface->setText(QStringLiteral("MINIMIZED"));

            continue;
        }

        const QRect preview_viewport_rect(
            preview->surface->mapTo(
                _scroll_area->viewport(),
                QPoint(0 , 0)
            ),
            preview->surface->size()
        );

        if (!viewport_rect.contains(preview_viewport_rect)) {
            if (preview->thumbnail) {
                DWM_THUMBNAIL_PROPERTIES properties{};

                properties.dwFlags = DWM_TNP_VISIBLE;
                properties.fVisible = FALSE;

                DwmUpdateThumbnailProperties(
                    preview->thumbnail,
                    &properties
                );
            }

            continue;
        }

        preview->surface->setText(QString());

        if (!preview->thumbnail) {
            const HRESULT result = DwmRegisterThumbnail(
                destination,
                preview->source,
                &preview->thumbnail
            );

            if (FAILED(result)) {
                preview->surface->setText(
                    QStringLiteral("Preview unavailable")
                );

                continue;
            }
        }

        SIZE source_size{};

        if (FAILED(DwmQueryThumbnailSourceSize(
            preview->thumbnail,
            &source_size
        )) || source_size.cx <= 0 || source_size.cy <= 0) {
            continue;
        }

        const QRect area = preview->surface->rect();

        const qreal scale = std::min(
            static_cast<qreal>(area.width()) / source_size.cx,
            static_cast<qreal>(area.height()) / source_size.cy
        );

        const int width = qRound(source_size.cx * scale);
        const int height = qRound(source_size.cy * scale);

        const QPoint offset(
            (area.width() - width) / 2,
            (area.height() - height) / 2
        );

        const QPoint destination_position = preview->surface->mapTo(
            destination_window,
            offset
        );

        RECT destination_rect{};

        destination_rect.left = qRound(destination_position.x() * device_scale);
        destination_rect.top = qRound(destination_position.y() * device_scale);
        destination_rect.right = qRound((destination_position.x() + width) * device_scale);
        destination_rect.bottom = qRound((destination_position.y() + height) * device_scale);

        DWM_THUMBNAIL_PROPERTIES properties{};

        properties.dwFlags =
            DWM_TNP_RECTDESTINATION |
            DWM_TNP_VISIBLE |
            DWM_TNP_OPACITY;

        properties.rcDestination = destination_rect;
        properties.fVisible = TRUE;
        properties.opacity = 255;

        const HRESULT result = DwmUpdateThumbnailProperties(
            preview->thumbnail,
            &properties
        );

        if (FAILED(result)) {
            DwmUnregisterThumbnail(preview->thumbnail);
            preview->thumbnail = nullptr;

            preview->surface->setText(
                QStringLiteral("Preview unavailable")
            );
        }
    }
}

void LiveWindowsWidget::_release_preview(LiveWindowPreview *preview) {
    if (!preview) {
        return;
    }

    if (preview->thumbnail) {
        DwmUnregisterThumbnail(preview->thumbnail);
        preview->thumbnail = nullptr;
    }

    _content_layout->removeWidget(preview->card);

    delete preview->card;
    delete preview;
}

void LiveWindowsWidget::_activate_window(quintptr handle) {
    const HWND hwnd = reinterpret_cast<HWND>(handle);

    if (!IsWindow(hwnd)) {
        _refresh_windows();

        return;
    }

    if (IsIconic(hwnd)) {
        ShowWindow(hwnd , SW_RESTORE);
    }

    SetForegroundWindow(hwnd);
}

void LiveWindowsWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    _update_preview_sizes();

    QTimer::singleShot(
        0,
        this,
        &LiveWindowsWidget::_update_thumbnails
    );
}

void LiveWindowsWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QPainter painter(this);

    _paint_panel(painter , _title);
}

}

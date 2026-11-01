#pragma once

#include <QObject>
#include <QElapsedTimer>
#include <QTimer>

namespace ttydash {

class SystemMonitor : public QObject {
    Q_OBJECT

public:
    SystemMonitor();

    void _start(int interval_ms);
    void _set_interval(int interval_ms);

signals:
    void cpu_updated(qreal usage_percent);
    void memory_updated(quint64 total_bytes , quint64 used_bytes);
    void network_updated(quint64 upload_bytes_per_second , quint64 download_bytes_per_second);

private:
    void _collect();

    QTimer _timer;
    bool _has_cpu_sample = false;
    quint64 _previous_idle = 0;
    quint64 _previous_kernel = 0;
    quint64 _previous_user = 0;
    bool _has_network_sample = false;
    quint64 _previous_sent = 0;
    quint64 _previous_received = 0;
    QElapsedTimer _network_elapsed;
};

}

#include "services/SystemMonitor.h"

#include "utils/Logger.h"

#include <algorithm>

#ifdef Q_OS_WIN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iphlpapi.h>
#include <netioapi.h>
#endif

namespace ttydash {
namespace {

#ifdef Q_OS_WIN
quint64 _filetime_value(const FILETIME &file_time) {
    ULARGE_INTEGER value;
    value.LowPart = file_time.dwLowDateTime;
    value.HighPart = file_time.dwHighDateTime;

    return value.QuadPart;
}
#endif

}

SystemMonitor::SystemMonitor() {
    connect(&_timer , &QTimer::timeout , this , [this]() {
        _collect();
    });
}

void SystemMonitor::_start(int interval_ms) {
    _set_interval(interval_ms);
    _network_elapsed.start();
    _timer.start();
    _collect();
}

void SystemMonitor::_set_interval(int interval_ms) {
    _timer.setInterval(std::clamp(interval_ms , 500 , 10000));
}

void SystemMonitor::_collect() {
#ifdef Q_OS_WIN
    FILETIME idle_time;
    FILETIME kernel_time;
    FILETIME user_time;
    if (GetSystemTimes(&idle_time , &kernel_time , &user_time)) {
        const quint64 idle = _filetime_value(idle_time);
        const quint64 kernel = _filetime_value(kernel_time);
        const quint64 user = _filetime_value(user_time);
        if (_has_cpu_sample) {
            const quint64 idle_delta = idle - _previous_idle;
            const quint64 kernel_delta = kernel - _previous_kernel;
            const quint64 user_delta = user - _previous_user;
            const quint64 total_delta = kernel_delta + user_delta;
            const qreal usage = total_delta == 0
                ? 0.0
                : std::clamp(100.0 * static_cast<qreal>(total_delta - std::min(total_delta , idle_delta)) / static_cast<qreal>(total_delta) , 0.0 , 100.0);
            emit cpu_updated(usage);
        }

        _previous_idle = idle;
        _previous_kernel = kernel;
        _previous_user = user;
        _has_cpu_sample = true;
    } else {
        Logger::_warning(QStringLiteral("GetSystemTimes failed with Windows error %1").arg(GetLastError()));
    }

    MEMORYSTATUSEX memory_status{};
    memory_status.dwLength = sizeof(memory_status);
    if (GlobalMemoryStatusEx(&memory_status)) {
        const quint64 used = memory_status.ullTotalPhys - memory_status.ullAvailPhys;
        emit memory_updated(memory_status.ullTotalPhys , used);
    } else {
        Logger::_warning(QStringLiteral("GlobalMemoryStatusEx failed with Windows error %1").arg(GetLastError()));
    }

    PMIB_IF_TABLE2 table = nullptr;
    if (GetIfTable2(&table) == NO_ERROR && table) {
        quint64 sent = 0;
        quint64 received = 0;
        for (ULONG index = 0; index < table->NumEntries; ++index) {
            const MIB_IF_ROW2 &row = table->Table[index];
            if (row.Type == IF_TYPE_SOFTWARE_LOOPBACK || row.OperStatus != IfOperStatusUp) {
                continue;
            }

            sent += row.OutOctets;
            received += row.InOctets;
        }
        FreeMibTable(table);

        const qint64 elapsed_ms = _network_elapsed.isValid() ? _network_elapsed.restart() : 0;
        if (_has_network_sample && elapsed_ms > 0 && sent >= _previous_sent && received >= _previous_received) {
            const quint64 upload = static_cast<quint64>(1000.0 * static_cast<qreal>(sent - _previous_sent) / static_cast<qreal>(elapsed_ms));
            const quint64 download = static_cast<quint64>(1000.0 * static_cast<qreal>(received - _previous_received) / static_cast<qreal>(elapsed_ms));
            emit network_updated(upload , download);
        }

        _previous_sent = sent;
        _previous_received = received;
        _has_network_sample = true;
    } else {
        if (table) {
            FreeMibTable(table);
        }
        Logger::_warning(QStringLiteral("GetIfTable2 failed; network rates are unavailable"));
    }
#else
    Logger::_warning(QStringLiteral("Windows system monitoring is unavailable on this platform"));
#endif
}

}

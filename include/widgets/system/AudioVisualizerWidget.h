#pragma once

#include "widgets/DashboardWidget.h"

#include <QTimer>

#include <array>

struct IAudioMeterInformation;

namespace ttydash {

class AudioVisualizerWidget : public DashboardWidget {
public:
    AudioVisualizerWidget(const WidgetConfig &config , QWidget *parent = nullptr);
    ~AudioVisualizerWidget() override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void _sample_audio();

    QTimer _timer;
    IAudioMeterInformation *_meter = nullptr;

    static constexpr int bar_count = 16;
    std::array<qreal , bar_count> _levels{};
    
    qreal _peak = 0.0;
    quint64 _frame = 0;
    bool _owns_com_initialization = false;
};

}

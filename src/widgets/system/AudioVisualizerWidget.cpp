#include "widgets/system/AudioVisualizerWidget.h"

#include <QPainter>

#ifdef Q_OS_WIN
#include <initguid.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <objbase.h>

MIDL_INTERFACE("c02216f6-8c67-4b5b-9d00-d008e73e0064")
IAudioMeterInformation : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(FLOAT *peak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(UINT *count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(UINT count , FLOAT *values) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD *support) = 0;
};

constexpr IID IID_IAudioMeterInformation{
    0xc02216f6 , 0x8c67 , 0x4b5b , {0x9d , 0x00 , 0xd0 , 0x08 , 0xe7 , 0x3e , 0x00 , 0x64}
};
#endif

#include <algorithm>
#include <cmath>

namespace ttydash {

AudioVisualizerWidget::AudioVisualizerWidget(const WidgetConfig &config , QWidget *parent)
    : DashboardWidget(config , parent) {
#ifdef Q_OS_WIN
    const HRESULT initialized = CoInitializeEx(nullptr , COINIT_APARTMENTTHREADED);
    _owns_com_initialization = SUCCEEDED(initialized);
    if (SUCCEEDED(initialized) || initialized == RPC_E_CHANGED_MODE) {
        IMMDeviceEnumerator *enumerator = nullptr;
        IMMDevice *device = nullptr;
        if (SUCCEEDED(CoCreateInstance(CLSID_MMDeviceEnumerator , nullptr , CLSCTX_ALL , IID_IMMDeviceEnumerator , reinterpret_cast<void **>(&enumerator))) && enumerator) {
            if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(eRender , eMultimedia , &device)) && device) {
                device->Activate(IID_IAudioMeterInformation , CLSCTX_ALL , nullptr , reinterpret_cast<void **>(&_meter));
                device->Release();
            }
            enumerator->Release();
        }
    }
#endif
    connect(&_timer , &QTimer::timeout , this , [this]() {
        _sample_audio();
    });
    _timer.start(30);
}

AudioVisualizerWidget::~AudioVisualizerWidget() {
    _timer.stop();
#ifdef Q_OS_WIN
    if (_meter) _meter->Release();
    if (_owns_com_initialization) CoUninitialize();
#endif
}

void AudioVisualizerWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    _paint_panel(painter , _title);
    painter.setPen(Qt::NoPen);

    constexpr int bar_count = 16;
    constexpr int gap = 4;
    const int top = 38;
    const int bottom = height() - 12;
    const int available_width = qMax(1 , width() - 28);
    const int bar_width = qMax(2 , (available_width - gap * (bar_count - 1)) / bar_count);
    const int bars_width = bar_width * bar_count + gap * (bar_count - 1);
    const int left = (width() - bars_width) / 2;
    const int max_height = qMax(2 , bottom - top);

    for (int index = 0; index < bar_count; ++index) {
        const int bar_height = qMax(2 , qRound(max_height * _levels.at(index)));
        const int x = left + index * (bar_width + gap);
        painter.setBrush(_accent_color);
        painter.drawRoundedRect(QRect(x , bottom - bar_height , bar_width , bar_height) , 2 , 2);
    }
}

void AudioVisualizerWidget::_sample_audio() {
    float measured_peak = 0.0F;
#ifdef Q_OS_WIN
    if (_meter) _meter->GetPeakValue(&measured_peak);
#endif
    _peak = std::clamp(static_cast<qreal>(measured_peak) , 0.0 , 1.0);
    const qreal audible = qBound(0.0 , (_peak - 0.005) / 0.22 , 1.0);
    ++_frame;

    for (int index = 0; index < static_cast<int>(_levels.size()); ++index) {
        const qreal wave = 0.18 + 0.82 * std::abs(std::sin(static_cast<qreal>(_frame) * 0.11 + index * 0.57));
        const qreal target = audible * wave;
        _levels[static_cast<size_t>(index)] = target > _levels[static_cast<size_t>(index)]
            ? target
            : _levels[static_cast<size_t>(index)] * 0.88;
    }
    update();
}

}

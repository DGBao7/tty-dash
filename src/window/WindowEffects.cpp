#include "window/WindowEffects.h"

#include "utils/Logger.h"

#ifdef Q_OS_WIN
#include <dwmapi.h>
#include <windows.h>

#include <cstring>

namespace ttydash {
namespace {

constexpr DWORD _dwmwa_use_immersive_dark_mode = 20;
constexpr DWORD _dwmwa_system_backdrop_type = 38;

struct AccentPolicy {
    int state;
    int flags;
    DWORD gradient_color;
    int animation_id;
};

struct CompositionAttributeData {
    int attribute;
    PVOID data;
    SIZE_T size;
};

using SetWindowCompositionAttributeFunction = BOOL(WINAPI *)(HWND , CompositionAttributeData *);

struct OsVersionInfo {
    ULONG size;
    ULONG major;
    ULONG minor;
    ULONG build;
    ULONG platform;
    WCHAR service_pack[128];
};

template <typename Function>
Function _load_function(HMODULE module , const char *name) {
    Function function = nullptr;
    const FARPROC address = GetProcAddress(module , name);
    static_assert(sizeof(function) == sizeof(address));
    std::memcpy(&function , &address , sizeof(function));

    return function;
}

bool _get_windows_version(ULONG &major , ULONG &build) {
    const HMODULE module = GetModuleHandleW(L"ntdll.dll");
    if (!module) {
        return false;
    }

    using RtlGetVersionFunction = LONG(WINAPI *)(OsVersionInfo *);
    const auto get_version = _load_function<RtlGetVersionFunction>(module , "RtlGetVersion");
    if (!get_version) {
        return false;
    }

    OsVersionInfo version{};
    version.size = sizeof(version);
    if (get_version(&version) != 0) {
        return false;
    }

    major = version.major;
    build = version.build;

    return true;
}

bool _apply_acrylic(HWND hwnd , const QColor &color) {
    const HMODULE user32 = GetModuleHandleW(L"user32.dll");
    const auto set_composition_attribute = user32
        ? _load_function<SetWindowCompositionAttributeFunction>(user32 , "SetWindowCompositionAttribute")
        : nullptr;
    if (!set_composition_attribute) {
        return false;
    }

    AccentPolicy policy{};
    policy.state = 4;
    policy.gradient_color = color.rgba();
    CompositionAttributeData data{};
    data.attribute = 19;
    data.data = &policy;
    data.size = sizeof(policy);

    return set_composition_attribute(hwnd , &data) != FALSE;
}

bool _apply_system_backdrop(HWND hwnd , DWORD backdrop_type) {
    BOOL dark_mode = TRUE;
    const HRESULT dark_result = DwmSetWindowAttribute(hwnd , _dwmwa_use_immersive_dark_mode , &dark_mode , sizeof(dark_mode));
    const HRESULT backdrop_result = DwmSetWindowAttribute(hwnd , _dwmwa_system_backdrop_type , &backdrop_type , sizeof(backdrop_type));
    if (FAILED(backdrop_result)) {
        return false;
    }

    MARGINS margins{-1 , -1 , -1 , -1};
    DwmExtendFrameIntoClientArea(hwnd , &margins);
    Q_UNUSED(dark_result)

    return true;
}

}

bool WindowEffects::_apply(HWND hwnd , const EffectsSettings &settings , const QString &backdrop_override , const QString &color_override) const {
    if (!hwnd || !settings.blur) {
        return false;
    }

    const QString backdrop = backdrop_override.isEmpty() ? settings.backdrop : backdrop_override;
    const QColor color(color_override.isEmpty() ? settings.background_color : color_override);
    ULONG major = 0;
    ULONG build = 0;
    const bool have_version = _get_windows_version(major , build);

    if (have_version && major >= 10 && build >= 22621) {
        const DWORD system_backdrop = backdrop == QStringLiteral("mica") ? 2 : 3;
        if ((backdrop == QStringLiteral("mica") || backdrop == QStringLiteral("acrylic"))
            && _apply_system_backdrop(hwnd , system_backdrop)) {
            return true;
        }
    }

    if ((backdrop == QStringLiteral("acrylic") || backdrop == QStringLiteral("mica"))
        && have_version && major >= 10 && build >= 17134 && _apply_acrylic(hwnd , color)) {
        return true;
    }

    if (have_version && major < 10) {
        DWM_BLURBEHIND blur{};
        blur.dwFlags = DWM_BB_ENABLE;
        blur.fEnable = TRUE;
        if (SUCCEEDED(DwmEnableBlurBehindWindow(hwnd , &blur))) {
            return true;
        }
    }

    Logger::_warning(QStringLiteral("Requested DWM backdrop is unavailable; dashboard host remains transparent"));

    return false;
}

void WindowEffects::_clear(HWND hwnd) const {
    if (!hwnd) {
        return;
    }

    const DWORD no_backdrop = 1;
    DwmSetWindowAttribute(hwnd , _dwmwa_system_backdrop_type , &no_backdrop , sizeof(no_backdrop));
    const MARGINS no_extended_frame{0 , 0 , 0 , 0};
    DwmExtendFrameIntoClientArea(hwnd , &no_extended_frame);

    const HMODULE user32 = GetModuleHandleW(L"user32.dll");
    const auto set_composition_attribute = user32
        ? _load_function<SetWindowCompositionAttributeFunction>(user32 , "SetWindowCompositionAttribute")
        : nullptr;
    if (set_composition_attribute) {
        AccentPolicy policy{};
        policy.state = 0;
        CompositionAttributeData data{};
        data.attribute = 19;
        data.data = &policy;
        data.size = sizeof(policy);
        set_composition_attribute(hwnd , &data);
    }

    DWM_BLURBEHIND blur{};
    blur.dwFlags = DWM_BB_ENABLE;
    blur.fEnable = FALSE;
    DwmEnableBlurBehindWindow(hwnd , &blur);
}

}
#endif

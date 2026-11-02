# TTY Dashboard

A lightweight Qt 6 desktop overlay for Windows 10 and Windows 11. The program uses Qt for rendering and monitor geometry, Win32 for click-through windows and global hotkeys, DWM for optional backdrops, and Windows APIs for system metrics.

## Build

Use an MSYS2 MINGW64 environment with CMake, Ninja, MinGW-w64, and Qt 6 Widgets installed:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="D:/works/tools/msys2/mingw64"
cmake --build build
```

Set `CMAKE_PREFIX_PATH` to the MSYS2 MINGW64 prefix on your machine.

## Run

```powershell
.\build\tty-dashboard.exe
```

## Configuration and controls

The bundled `configs/dashboard.json` is the first-run template. The writable configuration is created at Qt's `QStandardPaths::AppConfigLocation` as `dashboard.json`; if that location is unavailable, the app falls back to a portable `dashboard.json` beside the executable. Set `TTY_DASH_CONFIG` to choose an explicit path. Invalid configuration is replaced with defaults, and saves use `QSaveFile` to avoid partially written JSON.

Global shortcuts:

- `Ctrl+Alt+Q`: exit
- `Ctrl+Alt+H`: hide or show dashboards
- `Ctrl+Alt+R`: reload configuration
- `Ctrl+Alt+I`: toggle click-through

Dashboard widgets keep their configured positions and sizes. Drag the handle at the top of TerBon to move it, or use its lower-right grip to resize it. TerBon geometry is saved to the writable JSON file.

## Backdrop support

Mica and Desktop Acrylic use the documented `DWMWA_SYSTEMBACKDROP_TYPE` on Windows 11 build 22621 or newer. Windows 10 uses the Windows composition accent policy as an optional Acrylic fallback; that older API is resolved dynamically because it is not a documented public contract. On unsupported systems the app draws the configured translucent background color. Opacity remains a separate window setting. Windows does not expose a general blur-intensity control for these effects.

The app also places a control icon in the system tray when a tray is available. This remains usable if Windows refuses one or more global hotkey registrations.

# TTY Dashboard

**A lightweight Windows desktop overlay with live system widgets and a built-in command terminal.**

TTY Dashboard is a native desktop utility built with **C++20**, **Qt 6**, and the Windows API. It places configurable widgets directly on the desktop and includes **TerBon**, a compact terminal for everyday file and development tasks.

## Features

### Desktop Dashboard

- **Desktop overlay** — borderless dashboard windows designed to sit over the desktop without behaving like ordinary application windows.
- **Multi-monitor support** — create layouts for connected displays and restore them when display configuration changes.
- **Move and resize widgets** — arrange widgets directly on screen; their geometry is saved to the configuration.
- **Click-through mode** — let mouse clicks pass through the dashboard, then switch to interactive mode whenever you need to rearrange widgets.
- **Window controls** — optional always-on-top behavior, adjustable opacity, and translucent backgrounds.
- **System tray controls** — show or hide dashboards, toggle click-through, open TerBon, reload settings, or exit from the tray menu.
- **Quick Launch** — provides shortcuts to toggle the built-in TerBon window, open the Windows default browser, launch Visual Studio Code, open File Explorer, and launch GitHub Desktop.
- **Live Windows** — displays thumbnail previews of currently open application windows. Select a preview to bring the corresponding window to the foreground.

The left dashboard column is divided into two stacked sections: Quick Launch on top and Live Windows below.

### Built-in Widgets

| Widget | Description |
| --- | --- |
| Clock | Displays the current time and date. |
| CPU | Live processor usage with a history graph. |
| RAM | Physical memory usage with a history graph. |
| Network | Network throughput monitoring. |
| Disk | Storage information for connected drives. |
| Audio Visualizer | Visualizes the current system audio output level. It is **not** a music player. |
| Matrix | Animated Matrix-style visual effect. |
| Live Windows | Displays live thumbnail previews of open application windows and allows users to activate them. |

Widget positions, dimensions, visibility, and supported display options are configurable through `dashboard.json`.

### TerBon Terminal

TerBon is a built-in, command-driven terminal with its own interface and status bar. It is designed for quick file operations and development tasks without opening a separate terminal window.

- Fixed-size **700 × 350** window with a draggable top handle.
- Remembers its position when geometry saving is enabled.
- Displays the current working directory and command execution status.
- Supports command history navigation with **Up** and **Down**.
- Supports interrupting a running process with **Ctrl + C**.
- Can launch supported programs and stream their output inside the terminal.

TerBon uses its own command set; it is not intended to be a drop-in replacement for Command Prompt or PowerShell.

#### Available commands

| Command | Purpose |
| --- | --- |
| `cd <path>` | Change the working directory. |
| `pwd` | Print the current working directory. |
| `dir` | List the current directory. |
| `tre <depth>` | Display a directory tree up to the specified depth. |
| `ope <target> <argument>` | Open a supported file, folder, application, or URL. |
| `cre <target> <argument>` | Create a file or folder. |
| `del <target> <argument>` | Delete a file or folder; confirmation may be required. |
| `sea <target> <argument>` | Search for files or folders. |
| `run <type> <file>` | Run a supported C++, Python, JavaScript, or executable target. |
| `bui <target> <file> -- <sources>` | Build C++ source files. |
| `ana <target> <file>` | Analyze a supported source file. |
| `for <target> <file>` | Format supported C++ or Python files. |
| `cle` | Clear the terminal. |
| `help` / `help <command>` | List commands or show command usage. |

Some TerBon operations rely on external development tools. For example, running or building C++ code requires a working `g++` installation; Python and JavaScript execution require their respective runtimes to be available on `PATH`.

## Keyboard Shortcuts

| Shortcut | Action |
| --- | --- |
| `Ctrl + Alt + Q` | Exit TTY Dashboard. |
| `Ctrl + Alt + H` | Show or hide the dashboards. |
| `Ctrl + Alt + R` | Reload the configuration. |
| `Ctrl + Alt + I` | Toggle click-through mode. |
| `Ctrl + Alt + T` | Show or hide TerBon. |

Global shortcuts may be unavailable when another application has already registered the same key combination. The system tray menu provides an alternative for the main controls.

## Appearance and Backdrop Effects

TTY Dashboard supports translucent dashboard backgrounds and optional Windows backdrop effects:

- **Mica and system Acrylic** use the Windows DWM system-backdrop API on supported Windows 11 builds (22621 or newer).
- **Acrylic fallback** may be available on supported earlier Windows versions through a dynamically resolved Windows composition API.
- When a native backdrop is unavailable, the dashboard falls back to its configured translucent background color.

Backdrop availability depends on the Windows version and system composition settings. Window opacity is controlled separately from the backdrop effect.

## Configuration

The default dashboard template is bundled with the application. On first launch, TTY Dashboard creates a writable `dashboard.json` at Qt's `QStandardPaths::AppConfigLocation`.

- Set the `TTY_DASH_CONFIG` environment variable to use a specific configuration file.
- If the normal configuration location cannot be used, the application falls back to a portable `dashboard.json` beside the executable.
- Invalid configuration data is replaced with default settings.
- Configuration updates use `QSaveFile` to reduce the risk of partially written JSON files.

The main configuration controls application behavior, display layouts, widget settings, backdrop effects, and TerBon geometry. TerBon command descriptions and supported targets are defined in `configs/commands.json`.

## Requirements

- Windows 10 or Windows 11 (x64).
- Qt 6.2 or newer, with the **Core**, **Gui**, and **Widgets** modules.
- A C++20-compatible MinGW-w64 toolchain.
- CMake 3.21 or newer.
- Ninja build system.
- MSYS2 **MINGW64** environment.

## Build from Source

Open a PowerShell or terminal session configured to use the MSYS2 MINGW64 toolchain. Set `CMAKE_PREFIX_PATH` to the Qt/MSYS2 MINGW64 installation on your machine.

```powershell
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="D:/path/to/msys2/mingw64"
cmake --build build
```

Replace the example path with your actual MSYS2 MINGW64 prefix.

## Run

After a successful build:

```powershell
.\build\tty-dashboard.exe
```

## Build the Windows Installer

The Inno Setup definition is located at `installer/tty-dashboard.iss`.

1. Build the application and prepare the deployment bundle required by the `[Files]` section of the installer script. Include the Qt runtime DLLs, required MinGW runtime DLLs, and Qt platform plugins.
2. Make sure the source paths in `installer/tty-dashboard.iss` point to the latest build output and project assets.
3. Compile the installer from the repository root:

```powershell
ISCC.exe installer\tty-dashboard.iss
```

The installer output directory and filename are configured in the `.iss` file.

## Project Structure

```text
tty-dash/
├── assets/
│   └── fonts/
├── configs/
│   ├── commands.json
│   └── dashboard.json
├── include/
├── installer/
│   └── tty-dashboard.iss
├── resources/
│   ├── terbon/
│   └── themes/
├── src/
│   ├── app/
│   ├── config/
│   ├── display/
│   ├── hotkeys/
│   ├── layout/
│   ├── services/
│   ├── terbon/
│   ├── utils/
│   ├── widgets/
│   └── window/
├── CMakeLists.txt
└── README.md
```

## Credits

**Author:** Bon  
**Project:** [TTY Dashboard](https://github.com/DGBao7/tty-dash)
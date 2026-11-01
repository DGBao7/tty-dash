cmake_minimum_required(VERSION 3.21)

project(TtyDashboard
    VERSION 0.1.0
    LANGUAGES CXX
)

if(NOT WIN32)
    message(FATAL_ERROR "TTY Dashboard uses native Win32 APIs and must be built on Windows")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(TtyDashboard WIN32
    include/app/AppController.h
    include/config/ConfigManager.h
    include/display/DisplayManager.h
    include/hotkeys/HotkeyManager.h
    include/layout/LayoutManager.h
    include/services/SystemMonitor.h
    include/utils/Logger.h
    include/widgets/DashboardWidget.h
    include/widgets/WidgetManager.h
    include/widgets/ascii/MatrixWidget.h
    include/widgets/ascii/PipesWidget.h
    include/widgets/clock/ClockWidget.h
    include/widgets/system/CpuWidget.h
    include/widgets/system/RamWidget.h
    include/widgets/system/NetworkWidget.h
    include/window/DashboardWindow.h
    include/window/WindowEffects.h
    resources/resources.qrc
    src/main.cpp
    src/app/AppController.cpp
    src/config/ConfigManager.cpp
    src/display/DisplayManager.cpp
    src/hotkeys/HotkeyManager.cpp
    src/layout/LayoutManager.cpp
    src/services/SystemMonitor.cpp
    src/utils/Logger.cpp
    src/widgets/DashboardWidget.cpp
    src/widgets/WidgetManager.cpp
    src/widgets/ascii/MatrixWidget.cpp
    src/widgets/ascii/PipesWidget.cpp
    src/widgets/clock/ClockWidget.cpp
    src/widgets/system/CpuWidget.cpp
    src/widgets/system/RamWidget.cpp
    src/widgets/system/NetworkWidget.cpp
    src/window/DashboardWindow.cpp
    src/window/WindowEffects.cpp
)

target_include_directories(TtyDashboard
    PRIVATE
        include
)

target_compile_definitions(TtyDashboard
    PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        WINVER=0x0A00
        _WIN32_WINNT=0x0A00
)

target_link_libraries(TtyDashboard
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        dwmapi
        iphlpapi
        user32
)

if(MSVC)
    target_compile_options(TtyDashboard PRIVATE /W4)
else()
    target_compile_options(TtyDashboard PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(TtyDashboard PROPERTIES
    OUTPUT_NAME tty-dashboard
)

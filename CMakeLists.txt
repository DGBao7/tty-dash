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

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/configs")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/configs/dashboard.json"
    "${CMAKE_BINARY_DIR}/configs/dashboard.json"
    COPYONLY
)

add_executable(TtyDashboard
    include/app/AppController.h
    include/config/ConfigManager.h
    include/display/DisplayManager.h
    include/hotkeys/HotkeyManager.h
    include/layout/LayoutManager.h
    include/services/SystemMonitor.h
    include/theme/Theme.h
    include/terbon/TerBonController.h
    include/terbon/TerBonWindow.h
    include/terbon/ui/TerminalWidget.h
    include/terbon/ui/StatusBar.h
    include/terbon/ui/themes/Colors.h
    include/terbon/ui/themes/Interface.h
    include/terbon/pas/Command.h
    include/terbon/pas/Sys.h
    include/terbon/str/AnalizeStr.h
    include/terbon/str/AnalyzePath.h
    include/terbon/str/clangCode.h
    include/terbon/nlohmann/json.hpp
    include/utils/Logger.h
    include/widgets/DashboardWidget.h
    include/widgets/WidgetManager.h
    include/widgets/ascii/MatrixWidget.h
    include/widgets/ascii/PipesWidget.h
    include/widgets/clock/ClockWidget.h
    include/widgets/system/CpuWidget.h
    include/widgets/system/RamWidget.h
    include/widgets/system/NetworkWidget.h
    include/widgets/system/DiskWidget.h
    include/widgets/system/AudioVisualizerWidget.h
    include/window/DashboardWindow.h
    include/window/WindowEffects.h
    include/widgets/launch/QuickLaunchWidget.h
    src/widgets/launch/QuickLaunchWidget.cpp
    resources/resources.qrc
    src/main.cpp
    src/app/AppController.cpp
    src/config/ConfigManager.cpp
    src/display/DisplayManager.cpp
    src/hotkeys/HotkeyManager.cpp
    src/layout/LayoutManager.cpp
    src/services/SystemMonitor.cpp
    src/terbon/TerBonController.cpp
    src/terbon/TerBonWindow.cpp
    src/terbon/ui/TerminalWidget.cpp
    src/terbon/ui/StatusBar.cpp
    src/terbon/ui/themes/Colors.cpp
    src/terbon/ui/themes/Interface.cpp
    src/terbon/pas/Command.cpp
    src/terbon/pas/Sys.cpp
    src/terbon/str/AnalizeStr.cpp
    src/terbon/str/AnalyzePath.cpp
    src/terbon/str/clangCode.cpp
    src/utils/Logger.cpp
    src/widgets/DashboardWidget.cpp
    src/widgets/WidgetManager.cpp
    src/widgets/ascii/MatrixWidget.cpp
    src/widgets/ascii/PipesWidget.cpp
    src/widgets/clock/ClockWidget.cpp
    src/widgets/system/CpuWidget.cpp
    src/widgets/system/RamWidget.cpp
    src/widgets/system/NetworkWidget.cpp
    src/widgets/system/DiskWidget.cpp
    src/widgets/system/AudioVisualizerWidget.cpp
    src/window/DashboardWindow.cpp
    src/window/WindowEffects.cpp
)

target_include_directories(TtyDashboard
    PRIVATE
        include
        include/terbon
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
        ole32
        uuid
)

add_custom_command(TARGET TtyDashboard POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:TtyDashboard>/configs"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/configs/commands.json" "$<TARGET_FILE_DIR:TtyDashboard>/configs/commands.json"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/resources/terbon/clang-format" "$<TARGET_FILE_DIR:TtyDashboard>/.clang-format"
)

if(MSVC)
    target_compile_options(TtyDashboard PRIVATE /W4)
else()
    target_compile_options(TtyDashboard PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(TtyDashboard PROPERTIES
    OUTPUT_NAME tty-dashboard
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

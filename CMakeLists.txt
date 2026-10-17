cmake_minimum_required(VERSION 3.20)
project(MemStand LANGUAGES CXX RC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
    src/core/memory_manager.cpp
    src/core/timer_resolution.cpp
    src/core/automation_engine.cpp
    src/core/config_manager.cpp
    src/core/task_scheduler.cpp
    src/core/update_checker.cpp
    src/i18n/localization.cpp
    src/ui/tray_icon.cpp
    src/ui/main_window.cpp
    resources/MemStand.rc
)

add_executable(MemStand WIN32 ${SOURCES})

target_include_directories(MemStand PRIVATE
    .
    src
    resources
)

target_compile_definitions(MemStand PRIVATE
    UNICODE
    _UNICODE
    NOMINMAX
    WIN32_LEAN_AND_MEAN
)

target_link_libraries(MemStand PRIVATE
    user32
    gdi32
    comctl32
    shell32
    dwmapi
    shlwapi
    psapi
    advapi32
    winmm
    winhttp
)

if(MSVC)
    target_compile_options(MemStand PRIVATE /utf-8)
    target_link_options(MemStand PRIVATE /MANIFEST:NO)
endif()


# Copy languages folder on build
add_custom_command(TARGET MemStand POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/languages
        $<TARGET_FILE_DIR:MemStand>/languages
)

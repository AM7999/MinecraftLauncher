cmake_minimum_required(VERSION 4.0)

project(MinecraftLauncher)

find_package(CURL REQUIRED)
find_package(SDL3 REQUIRED)

include(FetchContent)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG        v1.92.5
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

add_executable(MinecraftLauncher
        src/Structs.h
        src/Main.cpp
        src/Window.cpp
        src/Window.hpp
        src/Dialogs.hpp
        src/Dialogs.cpp
        src/Logic.cpp
        src/Logic.hpp
)
target_link_libraries(MinecraftLauncher CURL::libcurl SDL3::SDL3 imgui)

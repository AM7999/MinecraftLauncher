cmake_minimum_required(VERSION 3.16...4.0)

set(CMAKE_GENERATOR "Ninja" CACHE INTERNAL "" FORCE)

project(MinecraftLauncher)

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(SDL3 REQUIRED)

include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG        v1.91.9b
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(imgui)

add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui_lib PUBLIC SDL3::SDL3)

add_executable(MinecraftLauncher WIN32
    src/Structs.h
    src/Main.cpp
    src/Window.cpp
    src/Window.hpp
    src/Dialogs.hpp
    src/Dialogs.cpp
    src/Logic.cpp
    src/Logic.hpp
)

set_target_properties(MinecraftLauncher PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(MinecraftLauncher
    PRIVATE
    SDL3::SDL3
    imgui_lib
    OpenSSL::SSL
    CURL::libcurl
)

if(WIN32)
    target_link_options(MinecraftLauncher PRIVATE -static -static-libgcc -static-libstdc++)
elseif(APPLE)

elseif(UNIX)
    target_link_options(MinecraftLauncher PRIVATE -static-libgcc -static-libstdc++)
endif()

if(WIN32)
    add_custom_command(TARGET MinecraftLauncher POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:MinecraftLauncher>
            $<TARGET_FILE_DIR:MinecraftLauncher>
        COMMAND_EXPAND_LISTS
    )
endif()
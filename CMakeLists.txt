cmake_minimum_required(VERSION 3.16...4.0)

project(MinecraftLauncher)

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(SDL3 REQUIRED)

set(CMAKE_CXX_STANDARD 20)

#set(CURL_USE_STATIC_LIBS ON)

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

target_link_libraries(imgui
        PUBLIC
        SDL3::SDL3
)

set(OPENSSL_USE_STATIC_LIBS TRUE)

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
target_link_libraries(MinecraftLauncher SDL3::SDL3 imgui OpenSSL::SSL)

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
elseif(APPLE)
    add_custom_command(TARGET MinecraftLauncher POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:MinecraftLauncher>/libSDL3.dylib
        COMMAND ${CMAKE_COMMAND} -E copy
            /usr/local/opt/openssl/lib/libssl.3.dylib
            $<TARGET_FILE_DIR:MinecraftLauncher>/libssl.3.dylib
        COMMAND ${CMAKE_COMMAND} -E copy
            /usr/local/opt/openssl/lib/libcrypto.3.dylib
            $<TARGET_FILE_DIR:MinecraftLauncher>/libcrypto.3.dylib
        COMMAND install_name_tool -id @executable_path/libSDL3.dylib
            $<TARGET_FILE_DIR:MinecraftLauncher>/libSDL3.dylib
        COMMAND install_name_tool -id @executable_path/libssl.3.dylib
            $<TARGET_FILE_DIR:MinecraftLauncher>/libssl.3.dylib
        COMMAND install_name_tool -id @executable_path/libcrypto.3.dylib
            $<TARGET_FILE_DIR:MinecraftLauncher>/libcrypto.3.dylib
        COMMAND install_name_tool -change
            $<TARGET_SONAME_FILE:SDL3::SDL3>
            @executable_path/libSDL3.dylib
            $<TARGET_FILE:MinecraftLauncher>
        COMMAND install_name_tool -change
            /usr/local/opt/openssl/lib/libssl.3.dylib
            @executable_path/libssl.3.dylib
            $<TARGET_FILE:MinecraftLauncher>
        COMMAND install_name_tool -change
            /usr/local/opt/openssl/lib/libcrypto.3.dylib
            @executable_path/libcrypto.3.dylib
            $<TARGET_FILE:MinecraftLauncher>
    )
elseif(UNIX)
    add_custom_command(TARGET MinecraftLauncher POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:MinecraftLauncher>/libSDL3.so
        COMMAND ${CMAKE_COMMAND} -E copy
            /usr/lib/libssl.so.3
            $<TARGET_FILE_DIR:MinecraftLauncher>/libssl.so.3
        COMMAND ${CMAKE_COMMAND} -E copy
            /usr/lib/libcrypto.so.3
            $<TARGET_FILE_DIR:MinecraftLauncher>/libcrypto.so.3
        COMMAND patchelf --set-rpath '$ORIGIN'
            $<TARGET_FILE:MinecraftLauncher>
    )
endif()
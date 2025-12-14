cmake_minimum_required(VERSION 3.16)
project(PacmanSFML LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(PACMAN_COPY_ASSETS "Copy assets next to executable" ON)
option(PACMAN_COPY_RUNTIME_DLLS "Copy runtime DLLs next to executable (Windows)" ON)

# vcpkg-friendly config mode
find_package(SFML CONFIG REQUIRED COMPONENTS graphics window system audio)

find_package(nlohmann_json CONFIG REQUIRED)

add_executable(pacman
    src/main.cpp
    src/Game.cpp
    src/Map.cpp
    src/Player.cpp
    src/Ghost.cpp
    src/Menu.cpp
    src/Renderer.cpp
    src/BitmapFont.cpp
    src/SpriteAtlas.cpp
    src/AudioManager.cpp
)

target_include_directories(pacman PRIVATE src)

target_link_libraries(pacman PRIVATE sfml-graphics sfml-window sfml-system sfml-audio nlohmann_json::nlohmann_json)

if(PACMAN_COPY_ASSETS)
    add_custom_command(TARGET pacman POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:pacman>/assets
    )
endif()

if(WIN32 AND PACMAN_COPY_RUNTIME_DLLS)
    # Prefer vcpkg's installed bin dirs when available (more reliable across toolchains).
    if(DEFINED VCPKG_INSTALLED_DIR AND DEFINED VCPKG_TARGET_TRIPLET)
        set(_pacman_vcpkg_bin "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")
        set(_pacman_vcpkg_debug_bin "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/bin")

        add_custom_command(TARGET pacman POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "$<IF:$<CONFIG:Debug>,${_pacman_vcpkg_debug_bin},${_pacman_vcpkg_bin}>"
                "$<TARGET_FILE_DIR:pacman>"
        )
    elseif(CMAKE_VERSION VERSION_GREATER_EQUAL "3.21")
        add_custom_command(TARGET pacman POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:pacman>
                $<TARGET_FILE_DIR:pacman>
            COMMAND_EXPAND_LISTS
        )
    else()
        message(STATUS "PACMAN_COPY_RUNTIME_DLLS: set VCPKG_INSTALLED_DIR/VCPKG_TARGET_TRIPLET or use CMake >= 3.21")
    endif()
endif()

if(MSVC)
    target_compile_options(pacman PRIVATE /W4)
else()
    target_compile_options(pacman PRIVATE -Wall -Wextra -Wpedantic)
endif()

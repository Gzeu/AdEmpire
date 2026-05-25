cmake_minimum_required(VERSION 3.16)
project(AdEmpire VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── Output directory ──────────────────────────────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ── Platform detection ────────────────────────────────────────────────────────
if(WIN32)
    message(STATUS "[AdEmpire] Platform: Windows (vcpkg recommended)")
elseif(APPLE)
    message(STATUS "[AdEmpire] Platform: macOS")
else()
    message(STATUS "[AdEmpire] Platform: Linux/Ubuntu")
endif()

# ── Dependencies ──────────────────────────────────────────────────────────────

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW
find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
    # Fallback: try pkg-config (Ubuntu / Homebrew)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(GLFW REQUIRED glfw3)
    else()
        message(FATAL_ERROR
            "GLFW3 not found.\n"
            "  Ubuntu:  sudo apt-get install libglfw3-dev\n"
            "  macOS:   brew install glfw\n"
            "  Windows: vcpkg install glfw3:x64-windows")
    endif()
endif()

# nlohmann/json — download automatically if not present
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# Dear ImGui (git submodule at extern/imgui)
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/extern/imgui)
if(NOT EXISTS ${IMGUI_DIR}/imgui.h)
    message(FATAL_ERROR
        "ImGui submodule not found. Run:\n"
        "  git submodule update --init --recursive")
endif()

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# ── Collect project sources ───────────────────────────────────────────────────
file(GLOB_RECURSE GAME_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/**/*.cpp
)

# ── Executable ───────────────────────────────────────────────────────────────
add_executable(AdEmpire
    ${GAME_SOURCES}
    ${IMGUI_SOURCES}
)

# ── Include paths ─────────────────────────────────────────────────────────────
target_include_directories(AdEmpire PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/systems
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/audio
    ${CMAKE_SOURCE_DIR}/src/network
    ${CMAKE_SOURCE_DIR}/src/platform
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${nlohmann_json_SOURCE_DIR}
)

# ── Link libraries ────────────────────────────────────────────────────────────
if(glfw3_FOUND)
    target_link_libraries(AdEmpire PRIVATE OpenGL::GL glfw)
elseif(GLFW_FOUND)
    # pkg-config path
    target_include_directories(AdEmpire PRIVATE ${GLFW_INCLUDE_DIRS})
    target_link_libraries(AdEmpire PRIVATE OpenGL::GL ${GLFW_LIBRARIES})
endif()

if(APPLE)
    target_link_libraries(AdEmpire PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(AdEmpire PRIVATE dl pthread)
endif()

# ── Compiler flags ────────────────────────────────────────────────────────────
if(MSVC)
    target_compile_options(AdEmpire PRIVATE /W3 /O2)
else()
    target_compile_options(AdEmpire PRIVATE
        -Wall -Wextra -Wpedantic
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
        $<$<CONFIG:Debug>:-g -O0>
    )
endif()

# ── Assets: copy to build dir ─────────────────────────────────────────────────
if(EXISTS ${CMAKE_SOURCE_DIR}/assets)
    add_custom_command(TARGET AdEmpire POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:AdEmpire>/assets
        COMMENT "Copying assets to build directory"
    )
endif()

# ── Install rules (used by CPack / GitHub Release) ────────────────────────────
install(TARGETS AdEmpire
    RUNTIME DESTINATION bin
)
install(DIRECTORY assets/ DESTINATION bin/assets OPTIONAL)

# ── CPack packaging ───────────────────────────────────────────────────────────
set(CPACK_PACKAGE_NAME              "AdEmpire")
set(CPACK_PACKAGE_VENDOR            "Gzeu")
set(CPACK_PACKAGE_DESCRIPTION_SHORT "Marketing tycoon game — build your Ad Empire")
set(CPACK_PACKAGE_VERSION           ${PROJECT_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "AdEmpire")

if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
elseif(APPLE)
    set(CPACK_GENERATOR "ZIP;DragNDrop")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)

message(STATUS "[AdEmpire] Configure done — run: cmake --build . --config Release")

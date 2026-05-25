cmake_minimum_required(VERSION 3.16)
project(AdEmpire VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── Output directory ──────────────────────────────────────────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ── Platform detection ─────────────────────────────────────────────────────────────────
if(WIN32)
    message(STATUS "[AdEmpire] Platform: Windows")
elseif(APPLE)
    message(STATUS "[AdEmpire] Platform: macOS")
else()
    message(STATUS "[AdEmpire] Platform: Linux/Ubuntu")
endif()

# ── Dependencies ────────────────────────────────────────────────────────────────────────

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW
find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
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

# ── OpenSSL (required for cpp-httplib HTTPS / SSLClient) ─────────────────────────
find_package(OpenSSL QUIET)
if(OpenSSL_FOUND)
    message(STATUS "[AdEmpire] OpenSSL ${OPENSSL_VERSION} found — HTTPS feeds enabled")
else()
    message(STATUS "[AdEmpire] OpenSSL NOT found — all HTTP feeds will fallback to offline data")
    message(STATUS "  Ubuntu:  sudo apt-get install libssl-dev")
    message(STATUS "  macOS:   brew install openssl")
    message(STATUS "  Windows: vcpkg install openssl:x64-windows")
endif()

# nlohmann/json
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# cpp-httplib — v0.15.3 with HTTPS support flag
FetchContent_Declare(
    cpp_httplib
    URL https://github.com/yhirose/cpp-httplib/archive/refs/tags/v0.15.3.tar.gz
)
FetchContent_MakeAvailable(cpp_httplib)

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

# ── Collect project sources ─────────────────────────────────────────────────────────────────
file(GLOB_RECURSE GAME_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/**/*.cpp
)

# ── Executable ────────────────────────────────────────────────────────────────────────────
add_executable(AdEmpire
    ${GAME_SOURCES}
    ${IMGUI_SOURCES}
)

# ── Compile definitions ─────────────────────────────────────────────────────────────────
if(OpenSSL_FOUND)
    # Tell cpp-httplib to compile with SSL support
    target_compile_definitions(AdEmpire PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)
else()
    # Fallback: no SSL — all SSLClient calls will fail gracefully
    target_compile_definitions(AdEmpire PRIVATE CPPHTTPLIB_NO_SSL)
endif()

# ── Include paths ────────────────────────────────────────────────────────────────────────
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
    ${cpp_httplib_SOURCE_DIR}
)

# ── Link libraries ──────────────────────────────────────────────────────────────────────
if(glfw3_FOUND)
    target_link_libraries(AdEmpire PRIVATE OpenGL::GL glfw)
elseif(GLFW_FOUND)
    target_include_directories(AdEmpire PRIVATE ${GLFW_INCLUDE_DIRS})
    target_link_libraries(AdEmpire PRIVATE OpenGL::GL ${GLFW_LIBRARIES})
endif()

# Link OpenSSL if available
if(OpenSSL_FOUND)
    target_link_libraries(AdEmpire PRIVATE OpenSSL::SSL OpenSSL::Crypto)
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

# ── Compiler flags ────────────────────────────────────────────────────────────────────────
if(MSVC)
    target_compile_options(AdEmpire PRIVATE /W3 /O2)
else()
    target_compile_options(AdEmpire PRIVATE
        -Wall -Wextra -Wpedantic
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
        $<$<CONFIG:Debug>:-g -O0>
    )
endif()

# ── Assets: copy to build dir ─────────────────────────────────────────────────────────────────
if(EXISTS ${CMAKE_SOURCE_DIR}/assets)
    add_custom_command(TARGET AdEmpire POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:AdEmpire>/assets
        COMMENT "Copying assets to build directory"
    )
endif()

# ── Install + CPack ────────────────────────────────────────────────────────────────────────
install(TARGETS AdEmpire RUNTIME DESTINATION bin)
install(DIRECTORY assets/ DESTINATION bin/assets OPTIONAL)

set(CPACK_PACKAGE_NAME              "AdEmpire")
set(CPACK_PACKAGE_VENDOR            "Gzeu")
set(CPACK_PACKAGE_DESCRIPTION_SHORT "Marketing tycoon — play the market in real time")
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

message(STATUS "[AdEmpire] Configure done.")
message(STATUS "  Build:  cmake --build build --config Release")
message(STATUS "  Run:    ./build/bin/AdEmpire")

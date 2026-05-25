cmake_minimum_required(VERSION 3.16)
project(AdEmpire VERSION 0.8.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Build options ────────────────────────────────────────────────────────────
option(AUDIO    "Enable OpenAL audio (requires libopenal-dev)" OFF)
option(NETWORK  "Enable REST leaderboard via cpp-httplib"      ON)

# ── Auto-download nlohmann/json if missing ──────────────────────────────────
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/lib/json.hpp")
    message(STATUS "[AdEmpire] Downloading nlohmann/json...")
    file(DOWNLOAD
        https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp
        ${CMAKE_SOURCE_DIR}/lib/json.hpp
        SHOW_PROGRESS
    )
endif()

# ── Auto-download cpp-httplib if NETWORK=ON ──────────────────────────────────
if(NETWORK)
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/lib/httplib.h")
        message(STATUS "[AdEmpire] Downloading cpp-httplib...")
        file(DOWNLOAD
            https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h
            ${CMAKE_SOURCE_DIR}/lib/httplib.h
            SHOW_PROGRESS
        )
    endif()
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# ── OpenAL (optional) ────────────────────────────────────────────────────────
if(AUDIO)
    find_package(OpenAL)
    if(OpenAL_FOUND OR OPENAL_FOUND)
        message(STATUS "[AdEmpire] OpenAL found — audio ENABLED")
        add_compile_definitions(AUDIO_ENABLED)
    else()
        message(WARNING "[AdEmpire] OpenAL NOT found — audio disabled. Run: sudo apt install libopenal-dev")
    endif()
endif()

# ── cpp-httplib (optional) ───────────────────────────────────────────────────
if(NETWORK AND EXISTS "${CMAKE_SOURCE_DIR}/lib/httplib.h")
    message(STATUS "[AdEmpire] cpp-httplib found — REST leaderboard ENABLED")
    add_compile_definitions(HTTPLIB_AVAILABLE)
endif()

# ── ImGui sources ────────────────────────────────────────────────────────────
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/lib/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# ── Game sources ─────────────────────────────────────────────────────────────
file(GLOB_RECURSE GAME_SOURCES
    src/*.cpp
    src/**/*.cpp
)

add_executable(AdEmpire ${GAME_SOURCES} ${IMGUI_SOURCES})

target_include_directories(AdEmpire PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/lib
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(AdEmpire PRIVATE
    OpenGL::GL
    glfw
    ${CMAKE_DL_LIBS}
)

# Link OpenAL if enabled and found
if(AUDIO)
    if(TARGET OpenAL::OpenAL)
        target_link_libraries(AdEmpire PRIVATE OpenAL::OpenAL)
    elseif(OPENAL_LIBRARY)
        target_link_libraries(AdEmpire PRIVATE ${OPENAL_LIBRARY})
        target_include_directories(AdEmpire PRIVATE ${OPENAL_INCLUDE_DIR})
    endif()
endif()

# Link pthread for std::thread (LeaderboardClient)
target_link_libraries(AdEmpire PRIVATE pthread)

add_custom_command(TARGET AdEmpire POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:AdEmpire>/assets
)

message(STATUS "──────────────────────────────────────────")
message(STATUS " AdEmpire v0.8 build configuration:")
message(STATUS "   Audio (OpenAL):       ${AUDIO}")
message(STATUS "   Network (Leaderboard): ${NETWORK}")
message(STATUS "──────────────────────────────────────────")
message(STATUS " Standard build (no audio):")
message(STATUS "   cmake .. && make -j\$(nproc)")
message(STATUS " With audio:")
message(STATUS "   sudo apt install libopenal-dev")
message(STATUS "   cmake .. -DAUDIO=ON && make -j\$(nproc)")
message(STATUS "──────────────────────────────────────────")

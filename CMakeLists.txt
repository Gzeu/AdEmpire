cmake_minimum_required(VERSION 3.16)
project(AdEmpire VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# ImGui sources
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

# Game sources
file(GLOB_RECURSE GAME_SOURCES
    src/*.cpp
    src/**/*.cpp
)

add_executable(AdEmpire
    ${GAME_SOURCES}
    ${IMGUI_SOURCES}
)

target_include_directories(AdEmpire PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(AdEmpire PRIVATE
    OpenGL::GL
    glfw
    ${CMAKE_DL_LIBS}
)

# Copy assets to build directory
add_custom_command(TARGET AdEmpire POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:AdEmpire>/assets
)

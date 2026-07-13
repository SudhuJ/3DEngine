cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(flow)

# gather src files
file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
list(FILTER sources EXCLUDE REGEX ".*/vendors/.*")
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(${PROJECT_NAME} ${sources} ${headers})

# include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

# link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    engine
)

target_sources(${PROJECT_NAME} PRIVATE
    src/vendors/imgui/imgui.cpp
    src/vendors/imgui/imgui_demo.cpp
    src/vendors/imgui/imgui_draw.cpp
    src/vendors/imgui/imgui_widgets.cpp
    src/vendors/imgui/imgui_tables.cpp
    src/vendors/imgui/backends/imgui_impl_glfw.cpp
    src/vendors/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src/vendors/imgui
    src/vendors/imgui/backends
)

target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM)

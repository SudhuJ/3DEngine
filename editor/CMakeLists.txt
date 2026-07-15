cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(flow)

find_package(imgui CONFIG REQUIRED)

# gather src files
file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
list(FILTER sources EXCLUDE REGEX ".*/vendors/.*")
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(${PROJECT_NAME} ${sources} ${headers})

# compile conan backend sources (not in libimgui.a)
target_sources(${PROJECT_NAME} PRIVATE
    "${imgui_PACKAGE_FOLDER_DEBUG}/res/bindings/imgui_impl_glfw.cpp"
    "${imgui_PACKAGE_FOLDER_DEBUG}/res/bindings/imgui_impl_opengl3.cpp"
)

# include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    ${imgui_INCLUDE_DIRS}
    "${imgui_PACKAGE_FOLDER_DEBUG}/res/bindings"
)

# link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    engine
    imgui::imgui
)

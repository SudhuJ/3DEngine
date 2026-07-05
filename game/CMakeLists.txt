cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(game)

# gather src files
file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(${PROJECT_NAME} ${sources} ${headers})

# include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

# link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE engine)

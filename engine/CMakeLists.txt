cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(engine)

find_package(spdlog CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(stb CONFIG REQUIRED)
find_package(PhysX CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(lua CONFIG REQUIRED)

# gather src files
file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} SHARED ${sources} ${headers})

# export engine symbols
target_compile_definitions(${PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:FLOW_ENABLE_LOG>)

# include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
    spdlog::spdlog
    glfw
    glad::glad
    glm::glm
    EnTT::EnTT
    assimp::assimp
    stb::stb
    sol2::sol2
    lua::lua
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    physx::physx
)

# copy resources
if (EXISTS ${CMAKE_SOURCE_DIR}/resources)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND
        ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
    )
else()
  message(WARNING "[WARNING] no resource directory!")
endif()

# if (EXISTS ${CMAKE_SOURCE_DIR}/script)
#   add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND
#         ${CMAKE_COMMAND} -E copy_directory
#         ${CMAKE_SOURCE_DIR}/script
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/scripts
#     )
# else()
#   message(WARNING "[WARNING] no script directory!")
# endif()

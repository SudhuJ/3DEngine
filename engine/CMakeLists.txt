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
find_package(yaml-cpp CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
# find_package(imgui CONFIG REQUIRED)

# gather src files
file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} SHARED ${sources} ${headers})

# export engine symbols
target_compile_definitions(${PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:FLOW_ENABLE_LOG>)

# projects live in the source tree so saves survive rebuilds
target_compile_definitions(${PROJECT_NAME} PUBLIC
    FLOW_PROJECTS_DIR="${CMAKE_SOURCE_DIR}/resources/projects"
)

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
    yaml-cpp::yaml-cpp
    magic_enum::magic_enum
    physx::physx
)

# copy resources (except projects, which are read/written in the source tree)
if (EXISTS ${CMAKE_SOURCE_DIR}/resources)
  # runs on every build (not just when engine relinks) so shader/asset edits always land in bin/
  add_custom_target(${PROJECT_NAME}_resources ALL)
  file(GLOB resource_dirs LIST_DIRECTORIES true ${CMAKE_SOURCE_DIR}/resources/*)
  foreach(resource_dir ${resource_dirs})
    get_filename_component(resource_name ${resource_dir} NAME)
    if (IS_DIRECTORY ${resource_dir} AND NOT resource_name STREQUAL "projects")
      add_custom_command(TARGET ${PROJECT_NAME}_resources POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy_directory
            ${resource_dir}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/${resource_name}
        )
    endif()
  endforeach()
  add_dependencies(${PROJECT_NAME}_resources ${PROJECT_NAME})
else()
  message(WARNING "[WARNING] no resource directory!")
endif()

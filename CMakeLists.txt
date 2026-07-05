cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# build output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# cpp standard options
project(flowEngine)
set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 17)

# enable console logging
if(${CMAKE_BUILD_TYPE} MATCHES Debug)
  add_definitions(-DFLOW_ENABLE_LOG)
  message(STATUS "Debug build")
endif()

# project subdirectories
add_subdirectory(engine)
add_subdirectory(editor)
add_subdirectory(game)

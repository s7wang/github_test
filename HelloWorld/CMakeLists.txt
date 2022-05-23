# CMake minimum version requirements
cmake_minimum_required(VERSION 3.12)

# set(EXECUTABLE_OUTPUT_PATH /home/wangs7/VSCode)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
# Support c++ 11
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
message(STATUS "optional:-std=c++11")
message(STATUS "${EXECUTABLE_OUTPUT_PATH}")
# Project Name
project(HelloWorld)

# Add compilation options
add_definitions("-Wall -g") #"-g" means allow using gdb debug
# or cmake with -DCMAKE_BUILD_TYPE=Debug

# include source files or head files
include_directories("${CMAKE_SOURCE_DIR}/src") 

# Generate target file
add_executable(HelloWorld src/HelloWorld.cpp)

# Set up link library
target_link_libraries(HelloWorld)

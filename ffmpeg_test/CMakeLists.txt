cmake_minimum_required(VERSION 2.8)

project(test)

include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories("/ffmpeg/lib")

add_executable(exec test.cpp)

target_link_libraries(exec avcodec)

cmake_minimum_required(VERSION 2.8)
project(TcpServer)
# Support c++ 11
set(CMAKE_CXX_FLAGS "-std=c++11 -g ${CMAKE_CXX_FLAGS}")

# include source files or head files
include_directories("${CMAKE_SOURCE_DIR}/src") 
include_directories("${CMAKE_SOURCE_DIR}/src/tcpserver")
include_directories("${CMAKE_SOURCE_DIR}/src/tcpserver/tcpuitls")  

file(GLOB_RECURSE native_srcs ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(tcp_server ${native_srcs})
target_link_libraries(tcp_server pthread)
cmake_minimum_required(VERSION 3.12)

project(app_mypow)

set(SRC ${CMAKE_SOURCE_DIR}/src)
set(ICD ${SRC}/include)

add_library(mypow ${ICD}/mypow.cpp)
add_executable(exec ${SRC}/main.cpp)
target_link_libraries(exec mypow)

install(TARGETS exec
        DESTINATION  $ENV{HOME}/mypow)
# set_target_properties(mypow PROPERTIES PUBLIC_HEADER ${ICD}/mypow.cpp)
install(TARGETS mypow
        DESTINATION $ENV{HOME}/mypow)

install(DIRECTORY ${ICD} 
        DESTINATION $ENV{HOME}/mypow
        FILES_MATCHING PATTERN "*.h")
cmake_minimum_required(VERSION 3.17)
project(opsa_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)
add_compile_options(
        -std=c++11
        #        -D_DEBUG
        #        -g
)

## Add headers files
include_directories (
        include/
)

set(HEADERS
        src/safeQueue/messageQueue.h src/audThread/threadPool.h)

set(SOURCES test.cpp src/safeQueue/messageQueue.cpp src/audThread/threadPool.cpp)

add_executable(opsa_test ${HEADERS} ${SOURCES})

target_link_libraries(opsa_test)
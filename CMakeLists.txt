cmake_minimum_required(VERSION 3.17)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)
add_compile_options(
        -std=c++11
        #        -D_DEBUG
        #        -g
)

## Add headers files
set(HEADERS
        src/messageQueue.h src/threadPool.h AuditLog.h)
## Add sources files
set(SOURCES test.cpp )

add_executable(ThreadPool ${HEADERS} ${SOURCES})

target_link_libraries(ThreadPool)
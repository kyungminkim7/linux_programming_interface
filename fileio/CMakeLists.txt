cmake_minimum_required(VERSION 3.5)
project(fileio C)

link_libraries(linux_programming_interface)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_executable(copy src/copy.c)
add_executable(seek_io src/seek_io.c)
add_executable(t_readv src/t_readv.c)

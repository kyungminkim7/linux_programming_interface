cmake_minimum_required(VERSION 3.5)
project(threads C)

link_libraries(linux_programming_interface)

find_package(Threads)
link_libraries(Threads::Threads)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_executable(simple_thread simple_thread.c)

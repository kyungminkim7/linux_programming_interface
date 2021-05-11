cmake_minimum_required(VERSION 3.5)
project(time C)

link_libraries(linux_programming_interface)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_executable(calendar_time src/calendar_time.c)
add_executable(show_time src/show_time.c)

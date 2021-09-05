cmake_minimum_required(VERSION 3.5)
project(sockets C)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(us_xfr_sv us_xfr_sv.c)

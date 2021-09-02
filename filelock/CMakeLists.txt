cmake_minimum_required(VERSION 3.5)
project(filelock C)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(t_flock t_flock.c)
add_executable(i_fcntl_locking i_fcntl_locking.c)

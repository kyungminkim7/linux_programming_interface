cmake_minimum_required(VERSION 3.5)
project(procexec C)

link_libraries(linux_programming_interface)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_executable(t_fork t_fork.c)
add_executable(fork_file_sharing fork_file_sharing.c)

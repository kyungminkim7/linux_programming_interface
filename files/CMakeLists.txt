cmake_minimum_required(VERSION 3.5)
project(files C)

link_libraries(linux_programming_interface)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_executable(t_stat src/t_stat.c)
add_executable(t_chown src/t_chown.c)
add_executable(t_umask src/t_umask.c)

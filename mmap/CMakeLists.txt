cmake_minimum_required(VERSION 3.5)
project(mmap C)

link_libraries(linux_programming_interface)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(mmcat mmcat.c)
add_executable(t_mmap t_mmap.c)
add_executable(anon_mmap anon_mmap.c)

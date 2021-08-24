cmake_minimum_required(VERSION 3.5)
project(svsem C)

link_libraries(linux_programming_interface)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(svsem_demo svsem_demo.c)
add_executable(svsem_mon svsem_mon.c)
add_executable(svsem_setall svsem_setall.c)

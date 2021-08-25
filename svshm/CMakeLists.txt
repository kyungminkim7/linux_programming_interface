cmake_minimum_required(VERSION 3.5)
project(svshm C)

link_libraries(linux_programming_interface)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(svshm_xfr_writer svshm_xfr_writer.c binary_sems.c)
add_executable(svshm_xfr_reader svshm_xfr_reader.c binary_sems.c)

cmake_minimum_required(VERSION 3.5)
project(pshm C)

link_libraries(rt)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(pshm_create pshm_create.c)
add_executable(pshm_write pshm_write.c)
add_executable(pshm_read pshm_read.c)
add_executable(pshm_unlink pshm_unlink.c)

cmake_minimum_required(VERSION 3.5)
project(pmsg C)

link_libraries(linux_programming_interface rt)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(pmsg_unlink pmsg_unlink.c)
add_executable(pmsg_create pmsg_create.c)
add_executable(pmsg_getattr pmsg_getattr.c)

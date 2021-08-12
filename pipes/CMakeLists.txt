cmake_minimum_required(VERSION 3.5)
project(pipes C)

link_libraries(linux_programming_interface)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(simple_pipe simple_pipe.c)
add_executable(pipe_sync pipe_sync.c)
add_executable(pipe_ls_wc pipe_ls_wc.c)
add_executable(fifo_seqnum_server fifo_seqnum_server.c)
add_executable(fifo_seqnum_client fifo_seqnum_client.c)

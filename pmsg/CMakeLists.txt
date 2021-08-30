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
add_executable(pmsg_send pmsg_send.c)
add_executable(pmsg_receive pmsg_receive.c)
add_executable(mq_notify_sig mq_notify_sig.c)

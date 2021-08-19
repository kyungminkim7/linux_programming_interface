cmake_minimum_required(VERSION 3.5)
project(svipc C)

link_libraries(linux_programming_interface)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(svmsg_demo_server svmsg_demo_server.c)
add_executable(svmsg_create svmsg_create.c)
add_executable(svmsg_send svmsg_send.c)
add_executable(svmsg_receive svmsg_receive.c)
add_executable(svmsg_rm svmsg_rm.c)
add_executable(svmsg_file_server svmsg_file_server.c)
add_executable(svmsg_file_client svmsg_file_client.c)

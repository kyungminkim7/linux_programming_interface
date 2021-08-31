cmake_minimum_required(VERSION 3.5)
project(psem C)

find_package(Threads)
link_libraries(Threads::Threads)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(psem_create psem_create.c)
add_executable(psem_unlink psem_unlink.c)
add_executable(psem_wait psem_wait.c)

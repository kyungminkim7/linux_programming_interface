cmake_minimum_required(VERSION 3.5)
project(linux_programming_interface)

include_directories(${PROJECT_NAME} PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
)

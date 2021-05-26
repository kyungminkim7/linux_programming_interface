cmake_minimum_required(VERSION 3.5)
project(lpi_tests C)

link_libraries(linux_programming_interface cmocka::cmocka)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_executable(test_FilePerms test_FilePerms.c)
add_test(NAME test_FilePerms COMMAND test_FilePerms)

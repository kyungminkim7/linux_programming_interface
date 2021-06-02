cmake_minimum_required(VERSION 3.5)
project(linux_programming_interface C)

add_library(${PROJECT_NAME}
    "src/Error.c"
    "src/FilePerms.c"
)

target_include_directories(${PROJECT_NAME} 
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_subdirectory(fileio)
add_subdirectory(getopt)
add_subdirectory(proc)
add_subdirectory(memalloc)
add_subdirectory(users_groups)
add_subdirectory(proccred)
add_subdirectory(time)
add_subdirectory(syslim)
add_subdirectory(sysinfo)
add_subdirectory(filesys)
add_subdirectory(files)
add_subdirectory(xattr)
add_subdirectory(dirs_links)

# Tests
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)

    option(WITH_EXAMPLES "Build examples" OFF)
    add_subdirectory(extern/cmocka)
    add_subdirectory(tests)
endif()

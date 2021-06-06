cmake_minimum_required(VERSION 3.5)
project(dirs_links C)

link_libraries(linux_programming_interface)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_executable(t_unlink src/t_unlink.c)
add_executable(list_files src/list_files.c)
add_executable(nftw_dir_tree src/nftw_dir_tree.c)
add_executable(view_symlink src/view_symlink.c)

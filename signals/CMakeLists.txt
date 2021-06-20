cmake_minimum_required(VERSION 3.5)
project(signals C)

link_libraries(linux_programming_interface)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_executable(ouch src/ouch.c)
add_executable(intquit src/intquit.c)
add_executable(t_kill src/t_kill.c)
add_executable(sig_sender src/sig_sender.c)
add_executable(sig_receiver src/sig_receiver.c)

add_executable(nonreentrant src/nonreentrant.c)
target_link_libraries(nonreentrant crypt)

add_executable(sigmask_longjmp src/sigmask_longjmp.c)
add_executable(sigmask_longjmp_sigsetjump src/sigmask_longjmp.c)
target_compile_definitions(sigmask_longjmp_sigsetjump PRIVATE USE_SIGSETJUMP)

add_executable(t_sigaltstack src/t_sigaltstack.c)

cmake_minimum_required(VERSION 3.5)
project(sockets C)

include_directories(
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(us_xfr_sv us_xfr_sv.c)
add_executable(us_xfr_cl us_xfr_cl.c)
add_executable(ud_case_sv ud_case_sv.c)
add_executable(ud_case_cl ud_case_cl.c)
add_executable(i6d_ucase_sv i6d_ucase_sv.c)
add_executable(i6d_ucase_cl i6d_ucase_cl.c)
add_executable(is_seqnum_sv is_seqnum_sv.c)
add_executable(is_seqnum_cl is_seqnum_cl.c)

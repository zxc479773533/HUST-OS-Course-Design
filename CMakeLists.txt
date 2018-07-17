cmake_minimum_required(VERSION 3.9)

project(HUST_OS_design)

# add zxcpyp lib
add_subdirectory(./zxcpyplib)

# build Part I
add_executable(copy ./PartI-Linux_basics/copy.c)
target_link_libraries(copy zxcpyp_lib)
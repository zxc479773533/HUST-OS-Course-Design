cmake_minimum_required(VERSION 3.9)

project(HUST_OS_design)

# Add GTK3
find_package(PkgConfig REQUIRED)
pkg_check_modules (GTK3 REQUIRED gtk+-3.0)
set(CMAKE_C_STANDARD 11)
include_directories (${GTK3_INCLUDE_DIRS})
link_directories (${GTK3_LIBRARY_DIRS})
add_definitions (${GTK3_CFLAGS_OTHER})

# Add zxcpyp lib
add_subdirectory(./lib)

# Build Part I
add_executable(copy ./PartI-Linux_basics/copy.c)
target_link_libraries(copy lib)
add_executable(fork_demo ./PartI-Linux_basics/fork_demo.c)
target_link_libraries(fork_demo lib ${GTK3_LIBRARIES})

# Build Part II
add_executable(testcall ./PartII-Syscall/testcall.c)
target_link_libraries(testcall lib)
add_executable(teststr ./PartII-Syscall/teststr.c)
target_link_libraries(teststr lib)
add_executable(testcp ./PartII-Syscall/testcp.c)
target_link_libraries(testcp lib)

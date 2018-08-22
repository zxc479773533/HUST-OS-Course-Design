cmake_minimum_required(VERSION 3.9)

project(HUST_OS_design)

# Add GTK2
find_package(PkgConfig REQUIRED)
pkg_check_modules (GTK2 REQUIRED gtk+-2.0)
set(CMAKE_C_STANDARD 11)
include_directories (${GTK2_INCLUDE_DIRS})
link_directories (${GTK2_LIBRARY_DIRS})
add_definitions (${GTK2_CFLAGS_OTHER})

# Add zxcpyp lib
add_subdirectory(./lib)

# Build Part I
add_executable(copy ./PartI-Linux_basics/copy.c)
target_link_libraries(copy lib)
add_executable(fork_demo ./PartI-Linux_basics/fork_demo.c)
target_link_libraries(fork_demo lib ${GTK2_LIBRARIES})

# Build Part II
add_executable(testcall ./PartII-Syscall/testcall.c)
add_executable(teststr ./PartII-Syscall/teststr.c)
add_executable(testcp ./PartII-Syscall/testcp.c)

# Build Part III
add_executable(testdev ./PartIII-Device_driver/testdev.c)

# Build Part IV
aux_source_directory(./PartIV-System_monitor sysmonitor)
add_executable(py_sysmonitor ${sysmonitor})
target_link_libraries(py_sysmonitor lib ${GTK2_LIBRARIES})
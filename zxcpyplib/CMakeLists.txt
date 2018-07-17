cmake_minimum_required(VERSION 3.9)

project(OS_labs)

aux_source_directory(. dir_main)
add_library(zxcpyp_lib ${dir_main})
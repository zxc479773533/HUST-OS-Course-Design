cmake_minimum_required(VERSION 3.9)

project(HUST_OS_design)

aux_source_directory(. dir_main)
add_library(lib ${dir_main})
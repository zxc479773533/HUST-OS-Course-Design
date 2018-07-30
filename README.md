# HUST-OS-design
HUST Operating System curriculum design 2018 Summer.

## About

Environment: Arch Linux x86_64

Kernel version: 4.17.8

CMake minimum required: version 3.9

## Compile

```sh
mkdir build
cd build
cmake ..
make
```

## Part I: Linux_basics

1. Implement a program to copy file.
2. Implement a fork demo using GTK+

## Part II: Systall

Adding a system call to copy file in linux kernel.

Here I use ***Arch Linux x86_64*** and ***kernel 4.17.8***, it may be a little diffient from the way it used to be on ***Ubuntu*** or ***Linux kernel version <= 4.15*** because of the fucking NVIDIA.

The details can be seen [here](PartII-Syscall/README.md) (README in Part II)

## Part III: Character Device driver

Implement a character device driver by linux kernel module.

Here I added read, write, lseek, ioctl(for clear) functions.

The details can be seen [here](PartIII-Device_driver/README.md) (README in Part III)
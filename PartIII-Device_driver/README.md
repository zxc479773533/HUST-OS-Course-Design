# Part III - Add a character device driver

## About

1. Auto create and delete device file when load or unload the module.
2. Implemented function: read, wrire, lseek, ioctl.

## Ioctl cmd

Here I use ioctl to add clear all memory function, use cmd=1 to clear the memory.

## Compile and install kernel module

```sh
cd zxcpyp_dev
make
sudo insmod zxcpyp_dev.ko
```

## Uninstall the kernel module

```sh
sudo rmmod zxcpyp_dev
```
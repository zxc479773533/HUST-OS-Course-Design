# Part II - Add a system call

## About

Here are steps for Compileing and installing the ***Linux kernel 4.17.8*** on ***Arch Linux x86_64***. This can be a little diffient from the way it used to be on ***Ubuntu*** or ***Linux kernel version <= 4.15***.

The details can be seen in `install.sh`.

Here I add three syscalls:

1. Test "Hello World"
2. Test parameter passing
3. Copy files

### Get linux kernel code 4.17.8

Here we get it from kernel.org.

``` sh
wget https://mirrors.edge.kernel.org/pub/linux/kernel/v4.x/linux-4.17.8.tar.xz
```

### Unzip the package

``` sh
`tar -xvf linux-4.17.8.tar.xz linux-4.17.8`
```

### Replace modified files

Copy the file I modified to new kernel code

``` sh
cp syscall_64.tbl linux-4.17.8/arch/x86/entry/syscalls/syscall_64.tbl
cp sys.c linux-4.17.8/kernel/sys.c
cp install.sh linux-4.17.8/install.sh
```

### Set config

Here we output the config from the running kernel.

```sh
cd linux-4.17.8
zcat /proc/config.gz > .config
```

### Set your personal suffix

In `.config`, the normal kernel suffix is `-ARCH`, whiched set by `CONFIG_LOCALVERSION="-ARCH"`, change it to your suffix, for me, it is `-zxcpyp`

### Execute the installation script

Before excute the script, be sure to change the suffix in script.

```sh
chmod +x install.sh
sudo sh ./install.sh
```

### Reboot and Install NVIDIA drivers in kernel modules

***Attention: Skip this step may cause the graphical interface to fail to open***

Get the nvidia driver from offical website, choose the version that matches your Graphics.

https://www.nvidia.com/Download/index.aspx

If you do not know what graphics card you have, find out by issuing:

``` sh
lspci -k | grep -A 2 -E "(VGA|3D)"
```

For exaple, my graphics is `GTX 1060`, the I get `NVIDIA-Linux-x86_64-390.77.run`, copy it to `/root`(Here may need `su`). After reboot, use `Ctrl + Alt + F3` change to tty3 and issuing:

```sh
./NVIDIA-Linux-x86_64-390.77.run
reboot
```

After reboot, issuing `uname -a`, you will see like this:

``` sh
linux > uname -a
Linux Saltedfish 4.17.8-zxcpyp #1 SMP PREEMPT Mon Jul 23 22:03:07 CST 2018 x86_64 GNU/Linux
```
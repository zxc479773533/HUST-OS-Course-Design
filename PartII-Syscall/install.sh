#!/usr/bin/bash
# Compile and install linux kernel on Arch Linux x86_64

# Set suffix
SUFFIX="-zxcpyp"

# Compile the kernel
make

# Install the kernel modules
make modules_install

# Install kernel image
cp arch/x86_64/boot/bzImage /boot/vmlinuz-linux$SUFFIX

# Create preset
sed s/linux/linux$SUFFIX/g </etc/mkinitcpio.d/linux.preset >/etc/mkinitcpio.d/linux$SUFFIX.preset

# Build initramfs
mkinitcpio -p linux$SUFFIX

# Update bootloader
grub-mkconfig -o /boot/grub/grub.cfg
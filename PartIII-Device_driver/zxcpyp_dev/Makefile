# Kernel lib
KVERS :=$(shell uname -r)

# Path
PWD :=$(shell pwd)

# Build modules
obj-m += zxcpyp_dev.o

build:
	make -C /lib/modules/$(KVERS)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(KVERS)/build M=$(PWD) clean
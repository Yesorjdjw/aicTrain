obj-m += stENG_drv.o 


#KERNEL_DIR := /opt/wkspace/mkplat/bkernel/kernel-3.4.39
KERNEL_DIR := /opt/emb-linux/linux-3.4.39

all:
	make modules ARCH=arm CROSS_COMPILE=/usr/local/arm/ext-toolchain/bin/arm-cortex_a9-linux-gnueabi- M=`pwd` -C $(KERNEL_DIR)
	#arm-linux-gcc app.c -o app
clean:
	make modules clean  ARCH=arm CROSS_COMPILE=/usr/local/arm/ext-toolchain/bin/arm-cortex_a9-linux-gnueabi- M=`pwd` -C $(KERNEL_DIR)
	#rm -rf app

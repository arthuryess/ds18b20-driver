PREFIX_DIR		=	/
KERNEL_DIRNAME	=	kernels
KENERL_VER		=	3.0.1

ifeq($(DS18B20_PLATFORM), )
DS18B20_PLATFORM		=	ok6410
endif

ifneq($(KERNELRELEASE), )
	obj-m += ds18b20_dev.o
	ds18b20_dev-objs = linux_driver.o ds18b20_driver.o ds18b20_hal.o $(DS18B20_PLATFORM).o 
else
KERNELDIR	=	$(PREFIX_DIR)/$(KERNEL_DIRNAME)/$(KERNEL_VER)
all:
	make -C $(KERNELDIR) modules
.PHONY:clean
clean:
	rm -f *.o *.cmd *.cmd.c *.ko *.tmp*
endif

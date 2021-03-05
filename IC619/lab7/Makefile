# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
	    obj-m := scull.o

# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD  := $(shell pwd)
	
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif

quiz1:
	gcc quiz1.c -o quiz1
quiz2:
	gcc quiz2.c -o quiz2
change:
	gcc change.c -o change
info:
	gcc info.c -o info
clean:
	rm -rf *.o *.ko *.mod.c modules.order Module.symvers quiz1 quiz2 change info



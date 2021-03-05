CC = gcc
#CFLAGS = -g -W -Wall
#LOADLIBES = -lcrypto
TARGET1 = bench1.c
TARGET2 = bench2.c
OBJECT1 = bench1
OBJECT2 = bench2

ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSBULL_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

EXTRA_CFLAGS += $(DEBFLAGS)
EXTRA_CFLAGS += -I..

ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m	:= sbull.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	$(CC) $(CFLAGS) -o $(OBJECT1) $(TARGET1)
	$(CC) $(CFLAGS) -o $(OBJECT2) $(TARGET2)
endif


clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.order *.symvers
	rm $(OBJECT1) $(OBJECT2)

depend .depend dep:
	$(CC) $(EXTRA_CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif



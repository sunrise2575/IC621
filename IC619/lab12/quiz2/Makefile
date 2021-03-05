obj-m := chardev.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Some tutorials use M= instead of SUBDIRS= You may need to be root to
# compile the module. You must be root to insert it.
default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	gcc -o quiz2 quiz2.c
clean:
	rm -f quiz2
	$(MAKE) -C $(KDIR) M=$(PWD) clean

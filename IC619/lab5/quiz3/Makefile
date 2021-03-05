obj-m += quiz3_all.o
quiz3_all-objs := quiz3_init.o quiz3.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

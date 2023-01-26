obj-m := larlena_module.o
# ccflags-y=-std=c11

all :
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean :
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

re : clear all
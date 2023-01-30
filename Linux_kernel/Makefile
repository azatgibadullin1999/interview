obj-m := larlena_module_pipe.o
EXTRA_CFLAGS += -O2
# ccflags-y=-std=c11

all :
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean :
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

re : clean all
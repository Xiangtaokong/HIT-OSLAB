obj-m += hello.o

all     :
	make -C /lib/modules/2.6.32-642.el6.x86_64/build M=$(PWD) modules
clean   :
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


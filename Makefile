obj-m := fs_mod.o
fs_mod-objs := fs.o 
all: module mainfs


module:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm mainfs

obj-m := launcher.o
KDIR := /home/vgalaxy/Downloads/linux-5.17.3

default:
	$(MAKE) -C $(KDIR) M=$(PWD)
	cp launcher.ko ../initramfs/nuke/

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

.PHONY: module clean

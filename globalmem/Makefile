KERVER = $(shell uname -r)
obj-m = globalmem.o

globalmem:
	make -C /lib/modules/$(KERVER)/build M=$(CURDIR) modules

clean:
	make -C /lib/modules/$(KERVER)/build M=$(CURDIR) clean


obj-m+=deviceDriverExample.o
 
all:
    make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
    $(CC) exampleTest.c -o test
clean:
    make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
obj-m += timerMod.o
CC=gcc


all: module client

module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	
client: user.o
	$(CC) -Wall -pedantic -Werror -fopenmp -o client user.o
	
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f *.o *~

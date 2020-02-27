CFLAGS=-Wall -g -DNDEBUG
all: ex22_main ex22

ex22_main: ex22 ex22_main.c
	cc ex22_main.c ex22.o -o ex22_main
	
ex22: ex22.h ex22.c
	cc -c -o ex22.o ex22.c

clean:
	rm -f ex22.o ex22_main
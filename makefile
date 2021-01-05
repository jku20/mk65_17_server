CC=gcc
all: processer console

processer: processer.o
	$(CC) -o processer processer.o

console: console.o
	$(CC) -o console console.o

processer.o: processer.c
console.o: console.c

.PHONY: clean

clean:
	rm *.o

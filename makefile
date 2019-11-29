all: fat
fat:
	gcc -Wall disk.c fat.c -o fat
clean:
	rm fat
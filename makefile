all: file_system test
# fat: fat.c
# 	gcc fat.c -o fat
file_system: file_system.c
	gcc file_system.c -o file_system
test: tester.c
	gcc tester.c -o test
cln:
	rm file_system
	rm test
all: program.o csapp.o
	cc program.o csapp.o -lreadline -o a.out 
program.o: program.c
	cc -c program.c -o program.o
csapp.o: csapp.c
	cc -c csapp.c -o csapp.o


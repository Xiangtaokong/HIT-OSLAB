#makefile simple example


test	:	main.o	sub1.o	sub2.o
	gcc -o test main.o sub1.o sub2.o
main.o	:	main.c
	gcc -c main.c
sub1.o	:	sub1.c
	gcc -c sub1.c
sub2.o	:	sub2.c
	gcc -c sub2.c
clean	:
	rm -f main.o sub*.o

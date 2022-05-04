all	: tetris.o
	gcc -o a.out tetris.c -lncurses


clean:
	rm a.out *.o

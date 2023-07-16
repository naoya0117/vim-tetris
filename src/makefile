main : main.o tetris.o menu.o help.o cursor.o ranking.o command.o block.o score.o clipboard.o
	gcc -o main main.o tetris.o menu.o cursor.o ranking.o help.o command.o block.o score.o clipboard.o -lncursesw -pthread
main.o : main.c menu.h command.h
	gcc -c main.c 
tetris.o : tetris.c block.h command.h mutex_shared.h score.h clipboard.h
	gcc -c tetris.c
menu.o : menu.c tetris.h ranking.h cursor.h command.h
	gcc -c menu.c
command.o : command.c help.h mutex_shared.h
	gcc -c command.c
help.o : help.c help.h
	gcc -c help.c
corsor.o : cursor.c
	gcc -c cursor.c
block.o : block.c block.h
	gcc -c block.c
ranking.o : ranking.c
	gcc -c ranking.c
	score.o : score.c
	gcc -c score.c mutex_shared.h
clipboard.o : clipboard.c
	gcc -c clipboard.c
clear :
	rm *.o main

#ifndef COMMAND_H
#define COMMAND_H

#define MAX_COMMAND_LENGTH 32

#include <ncurses.h>

extern WINDOW *cmdwin;
int call_command(int isallowed, char *buffer, int length);
void show_message(char *msg);

WINDOW *create_commandwin() ;


#endif 
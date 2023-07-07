#ifndef HEADER_NAME_H
#define HEADER_NAME_H

#include <ncurses.h>

extern WINDOW *cmdwin;
int call_command(int isallowed, char *buffer, int length);
void show_message(char *msg);

WINDOW *create_commandwin() ;


#endif /* HEADER_NAME_H */
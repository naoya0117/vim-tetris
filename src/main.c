#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <locale.h>
#include "menu.h"
#include "command.h"
#include "mutex_shared.h"

void error(char *msg);

int main(int argc, char **argv) {

   if (argc != 2) error("\t引数に異常があります。\n\tユーザ名を1つ指定してください。\n");

   setlocale(LC_ALL, "");

   initscr();
   cmdwin = create_commandwin();

   noecho();

   keypad(stdscr, TRUE);
   start_color();
   bkgd(COLOR_PAIR(COLOR_BLACK));
   refresh();


   init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
   init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
   init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);

   init_sharedmutex();

   menu(argv[1]);

   delwin(cmdwin);
   endwin();

   return 0;
}

void error(char *msg) {
   fprintf(stderr, "error: %s", msg);
   exit(1);
}

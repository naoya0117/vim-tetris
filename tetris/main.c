#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <locale.h>
#include "menu.h"

void error(char *msg);

int main(int argc, char **argv) {

   if (argc != 2) error("\t引数に異常があります。\n\tユーザ名を1つ指定してください。\n");

   setlocale(LC_ALL, "");

   initscr();
   keypad(stdscr, FALSE);
   noecho();
   start_color();
   bkgd(COLOR_PAIR(COLOR_BLACK));

   init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
   init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
   init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);

   menu();

   endwin();

   return 0;
}

void error(char *msg) {
   fprintf(stderr, "error: %s", msg);
   exit(1);
}
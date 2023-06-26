#include <curses.h>
#include <ncurses.h>
#include "cursor.h"

void setCursor(CURSOR *cursor, int y, int x){
   cursor->y = y;
   cursor->x = x;
   mvaddch(cursor->y, cursor->x, '|');
}

void mvcursor(CURSOR *cursor, int dy) {
   mvaddch(cursor->y, cursor->x, ' ');
   setCursor(cursor,cursor->y + dy, cursor->x);
}

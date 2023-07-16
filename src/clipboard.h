#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <ncurses.h>

WINDOW *init_selectionWindow(int y, int x);
WINDOW *init_clipboardWindow(int y, int x);

int update_selection(WINDOW *window,int kind);
int update_clipboard(WINDOW *window,int kind);

#endif
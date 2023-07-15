#ifndef SCORE_H
#define SCORE_H

#include <ncurses.h>


WINDOW *init_scoreWindow(int y, int x) ;
void update_score(int removedrow, int *score, WINDOW *window);

#endif

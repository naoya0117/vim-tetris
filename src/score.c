#include <ncurses.h>
#include "mutex_shared.h"
#include "string.h"

#define WINDOW_COL 2
#define WINDOW_ROW 20

WINDOW *init_scoreWindow(int y, int x) {
    WINDOW *window =  newwin(WINDOW_COL, WINDOW_ROW, y, x);
    init_pair(COLOR_WHITE, COLOR_BLACK, COLOR_WHITE);
    wbkgd(window, COLOR_PAIR(COLOR_WHITE));
    wmove(window, 0, 1);
    waddstr(window, "SCORE");
    wmove(window, 1, 1);
    waddstr(window, "0");
    wrefresh(window);
    return window;

}

void update_score(int removedrow, int *score, WINDOW *window){
    char str[WINDOW_ROW-2];
    *score += removedrow * removedrow * 200;
    pthread_mutex_lock(&mutex);
    wclear(window);
    sprintf(str, "%d", *score);
    wmove(window, 1, 1);
    waddstr(window,str);
    wrefresh(window);
    pthread_mutex_unlock(&mutex);
}
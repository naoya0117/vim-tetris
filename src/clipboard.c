#include <ncurses.h>
#include "mutex_shared.h"
#include "block.h"

#define WINDOW_COL 7
#define WINDOW_ROW 12

WINDOW *init_selectionWindow(int y, int x) {
    WINDOW *window =  newwin(WINDOW_COL, WINDOW_ROW, y, x);
    init_pair(COLOR_WHITE, COLOR_BLACK, COLOR_WHITE);
    wbkgd(window, COLOR_PAIR(COLOR_WHITE));
    wmove(window, 6, 1);
    waddstr(window, "yでコピー");
    wrefresh(window);
    return window;
}

int update_selection(WINDOW *window, int kind) {
    BLOCK block;

    pthread_mutex_lock(&mutex);
    wclear(window);

    wmove(window, 6, 1);
    waddstr(window, "yでコピー");

    generateBlock(&block,3, 3, kind);
    wcolorBlock(window,&block,COLOR_BLUE_BLOCK);

    wrefresh(window);
    pthread_mutex_unlock(&mutex);
    return 0;
}

WINDOW *init_clipboardWindow(int y, int x) {
    WINDOW *window =  newwin(WINDOW_COL, WINDOW_ROW, y, x);
    init_pair(COLOR_WHITE, COLOR_BLACK, COLOR_WHITE);
    wbkgd(window, COLOR_PAIR(COLOR_WHITE));
    wmove(window, 6, 1);
    waddstr(window, "pで使用");
    wrefresh(window);
    return window;
}

int update_clipboard(WINDOW *window, int kind) {
    BLOCK block;
    pthread_mutex_lock(&mutex);
    wclear(window);

    wmove(window, 6, 1);
    waddstr(window, "pで使用");

    if (kind != -1) {
        generateBlock(&block,3, 3,kind);
        wcolorBlock(window,&block,COLOR_BLUE_BLOCK);
    }

    wrefresh(window);
    pthread_mutex_unlock(&mutex);
    return 0;
}

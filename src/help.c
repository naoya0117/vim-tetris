#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "mutex_shared.h"

#define MAX_LINE 256


void help() {
    pthread_mutex_lock(&mutex);
    scr_dump("tmpscr.dump");

    int maxy, maxx;
    FILE *help_fp;
    char buffer[MAX_LINE];

    clear();
    timeout(-1);
    getmaxyx(stdscr,maxy,maxx);

    if ((help_fp = fopen("help.txt", "r")) == NULL) {
        fprintf(stderr, "help.txtがありません。\n");
        exit(1);
    }

    int x= 1,y = 0;
    int len;
    while(fgets(buffer, MAX_LINE, help_fp) != NULL) {
        len = strlen(buffer);
        if(buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        mvaddstr(y++, x, buffer);
        refresh();
    }
    fclose(help_fp);

    

    getch();
    refresh();

    scr_restore("tmpscr.dump");
    noecho();
    pthread_mutex_unlock(&mutex);

}


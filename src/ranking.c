#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"

#define NUM_OF_DATA 32
#define MAX_LINE 256

struct data {
   char name[16];
   int score;
} typedef DATA;

void bubbleSort(DATA data[], int n) ;

void edit_ranking(char *name, int score) {
   char buffer[MAX_LINE];
   FILE *fp;
   if ((fp = fopen("ranking.txt", "w"))== NULL) {
      fprintf(stderr, "ranking.txtが有りません。\n");
   }

   sprintf(buffer, "%s\t%d", name, score);
   fputs(buffer, fp);

}
void call_ranking() {
   FILE *fp;
   char buffer[256];
   char str[256];
   DATA data[NUM_OF_DATA];
   int n;
   int y, x;
   getmaxyx(stdscr, y, x);
   y /= 6;
   x /= 4;


   if ((fp = fopen("ranking.txt", "r"))== NULL) {
      fprintf(stderr, "ranking.txtがありません。\n");
      exit(1);
   }

   n = 0;
   int len;
   while(fgets(buffer, MAX_LINE, fp) != NULL) {
      len = strlen(buffer);
        if(buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }

      strcpy(str,strtok(buffer, "\t"));
      strcpy(data[n].name, str);
      strcpy(str,strtok(buffer, "\t"));
      data[n].score = atoi(str);
      n++;
   }

   bubbleSort(data, n);

   for (int i=0; i<n; i++) {
      sprintf(buffer, "%d位\t%d点\t%s", i+1, data[i].score, data[i].name);
      mvaddstr(y, x, buffer);
      refresh();
   }

   int ch;
   char cmd_buffer[MAX_COMMAND_LENGTH];
   timeout(-1);
   while(1) {
      if((ch = getch()) == ':') {
         call_command(0, buffer, MAX_COMMAND_LENGTH);
      }
   }
}


void bubbleSort(DATA data[], int n) {
    int i, j;
    DATA tmp;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (data[j].score < data[j+1].score) {
                tmp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = tmp;
            }
        }
    }
}
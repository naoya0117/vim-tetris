#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"

#define NUM_OF_DATA 12
#define MAX_LINE 256


struct data {
   char name[16];
   int score;
   int isCurrentScore;
} typedef DATA;

void insert_playerRank(char *user, int score, DATA *data, int n) ;
void write_ranking(char *user, int score, DATA *data, int n) ;
void bubbleSort(DATA data[], int n) ;
int read_ranking(DATA *data);

void call_ranking(char *user, int score) {
   FILE *fp;
   char buffer[256];
   char str[256];
   DATA data[NUM_OF_DATA];
   int i,n;
   int y, x;
   getmaxyx(stdscr, y, x);
   y /= 6;
   x /= 4;

   n = read_ranking(data);

   if (user != NULL) {
      n = n+1;
      insert_playerRank(user, score, data, n);
   }
   bubbleSort(data, n);


   for (int i=0; i<n; i++) {
      if (data[i].isCurrentScore) {
         mvaddstr(y + i*2, x - 15, "現在のスコア:");
      }
      sprintf(buffer, "%d位\t%d点\t%s", i+1, data[i].score, data[i].name);
      mvaddstr(y + i*2, x, buffer);
      refresh();
   }


   int ch;
   char cmd_buffer[MAX_COMMAND_LENGTH];
   timeout(-1);
   while(1) {
      if((ch = getch()) == ':') {
         call_command(0, cmd_buffer, MAX_COMMAND_LENGTH);
         if (!strcmp(cmd_buffer, "w") || !strcmp(cmd_buffer, "write")) {
            write_ranking(user, score,data, n);
         }
      }
   }

}




void update_ranking(char *user, int score) {
   int n;
   DATA data[MAX_LINE];

   n = read_ranking(data);
   n++;
   insert_playerRank(user, score, data, n);
   bubbleSort(data, n);
   write_ranking(user, score, data, n);
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

void insert_playerRank(char *user, int player_score, DATA *data, int n) {

   DATA player;
   strcpy(player.name, user);
   player.score = player_score;
   player.isCurrentScore = 1;


   data[n-1] = player;
}

void write_ranking(char *user, int score, DATA *data, int n) {
   FILE *fp;
   char buffer[MAX_LINE];
   if (user != NULL) {
      if((fp=fopen("ranking.txt","w")) == NULL) {
         fprintf(stderr, "ranking.txtがありません。");
         endwin();
         exit(1);
      }

      int cnt = 0;
      for  (int i=0; i<n && cnt < NUM_OF_DATA - 1; i++) {
         if (!strcmp(data[i].name, user) && !data[i].isCurrentScore) {
            continue;
         }
         sprintf(buffer, "%s\t%d\n", data[i].name, data[i].score);
         fputs(buffer, fp);
         cnt++;
      }
      refresh();
      show_message("現在のスコアをランキングに保存しました。");
      fclose(fp);
   }
}

int read_ranking(DATA *data) {
   FILE *fp;
   char buffer[MAX_LINE];
   char str[MAX_LINE];

   if ((fp = fopen("ranking.txt", "r"))== NULL) {
      if((fp = fopen("ranking.txt", "w"))== NULL) {
         endwin();
         fprintf(stderr, "ファイルの作成に失敗しました。\n");
         exit(1);
      }
      fclose(fp);
      if ((fp = fopen("ranking.txt", "r"))== NULL) {
         endwin();
         fprintf(stderr, "ranking.txtを開けませんでした。\n終了します。\n");
         exit(1);
      }
   }

   int n = 0;
   int len;
   while(fgets(buffer, MAX_LINE, fp) != NULL) {
      len = strlen(buffer);
        if(buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }

      strcpy(str,strtok(buffer, "\t"));
      strcpy(data[n].name, str);

      strcpy(str,strtok(NULL, "\t"));
      data[n].score = atoi(str);
      data[n].isCurrentScore = 0;
      n++;
   }
   fclose(fp);

   return n;
}

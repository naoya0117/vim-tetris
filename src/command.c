#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "help.h"
#include <pthread.h>
#include "mutex_shared.h"

WINDOW *cmdwin;
pthread_mutex_t mutex;
void init_sharedmutex() {
  pthread_mutex_init(&mutex, NULL);
}
struct position {
   int x;
   int y;
} typedef POSITION;
void get_command(char *buffer, int length);
void show_message(char *msg);
POSITION calc_commandArea();


WINDOW *create_commandwin() {

  return subwin(stdscr, 1, 50 , calc_commandArea().y, calc_commandArea().x);
}


int call_command(int isallowed, char str[], int length) {
  char *cmd;
  int force = 0;
  int find = 0;

  memset(str, '\0', length);


  pthread_mutex_lock(&mutex);
  wclear(cmdwin);
  pthread_mutex_unlock(&mutex);
  get_command(str, length);
  int y, x;
  getyx(cmdwin, y, x);

  getyx(cmdwin, y, x);


  if (strlen(str) == 0) return find = 1;

  if (strchr(str, '!')) {
    force = 1;
    strtok(str, "!");
  }

  if (!strcmp(str, "q") || !strcmp(str, "quit")) {
    find = 1;
    if (isallowed || force) {
        pthread_mutex_lock(&mutex);
        wclear(cmdwin);
        clear();
        endwin();
        pthread_mutex_unlock(&mutex);
         exit(0);
     } else {
      show_message("エラー:強制終了するには!を加えてください。");
     }
  }

  if (!strcmp(str, "h") || !strcmp(str, "help")) {
    find = 1;
    if (force) show_message("Error: 慌てないでください");
    else help();
  }
  return find;
}

POSITION calc_commandArea() {
  int max_y, max_x;
  POSITION pos;
  pthread_mutex_lock(&mutex);
  getmaxyx(stdscr, max_y, max_x);
  pos.x = 0;
  pos.y = max_y - 1;
  pthread_mutex_unlock(&mutex);

  return pos;
}

void get_command(char *buffer, int length) {
  int ch;
  keypad(cmdwin, TRUE);
  POSITION inp_pos;
  inp_pos.x = 0;
  inp_pos.y = getmaxy(cmdwin) - 1; 

  char *p = buffer;

  pthread_mutex_lock(&mutex);
  wclear(cmdwin);
  wmove(cmdwin, inp_pos.y , inp_pos.x);
  waddch(cmdwin , ':');
  wrefresh(cmdwin);
  pthread_mutex_unlock(&mutex);

  inp_pos.x++;

  while((ch = wgetch(cmdwin)) != '\n') {
    if (ch == 27) {//escキー
      memset(buffer, '\0', length);
      return;
    }

    if(ch == KEY_BACKSPACE || ch == 127) { //c1-byodでbackspaceは'^?'(127)
      if (p > buffer) {
        pthread_mutex_lock(&mutex);
        wmove(cmdwin, inp_pos.y, --inp_pos.x);
        wdelch(cmdwin);
        pthread_mutex_unlock(&mutex);
        *--p = '\0';
      } else if (p <= buffer) {
        pthread_mutex_lock(&mutex);
        wmove(cmdwin, inp_pos.y, --inp_pos.x);
        wdelch(cmdwin);
        wrefresh(cmdwin);
        pthread_mutex_unlock(&mutex);
        break;
      }
    } else {
      if (p >= buffer + length - 1)
        continue;
      pthread_mutex_lock(&mutex);
      wmove(cmdwin, inp_pos.y, inp_pos.x);
      waddch(cmdwin, ch);
      pthread_mutex_unlock(&mutex);
      inp_pos.x++;
      *p++ = ch;
    }
    pthread_mutex_lock(&mutex);
    wrefresh(cmdwin);
    pthread_mutex_unlock(&mutex);
  }


}




void show_message(char *msg) {
  POSITION cmd_area=calc_commandArea();
  wclear(cmdwin);
  pthread_mutex_lock(&mutex);
  wmove(cmdwin, cmd_area.y, cmd_area.x);
  waddstr(cmdwin , msg );
  wrefresh(cmdwin);
  pthread_mutex_unlock(&mutex);
}

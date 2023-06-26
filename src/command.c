#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "help.h"

void get_command(char *buffer, int length);
void clear_commandArea();
void show_message(char *msg);

struct position {
   int x;
   int y;
} typedef POSITION;

int call_command(int isallowed, char str[], int length) {
  char *cmd;
  int force = 0;
  int find = 0;

  memset(str, '\0', length);


  clear_commandArea();
  get_command(str, length);


  if (strlen(str) == 0) return find;

  if (strchr(str, '!')) {
    force = 1;
    strtok(str, "!");
  }

  if (!strcmp(str, "q") || !strcmp(str, "quit")) {
    find = 1;
    if (isallowed || force) {
         clear();
         refresh();
         exit(0);
     } else {
      show_message("エラー:ファイルが最新ではないため終了できません。強制実行するには!を加えてください。");
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
  getmaxyx(stdscr, max_y, max_x);
  pos.x = 0;
  pos.y = max_y -1;

  return pos;
}

void get_command(char *buffer, int length) {
  int ch;
  keypad(stdscr, TRUE);
  POSITION inp_pos;

  char *p = buffer;

  mvaddch(inp_pos.y, inp_pos.x++, ':');

  while((ch = getch()) != '\n') {

    if(ch == KEY_BACKSPACE || ch == 127) { //c1-byodでbackspaceは'^?'(127)
      if (p > buffer) {
        mvdelch(inp_pos.y, --inp_pos.x);
        *--p = '\0';
      } else if (p <= buffer) {
        mvdelch(inp_pos.y, --inp_pos.x);
        break;
      }
    } else {
      if (p >= buffer + length - 1)
        continue;
      mvaddch(inp_pos.y, inp_pos.x++, ch);
      *p++ = ch;
    }
    refresh();

  }


}



void clear_commandArea () {
  POSITION cmd_area = calc_commandArea();
  int x;
  int start_x = cmd_area.x;
  int end_x = getmaxx(stdscr);

  for (x=start_x; x<end_x; x++) {
    mvaddch(cmd_area.y, x , ' ');
  }
}

void show_message(char *msg) {
  POSITION cmd_area=calc_commandArea();
  attron(COLOR_PAIR(COLOR_RED | A_BOLD));
  mvaddstr(cmd_area.y, cmd_area.x, msg );
  attroff(COLOR_PAIR(COLOR_RED | A_BOLD));
  refresh();
}

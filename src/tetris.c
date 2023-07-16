#include "block.h"
#include "command.h"
#include <ncurses.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "mutex_shared.h"
#include "cursor.h"
#include "score.h"
#include "ranking.h"
#include "clipboard.h"

#define GAME_XLENGTH 12
#define GAME_YLENGTH 22

struct screen {
  int x;
  int y;
} typedef SCREEN;

struct thread_args {
  char *buffer;
  int *iscmdmode;
  int result;
} typedef THREAD_ARGS;

void draw_gameScreen(SCREEN base);
int tetris(SCREEN base, char *user);
void init_blockData(int isblock[GAME_XLENGTH][GAME_YLENGTH], int isRowFull[]);

int get_scrx(int x, SCREEN base);
int get_scry(int y, SCREEN base);
int get_absx(int scrx, SCREEN base);
int get_absy(int scry,SCREEN base);
int canBlockMove(int isblock[GAME_XLENGTH][GAME_YLENGTH], BLOCK block, int dy,
                 int dx, SCREEN gameScreen);
int canBlockRotate(int isblock[GAME_XLENGTH][GAME_YLENGTH], BLOCK block,
                   int rotate, SCREEN gameScreen);
int canBlockChange(int isblock[GAME_XLENGTH][GAME_YLENGTH], BLOCK block, int kind, SCREEN gameScreen) ;
void stack_block(int isBlock[GAME_XLENGTH][GAME_YLENGTH], BLOCK block,
                 SCREEN gameScreen);
void *command(void *args);
void colorRow(int y, SCREEN screen,short color) ;
void checkRowFull(int isBlock[GAME_XLENGTH][GAME_YLENGTH],int isRowFull[], SCREEN base) ;
int deleteRow(int starty, int endy, int isBlock[GAME_XLENGTH][GAME_YLENGTH], int isRowFull[], SCREEN base) ;
void show_gameOver(int y, int x);

void call_tetris(char *user) {
  int max_x, max_y;
  int score;
  SCREEN gameScreen;

  add_blockcolor();

  getmaxyx(stdscr, max_y, max_x);

  gameScreen.x = max_x / 2 - GAME_XLENGTH;
  gameScreen.y = max_y / 10;

  draw_gameScreen(gameScreen);
  score = tetris(gameScreen, user);
  show_gameOver(gameScreen.y-1, gameScreen.x);

  clear();
  call_ranking(user, score);
}

void draw_gameScreen(SCREEN base) {
  int x = base.x;
  int y = base.y;

  int i=0;
  while (i<GAME_YLENGTH) {
    squire(y, x, COLOR_WHITE_BLOCK);
    squire(y, x + SQUIRE_XLENGTH * (GAME_XLENGTH-1), COLOR_WHITE_BLOCK);
    y += SQUIRE_YLENGTH;
    i++;
  }
  i = 0;
  y = base.y;
  while (i<GAME_XLENGTH) {
    squire(y , x, COLOR_WHITE_BLOCK);
    squire(y + (GAME_YLENGTH-1) * SQUIRE_YLENGTH, x, COLOR_WHITE_BLOCK);
    x += SQUIRE_XLENGTH;
    i++;
  }


  refresh();
}

int tetris(SCREEN base, char *user) {
  pthread_t command_thread;
  int thread_flag = 0;
  void* thread_result;
  int command_mode = 0;
  int score = 0;
  WINDOW *score_win;
  WINDOW *sel_win;
  WINDOW *clip_win;
  SCREEN selwin_base;
  SCREEN clipwin_base;
  selwin_base.y = 4;
  selwin_base.x = base.x -15;
  clipwin_base.y = 12;
  clipwin_base.x = base.x -15;

  score_win = init_scoreWindow(10, base.x + GAME_XLENGTH * SQUIRE_XLENGTH + 5);
  sel_win = init_selectionWindow(selwin_base.y, selwin_base.x );
  clip_win = init_clipboardWindow(clipwin_base.y, clipwin_base.x);

  CURSOR cursor;

  setCursor(&cursor, base.y + GAME_YLENGTH/2, base.x - 1) ;


  struct timeval start, end;
  double d_time = 0;
  gettimeofday(&start, NULL);
  char cmd_buffer[MAX_COMMAND_LENGTH];

  BLOCK focusedBlock;
  THREAD_ARGS command_args;
  int rotate;
  int kind;
  int isBlock[GAME_XLENGTH][GAME_YLENGTH];
  int isRowFull[GAME_YLENGTH];

  int sel_kind;
  int clip_kind = -1;

  int ch;
  int block_x, block_y;
  int dx = 0, dy = 0;
  int ret;

  int delete_y = 0;

  int delete_flag = 0;
  int virtual_mode = 0;
  int insertion_mode = 1;

  int deletedRow_n;
  int gameover_flag = 0;


  rotate = 0;
  init_blockData(isBlock, isRowFull);

  show_message("挿入モード:h,l,j,kでブロックを動かせます");

  while (!gameover_flag && (gameover_flag = 1)) {
    sel_kind = rand() % 7;
    update_selection(sel_win,sel_kind);

    kind = rand() % 7;
    generateBlock(&focusedBlock, base.y + 3, base.x + GAME_XLENGTH - 2, kind);
    pthread_mutex_lock(&mutex);
    mvBlock(&focusedBlock, 0, 0);
    pthread_mutex_unlock(&mutex);

    srand((unsigned int)time(NULL));

    while (canBlockMove(isBlock, focusedBlock, 1, 0, base)) {
      gameover_flag = 0;

      dx = 0;
      dy = 0;

      gettimeofday(&end, NULL);
      d_time = (double)(end.tv_sec - start.tv_sec) +
               (double)(end.tv_usec - start.tv_usec) / (1000 * 1000);
      if (d_time >= 0.4) {
        gettimeofday(&start, NULL);

        if (canBlockMove(isBlock, focusedBlock, dy + 1, dx, base)) {
          pthread_mutex_lock(&mutex);
          mvBlock(&focusedBlock, 1, 0);
          pthread_mutex_unlock(&mutex);
        }
        d_time = 0;
      }
      if (!command_mode) {
        if (thread_flag) {
          thread_flag = 0;
          pthread_join(command_thread, NULL);
          if (!command_args.result) {
            if (!strcmp(cmd_buffer, "w") || !strcmp(cmd_buffer, "write")) {
              update_ranking(user, score);
            } else {
              show_message("コマンドが存在しません。");
            }
          }
          insertion_mode=0;
        }
        timeout(400);
        ch = getch();
        if (ch == ':' && !thread_flag) {
            command_args.buffer = cmd_buffer;
            command_args.iscmdmode = &command_mode;
            command_mode = 1;
            thread_flag = 1;
            pthread_create(&command_thread, NULL,
                                command, (void *)&command_args);
        } else if (insertion_mode) {
          if (ch == 'k' &&
              canBlockRotate(isBlock, focusedBlock, rotate, base)) {
              rotateBlock(&focusedBlock, rotate++);
          } else if (ch == 'j' &&
              canBlockMove(isBlock, focusedBlock, dy + 1, dx, base)) {
              dy = 1;
          } else if (ch == 'h' &&
              canBlockMove(isBlock, focusedBlock, dy, dx - 1, base)) {
              dx = -1;
          } else if (ch == 'l' &&
              canBlockMove(isBlock, focusedBlock, dy, dx + 1, base)) {
              dx = 1;
          } else if (ch == 27) {
              insertion_mode = 0;
              show_message("通常モード:j,kでカーソル移動、ddで一行削除");
          }
        } else {
          if (ch == 'k' && get_scry(cursor.y, base) > 1) {
            mvcursor(&cursor, -1);
          } else if (ch == 'j' && get_scry(cursor.y, base) < GAME_YLENGTH - 2) {
            mvcursor(&cursor, 1);
          } else if (ch == 'd') {

            if (!delete_flag) delete_flag = 1;
            else {
                if (!delete_y) delete_y = get_scry(cursor.y, base);

                deletedRow_n = deleteRow(get_scry(cursor.y, base), delete_y, isBlock, isRowFull, base);
                update_score(deletedRow_n, &score, score_win);

                delete_flag = 0;
                delete_y = 0;
                virtual_mode = 0;
            }

          } else if (ch == 'y') {
            update_clipboard(clip_win, sel_kind);
            clip_kind = sel_kind;
          } else if (ch == 'p' && clip_kind != -1 &&canBlockChange(isBlock, focusedBlock, clip_kind, base) ) {
            kind = clip_kind;
            colorBlock(&focusedBlock, COLOR_BLACK_BLOCK);
            generateBlock(&focusedBlock, focusedBlock.y1, focusedBlock.x1, kind);
            clip_kind = -1;
            update_clipboard(clip_win, clip_kind);
            refresh();
          }if (ch == 'V') {
            if (virtual_mode) {
              virtual_mode = 0;
              delete_y = 0;
              show_message("通常モード:j,kでカーソル移動、ddで一行削除");
            } else {
              delete_flag = 1;
              virtual_mode = 1;
              delete_y = get_scry(cursor.y, base);
              show_message("選択モード");
            }
          } else if (ch == 'i' || ch == 'a') {
            insertion_mode= 1;
            show_message("挿入モード: h,l,j,kでブロックを動かせます");
          }
        }
      }
        pthread_mutex_lock(&mutex);
        mvBlock(&focusedBlock, dy, dx);
        refresh();
        pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_lock(&mutex);
    colorBlock(&focusedBlock, COLOR_WHITE_BLOCK);
    stack_block(isBlock, focusedBlock, base);
    checkRowFull(isBlock,isRowFull,base);
    pthread_mutex_unlock(&mutex);

  }
  refresh();

  return score;
}

int isGameOver(int flag) { return flag; }

void init_blockData(int isblock[GAME_XLENGTH][GAME_YLENGTH], int isRowFull[]) {
  for (int x = 0; x < GAME_XLENGTH; x++) {
    for (int y = 0; y < GAME_YLENGTH; y++) {
      if (x == 0 || x == GAME_XLENGTH - 1 || y == GAME_YLENGTH - 1)
        isblock[x][y] = 1;
      else
        isblock[x][y] = 0;
    }
  }
  for (int y = 0; y< GAME_YLENGTH; y++) {
    isRowFull[y] = 0;
  }
}

int canBlockMove(int isblock[GAME_XLENGTH][GAME_YLENGTH], BLOCK block, int dy,
                 int dx, SCREEN gameScreen) {
  int x1, x2, x3, x4;
  int y1, y2, y3, y4;

  BLOCK nextBlock = calc_move(block, dy, dx);

  x1 = get_scrx(nextBlock.x1, gameScreen);
  y1 = get_scry(nextBlock.y1, gameScreen);
  x2 = get_scrx(nextBlock.x2, gameScreen);
  y2 = get_scry(nextBlock.y2, gameScreen);
  x3 = get_scrx(nextBlock.x3, gameScreen);
  y3 = get_scry(nextBlock.y3, gameScreen);
  x4 = get_scrx(nextBlock.x4, gameScreen);
  y4 = get_scry(nextBlock.y4, gameScreen);

  return !isblock[x1][y1] && !isblock[x2][y2] && !isblock[x3][y3] &&
         !isblock[x4][y4];
}

int canBlockRotate(int isblock[GAME_XLENGTH][GAME_YLENGTH], BLOCK block,
                   int rotate, SCREEN gameScreen) {
  int x1, x2, x3, x4;
  int y1, y2, y3, y4;
  BLOCK nextBlock = calc_ChangeBlock(block, rotate);

  x1 = get_scrx(nextBlock.x1, gameScreen);
  y1 = get_scry(nextBlock.y1, gameScreen);
  x2 = get_scrx(nextBlock.x2, gameScreen);
  y2 = get_scry(nextBlock.y2, gameScreen);
  x3 = get_scrx(nextBlock.x3, gameScreen);
  y3 = get_scry(nextBlock.y3, gameScreen);
  x4 = get_scrx(nextBlock.x4, gameScreen);
  y4 = get_scry(nextBlock.y4, gameScreen);

  return !isblock[x1][y1] && !isblock[x2][y2] && !isblock[x3][y3] &&
         !isblock[x4][y4];
}

int canBlockChange(int isblock[GAME_XLENGTH][GAME_YLENGTH], BLOCK block, int kind, SCREEN gameScreen) {
  int x1, x2, x3, x4;
  int y1, y2, y3, y4;

  BLOCK nextBlock = calc_ChangeBlock(block, kind);

  x1 = get_scrx(nextBlock.x1, gameScreen);
  y1 = get_scry(nextBlock.y1, gameScreen);
  x2 = get_scrx(nextBlock.x2, gameScreen);
  y2 = get_scry(nextBlock.y2, gameScreen);
  x3 = get_scrx(nextBlock.x3, gameScreen);
  y3 = get_scry(nextBlock.y3, gameScreen);
  x4 = get_scrx(nextBlock.x4, gameScreen);
  y4 = get_scry(nextBlock.y4, gameScreen);

  return !isblock[x1][y1] && !isblock[x2][y2] && !isblock[x3][y3] &&
         !isblock[x4][y4];
}
void stack_block(int isBlock[GAME_XLENGTH][GAME_YLENGTH], BLOCK block,
                 SCREEN base) {
  int x1, x2, x3, x4;
  int y1, y2, y3, y4;

  x1 = get_scrx(block.x1, base);
  y1 = get_scry(block.y1, base);
  x2 = get_scrx(block.x2, base);
  y2 = get_scry(block.y2, base);
  x3 = get_scrx(block.x3, base);
  y3 = get_scry(block.y3, base);
  x4 = get_scrx(block.x4, base);
  y4 = get_scry(block.y4, base);

  isBlock[x1][y1] = 1;
  isBlock[x2][y2] = 1;
  isBlock[x3][y3] = 1;
  isBlock[x4][y4] = 1;
}

int get_scrx(int x, SCREEN base) { return (x - base.x) / SQUIRE_XLENGTH; }
int get_scry(int y, SCREEN base) { return (y - base.y) / SQUIRE_YLENGTH; }

void *command(void * arg) {
  int result;
  THREAD_ARGS *args = (THREAD_ARGS*)arg;
  *(args->iscmdmode) = 1;
  args->result = call_command(0, (*args).buffer, MAX_COMMAND_LENGTH);
  *(args->iscmdmode) = 0;
  pthread_exit(NULL);
}


void checkRowFull(int isBlock[GAME_XLENGTH][GAME_YLENGTH],int isRowFull[], SCREEN base) {
   int flag;
   int x, y;

   for (y=GAME_YLENGTH-2; y>=1; y--) {
      flag = 1;
      for (x=GAME_XLENGTH-2; x>=1; x--) {
         if(!isBlock[x][y])
            flag = 0;
      }
      if (flag) {
         isRowFull[y] = 1;
         colorRow(y, base, COLOR_RED_BLOCK);
      }
   }

}

void colorRow(int y, SCREEN screen,short color) {
  int x;
  for (x = 1; x<GAME_XLENGTH-1; x++) {
    squire(get_absy(y, screen), get_absx(x, screen), color);
  }
}

int get_absx(int scrx, SCREEN base) { return base.x + scrx * SQUIRE_XLENGTH;};
int get_absy(int scry,SCREEN base) { return base.y + scry * SQUIRE_YLENGTH;};

int deleteRow(int starty, int endy, int isBlock[GAME_XLENGTH][GAME_YLENGTH], int isRowFull[], SCREEN base) {
  int x,y;
  int cnt=0;

  if (starty > endy) {
    int tmp = starty;
    starty = endy;
    endy = tmp;
  }

  y = starty;
  while (isRowFull[y])y++;
  if (y<=endy) return 0;

  for (y = starty; y <= endy ; y++) {
    colorRow(y, base, COLOR_BLACK_BLOCK);
    isRowFull[y] = 0;
    cnt++;
    for (x = 1; x < GAME_XLENGTH - 1; x++) {
      isBlock[x][y] = 0;
    }
  }

  for (y = endy; y>= cnt + 1; y--) {
    colorRow(y-cnt,base, COLOR_BLACK_BLOCK);
    for (x = 1; x < GAME_XLENGTH -1 ; x++) {
      isBlock[x][y] = isBlock[x][y-cnt];
      isBlock[x][y-cnt] = 0;
      if(isBlock[x][y]) squire(get_absy(y, base), get_absx(x, base),COLOR_WHITE_BLOCK);
      else squire(get_absy(y, base), get_absx(x, base),COLOR_BLACK_BLOCK);
    }
  }
  checkRowFull(isBlock, isRowFull, base);

  return cnt;
}

void show_gameOver(int y, int x) {
  mvaddstr(y, x, "GameOver!!!");
  refresh();
  sleep(3);
}
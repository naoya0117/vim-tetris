#include "block.h"
#include "command.h"
#include <ncurses.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "mutex_shared.h"

#define GAME_XLENGTH 12
#define GAME_YLENGTH 22
#define MAX_COMMAND_LENGTH 32

struct screen {
  int x;
  int y;
} typedef SCREEN;

struct thread_args {
  char *buffer;
  int *iscmdmode;
  int result;
} typedef THREAD_ARGS;

void call_game();
void draw_gameScreen(SCREEN base);
int tetris(SCREEN base);
int isGameOver();
void init_blockData(int isblock[GAME_XLENGTH][GAME_YLENGTH], int isRowFull[]);

int get_scrx(int x, SCREEN base);
int get_scry(int y, SCREEN base);
int get_absx(int scrx, SCREEN base);
int get_absy(int scry,SCREEN base);
int canBlockMove(int isblock[GAME_XLENGTH][GAME_YLENGTH], BLOCK block, int dy,
                 int dx, SCREEN gameScreen);
int canBlockRotate(int isblock[GAME_XLENGTH][GAME_YLENGTH], BLOCK block,
                   int rotate, SCREEN gameScreen);
void stack_block(int isBlock[GAME_XLENGTH][GAME_YLENGTH], BLOCK block,
                 SCREEN gameScreen);

void *command(void *args);
void colorRow(int y, SCREEN screen,short color) ;
void checkFilledRow(int isBlock[GAME_XLENGTH][GAME_YLENGTH],int isRowFull[], SCREEN base) ;
void call_tetris() {
  int max_x, max_y;
  SCREEN gameScreen;

  add_blockcolor();

  getmaxyx(stdscr, max_y, max_x);

  gameScreen.x = max_x / 2 - GAME_XLENGTH;
  gameScreen.y = max_y / 10;

  draw_gameScreen(gameScreen);
  tetris(gameScreen);
}

void draw_gameScreen(SCREEN base) {
  move(base.y, base.x);
  hline(ACS_HLINE, GAME_XLENGTH - 1);
  vline(ACS_VLINE, GAME_YLENGTH - 1);
  move(base.y + GAME_YLENGTH - 1, base.x);
  hline(ACS_HLINE, GAME_XLENGTH);
  move(base.y, base.x + (GAME_XLENGTH - 1) * 2);
  vline(ACS_VLINE, GAME_YLENGTH - 1);
  refresh();
}

int tetris(SCREEN base) {
  pthread_t command_thread;
  int thread_flag = 0;
  void* thread_result;
  int command_mode = 0;
  


  srand((unsigned int)time(NULL));
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

  int ch;
  int block_x, block_y;
  int dx = 0, dy = 0;
  int ret;


  rotate = 0;
  init_blockData(isBlock, isRowFull);

  while (!isGameOver()) {
    kind = rand() % 7;

    generateBlock(&focusedBlock, base.y + 3, base.x + GAME_XLENGTH - 2, kind);
    while (canBlockMove(isBlock, focusedBlock, 1, 0, base)) {

      dx = 0;
      dy = 0;

      gettimeofday(&end, NULL);
      d_time = (double)(end.tv_sec - start.tv_sec) +
               (double)(end.tv_usec - start.tv_usec) / (1000 * 1000);
      if (d_time >= 0.6) {
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
          }
        }
        timeout(600);
        ch = getch();
        if (ch == ':' && !thread_flag) {
            command_args.buffer = cmd_buffer;
            command_args.iscmdmode = &command_mode;
            command_mode = 1;
            thread_flag = 1;
            pthread_create(&command_thread, NULL,
                                command, (void *)&command_args);
        } else if (ch == ' ' &&
            canBlockRotate(isBlock, focusedBlock, rotate, base)) {
            rotateBlock(&focusedBlock, rotate++);
        } else if (ch == '\n' &&
            canBlockMove(isBlock, focusedBlock, dy + 1, dx, base)) {
            dy = 1;
        } else if (ch == 'h' &&
            canBlockMove(isBlock, focusedBlock, dy, dx - 1, base)) {
            dx = -1;
        } else if (ch == 'l' &&
            canBlockMove(isBlock, focusedBlock, dy, dx + 1, base)) {
            dx = 1;
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
    checkFilledRow(isBlock,isRowFull,base);
    pthread_mutex_unlock(&mutex);
  }

  return 0;
}

int isGameOver() { return 0; }

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

  x1 = get_scrx(calc_move(block, dy, dx).x1, gameScreen);
  y1 = get_scry(calc_move(block, dy, dx).y1, gameScreen);
  x2 = get_scrx(calc_move(block, dy, dx).x2, gameScreen);
  y2 = get_scry(calc_move(block, dy, dx).y2, gameScreen);
  x3 = get_scrx(calc_move(block, dy, dx).x3, gameScreen);
  y3 = get_scry(calc_move(block, dy, dx).y3, gameScreen);
  x4 = get_scrx(calc_move(block, dy, dx).x4, gameScreen);
  y4 = get_scry(calc_move(block, dy, dx).y4, gameScreen);

  return !isblock[x1][y1] && !isblock[x2][y2] && !isblock[x3][y3] &&
         !isblock[x4][y4];
}

int canBlockRotate(int isblock[GAME_XLENGTH][GAME_YLENGTH], BLOCK block,
                   int rotate, SCREEN gameScreen) {
  int x1, x2, x3, x4;
  int y1, y2, y3, y4;

  x1 = get_scrx(calc_rotate(block, rotate).x1, gameScreen);
  y1 = get_scry(calc_rotate(block, rotate).y1, gameScreen);
  x2 = get_scrx(calc_rotate(block, rotate).x2, gameScreen);
  y2 = get_scry(calc_rotate(block, rotate).y2, gameScreen);
  x3 = get_scrx(calc_rotate(block, rotate).x3, gameScreen);
  y3 = get_scry(calc_rotate(block, rotate).y3, gameScreen);
  x4 = get_scrx(calc_rotate(block, rotate).x4, gameScreen);
  y4 = get_scry(calc_rotate(block, rotate).y4, gameScreen);

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


void checkFilledRow(int isBlock[GAME_XLENGTH][GAME_YLENGTH],int isRowFull[], SCREEN base) {
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
#include <ncurses.h>
#include "block.h"


#define COLOR_BLACK_BLOCK 8
#define COLOR_RED_BLOCK 9
#define COLOR_BLUE_BLOCK 10
#define COLOR_WHITE_BLOCK 11


void swap(int *a, int *b);
void wsquire(WINDOW *win,int y, int x, short color) ;
void wcolorBlock(WINDOW *win,BLOCK *block, short color) ;
void squire(int y, int x, short color);
void generateBlock(BLOCK *block, int y, int x, int kind) {
   if(kind == 0) {
      block->x1 = x;
      block->y1 = y;
      block->x2 = x;
      block->y2 = y- SQUIRE_YLENGTH;
      block->x3 = x;
      block->y3 = y- SQUIRE_YLENGTH * 2;
      block->x4 = x+ SQUIRE_XLENGTH;
      block->y4 = y;
   }
   if(kind == 1) {
      block->x1 = x;
      block->y1 = y;
      block->x2 = x;
      block->y2 = y- SQUIRE_YLENGTH;
      block->x3 = x;
      block->y3 = y+ SQUIRE_YLENGTH;
      block->x4 = x;
      block->y4 = y+ SQUIRE_YLENGTH * 2;
   }
   if(kind == 2) {
      block->x1 = x;
      block->y1 = y;
      block->x2 = x - SQUIRE_XLENGTH;
      block->y2 = y;
      block->x3 = x + SQUIRE_XLENGTH;
      block->y3 = y;
      block->x4 = x ;
      block->y4 = y - SQUIRE_YLENGTH;
   }
   if(kind == 3) {
      block->x1 = x;
      block->y1 = y;
      block->x2 = x;
      block->y2 = y- SQUIRE_YLENGTH;
      block->x3 = x;
      block->y3 = y- SQUIRE_YLENGTH * 2;
      block->x4 = x- SQUIRE_XLENGTH;
      block->y4 = y;
   }
   if(kind == 4) {
      block->x1 = x;
      block->y1 = y;
      block->x2 = x;
      block->y2 = y - SQUIRE_YLENGTH;
      block->x3 = x - SQUIRE_XLENGTH;
      block->y3 = y - SQUIRE_YLENGTH;
      block->x4 = x + SQUIRE_XLENGTH;
      block->y4 = y;
   }
   if (kind == 5) {
      block->x1 = x;
      block->y1 = y;
      block->x2 = x;
      block->y2 = y - SQUIRE_YLENGTH;
      block->x3 = x + SQUIRE_XLENGTH;
      block->y3 = y - SQUIRE_YLENGTH;
      block->x4 = x - SQUIRE_XLENGTH;
      block->y4 = y;
   }
   if (kind == 6) {
      block->x1 = x;
      block->y1 = y;
      block->x2 = x + SQUIRE_XLENGTH;
      block->y2 = y;
      block->x3 = x;
      block->y3 = y - SQUIRE_YLENGTH;
      block->x4 = x + SQUIRE_XLENGTH;
      block->y4 = y - SQUIRE_YLENGTH;
   }
}
void colorBlock(BLOCK *block, short color) {
   wcolorBlock(stdscr, block, color);
}
void wcolorBlock(WINDOW *win, BLOCK *block, short color) {
   wsquire(win,block->y1, block->x1, color);
   wsquire(win,block->y2, block->x2, color);
   wsquire(win,block->y3, block->x3, color);
   wsquire(win,block->y4, block->x4, color);
}

void mvBlock(BLOCK *block, int dy, int dx) {
   colorBlock(block, COLOR_BLACK_BLOCK);
   *block = calc_move(*block, dy, dx);
   colorBlock(block, COLOR_BLUE_BLOCK);
}


BLOCK calc_move(BLOCK block, int dy, int dx) {
   block.x1 = block.x1 + dx * SQUIRE_XLENGTH;
   block.y1 = block.y1 + dy * SQUIRE_YLENGTH;
   block.x2 = block.x2 + dx * SQUIRE_XLENGTH;
   block.y2 = block.y2 + dy * SQUIRE_YLENGTH;
   block.x3 = block.x3 + dx * SQUIRE_XLENGTH;
   block.y3 = block.y3 + dy * SQUIRE_YLENGTH;
   block.x4 = block.x4 + dx * SQUIRE_XLENGTH;
   block.y4 = block.y4 + dy * SQUIRE_YLENGTH;

   return block;
}


void rotateBlock(BLOCK *block, int rotate) {

   colorBlock(block, COLOR_BLACK_BLOCK);

   *block = calc_rotate(*block, rotate);

   colorBlock(block, COLOR_BLUE_BLOCK);
}


BLOCK calc_rotate(BLOCK block, int rotate) {
   int x1, x2, x3, x4;
   int y1, y2, y3, y4;

   x1 = 0;
   y1 = 0;
   x2 = (block.x2 - block.x1) / SQUIRE_XLENGTH;
   y2 = (block.y2 - block.y1) / SQUIRE_YLENGTH;
   x3 = (block.x3 - block.x1) / SQUIRE_XLENGTH;
   y3 = (block.y3 - block.y1) / SQUIRE_YLENGTH;
   x4 = (block.x4 - block.x1) / SQUIRE_XLENGTH;
   y4 = (block.y4 - block.y1) / SQUIRE_YLENGTH;

   swap(&x2, &y2);
   swap(&x3, &y3);
   swap(&x4, &y4);

   y2 *= -1;
   y3 *= -1;
   y4 *= -1;

   block.x2 = block.x1 + (x2 * SQUIRE_XLENGTH);
   block.y2 = block.y1 + (y2 * SQUIRE_YLENGTH);
   block.x3 = block.x1 + (x3 * SQUIRE_XLENGTH);
   block.y3 = block.y1 + (y3 * SQUIRE_YLENGTH);
   block.x4 = block.x1 + (x4 * SQUIRE_XLENGTH);
   block.y4 = block.y1 + (y4 * SQUIRE_YLENGTH);

   return block;

}

BLOCK calc_ChangeBlock(BLOCK block, int kind) {
   BLOCK next = block;
   generateBlock(&next, block.y1, block.x1, kind);
   return next;
}

void squire(int y, int x, short color) {
   wsquire(stdscr ,y, x, color);
}

void add_blockcolor() {
    init_pair(COLOR_WHITE_BLOCK, COLOR_WHITE, COLOR_WHITE);
    init_pair(COLOR_BLACK_BLOCK, COLOR_BLACK, COLOR_BLACK);
    init_pair(COLOR_BLUE_BLOCK, COLOR_BLUE, COLOR_BLUE);
    init_pair(COLOR_RED_BLOCK, COLOR_RED, COLOR_RED);
}


void setFilledRow(int **isblock,int *isFilledRow, int y, int x) {
   int flag;
   int i, j;
   for (i=y-1; i>=0; i--) {
      flag = 1;
      for (j=x-1; j>=0; j--) {
         if(!isblock[i][j])
            flag = 0;
      }
      if (flag) {
         isFilledRow[j] = 1;
      }
   }

}

void swap(int *a, int *b) {
   int tmp = *a;
   *a = *b;
   *b = tmp;
}

void wsquire(WINDOW *win, int y, int x, short color) {
   wmove(win, y, x);
   waddch(win, ' ' |COLOR_PAIR(color));
   wmove(win, y, x+1);
   waddch(win,  ' ' | COLOR_PAIR(color));
}

#include <ncurses.h>
#include <unistd.h>
#include <sys/time.h>


#define GAME_BASEX 50
#define GAME_BASEY 0

#define GAMEX_LENGTH 12
#define GAMEY_LENGTH 22

void squire(int y, int x, int color);
void draw_wall();
int getgmx(int squire_x);
int getgmy(int squire_y);
int isRowFull(int y,int isblock[][GAMEY_LENGTH]);
void color_row(int y, int color);
void initblocks(int xlength, int ylength);


struct timeval start, end;
double d_time = 0;

int tetris() {
   int sq_x, sq_y;
   int ch;
   int isblock[GAMEX_LENGTH][GAMEY_LENGTH];//ブロックの有無
   int isremovable[GAMEY_LENGTH];

   for (int i=0; i<GAMEY_LENGTH; i++) {
      for (int j=0; j<GAMEY_LENGTH; j++) {
         if (i==0 || i==GAMEX_LENGTH-1 || j== GAMEY_LENGTH-1)
            isblock[i][j] = 1;
         else isblock[i][j] = 0;
      }
   }

   for (int i=0; i<GAMEY_LENGTH; i++)
      isremovable[i] = 0;


   init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLUE);
   init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_YELLOW);
   init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_WHITE);
   init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
   init_pair(COLOR_RED, COLOR_RED, COLOR_RED);
   draw_wall();
   refresh();

   curs_set(0);

   timeout(900);
   noecho();

   while (1) {
      sq_x = 5;
      sq_y = 1;
      gettimeofday(&start, NULL);
      d_time=0;
      
      
      while(!isblock[sq_x][sq_y+1]) {

         gettimeofday(&end, NULL);
         d_time =  (double)(end.tv_sec - start.tv_sec) + (double) (end.tv_usec -start.tv_usec) / (1000* 1000);
         if (d_time >= 0.6) {
            gettimeofday(&start, NULL);
            sq_y++;
            d_time = 0;
         }
         
         squire(getgmx(sq_x),getgmy(sq_y), COLOR_YELLOW);
         refresh();
         ch = getch();
         squire(getgmx(sq_x),getgmy(sq_y), COLOR_BLACK);

         if(ch =='h' && !isblock[sq_x-1][sq_y]) sq_x--;
         if(ch =='l' && !isblock[sq_x+1][sq_y]) sq_x++;
         if(ch == '\n' && !isblock[sq_x][sq_y+1]) sq_y++;

      }
      isblock[sq_x][sq_y]=1;
      squire(getgmx(sq_x),getgmy(sq_y), COLOR_BLUE);

      for (int i=GAMEY_LENGTH-2; i>=1; i--) {
         if(isRowFull(i, isblock)){
            isremovable[i] = 1;
            color_row(i, COLOR_RED);
         }
      }
      refresh();
   }


   return 0;
}


int getgmx(int squire_x) {
   return GAME_BASEX + 2*squire_x;
}
int getgmy(int squire_y) {
   return GAME_BASEY + squire_y;
}

void draw_wall() {
   move(GAME_BASEY, GAME_BASEX);
   hline(ACS_HLINE, 11);
   vline(ACS_VLINE, 21);
   move (GAME_BASEY+21, GAME_BASEX);
   hline(ACS_HLINE, 11);
   move (GAME_BASEY, GAME_BASEX+22);
   vline(ACS_VLINE, 21);

}

void squire(int x, int y, int color ) {
   mvaddch(y, x,' ' | COLOR_PAIR(color));
   mvaddch(y, x+1,' ' | COLOR_PAIR(color));
}

int isRowFull(int y, int isblock[][22]) {
   for (int i=1; i<11; i++) {
      if(!isblock[i][y]) return 0;
   }
   return 1;
}

void color_row(int y, int color) {
   for (int i=1; i<11; i++) {
      squire(getgmx(i), getgmy(y), color);
   }
}

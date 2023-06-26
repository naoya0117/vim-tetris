#include <ncurses.h>
#include <unistd.h>
#include <sys/time.h>
#include "command.h"
#include "block.h"

#define GAME_XLENGTH 12
#define GAME_YLENGTH 22
#define MAX_COMMAND_LENGTH 32



void call_game();
void draw_gameScreen(int base_y, int base_x);
int tetris(int base_y, int base_x);
int isGameOver();
void init_blockData(int isblock[GAME_XLENGTH][GAME_YLENGTH]);



void call_tetris() {
    int max_x, max_y;
    int gameScreen_x, gameScreen_y;

    add_blockcolor();

    getmaxyx(stdscr, max_y, max_x);
    
    gameScreen_x = max_x /2 - GAME_XLENGTH;
    gameScreen_y = max_y /10;

    draw_gameScreen(gameScreen_y, gameScreen_x);
    tetris(gameScreen_y, gameScreen_x);

};

void draw_gameScreen(int basey, int basex) {
   move(basey, basex);
   hline(ACS_HLINE, GAME_XLENGTH - 1);
   vline(ACS_VLINE, GAME_YLENGTH - 1);
   move (basey+GAME_YLENGTH -1 , basex);
   hline(ACS_HLINE, GAME_XLENGTH);
   move (basey, basex+(GAME_XLENGTH -1 )* 2);
   vline(ACS_VLINE, GAME_YLENGTH -1 );
   refresh();

}

int tetris(int base_y, int base_x) {
    struct timeval start, end;
    double d_time = 0;
    gettimeofday(&start, NULL);
    char cmd_buffer[MAX_COMMAND_LENGTH];

    BLOCK focusedBlock;
    int rotate;
    int isBlock[GAME_XLENGTH][GAME_YLENGTH];

    int ch;
    int block_x, block_y;
    timeout(600);

    rotate = 0;
    init_blockData(isBlock);

    while(!isGameOver()) {
        block_x= GAME_XLENGTH/2, block_y = 3;
        generateBlock(&focusedBlock, base_y +3, base_x +GAME_XLENGTH-2 , 0);
        while (!isBlock[block_x][block_y+1]) {
            gettimeofday(&end, NULL);
            d_time =  (double)(end.tv_sec - start.tv_sec) + (double) (end.tv_usec -start.tv_usec) / (1000* 1000);
            if (d_time >= 0.6) {
                gettimeofday(&start, NULL);
                mvBlock(&focusedBlock,1, 0 );
                block_y++;
                d_time = 0;
            }


            refresh();
            ch = getch();

            if(ch == ':') {
                if (!call_command(0, cmd_buffer, MAX_COMMAND_LENGTH)) {
                    ;
                }
            }
            else if (ch == ' ') rotateBlock(&focusedBlock, rotate++);
        }
        colorBlock(&focusedBlock, COLOR_WHITE_BLOCK);
    }

    return 0;
}

int isGameOver() {
    return 0;
}

void init_blockData(int isblock[GAME_XLENGTH][GAME_YLENGTH]) {
    for (int x=0; x<GAME_XLENGTH; x++) {
        for (int y=0; y<GAME_YLENGTH; y++) {
            if(x == 0 || x == GAME_XLENGTH -1 || y==GAME_YLENGTH-1)
                isblock[x][y] = 1;
            else
                isblock[x][y] = 0;
        }
    }
}


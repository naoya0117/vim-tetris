#include <ncurses.h>
#include <unistd.h>
#include <sys/time.h>
#include "command.h"
#include "block.h"

#define GAME_XLENGTH 12
#define GAME_YLENGTH 22
#define MAX_COMMAND_LENGTH 32

struct screen {
    int x;
    int y;
} typedef SCREEN;



void call_game();
void draw_gameScreen(SCREEN base);
int tetris(SCREEN base);
int isGameOver();
void init_blockData(int isblock[GAME_XLENGTH][GAME_YLENGTH]);


int get_scrx(int x, SCREEN base);
int get_scry (int y, SCREEN base);


int checkIsBlock(int isblock[GAME_XLENGTH][GAME_YLENGTH], BLOCK block, int dy, int dx, SCREEN gameScreen);
void call_tetris() {
    int max_x, max_y;
    SCREEN gameScreen;

    add_blockcolor();

    getmaxyx(stdscr, max_y, max_x);
    
    gameScreen.x = max_x /2 - GAME_XLENGTH;
    gameScreen.y = max_y /10;

    draw_gameScreen(gameScreen);
    tetris(gameScreen);

};

void draw_gameScreen(SCREEN base) {
   move(base.y, base.x);
   hline(ACS_HLINE, GAME_XLENGTH - 1);
   vline(ACS_VLINE, GAME_YLENGTH - 1);
   move (base.y+GAME_YLENGTH -1 , base.x);
   hline(ACS_HLINE, GAME_XLENGTH);
   move (base.y, base.x+(GAME_XLENGTH -1 )* 2);
   vline(ACS_VLINE, GAME_YLENGTH -1 );
   refresh();

}

int tetris(SCREEN base) {
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
        generateBlock(&focusedBlock, base.y +3, base.x +GAME_XLENGTH-2 , 0);

        while (!checkIsBlock(isBlock, focusedBlock, 1, 0, base)) {

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

int checkIsBlock(int isblock[GAME_XLENGTH][GAME_YLENGTH], BLOCK block, int dy, int dx, SCREEN gameScreen) {
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

    return isblock[x1][y1] || isblock[x2][y2]|| isblock[x3][y3]|| isblock[x4][y4];
}


int get_scrx(int x, SCREEN base) {
    return (x - base.x)/SQUIRE_XLENGTH;
}
int get_scry(int y, SCREEN base) {
    return (y - base.y)/SQUIRE_YLENGTH;
}

#include <ncurses.h>
#include <unistd.h>
#include "tetris.h"
#include "ranking.h"
#include "command.h"
#include "cursor.h"

#define MAX_COMMAND_LENGTH 32
#define NUM_OF_MENU 2
#define COLUMN_SPACING 2

char cmd_buffer[MAX_COMMAND_LENGTH];

void draw_menu(int y, int x);

void menu() {

    int ch;
    CURSOR cursor;
    int max_x, max_y;
    int x, y;
    int current_index = 0;

    curs_set(0);
    timeout(-1);

    getmaxyx(stdscr, max_y, max_x);

    y = max_y / 2;
    x = max_x / 2 - 10;

    draw_menu(y, x);
    setCursor(&cursor, y, x -1 );
    refresh();

    while(1) {
       ch = getch();

       if (ch == 'i') {
        clear ();
        refresh();

        if (current_index==0) {
           call_tetris();
        } else if (current_index==1) 
           ranking();

        break;
       } 

       if (ch == ':') {
          if(!call_command(1, cmd_buffer, MAX_COMMAND_LENGTH))
            show_message("エラー:コマンドが見つかりませんでした");
       } else if (ch == 'k' && current_index > 0 ) {
        mvcursor(&cursor, COLUMN_SPACING * (-1));
        current_index--;
       } else if (ch == 'j' && current_index <  NUM_OF_MENU - 1){
        mvcursor(&cursor, COLUMN_SPACING);
        current_index++;
       }
    
    }

}
void draw_menu(int base_y, int base_x) {
   mvprintw(base_y , base_x, "ゲームを遊ぶ");
   mvprintw(base_y + COLUMN_SPACING, base_x, "ランキング");

   mvprintw(base_y + COLUMN_SPACING * 3, base_x - 10, "※ j,kで上下移動、iで開始");
   mvprintw(base_y + COLUMN_SPACING * 4, base_x - 10, "※ ヘルプを参照するには「:h」と入力してください");
   mvprintw(base_y + COLUMN_SPACING * 5, base_x - 10, "※ 入力したコマンドは左下に表示されます 確認してみましょう");

}

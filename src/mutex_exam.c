#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ncurses.h>

void command();
pthread_mutex_t mutex; // mutexオブジェクト

void *command_thread(void *arg) {
  // コマンドを実行する
  command();

  // mutexを解放する
  pthread_mutex_unlock(&mutex);

  pthread_exit(NULL);
}

int main() {
  int ch;
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE); // カーソルキー等の入力を有効化
  curs_set(0); // カーソル非表示

  pthread_mutex_init(&mutex, NULL); // mutexオブジェクトの初期化

  while (1) {
    ch = getch(); // キーボード入力を取得
    if (ch == ':') {
      pthread_mutex_lock(&mutex); // 現在のスレッドのmutexを取得
      
      // 新しいスレッドの作成
      pthread_t tid;
      pthread_attr_t attr;
      pthread_attr_init(&attr);
      pthread_create(&tid, &attr, command_thread, NULL);
      pthread_detach(tid);
      
    } else {
      // mutexがUnlockされる（コマンドが実行中でない）場合は、キー入力を処理する
      if(pthread_mutex_trylock(&mutex) == 0) {
        printw("Key pressed: %c\n", ch);
        refresh();
        pthread_mutex_unlock(&mutex); // mutexを解放する
      }
    }
  }

  pthread_mutex_destroy(&mutex); // mutexオブジェクトの破棄
  endwin();

  return 0;
}
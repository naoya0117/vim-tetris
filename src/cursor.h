struct SCREENCURSOR {
   int x;
   int y;
}typedef CURSOR;

void setCursor(CURSOR *cursor, int x, int y);
void mvcursor(CURSOR *cursor, int dy);

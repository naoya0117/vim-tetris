
#define COLOR_BLACK_BLOCK 8
#define COLOR_RED_BLOCK 9
#define COLOR_BLUE_BLOCK 10
#define COLOR_WHITE_BLOCK 11

struct block {
	int x1;
	int y1;
	int x2;
	int y2;
	int x3;
	int y3;
	int x4;
	int y4;
}typedef BLOCK;

void add_blockcolor();
void generateBlock(BLOCK *Block, int y, int x, int kind);
void mvBlock(BLOCK *Block, int dy, int dx);

void rotateBlock(BLOCK *Block, int rotate);
void colorBlock(BLOCK *Block, short color);

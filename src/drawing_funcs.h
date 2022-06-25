#ifndef DRAWING_FUNCS_H

#define DRAWING_FUNCS_H

#define SPRITE_CHAR_W 8
#define SPRITE_CHAR_H 8
#define SPRITE_ROW_0_F 0x68
#define SPRITE_ROW_G_V 0x70
#define SPRITE_ROW_W_Z 0x78
#define SPRITE_CHAR_BLANK_X 0x70
#define SPRITE_CHAR_BLANK_Y 0x78

#define SPRITE_FLIP_X 1
#define SPRITE_FLIP_Y 2

#define BG_COLOR 16
#define WINDOW_COLOR 0

#define SET_RECT(xm,ym,wm,hm,gxm,gym,cm,bm) rect.x = xm;\
    rect.y = ym; \
    rect.w = wm; \
    rect.h = hm; \
    rect.gx = gxm; \
    rect.gy = gym; \
    rect.c = cm; \
    rect.b = bm; \

typedef struct Frame {
    char x, y, w, h, gx, gy, c, b;
} Frame;

void load_spritesheet(char* spriteData, char bank);
void QueuePackedSprite(Frame *sprite_table, char x, char y, char frame, char flip, char bank, char offset);
void QueueSpriteRect();
void QueueFillRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char c);
void NextQueue();
void FlushQueue();


void CLB(char c);
void CLS(char c);

void FillRect(char x, char y, char w, char h, char c);

void SpriteRect(char x, char y, char w, char h, char gx, char gy);

void printnum(int num);

void print(char* str);

extern char cursorX, cursorY;
extern unsigned char queue_start, queue_end, queue_pending, queue_count;
extern unsigned char queue_flags_param; //defined in draw_util.s

extern Frame rect; //Defined in draw_util.s

#endif
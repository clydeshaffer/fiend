#ifndef DRAWING_FUNCS_H

#define DRAWING_FUNCS_H

#define SPRITE_CHAR_W 8
#define SPRITE_CHAR_H 8
#define SPRITE_ROW_0_F 0x60
#define SPRITE_ROW_G_V 0x68
#define SPRITE_ROW_W_Z 0x70
#define SPRITE_CHAR_BLANK_X 0x70
#define SPRITE_CHAR_BLANK_Y 0x70

#define SPRITE_FLIP_X 1
#define SPRITE_FLIP_Y 2

#define BG_COLOR 16
#define WINDOW_COLOR 0

typedef struct {
    char ovx, ovy, w, h, gx, gy, pad1, pad2;
} SpriteMetadata;

void load_spritesheet();
void QueuePackedSprite(SpriteMetadata *sprite_table, char x, char y, char frame, char flip, char flags);
void QueueSpriteRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char gx, unsigned char gy, unsigned char flags);
void QueueFillRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char c, unsigned char flags);
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

#endif
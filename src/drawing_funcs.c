/*
 * GameTank-specific implementation of drawing functions
 */
#include <zlib.h>
#include "drawing_funcs.h"
#include "gametank.h"

char cursorX, cursorY;

extern const unsigned char* GameSprites;
extern const unsigned char* HeroSprites;
extern const unsigned char* EnemySprites;

extern void wait();
extern void nop5();
extern void nop10();

void load_spritesheet() {
    flagsMirror = DMA_NMI | DMA_IRQ;
    *dma_flags = flagsMirror;
    banksMirror = bankflip | GRAM_PAGE(0);
    *bank_reg = banksMirror;
    inflatemem(vram, &GameSprites);
    inflatemem(vram + (ENEMY_SPRITES_OFFSET*128), &EnemySprites);
    flagsMirror = DMA_NMI | DMA_IRQ;
    *dma_flags = flagsMirror;
    banksMirror = bankflip | GRAM_PAGE(1);
    *bank_reg = banksMirror;
    inflatemem(vram, &HeroSprites);
    banksMirror = bankflip;
    *bank_reg = banksMirror;
}

unsigned char queue_start = 0;
unsigned char queue_end = 0;
unsigned char queue_count = 0;
unsigned char queue_pending = 0;
#define QUEUE_MAX 250
Frame temp_frame;
Frame rect;
unsigned char queue_flags_param;

void pushRect();

void QueuePackedSprite(Frame* sprite_table, char x, char y, char frame, char flip, char bank, char offset) {
    while(queue_count >= QUEUE_MAX) {
        asm("CLI");
        wait();
    }
    asm("SEI");
    temp_frame = sprite_table[frame];
    rect.b = bank;

    if(flip & SPRITE_FLIP_X) {
        rect.x = (x - temp_frame.w - temp_frame.x - 1);
    } else {
        rect.x = (temp_frame.x + x);
    }
    
    if(rect.x & 128) {
        temp_frame.w -= 0 - rect.x;
        temp_frame.gx += 0 - rect.x;
        rect.x = 0;
    }

    rect.y = (temp_frame.y + y);
    if(rect.y & 128) {
        temp_frame.h -= 0 - rect.y;
        temp_frame.gy += 0 - rect.y;
        rect.y = 0;
    }
    

    rect.gx = temp_frame.gx;
    if(flip & SPRITE_FLIP_X) {
        rect.gx ^= 0xFF;
        rect.gx -= temp_frame.w - 1;
    }
    rect.gy = temp_frame.gy + offset;
    rect.w = temp_frame.w | (flip & SPRITE_FLIP_X ? 128 : 0);
    rect.h = temp_frame.h;

    pushRect();

    if(queue_pending == 0) {
        queue_pending = 1;
        NextQueue();
    }
    asm("CLI");
}

void QueueSpriteRect() {
    if(rect.x > 127) {
        return;
    }
    if(rect.y > 127) {
        return;
    }
    if(rect.w == 0) {
        return;
    }
    if(rect.h == 0) {
        return;
    }
    if(queue_count >= QUEUE_MAX) {
        asm("CLI");
        wait();
    }

    if(rect.x + rect.w >= 128) {
        rect.w = 128 - rect.x;
    }
    if(rect.y + rect.h >= 128) {
        rect.h = 128 - rect.y;
    }

    asm("SEI");
    pushRect();

    if(queue_pending == 0) {
        queue_pending = 1;
        NextQueue();
    }
    asm("CLI");
}

void QueueFillRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char c) {
    if(x > 127) {
        return;
    }
    if(y > 127) {
        return;
    }
    if(w == 0) {
        return;
    }
    if(h == 0) {
        return;
    }
    while(queue_count >= QUEUE_MAX) {
        asm("CLI");
        wait();
    }
    if(x + w >= 128) {
        w = 128 - x;
    }
    if(y + h >= 128) {
        h = 128 - y;
    }
    asm("SEI");

    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    rect.gx = 0;
    rect.gy = 0;
    rect.b = bankflip;
    rect.c = ~c;
    queue_flags_param = DMA_COLORFILL_ENABLE | DMA_OPAQUE;
    pushRect();
    

    if(queue_pending == 0) {
        queue_pending = 1;
        NextQueue();
    }
    asm("CLI");
}

void NextQueue() {
    asm("BRK");
}

void FlushQueue() {
    asm ("SEI");
    if(queue_pending != 0) {
        wait();
    }
    while(queue_end != queue_start) {
        NextQueue();
        wait();
    }
    vram[START] = 0;
    queue_pending = 0;
    asm ("CLI");
}

void CLB(char c) {
    QueueFillRect(0, 0, SCREEN_WIDTH-1, 7, c);
    QueueFillRect(0, 7, 1, SCREEN_HEIGHT-7, c);
    QueueFillRect(1, SCREEN_HEIGHT-8, SCREEN_WIDTH-1, 8, c);
    QueueFillRect(SCREEN_WIDTH-1, 0, 1, SCREEN_HEIGHT-8, c);
}

void CLS(char c) {
    *dma_flags = flagsMirror | DMA_COLORFILL_ENABLE | DMA_OPAQUE;
    queue_pending = 1;
    vram[VX] = 1;
    vram[VY] = 7;
    vram[GX] = 0;
    vram[GY] = 0;
    vram[WIDTH] = SCREEN_WIDTH-2;
    vram[HEIGHT] = SCREEN_HEIGHT-7-8;
    vram[COLOR] = ~c;
    vram[START] = 1;
    *dma_flags = flagsMirror;
}

void FillRect(char x, char y, char w, char h, char c) {
    *dma_flags = flagsMirror | DMA_COLORFILL_ENABLE | DMA_OPAQUE;
    vram[VX] = x;
    vram[VY] = y;
    vram[GX] = 0;
    vram[GY] = 0;
    vram[WIDTH] = w;
    vram[HEIGHT] = h;
    vram[COLOR] = ~c;
    vram[START] = 1;
    *dma_flags = flagsMirror;
}

void SpriteRect(char x, char y, char w, char h, char gx, char gy) {
    if(x + w >= 128) {
        w = 128 - x;
    }
    if(y + h >= 128) {
        h = 128 - y;
    }
    vram[VX] = x;
    vram[VY] = y;
    vram[GX] = gx;
    vram[GY] = gy;
    vram[WIDTH] = w;
    vram[HEIGHT] = h;
    vram[START] = 1;
}

void printnum(int num) {
    vram[VX] = cursorX;
    vram[VY] = cursorY;
    vram[GY] = SPRITE_ROW_0_F;
    vram[WIDTH] = SPRITE_CHAR_W;
    vram[HEIGHT] = SPRITE_CHAR_H;
    if(num == 0) {
        vram[GX] = 0;
        vram[START] = 1;
        wait();
    } else {
        while(num != 0) {
            vram[GX] = (num % 10) << 3;
            vram[START] = 1;
            wait();
            cursorX -= 8;
            num = num / 10;
            vram[VX] = cursorX;
        }
    }
}

void print(char* str) {
    flagsMirror &= ~DMA_COLORFILL_ENABLE;
    *dma_flags = flagsMirror;
    vram[WIDTH] = SPRITE_CHAR_W;
    vram[HEIGHT] = SPRITE_CHAR_H;
    while(*str != 0) {
        if(*str >= '0' && *str <= '9') {
            vram[GX] = (*str - '0') << 3;
            vram[GY] = SPRITE_ROW_0_F;
        } else if(*str >= 'a' && *str <= 'f') {
            vram[GX] = ((*str - 'a') << 3) + 0x50;
            vram[GY] = SPRITE_ROW_0_F;
        } else if(*str >= 'g' && *str <= 'v') {
            vram[GX] = (*str - 'g') << 3;
            vram[GY] = SPRITE_ROW_G_V;
        } else if(*str >= 'w' && *str <= 'z') {
            vram[GX] = (*str - 'w') << 3;
            vram[GY] = SPRITE_ROW_W_Z;
        } else {
            vram[GX] = SPRITE_CHAR_BLANK_X;
            vram[GY] = SPRITE_CHAR_BLANK_Y;
        }
        if(*str == '\n') {
            cursorX = 0;
            cursorY += 8;
        } else {
            vram[VX] = cursorX;
            vram[VY] = cursorY;
            vram[START] = 1;
            wait();
            cursorX += 8;
        }
        str++;
        if(cursorX >= 128) {
            cursorX = 0;
            cursorY += 8;
        }
        if(cursorY >= 128) {
            cursorX = 0;
            cursorY = 0;
        }
    }
}
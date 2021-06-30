/*
 * GameTank-specific implementation of drawing functions
 */
#include <zlib.h>
#include "drawing_funcs.h"
#include "gametank.h"

char cursorX, cursorY;

extern const unsigned char* GameSprites;

extern void wait();
extern void nop5();
extern void nop10();

void load_spritesheet() {
    flagsMirror = DMA_NMI | DMA_IRQ;
    *dma_flags = flagsMirror;
    inflatemem(vram, &GameSprites);
}

unsigned char draw_queue[256];
unsigned char queue_start = 0;
unsigned char queue_end = 0;
unsigned char queue_count = 0;
unsigned char queue_pending = 0;
#define QUEUE_MAX 32

void QueueSpriteRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char gx, unsigned char gy) {
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
    if(queue_count >= QUEUE_MAX) {
        return;
    }

    if(x + w >= 128) {
        w = 128 - x;
    }
    if(y + h >= 128) {
        h = 128 - y;
    }
    asm("SEI");
    draw_queue[queue_end++] = x | 128;
    draw_queue[queue_end++] = y;
    draw_queue[queue_end++] = gx;
    draw_queue[queue_end++] = gy;
    draw_queue[queue_end++] = w;
    draw_queue[queue_end++] = h;
    draw_queue[queue_end++] = 0;
    queue_count++;

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
    if(queue_count >= QUEUE_MAX) {
        return;
    }
    if(x + w >= 128) {
        w = 128 - x;
    }
    if(y + h >= 128) {
        h = 128 - y;
    }
    asm("SEI");
    draw_queue[queue_end++] = x | 128;
    draw_queue[queue_end++] = y;
    draw_queue[queue_end++] = DMA_GX_SOLIDCOLOR_FLAG;
    draw_queue[queue_end++] = 0;
    draw_queue[queue_end++] = w;
    draw_queue[queue_end++] = h;
    draw_queue[queue_end++] = ~c;
    queue_count++;
    

    if(queue_pending == 0) {
        queue_pending = 1;
        NextQueue();
    }
    asm("CLI");
}

void NextQueue() {
    vram[VX] = draw_queue[queue_start++];
    vram[VY] = draw_queue[queue_start++];
    vram[GX] = draw_queue[queue_start++];
    vram[GY] = draw_queue[queue_start++];
    vram[WIDTH] = draw_queue[queue_start++];
    vram[HEIGHT] = draw_queue[queue_start++];
    vram[COLOR] = draw_queue[queue_start++];
    vram[START] = 1;
    queue_count--;
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
    queue_pending = 1;
    vram[VX] = 1;
    vram[VY] = 7;
    vram[GX] = DMA_GX_SOLIDCOLOR_FLAG;
    vram[GY] = 0;
    vram[WIDTH] = SCREEN_WIDTH-2;
    vram[HEIGHT] = SCREEN_HEIGHT-7-8;
    vram[COLOR] = ~c;
    vram[START] = 1;
}

void FillRect(char x, char y, char w, char h, char c) {
    vram[VX] = x;
    vram[VY] = y;
    vram[GX] = DMA_GX_SOLIDCOLOR_FLAG;
    vram[GY] = 0;
    vram[WIDTH] = w;
    vram[HEIGHT] = h;
    vram[COLOR] = ~c;
    vram[START] = 1;
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
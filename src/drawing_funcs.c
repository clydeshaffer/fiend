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
    inflatemem(vram, &GameSprites);
    inflatemem(vram + (ENEMY_SPRITES_OFFSET*128), &EnemySprites);
    flagsMirror = DMA_NMI | DMA_IRQ | DMA_GRAM_PAGE;
    *dma_flags = flagsMirror;
    inflatemem(vram, &HeroSprites);
}

unsigned char draw_queue[256];
unsigned char queue_start = 0;
unsigned char queue_end = 0;
unsigned char queue_count = 0;
unsigned char queue_pending = 0;
#define QUEUE_MAX 32
Frame temp_frame;
Frame rect;

void pushRect();

void QueuePackedSprite(Frame* sprite_table, char x, char y, char frame, char flip, char flags, char offset) {
    while(queue_count >= QUEUE_MAX) {
        asm("CLI");
        wait();
    }

    asm("SEI");
    temp_frame = sprite_table[frame];
    draw_queue[queue_end++] = flags;

    if(flip & SPRITE_FLIP_X) {
        draw_queue[queue_end] = (x - temp_frame.w - temp_frame.x - 1);
    } else {
        draw_queue[queue_end] = (temp_frame.x + x);
    }
    if(draw_queue[queue_end] & 128) {
        temp_frame.w -= 0 - draw_queue[queue_end];
        temp_frame.gx += 0 - draw_queue[queue_end];
        draw_queue[queue_end] = 0;
    }

    draw_queue[queue_end] |= 128;
    ++queue_end;

    draw_queue[queue_end] = (temp_frame.y + y);
    if(draw_queue[queue_end] & 128) {
        temp_frame.h -= 0 - draw_queue[queue_end];
        temp_frame.gy += 0 - draw_queue[queue_end];
        draw_queue[queue_end] = 0;
    }
    draw_queue[queue_end] |= 128;
    ++queue_end;

    draw_queue[queue_end] = temp_frame.gx;
    if(flip & SPRITE_FLIP_X) {
        draw_queue[queue_end] ^= 0x7F;
        draw_queue[queue_end] -= temp_frame.w - 1;
    }
    queue_end++;

    draw_queue[queue_end++] = temp_frame.gy + offset;
    draw_queue[queue_end++] = temp_frame.w | (flip & SPRITE_FLIP_X ? 128 : 0);
    draw_queue[queue_end++] = temp_frame.h;
    draw_queue[queue_end++] = 0;
    queue_count++;

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

void QueueFillRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char c, unsigned char flags) {
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
    draw_queue[queue_end++] = flags;
    draw_queue[queue_end++] = x | 128;
    draw_queue[queue_end++] = y | 128;
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
    flagsMirror = draw_queue[queue_start++] | DMA_NMI | DMA_ENABLE | DMA_IRQ | frameflip;
    *dma_flags = flagsMirror;
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
    QueueFillRect(0, 0, SCREEN_WIDTH-1, 7, c, DMA_OPAQUE);
    QueueFillRect(0, 7, 1, SCREEN_HEIGHT-7, c, DMA_OPAQUE);
    QueueFillRect(1, SCREEN_HEIGHT-8, SCREEN_WIDTH-1, 8, c, DMA_OPAQUE);
    QueueFillRect(SCREEN_WIDTH-1, 0, 1, SCREEN_HEIGHT-8, c, DMA_OPAQUE);
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
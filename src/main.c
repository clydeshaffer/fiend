#include <zlib.h>
#include "gametank.h"
#include "dynawave.h"
#include "drawing_funcs.h"

int inputs = 0, last_inputs = 0;
int inputs2 = 0, last_inputs2 = 0;

extern void wait();
extern void nop5();

void IRQHandler();

#pragma optimize(off)
void updateInputs(){
    char inputsA, inputsB;
    inputsA = *gamepad_2;
    inputsA = *gamepad_1;
    inputsB = *gamepad_1;

    last_inputs = inputs;
    inputs = ~((((int) inputsB) << 8) | inputsA);
    inputs &= INPUT_MASK_ALL_KEYS;

    inputsA = *gamepad_2;
    inputsB = *gamepad_2;
    last_inputs2 = inputs2;
    inputs2 = ~((((int) inputsB) << 8) | inputsA);
    inputs2 &= INPUT_MASK_ALL_KEYS;
}
#pragma optimize(on)

unsigned char camera_x = 0;
unsigned char camera_y = 0;
unsigned char tile_scroll_x = 0;
unsigned char tile_scroll_y = 0;
unsigned char tiles[256];
extern const unsigned char* TestMap;


void draw_world() {
    unsigned char r, c, t;

    r = 0;
    c = 0;
    t = (camera_x + c) + (16 * (camera_y + r));
    if(tiles[t] != 0) {
        QueueSpriteRect(0, 0, 32 - tile_scroll_x, 32 - tile_scroll_y, tile_scroll_x + (tiles[t] << 5), tile_scroll_y);
    }
    for(c = 1; c < 5; c++) {
        if((camera_x + c) < 16) {
            t++;
            if(tiles[t] != 0) {
                QueueSpriteRect((c << 5) - tile_scroll_x, 0, 32, 32 - tile_scroll_y, tiles[t] << 5, tile_scroll_y);
            }
        }
    }

    c = 0;
    r = 1;
    t = (camera_x + c) + ((camera_y + r) << 4);
    for(r = 1; r < 5; r++) {
        if((camera_y + r) < 16) {
            
            if(tiles[t] != 0) {
                QueueSpriteRect(0, (r << 5) - tile_scroll_y, 32 - tile_scroll_x, 32, tile_scroll_x + (tiles[t] << 5), 0);
            }
            t++;
            for(c = 1; c < 5; c++) {
                if((camera_x + c) < 16) {
                    if(tiles[t] != 0) {
                        QueueSpriteRect((c << 5) - tile_scroll_x, (r << 5) - tile_scroll_y, 32, 32, tiles[t] << 5, 0);
                    }
                }
                t++;
            }
        } else { 
            t += 5;
        }
        t += 11;
    }
}

void Sleep(int frames) {
    int i;
    for(i = 0; i < frames; i++) {
        frameflag = 1;
        while(frameflag) {}
    }
}

void main() {
    unsigned char i;
    asm ("SEI");

    init_dynawave();

    load_spritesheet();

    *dma_flags = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_TRANS;
    FillRect(0, SCREEN_HEIGHT-1, SCREEN_WIDTH - 1, 1, 0);
    wait();
    *dma_flags = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_TRANS | DMA_VRAM_PAGE | DMA_PAGE_OUT;
    FillRect(0, SCREEN_HEIGHT-1, SCREEN_WIDTH - 1, 1, 0);
    wait();
    *dma_flags = DMA_NMI | DMA_CPU_TO_VRAM;
    vram[SCREEN_HEIGHT*SCREEN_WIDTH-1] = 0;
    *dma_flags = DMA_NMI | DMA_CPU_TO_VRAM | DMA_VRAM_PAGE;
    vram[SCREEN_HEIGHT*SCREEN_WIDTH-1] = 0;

    flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_TRANS | frameflip;
    *dma_flags = flagsMirror;

    queue_start = 0;
    queue_end = 0;
    queue_pending = 0;
    vram[START] = 0;

    inflatemem(tiles, &TestMap);

    via[DDRB] = 0xFF;

    asm ("CLI");
    while(1){
        flagsMirror &= ~DMA_TRANS;
        *dma_flags = flagsMirror;

        updateInputs();

        if(inputs & INPUT_MASK_RIGHT) {
            tile_scroll_x++;
            if(tile_scroll_x == 32) {
                tile_scroll_x = 0;
                camera_x++;
                if(camera_x == 13) {
                    camera_x = 12;
                    tile_scroll_x = 31;
                }
            }
        } else if(inputs & INPUT_MASK_LEFT) {
            tile_scroll_x--;
            if(tile_scroll_x == 255) {
                tile_scroll_x = 31;
                camera_x--;
                if(camera_x == 255) {
                    camera_x = 0;
                    tile_scroll_x = 0;
                }
            }
        }

        if(inputs & INPUT_MASK_DOWN) {
            tile_scroll_y++;
            if(tile_scroll_y == 32) {
                tile_scroll_y = 0;
                camera_y++;
                if(camera_y == 13) {
                    camera_y = 12;
                    tile_scroll_y = 31;
                }
            }
        } else if(inputs & INPUT_MASK_UP) {
            tile_scroll_y--;
            if(tile_scroll_y == 255) {
                tile_scroll_y = 31;
                camera_y--;
                if(camera_y == 255) {
                    camera_y = 0;
                    tile_scroll_y = 0;
                }
            }
        }

        asm("SEI");
        queue_start = 0;
        queue_end = 0;
        queue_pending = 0;
        queue_count = 0;
        vram[START] = 0;
        asm("CLI");

        QueueFillRect(1, 7, SCREEN_WIDTH-2, SCREEN_HEIGHT-7-8, BG_COLOR);
        
        draw_world();
        CLB(16);
        
        QueueSpriteRect(64 - 8, 64 - 8, 16, 16, 0, 64);
        while(queue_pending != 0) {
            wait();
        }

        Sleep(1);
        frameflip ^= DMA_PAGE_OUT | DMA_VRAM_PAGE;
        flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_TRANS | frameflip;
        *dma_flags = flagsMirror;
    }
}

void IRQHandler() {
    queue_pending = 0;
    if(queue_start != queue_end) {
        queue_pending = 1;
        NextQueue();
        nop5();
    } else {
        vram[START] = 0;
        queue_pending = 0;
    }
}

void NMIHandler() {
    frameflag = 0;
}
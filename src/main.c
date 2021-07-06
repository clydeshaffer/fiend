#include <zlib.h>
#include "gametank.h"
#include "dynawave.h"
#include "drawing_funcs.h"

int inputs = 0, last_inputs = 0;
int inputs2 = 0, last_inputs2 = 0;

extern void wait();
extern void nop5();

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

#define MAP_SIZE 1024
#define MAP_W 32
#define MAP_H 32
#define MAP_ORD 5
#define TILE_SIZE 32
#define TILE_ORD 5
#define VISIBLE_W 5
#define VISIBLE_H 5
#define CAMERA_LIMIT 895 // (TILE_SIZE * (MAP_W - VISIBLE_W + 1)) - 1
#define MAX_ENEMIES 32

int xorshift16(int x) {
    x |= x == 0;   /* if x == 0, set x = 1 instead */
    x ^= (x & 0x07ff) << 5;
    x ^= x >> 7;
    x ^= (x & 0x0003) << 14;
    return x;
}

int rnd_seed = 234;

int rnd() {
    rnd_seed = xorshift16(rnd_seed);
    return rnd_seed;
}

unsigned int camera_x = 0;
unsigned int camera_y = 0;
unsigned char tiles[MAP_SIZE];
extern const unsigned char* TestMap;
extern const Frame* HeroFrames;
extern const Frame* EnemyFrames;
unsigned char walk_cycle[4] = {3, 4, 5, 6};
unsigned int player_x = 48, player_y = 48;
signed char player_dir_x = 0, player_dir_y = 16;
unsigned char player_hitbox_damage = 0;
unsigned char player_anim_frame = 0;

#define PLAYER_STATE_NEUTRAL 0
#define PLAYER_STATE_ATTACK 1

int temp1;
int temp2;
int temp3;
int temp4;
char* tmpptr_char;

typedef struct {
    char anim_frame, anim_dir, anim_flip, mode;
    int x, y;
} MobState;

MobState tempEnemy;
#define ch_tempEnemy ((char*) &tempEnemy)
#define ch_enemy ((char*) enemy)

void LD_tempEnemy(MobState *enemy) {
    ch_tempEnemy[0] = ch_enemy[0];
    ch_tempEnemy[1] = ch_enemy[1];
    ch_tempEnemy[2] = ch_enemy[2];
    ch_tempEnemy[3] = ch_enemy[3];
    ch_tempEnemy[4] = ch_enemy[4];
    ch_tempEnemy[5] = ch_enemy[5];
    ch_tempEnemy[6] = ch_enemy[6];
    ch_tempEnemy[7] = ch_enemy[7];
}

void ST_tempEnemy(MobState *enemy) {
    ch_enemy[0] = ch_tempEnemy[0];
    ch_enemy[1] = ch_tempEnemy[1];
    ch_enemy[2] = ch_tempEnemy[2];
    ch_enemy[3] = ch_tempEnemy[3];
    ch_enemy[4] = ch_tempEnemy[4];
    ch_enemy[5] = ch_tempEnemy[5];
    ch_enemy[6] = ch_tempEnemy[6];
    ch_enemy[7] = ch_tempEnemy[7];
}

MobState enemies[MAX_ENEMIES];

#define HITBOX_X -6
#define HITBOX_Y 4
#define HITBOX_W 10
#define HITBOX_H 3


char tile_passmap[4] = { 0, 1, 0, 1};

char character_tilemap_check(unsigned int pos_x, unsigned int pos_y) {
    pos_x += HITBOX_X;
    pos_y += HITBOX_Y;
    tmpptr_char = tiles + (pos_x >> TILE_ORD) + ((pos_y >> TILE_ORD) << MAP_ORD);
    pos_x &= (TILE_SIZE-1);
    pos_y &= (TILE_SIZE-1);
    if(!tile_passmap[*tmpptr_char]) {
        return 0;
    }
    tmpptr_char++;
    if(pos_x + HITBOX_W >= TILE_SIZE) {
        if(!tile_passmap[*tmpptr_char]) {
            return 0;
        }   
    }
    tmpptr_char += MAP_W - 1;
    if(pos_y + HITBOX_H >= TILE_SIZE) {
        if(!tile_passmap[*tmpptr_char]) {
            return 0;
        }   
    }
    tmpptr_char++;
    if((pos_x + HITBOX_W >= TILE_SIZE) && (pos_y + HITBOX_H >= TILE_SIZE)) {
        if(!tile_passmap[*tmpptr_char]) {
            return 0;
        }   
    }
    return 1;
}

void draw_world() {
    char r, c, tile_scroll_x, tile_scroll_y, cam_x, cam_y;
    int t;
    tile_scroll_x = camera_x & (TILE_SIZE-1);
    tile_scroll_y = camera_y & (TILE_SIZE-1);
    cam_x = camera_x >> TILE_ORD;
    cam_y = camera_y >> TILE_ORD;

    r = 0;
    c = 0;
    t = (cam_x + c) + ((cam_y + r) << MAP_ORD);
    if(tiles[t] != 0) {
        QueueSpriteRect(0, 0, TILE_SIZE - tile_scroll_x, TILE_SIZE - tile_scroll_y, tile_scroll_x + (tiles[t] << TILE_ORD), tile_scroll_y, 0);
    }
    t++;
    for(c = 1; c < VISIBLE_W; ++c) {
        if((cam_x + c) < MAP_W) {
           
            if(tiles[t] != 0) {
                QueueSpriteRect((c << TILE_ORD) - tile_scroll_x, 0, TILE_SIZE, TILE_SIZE - tile_scroll_y, tiles[t] << TILE_ORD, tile_scroll_y, 0);
            }
        }
        t++;
    }
    t += MAP_W - VISIBLE_W;

    c = 0;
    r = 1;
    for(r = 1; r < VISIBLE_H; ++r) {
        if((cam_y + r) < MAP_H) {
            
            if(tiles[t] != 0) {
                
                QueueSpriteRect(0, (r << TILE_ORD) - tile_scroll_y, TILE_SIZE - tile_scroll_x, TILE_SIZE, tile_scroll_x + (tiles[t] << TILE_ORD), 0, 0);
            }
            t++;
            for(c = 1; c < VISIBLE_W; ++c) {
                if((cam_x + c) < MAP_W) {
                    if(tiles[t] != 0) {
                        QueueSpriteRect((c << TILE_ORD) - tile_scroll_x, (r << TILE_ORD) - tile_scroll_y, TILE_SIZE, TILE_SIZE, tiles[t] << TILE_ORD, 0, 0);
                    }
                }
                t++;
            }
        } else { 
            t += VISIBLE_W;
        }
        t += MAP_W - VISIBLE_W;
    }
}

void clear_enemies() {
    char i;
    for(i = 0; i < MAX_ENEMIES; ++i) {
        enemies[i].mode = 0;
        enemies[i].x = 0;
        enemies[i].y = 0;
        enemies[i].anim_flip = 0;
        enemies[i].anim_dir = 0;
        enemies[i].anim_frame = 0; 
    }
}

void draw_enemies() {
    char i;
    MobState *enemy = enemies;
    for(i = 0; i < MAX_ENEMIES; ++i) {
        if(enemy->mode != 0) {
            if(enemy->x > (camera_x-8)
                && enemy->y > camera_y
                && enemy->x < (camera_x + 136)
                && enemy->y < (camera_y + 128)) {
                    QueuePackedSprite(&EnemyFrames, enemy->x - camera_x, enemy->y - camera_y, ((enemy->anim_frame >> 2) & 3) + enemy->anim_dir, enemy->anim_flip, 0, ENEMY_SPRITES_OFFSET);
                } 
        }
        ++enemy;
    }
}

void face_player() {
    temp3 = player_x - tempEnemy.x;
    temp4 = player_y - tempEnemy.y;
    temp1 = temp3;
    temp2 = temp4;
    
    if(temp3 < 0) {
        temp1 = -temp1;
    }
    if(temp4 < 0) {
        temp2 = -temp2;
    }
    if(temp1 > temp2) {
        if(temp3 > 0) {
            tempEnemy.anim_dir = 4;
            tempEnemy.anim_flip = 0;
        } else {
            tempEnemy.anim_dir = 4;
            tempEnemy.anim_flip = SPRITE_FLIP_X;
        }
    } else {
        if(temp4 > 0) {
            tempEnemy.anim_flip = 0;
            tempEnemy.anim_dir = 0;
        } else {
            tempEnemy.anim_flip = 0;
            tempEnemy.anim_dir = 8;
        }
    }
}

void update_enemies() {
    char i;
    MobState *enemy = enemies;
    for(i = 0; i < MAX_ENEMIES; ++i) {
        if(enemy->x > (camera_x-8)
                && enemy->y > camera_y
                && enemy->x < (camera_x + 136)
                && enemy->y < (camera_y + 128)) {
            LD_tempEnemy(enemy);
            if(tempEnemy.mode == 1) {
                temp1 = tempEnemy.x;
                temp2 = tempEnemy.y;
                ++(tempEnemy.anim_frame);
                if((tempEnemy.anim_frame & 3) == 0) {
                    if(tempEnemy.x > player_x) {
                        --(tempEnemy.x);
                    } else {
                        ++(tempEnemy.x);
                    }
                    if(!character_tilemap_check(tempEnemy.x, tempEnemy.y)) {
                        tempEnemy.x = temp1;
                    }
                    if(tempEnemy.y > player_y) {
                        --(tempEnemy.y);
                    } else {
                        ++(tempEnemy.y);
                    }
                    
                    if(!character_tilemap_check(tempEnemy.x, tempEnemy.y)) {
                        tempEnemy.y = temp2;
                    }
                    
                    face_player();
                }

                if(player_hitbox_damage) {
                    temp1 = player_x + player_dir_x - tempEnemy.x;
                    temp2 = player_y + player_dir_y - tempEnemy.y;
                    if(temp1 < 0) {
                        temp1 = -temp1;
                    }
                    if(temp2 < 0) {
                        temp2 = -temp2;
                    }
                    if(temp1 + temp2 < 8) {
                        tempEnemy.mode = 2;
                        tempEnemy.anim_frame = 0;
                    }
                }
            } else if(tempEnemy.mode == 2) {
                ++(tempEnemy.anim_frame);
                tempEnemy.anim_dir = (tempEnemy.anim_dir + 1) & 7;
                if(tempEnemy.anim_frame & 1) {
                    tempEnemy.x += player_dir_x >> 4;
                    tempEnemy.y += player_dir_y >> 4;
                }
                if(tempEnemy.anim_frame == 12) {
                    tempEnemy.mode = 0;
                }
            }
            ST_tempEnemy(enemy);
        }
        ++enemy;
    }
}

void Sleep(int frames) {
    int i;
    for(i = 0; i < frames; ++i) {
        frameflag = 1;
        while(frameflag) {}
    }
}

void main() {
    unsigned char i;
    unsigned char player_anim_dir = 4;
    unsigned char anim_flip = 0;
    unsigned int tx, ty;
    unsigned char player_anim_state = PLAYER_STATE_NEUTRAL;
    asm ("SEI");

    init_dynawave();

    load_spritesheet();

    *dma_flags = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_OPAQUE;
    FillRect(0, SCREEN_HEIGHT-1, SCREEN_WIDTH - 1, 1, 0);
    wait();
    *dma_flags = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_OPAQUE | DMA_VRAM_PAGE | DMA_PAGE_OUT;
    FillRect(0, SCREEN_HEIGHT-1, SCREEN_WIDTH - 1, 1, 0);
    wait();
    *dma_flags = DMA_NMI | DMA_CPU_TO_VRAM;
    vram[SCREEN_HEIGHT*SCREEN_WIDTH-1] = 0;
    *dma_flags = DMA_NMI | DMA_CPU_TO_VRAM | DMA_VRAM_PAGE;
    vram[SCREEN_HEIGHT*SCREEN_WIDTH-1] = 0;

    flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_OPAQUE | frameflip;
    *dma_flags = flagsMirror;

    queue_start = 0;
    queue_end = 0;
    queue_pending = 0;
    vram[START] = 0;

    inflatemem(tiles, &TestMap);

    clear_enemies(); 

    for(i = 0; i < 10; ++i) {
        enemies[i].mode = 1;
        enemies[i].x = (rnd() & 0b1111100000) | 16;
        enemies[i].y = (rnd() & 0b1111100000) | 16;
        if(!character_tilemap_check(enemies[i].x, enemies[i].y)) {
            enemies[i].mode = 0;
        }
    }

    via[DDRB] = 0xFF;

    asm ("CLI");
    while(1){
        updateInputs();

        i = 0;
        tx = player_x;
        ty = player_y;
        player_hitbox_damage = 0;

        if(player_anim_state == PLAYER_STATE_ATTACK) {
            i = (player_anim_frame >> 3) & 3;
            if(i == 1 || i == 2) {
                player_hitbox_damage = 1;
            }
            i = 1;
            if(player_anim_frame == 32) {
                player_anim_state = PLAYER_STATE_NEUTRAL;
            }
        } else {
            if(inputs & INPUT_MASK_A & ~last_inputs) {
                player_anim_frame = 0;
                player_anim_state = PLAYER_STATE_ATTACK;
                i = 1;
            } else {
                if(inputs & INPUT_MASK_RIGHT) {
                    player_anim_dir = 4;
                    anim_flip = 0;
                    i = 1;
                    player_x++;
                    player_dir_x = 16;
                    player_dir_y = 0;
                } else if(inputs & INPUT_MASK_LEFT) {
                    player_anim_dir = 4;
                    anim_flip = SPRITE_FLIP_X;
                    i = 1;
                    player_x--;
                    player_dir_x = -16;
                    player_dir_y = 0;
                }
                if(!character_tilemap_check(player_x, player_y)) {
                    player_x = tx;
                }

                if(inputs & INPUT_MASK_DOWN) {
                    player_anim_dir = 0;
                    anim_flip = 0;
                    i = 1;
                    player_y++;
                    player_dir_x = 0;
                    player_dir_y = 16;
                } else if(inputs & INPUT_MASK_UP) {
                    player_anim_dir = 8;
                    anim_flip = 0;
                    i = 1;
                    player_y--;
                    player_dir_x = 0;
                    player_dir_y = -16;
                }
                if(!character_tilemap_check(player_x, player_y)) {
                    player_y = ty;
                }
            }
        }

        if(i == 1) {
            player_anim_frame++;
        }

        update_enemies();

        asm("SEI");
        queue_start = 0;
        queue_end = 0;
        queue_pending = 0;
        queue_count = 0;
        vram[START] = 0;
        asm("CLI");

        QueueFillRect(1, 7, SCREEN_WIDTH-2, SCREEN_HEIGHT-7-8, BG_COLOR, 0);
        
        draw_world();
        draw_enemies();
        CLB(16);
        
        camera_x = player_x - 64;
        camera_y = player_y - 64;
        if(camera_x & 0x8000) {
            camera_x = 0;
        }
        if(camera_y & 0x8000) {
            camera_y = 0;
        }
        if(camera_x > CAMERA_LIMIT) {
            camera_x = CAMERA_LIMIT;
        }
        if(camera_y > CAMERA_LIMIT) {
            camera_y = CAMERA_LIMIT;
        }

        QueuePackedSprite(&HeroFrames, player_x - camera_x, player_y - camera_y, walk_cycle[(player_anim_frame >> 3) & 3] + player_anim_dir + (player_anim_state == PLAYER_STATE_ATTACK ? 12 : 0), anim_flip, DMA_GRAM_PAGE, 0);
        while(queue_pending != 0) {
            asm("CLI");
            wait();
        }

        Sleep(1);
        frameflip ^= DMA_PAGE_OUT | DMA_VRAM_PAGE;
        flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | frameflip;
        *dma_flags = flagsMirror;
    }
}
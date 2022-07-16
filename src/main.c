#include <zlib.h>
#include "globals.h"
#include "gametank.h"
#include "dynawave.h"
#include "drawing_funcs.h"
#include "tilemap.h"
#include "random.h"
#include "music.h"
#include "enemies.h"
#include "level.h"
#include "banking.h"

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

#define CAMERA_LIMIT 7160 // (TILE_SIZE * (MAP_W - VISIBLE_W + 1)) - 1

#define GAME_STATE_TITLE 0
#define GAME_STATE_PLAY 1
#define GAME_STATE_FADEIN 2
#define GAME_STATE_FADEOUT 3
#define GAME_STATE_PAUSED 4
#define GAME_STATE_ENDSCREEN 5

unsigned char game_state = GAME_STATE_TITLE;
unsigned char game_over_timer = 256;

#define PLAYER_START_X 48
#define PLAYER_START_Y 48

unsigned char go_to_next_level = 0;

void Sleep(int frames) {
    int i;
    for(i = 0; i < frames; ++i) {
        frameflag = 1;
        while(frameflag) {}
    }
}

void init_game_state(unsigned char new_state) {
    unsigned char i, old_state;
    old_state = game_state;
    game_state = new_state;
    stop_music();
    if(new_state == GAME_STATE_TITLE) {
        player_dir_x = 8;
        player_dir_y = 8;
        camera_x.i = 64;
        camera_y.i = 0;
        player_anim_frame = 0;
        player_health = 0;
        init_level(0);
        ChangeRomBank(BANK_COMMON);
        play_track(MUSIC_TRACK_TITLE, REPEAT_NONE);
    } else if(new_state == GAME_STATE_PLAY) {
        player_dir_x = 0;
        player_dir_y = 128;
        player_anim_state = PLAYER_STATE_NEUTRAL;
        player_anim_frame = 0;
        if(player_health != 0) {
            next_level();
            ChangeRomBank(BANK_COMMON);
        }
        player_max_health = INITIAL_MAX_HEALTH;
        player_health = INITIAL_MAX_HEALTH;

        camera_x.i = player_x.i - 512;
        camera_y.i = player_y.i - 512;
        if(old_state == GAME_STATE_FADEOUT) {
            game_state = GAME_STATE_FADEIN;
            if(level_number == 7) {
                game_state = GAME_STATE_ENDSCREEN;
            }
        }
        if(game_state == GAME_STATE_ENDSCREEN)
            play_track(MUSIC_TRACK_END, REPEAT_NONE);
        else 
            play_track(music_for_level(), REPEAT_LOOP);
    }
}

void draw_player() {
    ChangeRomBank(BANK_COMMON);
    queue_flags_param = DMA_GCARRY;
    QueuePackedSprite(&HeroFrames, (player_x.i - camera_x.i)>>3, (player_y.i - camera_y.i)>>3, (3 & (player_anim_frame >> 3)) + player_anim_dir + player_state_offsets[player_anim_state], player_anim_flip, bankflip | GRAM_PAGE(1) | BANK_CLIP_X | BANK_CLIP_Y, 0);
}

void draw_hud() {
    unsigned char i;
    queue_flags_param = DMA_GCARRY;
    for(i = 0; i < player_health; ++i) {
        SET_RECT((i << 3) + 4, 10, 8, 8, 88, 120, 0, bankflip)
        QueueSpriteRect();
    }
    for(;i < player_max_health; ++i) {
        SET_RECT((i<<3)+4, 10, 8,8, 96, 120, 0, bankflip)
        QueueSpriteRect();
    }
}
//Separate from draw_hud because of synchronous draw calls
void print_floor_number() {
    cursorX = 72;
    cursorY = 108;
    if(level_number == 0) {
        print("surface");    
    } else if(level_number == 6) {
        cursorX = 96;
        print("lair");
    } else {
        print("floor");
        cursorX = 116;
        printnum(level_number);
    }
}

void fill_local_map() {
    static unsigned char i, j, k;
    static char *tmpptr_char, *tmpptr_char2;
    *vram_VX = 0;
    *vram_VY = 0;
    *vram_GX = 0;
    *vram_GY = 128;
    *vram_WIDTH = 1;
    *vram_HEIGHT = 1;
    *vram_START = 1;
    *vram_START = 0;
    flagsMirror = DMA_NMI | DMA_IRQ | frameflip;
    banksMirror = bankflip;
    *dma_flags = flagsMirror;
    *bank_reg = banksMirror;
    cursorX = camera_x.b.msb;
    cursorY = camera_y.b.msb;
    tmpptr_char = &vram[(cursorY << 7) + cursorX];
    tmpptr_char2 = &tiles[(cursorY << 5) + cursorX];
    for(i = 0; i < 5; ++i) {
        for(j = 0; j < 5; ++j) {
            k = *tmpptr_char2;
            if(k & GROUND_TILE) {
                *tmpptr_char = (k == STAIRS_TILE) ? 28 : 75;
            }
            ++tmpptr_char;
            ++tmpptr_char2;
        }
        tmpptr_char2 += 27;
        tmpptr_char += 123;
    }
}

void fill_whole_map() {
    unsigned char i, j, k;
    char *tmpptr_char, *tmpptr_char2;
    *vram_VX = 0;
    *vram_VY = 0;
    *vram_GX = 0;
    *vram_GY = 128;
    *vram_WIDTH = 1;
    *vram_HEIGHT = 1;
    *vram_START = 1;
    *vram_START = 0;
    flagsMirror = DMA_NMI | DMA_IRQ | frameflip;
    banksMirror = bankflip;
    *dma_flags = flagsMirror;
    *bank_reg = banksMirror;
    cursorX = camera_x.b.msb;
    cursorY = camera_y.b.msb;
    tmpptr_char = vram;
    tmpptr_char2 = tiles;
    for(i = 0; i < MAP_H; ++i) {
        for(j = 0; j < MAP_W; ++j) {
            k = *tmpptr_char2;
            if(k & GROUND_TILE) {
                *tmpptr_char = (k == STAIRS_TILE) ? 28 : 75;
            }
            ++tmpptr_char;
            ++tmpptr_char2;
        }
        tmpptr_char += 96;
    }
}

void main() {
    unsigned char i, j, k;
    unsigned int tx, ty;
    asm ("SEI");

    flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_OPAQUE | DMA_COLORFILL_ENABLE;
    *dma_flags = flagsMirror;
    FillRect(0, SCREEN_HEIGHT-1, SCREEN_WIDTH - 1, 1, 16);
    wait();
    FillRect(SCREEN_WIDTH-1, 0, 1, SCREEN_HEIGHT-1, 16);
    wait();
    flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_OPAQUE | DMA_PAGE_OUT | DMA_COLORFILL_ENABLE;
    *dma_flags = flagsMirror;
    *bank_reg = BANK_SECOND_FRAMEBUFFER;
    FillRect(0, SCREEN_HEIGHT-1, SCREEN_WIDTH - 1, 1, 16);
    wait();
    FillRect(SCREEN_WIDTH-1, 0, 1, SCREEN_HEIGHT-1, 16);
    wait();
    *dma_flags = DMA_NMI | DMA_CPU_TO_VRAM;
    *bank_reg = 0;
    vram[SCREEN_HEIGHT*SCREEN_WIDTH-1] = 0;
    *dma_flags = DMA_NMI | DMA_CPU_TO_VRAM;
    *bank_reg = BANK_SECOND_FRAMEBUFFER;
    vram[SCREEN_HEIGHT*SCREEN_WIDTH-1] = 0;


    ChangeRomBank(BANK_INIT);
    init_dynawave();
    init_music();

    frameflip = 0;
    flagsMirror = DMA_NMI | DMA_IRQ | frameflip;
    bankflip = BANK_SECOND_FRAMEBUFFER;
    banksMirror = bankflip;

    load_spritesheet(&HudSprites, 0);
    load_spritesheet(&HeroSprites, 1);
    load_spritesheet(&PauseScreen, 8);

    flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_OPAQUE | frameflip;
    *dma_flags = flagsMirror;
    banksMirror = bankflip;
    *bank_reg = banksMirror;

    queue_start = 0;
    queue_end = 0;
    queue_pending = 0;
    vram[START] = 0;

    ChangeRomBank(BANK_COMMON);
    generate_map();

    via[DDRB] = 0xFF;

    asm ("CLI");

    for(game_over_timer = 0; game_over_timer < 116; game_over_timer+=4) {
        banksMirror = 0;
        *bank_reg = banksMirror;
        draw_fade(game_over_timer);
        Sleep(1);
    }

    init_game_state(GAME_STATE_TITLE);
    while(1){
        via[ORB] = 0x80;
        via[ORB] = 0x00;
        asm("SEI");
        queue_start = 0;
        queue_end = 0;
        queue_pending = 0;
        queue_count = 0;
        vram[START] = 0;
        asm("CLI");

        updateInputs();
        if(game_state == GAME_STATE_TITLE) {
            rnd();
            draw_world();
            ++player_anim_frame;
            if(player_anim_frame & 1) {
                camera_x.i += player_dir_x;
                if((camera_x.i + 32 > CAMERA_LIMIT) || (camera_x.i == 0)) {
                    player_dir_x = -player_dir_x;
                }
            }

            camera_y.i += player_dir_y;
            if((camera_y.i + 32 > CAMERA_LIMIT) || (camera_y.i == 0)) {
                player_dir_y = -player_dir_y;
            }
            if(inputs & INPUT_MASK_START) {
                init_game_state(GAME_STATE_PLAY);
            }
            //DRAW TITLE TEXT
            queue_flags_param = DMA_GCARRY;
            SET_RECT(2, 32, 126, 13, 0, 82, 0, bankflip);
            QueueSpriteRect();
            SET_RECT(35, 80, 59, 8, 0, 96, 0, bankflip);
            QueueSpriteRect();
        }
        else if(game_state == GAME_STATE_PLAY) {    
            i = 0;
            tx = player_x.i;
            ty = player_y.i;
            player_hitbox_damage = 0;

            if(player_anim_state == PLAYER_STATE_HITSTUN) {
                i = 1;
                player_x.i -= player_dir_x;
                player_y.i -= player_dir_y;
                if(!character_tilemap_check(player_x, player_y)) {
                    player_x.i += player_dir_x;
                    player_y.i += player_dir_y;
                }
                if(player_anim_frame == 24) {
                    player_anim_state = PLAYER_STATE_NEUTRAL;
                    if(player_health == 0) {
                        play_track(MUSIC_TRACK_DIED, REPEAT_NONE);
                        player_anim_state = PLAYER_STATE_DEAD;
                        player_anim_frame = 0;
                        game_over_timer = 255;
                        i = 0;
                    }
                } else if(player_health == 0) {
                    Sleep(1);
                }
            } else if(player_anim_state == PLAYER_STATE_ATTACK) {
                i = (player_anim_frame >> 3) & 3;
                if(i == 1 || i == 2) {
                    player_hitbox_damage = 1;
                }
                i = 1;
		if(level_number == 0) {
		    if(player_anim_frame == 8)
		    if(tile_at(player_x.i + player_dir_x, player_y.i + player_dir_y) == (GROUND_TILE | 128)) {
		        set_tile(player_x.i + player_dir_x, player_y.i + player_dir_y, STAIRS_TILE | 128);
		    }
		}
                if(player_anim_frame == 32) {
                    player_anim_state = PLAYER_STATE_NEUTRAL;
                }
            } else if(player_anim_state == PLAYER_STATE_NEUTRAL) {
                if(inputs & INPUT_MASK_START & ~last_inputs) {
                    game_state = GAME_STATE_PAUSED;
                }
                else if(inputs & INPUT_MASK_A & ~last_inputs) {
                    player_anim_frame = 0;
                    player_anim_state = PLAYER_STATE_ATTACK;
                    do_noise_effect(95, 12, 4);
                    i = 1;
                } else if(inputs & INPUT_MASK_B & ~last_inputs) {
                    if((tile_at(player_x.i, player_y.i) == STAIRS_TILE) || (inputs & INPUT_MASK_START)) {
                        play_track(MUSIC_TRACK_STAIRS, REPEAT_NONE);
                        game_state = GAME_STATE_FADEOUT;
                        go_to_next_level = 1;
                        game_over_timer = 0;
                    }
                } else {
                    if(inputs & INPUT_MASK_RIGHT) {
                        player_anim_dir = 4;
                        player_anim_flip = 0;
                        i = 1;
                        player_x.i+=8;
                        player_dir_x = 127;
                        player_dir_y = 0;
                    } else if(inputs & INPUT_MASK_LEFT) {
                        player_anim_dir = 4;
                        player_anim_flip = SPRITE_FLIP_X;
                        i = 1;
                        player_x.i-=8;
                        player_dir_x = -127;
                        player_dir_y = 0;
                    }
                    if(!character_tilemap_check(player_x, player_y)) {
                        player_x.i = tx;
                    }

                    if(inputs & INPUT_MASK_DOWN) {
                        player_anim_dir = 0;
                        player_anim_flip = 0;
                        i = 1;
                        player_y.i+=8;
                        player_dir_x = 0;
                        player_dir_y = 127;
                    } else if(inputs & INPUT_MASK_UP) {
                        player_anim_dir = 8;
                        player_anim_flip = 0;
                        i = 1;
                        player_y.i-=8;
                        player_dir_x = 0;
                        player_dir_y = -127;
                    }
                    if(!character_tilemap_check(player_x, player_y)) {
                        player_y.i = ty;
                    }
                }
            }

            if(i == 1) {
                player_anim_frame++;
            }
            
            camera_x.i = player_x.i - 512;
            camera_y.i = player_y.i - 512;
            if(camera_x.i & 0x8000) {
                camera_x.i = 0;
            }
            if(camera_y.i & 0x8000) {
                camera_y.i = 0;
            }
            if(camera_x.i > CAMERA_LIMIT) {
                camera_x.i = CAMERA_LIMIT;
            }
            if(camera_y.i > CAMERA_LIMIT) {
                camera_y.i = CAMERA_LIMIT;
            }

            draw_world();

            via[ORB] = 0x80;
            via[ORB] = 0x01;
            update_enemies();
            via[ORB] = 0x80;
            via[ORB] = 0x41;

            draw_enemies();

            draw_player();
            
            draw_hud();
            if(player_anim_state == PLAYER_STATE_DEAD) {
                if(game_over_timer > 223) {
                    queue_flags_param = 0;
                    SET_RECT(0, 60, 127, 8, ((255 - game_over_timer) << 2), 64, 0, bankflip);
                    QueueSpriteRect();
                } else if (game_over_timer > 196) {
                    queue_flags_param = 0;
                    SET_RECT(0, 60, 127, 8, 112, 64, 0, bankflip);
                    QueueSpriteRect();
                    queue_flags_param = DMA_GCARRY;
                    SET_RECT(40, 60, 47, 8, 64, 96, 0, bankflip);
                    QueueSpriteRect();
                    queue_flags_param = 0;
                    SET_RECT(0, 60, 127, 8, ((game_over_timer & 0x3C) << 2), 64, 0, bankflip);
                    QueueSpriteRect();
                } else {
                    queue_flags_param = 0;
                    SET_RECT(0, 60, 127, 8, 112, 64, 0, bankflip);
                    QueueSpriteRect();
                    queue_flags_param = DMA_GCARRY;
                    SET_RECT(40, 60, 47, 8, 64, 96, 0, bankflip);
                    QueueSpriteRect();
                }
            }
        } else if((game_state == GAME_STATE_FADEOUT) || (game_state == GAME_STATE_FADEIN)) {
            draw_world();
            draw_enemies();
            draw_player();
            draw_hud();
            if(player_anim_state == PLAYER_STATE_DEAD) {
                queue_flags_param = 0;
                SET_RECT(0, 60, 127, 8, 112, 64, 0, bankflip);
                QueueSpriteRect();
                queue_flags_param = DMA_GCARRY;
                SET_RECT(40, 60, 47, 8, 64, 96, 0, bankflip);
                QueueSpriteRect();
            }
        } else if(game_state == GAME_STATE_PAUSED) {
            draw_world();
            draw_enemies();
            draw_hud();
            SET_RECT(28, 16, 74,84, 156, 16, 0, bankflip)
            QueueSpriteRect();
            SET_RECT(48, 48, 32,32, 0, 128, 0, bankflip)
            QueueSpriteRect();
            if(inputs & INPUT_MASK_START & ~last_inputs) {
                game_state = GAME_STATE_PLAY;
            }
            //QueueFillRect(48+(player_x.i>>8), 48+(player_y.i>>8), 1, 1, 124);
            queue_flags_param = DMA_GCARRY;
            SET_RECT(47+(player_x.b.msb), 47+(player_y.b.msb), 3, 3, 104, 120, 0, bankflip);
            QueueSpriteRect();
        }
        
        CLB(16);

        while(queue_pending != 0) {
            asm("CLI");
            wait();
        }

        if(game_state == GAME_STATE_TITLE) {
            if(game_over_timer >= 4) {
                draw_fade(game_over_timer);
                game_over_timer -= 4;
            }
        } else if(game_state == GAME_STATE_FADEOUT) {
            if(game_over_timer < 116) {
                game_over_timer+=4;
            }
            draw_fade(game_over_timer);
            if(game_over_timer == 116) {
                if(player_anim_state == PLAYER_STATE_DEAD) {
                    init_game_state(GAME_STATE_TITLE);
                } else if(go_to_next_level) {
                    go_to_next_level = 0;
                    i = player_health;
                    init_game_state(GAME_STATE_PLAY);
                    player_health = i;
                } else {
                    game_state = GAME_STATE_FADEIN;
                }
            }
        } else if(game_state == GAME_STATE_FADEIN) {
            if(game_over_timer > 0) {
                game_over_timer -= 4;
            } else {
                game_state = GAME_STATE_PLAY;
            }
            draw_fade(game_over_timer);
        } else if(player_anim_state == PLAYER_STATE_DEAD) {
            game_over_timer--;
            if(game_over_timer == 0) {
                game_state = GAME_STATE_FADEOUT;
            }
        } else if(game_state == GAME_STATE_PLAY) {
            flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | frameflip;
            *dma_flags = flagsMirror;
            if(temp_msg_counter > 0) {
                cursorX = 1;
                cursorY = 108;
                print(message_string);
                --temp_msg_counter;
            } else {
                if(tile_at(player_x.i, player_y.i) == STAIRS_TILE) {
                    cursorX = 1;
                    cursorY = 108;
                    print("press b to enter");
                }
            }
            if(do_fill_map) {
                do_fill_map = 0;
                fill_whole_map();
            } else {
                fill_local_map();
            }
        } else if(game_state == GAME_STATE_PAUSED) {
            flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | frameflip;
            *dma_flags = flagsMirror;
            print_floor_number();
        } else if(game_state == GAME_STATE_ENDSCREEN) {
            cursorX = 8;
            cursorY = 60;
            print("fiend is slain");
            cursorX = 64;
            cursorY = 90;
            print("for now");
            if(inputs & ~last_inputs & INPUT_MASK_START) {
                init_game_state(GAME_STATE_TITLE);
            }
        }

        Sleep(1);
        frameflip ^= DMA_PAGE_OUT;
        bankflip ^= BANK_SECOND_FRAMEBUFFER;
        flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | frameflip;
        banksMirror = bankflip;
        *dma_flags = flagsMirror;
        *bank_reg = banksMirror;
        ChangeRomBank(BANK_COMMON);
        tick_music();
        via[ORB] = 0x80;
        via[ORB] = 0x40;
    }
}

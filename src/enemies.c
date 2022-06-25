#include "enemies.h"
#include "gametank.h"
#include "drawing_funcs.h"
#include "globals.h"
#include "tilemap.h"
#include "music.h"

MobState tempEnemy;

MobState enemies[MAX_ENEMIES];

unsigned char enemy_count = 0;

extern const Frame* EnemyFrames;

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
    enemy_count = 0;
}

void draw_enemies() {
    char i;
    MobState *enemy = enemies;
    queue_flags_param = DMA_GCARRY;
    for(i = 0; i < MAX_ENEMIES; ++i) {
        if(enemy->mode != 0) {
            if((enemy->x + 8) > (camera_x)
                && enemy->y > camera_y
                && enemy->x < (camera_x + 136)
                && enemy->y < (camera_y + 128)) {
                    QueuePackedSprite(&EnemyFrames, enemy->x - camera_x, enemy->y - camera_y, ((enemy->anim_frame >> 2) & 3) + enemy->anim_dir, enemy->anim_flip, GRAM_PAGE(0) | bankflip | BANK_CLIP_X | BANK_CLIP_Y, ENEMY_SPRITES_OFFSET);
                } 
        }
        ++enemy;
    }
}

void update_enemies() {
    char i;
    MobState *enemy = enemies;
    for(i = 0; i < MAX_ENEMIES; ++i) {
        if((enemy->x + 8) > (camera_x)
                && enemy->y > camera_y
                && enemy->x < (camera_x + 136)
                && enemy->y < (camera_y + 128)) {
            //LD_tempEnemy(enemy);
            tempEnemy = *enemy;
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
                    
                    ////////////FACE_PLAYER
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
                    ////////end FACE_PLAYER
                    
                    ///Check attack player hitbox
                    if(!((player_anim_state == PLAYER_STATE_HITSTUN) || (player_anim_state == PLAYER_STATE_DEAD))) {
                        temp1 = player_x - tempEnemy.x;
                        temp2 = player_y - tempEnemy.y;
                        if(temp1 < 0) {
                            temp1 = -temp1;
                        }
                        if(temp2 < 0) {
                            temp2 = -temp2;
                        }
                        if(temp1 + temp2 < 12) {
                            player_anim_state = PLAYER_STATE_HITSTUN;
                            player_anim_frame = 0;
                            --player_health;
                            player_face_enemy();
                            do_noise_effect(80, -8, 10);
                        }
                    }
                    ////end attack player check
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
                        do_noise_effect(70, -12, 8);
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
                    --enemy_count;
                }
            }
            *enemy = tempEnemy;
        }
        ++enemy;
    }
}

void player_face_enemy() {
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
        if(temp3 < 0) {
            player_anim_dir = 4;
            player_anim_flip = 0;
            player_dir_x = 1;
            player_dir_y = 0;
        } else {
            player_anim_dir = 4;
            player_anim_flip = SPRITE_FLIP_X;
            player_dir_x = -1;
            player_dir_y = 0;
        }
    } else {
        if(temp4 < 0) {
            player_anim_flip = 0;
            player_anim_dir = 0;
            player_dir_x = 0;
            player_dir_y = 1;
        } else {
            player_anim_flip = 0;
            player_anim_dir = 8;
            player_dir_x = 0;
            player_dir_y = -1;
        }
    }
}
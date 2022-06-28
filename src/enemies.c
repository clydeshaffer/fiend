#include "enemies.h"
#include "gametank.h"
#include "drawing_funcs.h"
#include "globals.h"
#include "tilemap.h"
#include "dynawave.h"
#include "music.h"
#include "random.h"
#include "banking.h"

MobState tempEnemy;

MobState enemies[MAX_ENEMIES];

unsigned char enemy_count = 0;

unsigned char enemy_type_slots[ENEMY_TYPE_NUM_SLOTS];
unsigned char enemy_type_used_slots = 0;

extern const unsigned char* EnemySprites_RAT;
extern const unsigned char* EnemySprites_SPIDER;
extern const unsigned char* EnemySprites_BAT;
extern const unsigned char* EnemySprites_ORC;

extern const Frame EnemyFrames_RAT;
extern const Frame EnemyFrames_SPIDER;
extern const Frame EnemyFrames_BAT;
extern const Frame EnemyFrames_ORC;

#define RANGE_IMPACT 12
#define RANGE_MELEE 18

//Added to coordinates when moving
char enemy_speeds[] = {
    1, //RAT
    1, //SPIDER
    1, //BAT
    1 //ORC
};

//Update if anim_frame & mask == 0
unsigned char move_frame_mask[] = {
    3,
    3,
    1,
    3
};

unsigned char anim_state_offsets[] = {
    0, //INACTIVE
    0, //NORMAL
    24, //KNOCKBACK
    12 //ATTACKING
};

//Flags mask for mob types
unsigned char enemy_type_flags[] = {
    EFLAGS_PRESET_DEFAULTENEMY, //RAT
    EFLAGS_PRESET_DEFAULTENEMY, //SPIDER
    EFLAGS_PRESET_DEFAULTENEMY, //BAT
    EFLAGS_PRESET_DEFAULTENEMY //ORC
};

const Frame* enemyFrameData[] = {
    &EnemyFrames_RAT,
    &EnemyFrames_SPIDER,
    &EnemyFrames_BAT,
    &EnemyFrames_ORC
};
const char* enemySpriteSheets[] = {
    &EnemySprites_RAT,
    &EnemySprites_SPIDER,
    &EnemySprites_BAT,
    &EnemySprites_ORC
};

void load_enemy_type(char type) {
    char i;
    ChangeRomBank(BANK_MONSTERS);
    for(i = 0; i < ENEMY_TYPE_NUM_SLOTS; ++i) {
        if(enemy_type_slots[i] == type) {
            return;
        }
    }
    for(i = 0; i < ENEMY_TYPE_NUM_SLOTS; ++i) {
        if(enemy_type_slots[i] == ENEMY_TYPE_NONE) {
            enemy_type_slots[i] = type;
            ++enemy_type_used_slots;
            load_spritesheet(enemySpriteSheets[type], i+2);
            return;
        }
    }
}

void clear_enemy_slots() {
    char i;
    enemy_type_used_slots = 0;
    for(i = 0; i < ENEMY_TYPE_NUM_SLOTS; ++i) {
        enemy_type_slots[i] = ENEMY_TYPE_NONE;
    }
}

unsigned char random_loaded_enemy_slot() {
    return (((unsigned char) rnd()) & 0x7F) % enemy_type_used_slots;
}

void init_enemy(char slot, MobState* enemy) {
    char type = enemy_type_slots[slot];
    enemy->mode = ENEMY_STATE_NORMAL; //change to set on per-enemy basis
    enemy->slot = slot;
    enemy->anim_flip = SPRITE_FLIP_NONE;
    switch(rnd()&3) {
        case 0:
            enemy->anim_flip = SPRITE_FLIP_X;
        case 1:
            enemy->anim_dir = ANIM_DIR_SIDE;
            break;
        case 2:
            enemy->anim_dir = ANIM_DIR_UP;
            break;
        case 3:
            enemy->anim_dir = ANIM_DIR_DOWN;
            break;
    }
    switch(type) {
    case ENEMY_TYPE_RAT:
            enemy->health = 1;
            break;
    case ENEMY_TYPE_SPIDER:
            enemy->health = 2;
            break;
    case ENEMY_TYPE_BAT:
            enemy->health = 1;
            break;
    case ENEMY_TYPE_ORC:
            enemy->health = 3;
            break;
    }
}

void clear_enemies() {
    char i;
    for(i = 0; i < MAX_ENEMIES; ++i) {
        enemies[i].mode = ENEMY_STATE_INACTIVE;
        enemies[i].x = 0;
        enemies[i].y = 0;
        enemies[i].anim_flip = 0;
        enemies[i].anim_dir = 0;
        enemies[i].anim_frame = 0; 
    }
    enemy_count = 0;
}

void place_enemies() {
    unsigned char i, attempt;
    do {
            //save the last three slots for projectile pool
            for(i = 0; i < MAX_ENEMIES-3; ++i) {
                enemies[i].x = (rnd() & 0b1111100000) | 16;
                enemies[i].y = (rnd() & 0b1111100000) | 16;
                init_enemy(random_loaded_enemy_slot(), &enemies[i]);
                attempt = 0;
                while(!character_tilemap_check(enemies[i].x, enemies[i].y)) {
                    enemies[i].x = (rnd() & 0b1111100000) | 16;
                    enemies[i].y = (rnd() & 0b1111100000) | 16;
                    attempt++;
                    if(attempt == 5) {
                        enemies[i].mode = 0;
                    }
                }
                if(enemies[i].mode != 0) {
                    enemy_count++;
                }
            }
            enemies[MAX_ENEMIES-1].mode = 0;
            enemies[MAX_ENEMIES-2].mode = 0;
            enemies[MAX_ENEMIES-3].mode = 0;
    } while(enemy_count == 0);
}

void draw_enemies() {
    unsigned char i;
    MobState *enemy = enemies;
    ChangeRomBank(BANK_COMMON);
    queue_flags_param = DMA_GCARRY;
    for(i = 0; i < MAX_ENEMIES; ++i) {
        if(enemy->mode != ENEMY_STATE_INACTIVE) {
            if((enemy->x + 8) > (camera_x)
                && enemy->y > camera_y
                && enemy->x < (camera_x + 136)
                && enemy->y < (camera_y + 128)) {
                    QueuePackedSprite(enemyFrameData[enemy_type_slots[enemy->slot]], enemy->x - camera_x, enemy->y - camera_y, ((enemy->anim_frame >> 2) & 3) + enemy->anim_dir + anim_state_offsets[enemy->mode], enemy->anim_flip, GRAM_PAGE(enemy->slot + 2) | bankflip | BANK_CLIP_X | BANK_CLIP_Y, 0);
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
            tempEnemy.anim_dir = ANIM_DIR_SIDE;
            tempEnemy.anim_flip = SPRITE_FLIP_NONE;
        } else {
            tempEnemy.anim_dir = ANIM_DIR_SIDE;
            tempEnemy.anim_flip = SPRITE_FLIP_X;
        }
    } else {
        if(temp4 > 0) {
            tempEnemy.anim_dir = ANIM_DIR_DOWN;
            tempEnemy.anim_flip = SPRITE_FLIP_NONE;
        } else {
            tempEnemy.anim_dir = ANIM_DIR_UP;
            tempEnemy.anim_flip = SPRITE_FLIP_NONE;
        }
    }
}

void check_player_attack() {
    if(player_hitbox_damage) {
        temp1 = player_x + (player_dir_x>>1) - tempEnemy.x;
        temp2 = player_y + (player_dir_y>>1) - tempEnemy.y;
        if(temp1 < 0) {
            temp1 = -temp1;
        }
        if(temp2 < 0) {
            temp2 = -temp2;
        }
        if(temp1 + temp2 < 14) {
            tempEnemy.mode = ENEMY_STATE_KNOCKBACK;
            tempEnemy.anim_frame = 0;
            --tempEnemy.health;
            do_noise_effect(70, -12, 8);
            face_player();
        }
    }
}

void update_enemies() {
    char i, speed, type, flags;
    MobState *enemy = enemies;
    for(i = 0; i < MAX_ENEMIES; ++i) {
        //Check enemy is on screen
        if((enemy->x + 8) > (camera_x)
                && enemy->y > camera_y
                && enemy->x < (camera_x + 136)
                && enemy->y < (camera_y + 128)) {
            //LD_tempEnemy(enemy);
            tempEnemy = *enemy;
            type = enemy_type_slots[tempEnemy.slot];
            speed = enemy_speeds[type];
            if(player_health == 0) {
                speed = 0;
            }
            flags = enemy_type_flags[type];
            switch(tempEnemy.mode) {
            case ENEMY_STATE_NORMAL:
                temp1 = tempEnemy.x;
                temp2 = tempEnemy.y;
                ++(tempEnemy.anim_frame);
                if((tempEnemy.anim_frame & move_frame_mask[type]) == 0) {
                    switch(flags & EFLAGS_MOVETYPE_MASK) {
                    case EFLAGS_MOVETYPE_CHASE:
                        if(tempEnemy.x > player_x) {
                            tempEnemy.x -= speed;
                        } else if(tempEnemy.x < player_x) {
                            tempEnemy.x += speed;
                        }
                        if(!character_tilemap_check(tempEnemy.x, tempEnemy.y)) {
                            tempEnemy.x = temp1;
                        }
                        if(tempEnemy.y > player_y) {
                            tempEnemy.y -= speed;
                        } else if (tempEnemy.y < player_y) {
                            tempEnemy.y += speed;
                        }
                        
                        if(!character_tilemap_check(tempEnemy.x, tempEnemy.y)) {
                            tempEnemy.y = temp2;
                        }
                        
                        face_player();
                        break;
                    case EFLAGS_MOVETYPE_DIRECTION:
                        switch(tempEnemy.anim_dir) {
                            case ANIM_DIR_DOWN:
                                tempEnemy.y += speed;
                                break;
                            case ANIM_DIR_UP:
                                tempEnemy.y -= speed;
                                break;
                            case ANIM_DIR_SIDE:
                                if(tempEnemy.anim_flip) {
                                    tempEnemy.x -= speed;
                                } else {
                                    tempEnemy.x += speed;
                                }
                                break;
                        }
                        if(!character_tilemap_check(tempEnemy.x, tempEnemy.y)) {
                            tempEnemy.mode = ENEMY_STATE_INACTIVE;
                            enemy_count--;   
                        }
                        break;
                    case EFLAGS_MOVETYPE_TELEPORT:
                        if((tempEnemy.anim_frame & 127) == 0) {
                            do {
                                temp3 = rnd() & 255;
                                temp4 = wavetable_page[temp3];
                                temp3 = (temp3 + 64) & 255;
                                temp3 = wavetable_page[temp3];
                                tempEnemy.x = ((temp3 << 1) + (temp3)) >> 3;
                                tempEnemy.y = ((temp4 << 1) + (temp4)) >> 3;
                                tempEnemy.x += player_x - 48;
                                tempEnemy.y += player_y - 48;
                            } while(!character_tilemap_check(tempEnemy.x, tempEnemy.y));
                        }
                        face_player();
                        break;
                    } //end switch MOVETYPE
                    
                    if(player_health > 0) {
                        switch(flags & EFLAGS_ATTACK_MASK) {
                            case EFLAGS_ATTACK_IMPACT:
                                if(!((player_anim_state == PLAYER_STATE_HITSTUN) || (player_anim_state == PLAYER_STATE_DEAD))) {
                                    temp1 = player_x - tempEnemy.x;
                                    temp2 = player_y - tempEnemy.y;
                                    if(temp1 < 0) {
                                        temp1 = -temp1;
                                    }
                                    if(temp2 < 0) {
                                        temp2 = -temp2;
                                    }
                                    if(temp1 + temp2 < RANGE_IMPACT) {
                                        player_anim_state = PLAYER_STATE_HITSTUN;
                                        player_anim_frame = 0;
                                        --player_health;
                                        player_face_enemy();
                                        do_noise_effect(80, -8, 10);
                                    }
                                }
                                break;
                            case EFLAGS_ATTACK_MELEE:
                                temp1 = player_x - tempEnemy.x;
                                temp2 = player_y - tempEnemy.y;
                                if(temp1 < 0) {
                                    temp1 = -temp1;
                                }
                                if(temp2 < 0) {
                                    temp2 = -temp2;
                                }
                                if(temp1 + temp2 < RANGE_MELEE) {
                                    tempEnemy.mode = ENEMY_STATE_ATTACKING;
                                    tempEnemy.anim_frame = 0;
                                }

                        }
                    }
                    
                }
                check_player_attack();
                break;//out of ENEMY_STATE_NORMAL
            case ENEMY_STATE_KNOCKBACK:
                ++(tempEnemy.anim_frame);
                temp1 = tempEnemy.x;
                temp2 = tempEnemy.y;
                switch(tempEnemy.anim_dir) {
                            case ANIM_DIR_DOWN:
                                tempEnemy.y -= 1;
                                break;
                            case ANIM_DIR_UP:
                                tempEnemy.y += 1;
                                break;
                            case ANIM_DIR_SIDE:
                                if(tempEnemy.anim_flip) {
                                    tempEnemy.x += 1;
                                } else {
                                    tempEnemy.x -= 1;
                                }
                                break;
                        }
                        if(!character_tilemap_check(tempEnemy.x, tempEnemy.y)) {
                            tempEnemy.x = temp1;
                            tempEnemy.y = temp2;
                        }
                if(tempEnemy.anim_frame == 16) {
                    if(enemy->health) {
                        tempEnemy.mode = ENEMY_STATE_NORMAL;
                    } else {
                        tempEnemy.mode = ENEMY_STATE_INACTIVE;
                        --enemy_count;
                    }
                }
                break;//out of ENEMY_STATE_KNOCKBACK
            case ENEMY_STATE_ATTACKING:
                if(tempEnemy.anim_frame & 128) {
                    tempEnemy.anim_frame -= 128;
                    ++(tempEnemy.anim_frame);
                } else {
                    tempEnemy.anim_frame += 128;
                }
                if(tempEnemy.anim_frame == 8) {
                    //calc melee hitbox
                    temp3 = 0;
                    temp4 = 0;
                    switch(tempEnemy.anim_dir) {
                        case ANIM_DIR_DOWN:
                            temp4 = RANGE_MELEE;
                            break;
                        case ANIM_DIR_UP:
                            temp4 = -RANGE_MELEE;
                            break;
                        case ANIM_DIR_SIDE:
                            if(tempEnemy.anim_flip) {
                                temp3 = -RANGE_MELEE;
                            } else {
                                temp3 = RANGE_MELEE;
                            }
                            break;
                    }
                        temp1 = tempEnemy.x + temp3 - player_x;
                        temp2 = tempEnemy.y + temp4 - player_y;
                        if(temp1 < 0) {
                            temp1 = -temp1;
                        }
                        if(temp2 < 0) {
                            temp2 = -temp2;
                        }
                        if(temp1 + temp2 < 8) {
                            player_anim_state = PLAYER_STATE_HITSTUN;
                            player_anim_frame = 0;
                            --player_health;
                            player_face_enemy();
                            do_noise_effect(80, -8, 10);
                        }
                }
                else if(tempEnemy.anim_frame == 16) {
                    tempEnemy.anim_frame = 0;
                    tempEnemy.mode = ENEMY_STATE_NORMAL;                    
                }
                check_player_attack();
                break; //out of enemy state attack
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
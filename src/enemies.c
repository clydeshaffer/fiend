#include "enemies.h"
#include "gametank.h"
#include "drawing_funcs.h"
#include "globals.h"
#include "tilemap.h"
#include "music.h"
#include "random.h"
#include "banking.h"

void LD_enemy_index(char i);
void ST_enemy_index(char i);
char enemy_index_on_screen(char i);

MobState tempEnemy;

MobState enemies[MAX_ENEMIES];

char *mapMessage = "map filled";
char *maxHpMessage = "hearts increased";

unsigned char enemy_count = 0;
unsigned char drop_counter = 16;

unsigned char enemy_type_slots[ENEMY_TYPE_NUM_SLOTS];
unsigned char enemy_type_used_slots = 0;

char ignoreCamera = 0;

extern const unsigned char* SineTable;

extern const unsigned char* EnemySprites_RAT;
extern const unsigned char* EnemySprites_SPIDER;
extern const unsigned char* EnemySprites_BAT;
extern const unsigned char* EnemySprites_ORC;
extern const unsigned char* EnemySprites_SNIPER;
extern const unsigned char* EnemySprites_ARROW;
extern const unsigned char* EnemySprites_GHOST;
extern const unsigned char* EnemySprites_FIREBALL;
extern const unsigned char* EnemySprites_SKELETON_0;
extern const unsigned char* EnemySprites_SKELETON_1;
extern const unsigned char* EnemySprites_SKELETON_2;
extern const unsigned char* EnemySprites_SKELETON_3;
extern const unsigned char* EnemySprites_CULTIST_0;
extern const unsigned char* EnemySprites_CULTIST_1;
extern const unsigned char* EnemySprites_CULTIST_2;
extern const unsigned char* EnemySprites_CULTIST_3;
extern const unsigned char* EnemySprites_BAT2;

extern const Frame EnemyFrames_RAT;
extern const Frame EnemyFrames_SPIDER;
extern const Frame EnemyFrames_BAT;
extern const Frame EnemyFrames_ORC;
extern const Frame EnemyFrames_SNIPER;
extern const Frame EnemyFrames_ARROW;
extern const Frame EnemyFrames_GHOST;
extern const Frame EnemyFrames_FIREBALL;
extern const Frame EnemyFrames_SKELETON;
extern const Frame EnemyFrames_CULTIST;
extern const Frame EnemyFrames_BAT2;

#define RANGE_IMPACT 96
#define RANGE_MELEE 144
#define RANGE_MELEE_HITSIZE 96
#define RANGE_SHOOT 256
#define RANGE_ARROW 48
#define RANGE_HURTBOX 112
#define RANGE_LARGE_HURTBOX 192
#define RANGE_ORC_CHARGE 384

//Added to coordinates when moving
char enemy_speeds[] = {
    8, //RAT
    8, //SPIDER
    8, //BAT
    8, //ORC
    8, //SNIPER
    24, //ARROW
    0, //GHOST
    8, //FIREBALL
    8, //SKELETON_BOSS
    8, //CULTIST_BOSS
    8, //GHOSTBAT
};

//Update if anim_frame & mask == 0
unsigned char move_frame_mask[] = {
    3, //RAT
    3, //SPIDER
    1, //BAT
    3, //ORC
    1, //SNIPER
    1,  //ARROW
    7, //GHOST
    1, //FIREBALL,
    1, //SKELETON_BOSS
    3, //CULTIST_BOSS,
    1, //GHOSTBAT
};

unsigned char anim_state_offsets[] = {
    0, //INACTIVE
    0, //NORMAL
    24, //KNOCKBACK
    12 //ATTACKING
};

unsigned char anim_state_offsets_big[] = {
    0, //INACTIVE
    0, //NORMAL
    48, //KNOCKBACK
    24 //ATTACKING
};

//Flags mask for mob types
unsigned char enemy_type_flags[] = {
    EFLAGS_PRESET_DEFAULTENEMY, //RAT
    EFLAGS_PRESET_DEFAULTENEMY, //SPIDER
    EFLAGS_PRESET_DEFAULTENEMY, //BAT
    EFLAGS_PRESET_DEFAULTENEMY, //ORC
    EFLAGS_PLACEABLE|EFLAGS_MOVETYPE_CHASE|EFLAGS_ATTACK_PROJECTILE, //SNIPER
    EFLAGS_MOVETYPE_DIRECTION|EFLAGS_ATTACK_IMPACT, //ARROW
    EFLAGS_PLACEABLE|EFLAGS_LARGE|EFLAGS_MOVETYPE_TELEPORT|EFLAGS_ATTACK_PROJECTILE|EFLAGS_PHASING, //GHOST
    EFLAGS_MOVETYPE_CHASE|EFLAGS_ATTACK_IMPACT|EFLAGS_PHASING, //FIREBALL
    EFLAGS_PLACEABLE|EFLAGS_LARGE|EFLAGS_MOVETYPE_CHASE|EFLAGS_ATTACK_MELEE, //SKELETON_BOSS
    EFLAGS_PLACEABLE|EFLAGS_LARGE|EFLAGS_MOVETYPE_CHASE|EFLAGS_ATTACK_PROJECTILE, //CULTIST_BOSS
    EFLAGS_PRESET_DEFAULTENEMY | EFLAGS_PHASING, //GHOSTBAT
};

const char* Multisheet_Skeleton[] = {
    &EnemySprites_SKELETON_0,
    &EnemySprites_SKELETON_1,
    &EnemySprites_SKELETON_2,
    &EnemySprites_SKELETON_3,
};

const char* Multisheet_Cultist[] = {
    &EnemySprites_CULTIST_0,
    &EnemySprites_CULTIST_1,
    &EnemySprites_CULTIST_2,
    &EnemySprites_CULTIST_3,
};

const Frame* enemyFrameData[] = {
    &EnemyFrames_RAT,
    &EnemyFrames_SPIDER,
    &EnemyFrames_BAT,
    &EnemyFrames_ORC,
    &EnemyFrames_SNIPER,
    &EnemyFrames_ARROW,
    &EnemyFrames_GHOST,
    &EnemyFrames_FIREBALL,
    &EnemyFrames_SKELETON,
    &EnemyFrames_CULTIST,
    &EnemyFrames_BAT2,
};
const char** enemySpriteSheets[] = {
    &EnemySprites_RAT,
    &EnemySprites_SPIDER,
    &EnemySprites_BAT,
    &EnemySprites_ORC,
    &EnemySprites_SNIPER,
    &EnemySprites_ARROW,
    &EnemySprites_GHOST,
    &EnemySprites_FIREBALL,
    &Multisheet_Skeleton,
    Multisheet_Cultist,
    &EnemySprites_BAT2,
};

const char loadSpecial[] = {
    0,0,0,0,0,0,0,0,4,4,0
};

void attack_sound_for_enemy(char type) {
    switch (type)
    {
    case ENEMY_TYPE_RAT:
            do_noise_effect(30, 64, 8);
            break;
    case ENEMY_TYPE_SPIDER:
            do_noise_effect(30, 32, 8);
            break;
    case ENEMY_TYPE_BAT:
            do_noise_effect(30, 32, 8);
            break;
    case ENEMY_TYPE_ORC:
            do_noise_effect(30, 32, 8);
            break;
    case ENEMY_TYPE_SNIPER: 
            do_noise_effect(80, 0, 1);
            break;
    case ENEMY_TYPE_ARROW:
            break;
    case ENEMY_TYPE_GHOST:
            do_noise_effect(107, -64, 16);
            break;
    case ENEMY_TYPE_FIREBALL:
            break;
    case ENEMY_TYPE_SKELETON_BOSS:
            do_noise_effect(60, 128, 30);
            break;
    case ENEMY_TYPE_CULTIST_BOSS:
            do_noise_effect(60, 128, 30);
            break;
    case ENEMY_TYPE_GHOSTBAT:
            do_noise_effect(30, 32, 8);
            break;
    }
}

char enemy_banks[] = {
    BANK_MONSTERS, BANK_MONSTERS, BANK_MONSTERS, BANK_MONSTERS, BANK_MONSTERS, BANK_MONSTERS,
    BANK_MONSTERS2, BANK_MONSTERS2, BANK_MONSTERS2,
    BANK_MONSTERS3, BANK_MONSTERS3,
};

void load_enemy_type(char type) {
    char i, j;
    ChangeRomBank(enemy_banks[type]);
    for(i = 0; i < ENEMY_TYPE_NUM_SLOTS; ++i) {
        if(enemy_type_slots[i] == type) {
            return;
        }
    }
    for(i = 0; i < ENEMY_TYPE_NUM_SLOTS; ++i) {
        if(enemy_type_slots[i] == ENEMY_TYPE_NONE) {
            enemy_type_slots[i] = type;
            ++enemy_type_used_slots;
            if(loadSpecial[type]) {
                for(j = 0; j < loadSpecial[type]; ++j) {
                    load_spritesheet( enemySpriteSheets[type][j], (i+2) | (j << 3));    
                }
            } else {
                load_spritesheet(enemySpriteSheets[type], i+2);
            }
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
    char selected;
    do {
        selected = (((unsigned char) rnd()) & 0x7F) % enemy_type_used_slots;
    } while ((enemy_type_flags[enemy_type_slots[selected]] & EFLAGS_PLACEABLE) == 0);
    return selected;
}

void init_enemy(char slot, MobState* enemy) {
    char type = enemy_type_slots[slot];
    enemy->mode = ENEMY_STATE_NORMAL; //change to set on per-enemy basis
    enemy->slot = slot;
    enemy->anim_flip = SPRITE_FLIP_NONE;
    enemy->on_screen = 0;
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
    case ENEMY_TYPE_SNIPER: 
            enemy->health = 2;
            break;
    case ENEMY_TYPE_ARROW:
            enemy->health = 0;
            break;
    case ENEMY_TYPE_GHOST:
            enemy->health = 2;
            break;
    case ENEMY_TYPE_FIREBALL:
            enemy->health = 0;
            break;
    case ENEMY_TYPE_SKELETON_BOSS:
            enemy->health = 16;
            break;
    case ENEMY_TYPE_CULTIST_BOSS:
            enemy->health = 8;
            break;
    }
}

void clear_enemies() {
    char i;
    ignoreCamera = 0;
    for(i = 0; i < MAX_ENEMIES; ++i) {
        enemies[i].mode = ENEMY_STATE_INACTIVE;
        enemies[i].x.i = 0;
        enemies[i].y.i = 0;
        enemies[i].anim_flip = 0;
        enemies[i].anim_dir = 0;
        enemies[i].anim_frame = 0; 
    }
    enemy_count = 0;
}

void close_wall_traps() {
    int r, c;
    for(r = 0; r < MAP_H; ++r) {
        for(c = 0; c < MAP_W; ++c) {
            if((tile_at_cell(c, r) & SPECIAL_TILE_MASK) == SPECIAL_TILE_WALLTRAP)
            {
                set_tile(c << 8, r << 8, WALL_TILE);
            }
        }
    }
}

void open_gates() {
    int r, c;
    for(r = 0; r < MAP_H; ++r) {
        for(c = 0; c < MAP_W; ++c) {
            if((tile_at_cell(c, r) & SPECIAL_TILE_MASK) == SPECIAL_TILE_GATE)
            {
                set_tile(c << 8, r << 8, GROUND_TILE);
            }
        }
    }
}

void place_enemies() {
    unsigned char i=0, r, c, attempt;
    for(r = 0; r < MAP_H; ++r) {
        for(c = 0; c < MAP_W; ++c) {
            attempt = tile_at_cell(c, r);
            if(attempt & SPECIAL_TILE_SPAWN_ENEMY) {
                attempt = attempt & (SPECIAL_TILE_SPAWN_ENEMY-1);
                init_enemy(attempt, &enemies[i]);
                enemies[i].x.i = ((c << TILE_ORD) | 16)<<3;
                enemies[i].y.i = ((r << TILE_ORD) | 16)<<3;
                enemy_index_on_screen(i);
                ++i;
                ++enemy_count;
            }
        }
    }
    if(i == 0) {
        do {
            i = 0;
                //save the last three slots for projectile pool
                for(; i < MAX_ENEMIES; ++i) {
                    enemies[i].mode = 0;
                    if(i < MAX_ENEMIES - RESERVED_PROJECTILE_SLOTS) {
                        enemies[i].x.i = (rnd() & 0x1F00) | 128;
                        enemies[i].y.i = (rnd() & 0x1F00) | 128;
                        init_enemy(random_loaded_enemy_slot(), &enemies[i]);
                        attempt = 0;
                     while((!character_tilemap_check(enemies[i].x, enemies[i].y)) ||
                            (enemies[i].x.i == player_x.i && enemies[i].y.i == player_y.i)) {
                            enemies[i].x.i = (rnd() &  0x1F00) | 128;
                            enemies[i].y.i = (rnd() &  0x1F00) | 128;
                            attempt++;
                            if(attempt == 5) {
                                enemies[i].mode = 0;
                            }
                        }
                        if(enemies[i].mode != 0) {
                            enemy_count++;
                        }
                    }
                    enemy_index_on_screen(i);
                }
        } while(enemy_count == 0);
    }
}

#pragma codeseg (push, "CODE2");
void draw_enemies() {
    unsigned char i = 0;
    MobState *enemy = enemies;
    queue_flags_param = DMA_GCARRY;
    for(; i < MAX_ENEMIES; ++i) {
        if(enemy->mode != ENEMY_STATE_INACTIVE) {
            if(enemy->on_screen) {
                    LD_enemy_index(i);
                    if(tempEnemy.mode == ENEMY_STATE_ITEM) {
                        QueuePackedSprite(&HeroFrames,
                            (tempEnemy.x.i - camera_x.i) >> 3, (tempEnemy.y.i - camera_y.i) >> 3,
                            tempEnemy.anim_frame,
                            0,
                            GRAM_PAGE(1) | bankflip | BANK_CLIP_X | BANK_CLIP_Y, 0);
                    } else {
                        if(enemy_type_slots[tempEnemy.slot] == ENEMY_TYPE_SKELETON_BOSS || enemy_type_slots[tempEnemy.slot] == ENEMY_TYPE_CULTIST_BOSS) {
                            QueuePackedSprite(enemyFrameData[enemy_type_slots[tempEnemy.slot]],
                            (tempEnemy.x.i - camera_x.i) >> 3, (tempEnemy.y.i - camera_y.i) >> 3,
                            ((tempEnemy.anim_frame >> 2) & 7) + (tempEnemy.anim_dir << 1) + anim_state_offsets_big[tempEnemy.mode],
                            tempEnemy.anim_flip,
                            GRAM_PAGE(tempEnemy.slot + 2) | bankflip | BANK_CLIP_X | BANK_CLIP_Y, 0);
                        } else {
                            QueuePackedSprite(enemyFrameData[enemy_type_slots[tempEnemy.slot]],
                            (tempEnemy.x.i - camera_x.i) >> 3, (tempEnemy.y.i - camera_y.i) >> 3,
                            ((tempEnemy.anim_frame >> 2) & 3) + tempEnemy.anim_dir + anim_state_offsets[tempEnemy.mode],
                            tempEnemy.anim_flip,
                            GRAM_PAGE(tempEnemy.slot + 2) | bankflip | BANK_CLIP_X | BANK_CLIP_Y, 0);
                        }
                    }
                } 
        }
        ++enemy;
    }
}


void face_player() {
    temp3 = player_x.i - tempEnemy.x.i;
    temp4 = player_y.i - tempEnemy.y.i;
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

void check_player_attack(char hitsize) {
    if(player_hitbox_damage) {
        temp1 = player_x.i + (player_dir_x>>1) - tempEnemy.x.i;
        temp2 = player_y.i + (player_dir_y>>1) - tempEnemy.y.i;
        if(temp1 < 0) {
            temp1 = -temp1;
        }
        if(temp2 < 0) {
            temp2 = -temp2;
        }
        if(temp1 + temp2 < hitsize) {
            tempEnemy.mode = ENEMY_STATE_KNOCKBACK;
            tempEnemy.anim_frame = 0;
            if(tempEnemy.health)
                --tempEnemy.health;
            do_noise_effect(70, -12, 8);
            face_player();
        }
    }
}

char player_dist_check(char dist) {
    temp1 = player_x.i - tempEnemy.x.i;
    temp2 = player_y.i - tempEnemy.y.i;
    if(temp1 < 0) {
        temp1 = -temp1;
    }
    if(temp2 < 0) {
        temp2 = -temp2;
    }
    return (temp1 + temp2) < dist;
}

void check_impact_attack(char dist) {
    if(!((player_anim_state == PLAYER_STATE_HITSTUN) || (player_anim_state == PLAYER_STATE_DEAD))) {
        if(player_dist_check(dist)) {
            player_anim_state = PLAYER_STATE_HITSTUN;
            player_anim_frame = 0;
            --player_health;
            player_face_enemy();
            do_noise_effect(80, -8, 10);
            if(tempEnemy.health == 0) {
                tempEnemy.mode = ENEMY_STATE_KNOCKBACK;
            }
        }
    }
}

char nextProjectile = MAX_ENEMIES-RESERVED_PROJECTILE_SLOTS;
MobState* spawn_projectile(char slot, int x, int y) {
    MobState* newProjectile = &enemies[nextProjectile];
    init_enemy(slot, newProjectile);
    enemies[nextProjectile].anim_dir = tempEnemy.anim_dir;
    enemies[nextProjectile].anim_flip = tempEnemy.anim_flip;
    enemies[nextProjectile].x.i = x;
    enemies[nextProjectile].y.i = y;
    ++nextProjectile;
    if(nextProjectile == MAX_ENEMIES) {
        nextProjectile = MAX_ENEMIES - RESERVED_PROJECTILE_SLOTS;
    }
    return newProjectile;
}

void circle_position(int* x, int* y, char angle) {
    temp3 = angle;
    temp4 = SineTable[temp3];
    temp3 = (temp3 + 64) & 255;
    temp3 = SineTable[temp3];
    *x = ((temp3 << 1) + (temp3));
    *y = ((temp4 << 1) + (temp4));
    *x += player_x.i - 384;
    *y += player_y.i - 384;
}

void big_circle(int* x, int* y, char angle) {
    temp3 = angle;
    temp4 = SineTable[temp3];
    temp3 = (temp3 + 192) & 255;
    temp3 = SineTable[temp3];
    *x = temp3 << 2;
    *y = temp4 << 2;
    *x += player_x.i - 512;
    *y += player_y.i - 512;
}

unsigned char update_enemies() {
    static char i = 0;
    static char speed, type, flags, cnt, movemask, moveflags, atkflags;
    static signed char chase_offset_x = 0;
    static signed char chase_offset_y = 0;
    MobState *proj;
    MobState *enemy = &enemies[i];
    cnt = 0;
    if(i == 0) {
        chase_offset_x = 0;
        chase_offset_y = 0;
    }
    for(; (i < MAX_ENEMIES) && (cnt < MAX_ENEMIES_PER_FRAME); ++i) {
        //Check enemy is on screen
        via[ORB] = 0x80;
        via[ORB] = 0x02;
        if(enemy->mode && (enemy_index_on_screen(i) || ignoreCamera)) {
            via[ORB] = 0x80;
            via[ORB] = 0x42;
            LD_enemy_index(i);
            type = enemy_type_slots[tempEnemy.slot];
            speed = enemy_speeds[type];
            if(type == ENEMY_TYPE_CULTIST_BOSS && !ignoreCamera) {
                ignoreCamera = 1;
                close_wall_traps();
                play_track(MUSIC_TRACK_BOSS, 1);
            }
            
            if(player_health == 0) {
                speed = 0;
            }
            if(i == MAX_ENEMIES-RESERVED_PROJECTILE_SLOTS) {
                chase_offset_x = 0;
                chase_offset_y = 0;
            }
            flags = enemy_type_flags[type];
            moveflags = flags & EFLAGS_MOVETYPE_MASK;
            atkflags = flags & EFLAGS_ATTACK_MASK;
            switch(tempEnemy.mode) {
            case ENEMY_STATE_NORMAL:
                temp1 = tempEnemy.x.i;
                temp2 = tempEnemy.y.i;
                ++(tempEnemy.anim_frame);
                movemask = move_frame_mask[type];
                if(type == ENEMY_TYPE_ORC) {
                    if(chase_offset_x == 0 && chase_offset_y == 0) {
                        temp3 = player_x.i - tempEnemy.x.i;
                        temp4 = player_y.i - tempEnemy.y.i;
                        if(temp3 < 0) temp3 = -temp3;
                        if(temp4 < 0) temp4 = -temp4;
                        if((temp3+temp4) < RANGE_ORC_CHARGE) {
                            movemask = 1;
                        }
                    }
                }
                if((tempEnemy.anim_frame & movemask) == 0) {
                    ++cnt;
                    switch(moveflags) {
                    case EFLAGS_MOVETYPE_CHASE:
                        if(atkflags != EFLAGS_ATTACK_IMPACT) {
                            player_x.i += chase_offset_x;
                            player_y.i += chase_offset_y;
                        }
                        if(type == ENEMY_TYPE_SNIPER) {
                            temp3 = player_x.i - tempEnemy.x.i;
                            temp4 = player_y.i - tempEnemy.y.i;
                            if(temp3 < 0) temp3 = -temp3;
                            if(temp4 < 0) temp4 = -temp4;
                            if(temp3 < temp4) {
                                tempEnemy.anim_dir = ANIM_DIR_SIDE;
                                if(tempEnemy.x.i > player_x.i) {
                                    tempEnemy.x.i -= speed;
                                    tempEnemy.anim_flip = SPRITE_FLIP_X;
                                } else if(tempEnemy.x.i < player_x.i) {
                                    tempEnemy.x.i += speed;
                                    tempEnemy.anim_flip = SPRITE_FLIP_NONE;
                                } else {
                                    face_player();
                                }
                                if(!character_tilemap_check(tempEnemy.x, tempEnemy.y)) {
                                    tempEnemy.x.i = temp1;
                                }
                            } else {
                                tempEnemy.anim_flip = SPRITE_FLIP_NONE;
                                if(tempEnemy.y.i > player_y.i) {
                                    tempEnemy.y.i -= speed;
                                    tempEnemy.anim_dir = ANIM_DIR_UP;
                                } else if (tempEnemy.y.i < player_y.i) {
                                    tempEnemy.y.i += speed;
                                    tempEnemy.anim_dir = ANIM_DIR_DOWN;
                                } else {
                                    face_player();
                                }
                                if(!character_tilemap_check(tempEnemy.x, tempEnemy.y)) {
                                    tempEnemy.y.i = temp2;
                                }
                            }
                        } else {
                            if(tempEnemy.x.i > player_x.i) {
                                tempEnemy.x.i -= speed;
                            } else if(tempEnemy.x.i < player_x.i) {
                                tempEnemy.x.i += speed;
                            }
                            if((~flags & EFLAGS_PHASING) && !character_tilemap_check(tempEnemy.x, tempEnemy.y)) {
                                tempEnemy.x.i = temp1;
                            }
                            if(tempEnemy.y.i > player_y.i) {
                                tempEnemy.y.i -= speed;
                            } else if (tempEnemy.y.i < player_y.i) {
                                tempEnemy.y.i += speed;
                            }
                            if((~flags & EFLAGS_PHASING) && !character_tilemap_check(tempEnemy.x, tempEnemy.y)) {
                                tempEnemy.y.i = temp2;
                            }
                        }
                        if(atkflags != EFLAGS_ATTACK_IMPACT) {
                            player_x.i -= chase_offset_x;
                            player_y.i -= chase_offset_y;
                        }
                        if(type != ENEMY_TYPE_SNIPER) {
                            face_player();
                        }
                        break;
                    case EFLAGS_MOVETYPE_DIRECTION:
                        switch(tempEnemy.anim_dir) {
                            case ANIM_DIR_DOWN:
                                tempEnemy.y.i += speed;
                                break;
                            case ANIM_DIR_UP:
                                tempEnemy.y.i -= speed;
                                break;
                            case ANIM_DIR_SIDE:
                                if(tempEnemy.anim_flip) {
                                    tempEnemy.x.i -= speed;
                                } else {
                                    tempEnemy.x.i += speed;
                                }
                                break;
                        }
                        if(!character_tilemap_check(tempEnemy.x, tempEnemy.y)) {
                            tempEnemy.mode = ENEMY_STATE_INACTIVE; 
                        }
                        break;
                    case EFLAGS_MOVETYPE_TELEPORT:
                        if((tempEnemy.anim_frame & 63) == 0) {
                            //do {
                                circle_position(&tempEnemy.x.i, &tempEnemy.y.i, rnd() & 255);
                            //} while(!character_tilemap_check(tempEnemy.x.i, tempEnemy.y.i));
                        }
                        face_player();
                        break;
                    } //end switch MOVETYPE
                    if(player_health > 0) {
                        switch(atkflags) {
                            case EFLAGS_ATTACK_MELEE:
                                temp1 = player_x.i - tempEnemy.x.i;
                                temp2 = player_y.i - tempEnemy.y.i;
                                if(temp1 < 0) {
                                    temp1 = -temp1;
                                }
                                if(temp2 < 0) {
                                    temp2 = -temp2;
                                }
                                if(temp1 + temp2 < RANGE_MELEE) {
                                    tempEnemy.mode = ENEMY_STATE_ATTACKING;
                                    tempEnemy.anim_frame = 0;
                                    attack_sound_for_enemy(type);
                                }
                            case EFLAGS_ATTACK_IMPACT:
                                check_impact_attack((type == ENEMY_TYPE_ARROW) ? RANGE_ARROW : RANGE_IMPACT);
                                break;
                            case EFLAGS_ATTACK_PROJECTILE:
                                if(type == ENEMY_TYPE_CULTIST_BOSS) {
                                    temp1 = (tempEnemy.anim_frame > 180);
                                } else if(moveflags == EFLAGS_MOVETYPE_TELEPORT) {
                                    temp1 = (tempEnemy.anim_frame > 100);
                                } else {
                                    temp1 = (tempEnemy.x.i == player_x.i+chase_offset_x) || (tempEnemy.y.i == player_y.i+chase_offset_y);
                                }
                                if(temp1) {
                                    tempEnemy.mode = ENEMY_STATE_ATTACKING;
                                    tempEnemy.anim_frame = 0;
                                    face_player();
                                }
                                break;
                        }
                    }
                }
                check_player_attack((flags & EFLAGS_LARGE) ? RANGE_LARGE_HURTBOX : RANGE_HURTBOX);
                chase_offset_x = tempEnemy.y.i - player_y.i;
                chase_offset_y = player_x.i - tempEnemy.x.i;
                break;//out of ENEMY_STATE_NORMAL
            case ENEMY_STATE_KNOCKBACK:
                ++(tempEnemy.anim_frame);
                temp1 = tempEnemy.x.i;
                temp2 = tempEnemy.y.i;
                temp3 = 16;
                if(type == ENEMY_TYPE_CULTIST_BOSS) {
                    temp3 = 32;
                }
                switch(tempEnemy.anim_dir) {
                            case ANIM_DIR_DOWN:
                                tempEnemy.y.i -= 8;
                                break;
                            case ANIM_DIR_UP:
                                tempEnemy.y.i += 8;
                                break;
                            case ANIM_DIR_SIDE:
                                if(tempEnemy.anim_flip) {
                                    tempEnemy.x.i += 8;
                                } else {
                                    tempEnemy.x.i -= 8;
                                }
                                break;
                        }
                        if(!character_tilemap_check(tempEnemy.x, tempEnemy.y)) {
                            tempEnemy.x.i = temp1;
                            tempEnemy.y.i = temp2;
                        }
                if(tempEnemy.anim_frame == temp3) {
                    if(tempEnemy.health) {
                        tempEnemy.mode = ENEMY_STATE_NORMAL;
                        if(type == ENEMY_TYPE_CULTIST_BOSS) {
                            tempEnemy.mode = ENEMY_STATE_ATTACKING;
                            tempEnemy.anim_frame = 0;
                        }
                    } else {
                        if(type == ENEMY_TYPE_CULTIST_BOSS) {
                            init_enemy(2, &tempEnemy);
                            do_noise_effect(99, -64, 60);
                            init_enemy(3, &enemies[1]);
                            init_enemy(3, &enemies[2]);
                            player_face_enemy();
                            player_anim_frame = 0;
                            player_anim_state == PLAYER_STATE_HITSTUN;
                            play_track(MUSIC_TRACK_BOSS2, 1);
                            enemy_count += 2;
                        } else {
                            tempEnemy.mode = ENEMY_STATE_INACTIVE;
                            if(i < (MAX_ENEMIES - RESERVED_PROJECTILE_SLOTS)) {
                                --enemy_count;
                                if(drop_counter == 0) {
                                    tempEnemy.mode = ENEMY_STATE_ITEM;
                                    tempEnemy.anim_frame = (rnd()&1) ? ITEM_TYPE_MAXHP : ITEM_TYPE_MAP;
                                    drop_counter = rnd_range(4,16);
                                } else {
                                    --drop_counter;
                                    if(!(rnd() & 7)) {
                                        tempEnemy.mode = ENEMY_STATE_ITEM;
                                        tempEnemy.anim_frame = ITEM_TYPE_HEART;
                                    }
                                }
                                if(enemy_count == 0) {
                                    open_gates();
                                    if(ignoreCamera) {
                                        stop_music();
                                    }
                                }
                            }
                        }
                    }
                }
                break;//out of ENEMY_STATE_KNOCKBACK
            case ENEMY_STATE_ATTACKING:
                check_impact_attack(RANGE_IMPACT);
                temp3 = 8;
                temp4 = 16;
                if(type == ENEMY_TYPE_SKELETON_BOSS) {
                    ++(tempEnemy.anim_frame);
                    temp3 = 16;
                    temp4 = 31;
                } else if(tempEnemy.anim_frame & 128) {
                    tempEnemy.anim_frame -= 128;
                    ++(tempEnemy.anim_frame);
                } else {
                    tempEnemy.anim_frame += 128;
                }
                if(tempEnemy.anim_frame == temp3) {
                    switch(atkflags) {
                    case EFLAGS_ATTACK_MELEE:
                        if(!((player_anim_state == PLAYER_STATE_HITSTUN) || (player_anim_state == PLAYER_STATE_DEAD))) {
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
                        
                            temp1 = tempEnemy.x.i + temp3 - player_x.i;
                            temp2 = tempEnemy.y.i + temp4 - player_y.i;
                            if(temp1 < 0) {
                                temp1 = -temp1;
                            }
                            if(temp2 < 0) {
                                temp2 = -temp2;
                            }
                            if(temp1 + temp2 < RANGE_MELEE_HITSIZE) {
                                player_anim_state = PLAYER_STATE_HITSTUN;
                                player_anim_frame = 0;
                                --player_health;
                                player_face_enemy();
                                do_noise_effect(80, -8, 10);
                            }
                        }
                        break;
                    case EFLAGS_ATTACK_PROJECTILE:
                        attack_sound_for_enemy(type);
                        if(type == ENEMY_TYPE_CULTIST_BOSS) {
                            proj = spawn_projectile(1, tempEnemy.x.i, tempEnemy.y.i);
                            big_circle(&proj->x, &proj->y, rnd()&255);
                            proj = spawn_projectile(1, tempEnemy.x.i, tempEnemy.y.i);
                            big_circle(&proj->x, &proj->y, rnd()&255);
                            proj = spawn_projectile(1, tempEnemy.x.i, tempEnemy.y.i);
                            big_circle(&proj->x, &proj->y, rnd()&255);
                        } else {
                            spawn_projectile(tempEnemy.slot+1, tempEnemy.x.i, tempEnemy.y.i);
                        }
                        break;
                    }
                }
                else if(tempEnemy.anim_frame == temp4) {
                    tempEnemy.anim_frame = 0;
                    tempEnemy.mode = ENEMY_STATE_NORMAL;                    
                }
                if(type != ENEMY_TYPE_CULTIST_BOSS)
                    check_player_attack((flags & EFLAGS_LARGE) ? RANGE_LARGE_HURTBOX : RANGE_HURTBOX);
                break; //out of enemy state attack
            case ENEMY_STATE_ITEM:
                if(player_dist_check(RANGE_IMPACT)) {
                    switch(tempEnemy.anim_frame) {
                        case ITEM_TYPE_HEART:
                        if(player_health < player_max_health) {
                            ++player_health;
                        }
                        pause_music();
                        play_track(MUSIC_TRACK_PICKUP, REPEAT_RESUME);
                        break;
                        case ITEM_TYPE_MAXHP:
                        ++player_max_health;
                        pause_music();
                        temp_msg_counter = 180;
                        message_string = maxHpMessage;
                        play_track(MUSIC_TRACK_FANFARE, REPEAT_RESUME);
                        break;
                        case ITEM_TYPE_MAP:
                        do_fill_map = 1;
                        pause_music();
                        play_track(MUSIC_TRACK_PICKUP, REPEAT_RESUME);
                        temp_msg_counter = 180;
                        message_string = mapMessage;
                        break;
                    }
                    tempEnemy.mode = ENEMY_STATE_INACTIVE;
                }
                break; //out of enemy_state_item
            }
            ST_enemy_index(i);
        } else {
            via[ORB] = 0x80;
            via[ORB] = 0x42;
        }
        ++enemy;
    }
    if(i == MAX_ENEMIES) {
        i = 0;
    }
    return cnt;
}

void player_face_enemy() {
    temp3 = player_x.i - tempEnemy.x.i;
    temp4 = player_y.i - tempEnemy.y.i;
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
            player_dir_x = 8;
            player_dir_y = 0;
        } else {
            player_anim_dir = 4;
            player_anim_flip = SPRITE_FLIP_X;
            player_dir_x = -8;
            player_dir_y = 0;
        }
    } else {
        if(temp4 < 0) {
            player_anim_flip = 0;
            player_anim_dir = 0;
            player_dir_x = 0;
            player_dir_y = 8;
        } else {
            player_anim_flip = 0;
            player_anim_dir = 8;
            player_dir_x = 0;
            player_dir_y = -8;
        }
    }
}
#pragma codeseg (pop);
#ifndef ENEMIES_H
#define ENEMIES_H

#define MAX_ENEMIES 32
#define ENEMY_TYPE_NUM_SLOTS 6

#define ENEMY_STATE_INACTIVE 0
#define ENEMY_STATE_ACTIVE 1
#define ENEMY_STATE_DYING 2

#define ENEMY_TYPE_RAT      0
#define ENEMY_TYPE_SPIDER   1
#define ENEMY_TYPE_BAT      2
#define ENEMY_TYPE_SKELETON 3
#define ENEMY_TYPE_NONE     255

typedef struct MobState {
    unsigned char anim_frame, anim_dir, anim_flip, mode;
    int x, y;
    unsigned char health, slot;
} MobState;

extern MobState enemies[MAX_ENEMIES];

extern unsigned char enemy_count;

void load_enemy_type(char type);

void clear_enemy_slots();

unsigned char random_loaded_enemy_slot();

void init_enemy(char type, MobState* enemy);

void clear_enemies();

void draw_enemies();

void update_enemies();

void player_face_enemy();

#endif
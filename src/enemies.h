#ifndef ENEMIES_H
#define ENEMIES_H

#define MAX_ENEMIES 32

typedef struct MobState {
    unsigned char anim_frame, anim_dir, anim_flip, mode;
    int x, y;
    unsigned char health, type;
} MobState;

extern MobState enemies[MAX_ENEMIES];

extern unsigned char enemy_count;

void clear_enemies();

void draw_enemies();

void update_enemies();

void player_face_enemy();

#endif
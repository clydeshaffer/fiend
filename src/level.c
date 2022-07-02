#include "enemies.h"
#include "tilemap.h"
#include "banking.h"
#include "music.h"

unsigned char level_number = 0;

char music_for_level() {
    switch(level_number) {
        case 0:
            return MUSIC_TRACK_MAIN;
        case 1:
        case 2:
            return MUSIC_TRACK_AREA2;
        case 3:
        case 4:
            return MUSIC_TRACK_AREA3;
        default:
            return MUSIC_TRACK_AREA4;
    }
}

void load_enemies_for_level(char levnum) {
    clear_enemy_slots();
    switch(levnum) {
        case 0:
            load_enemy_type(ENEMY_TYPE_RAT);
            load_enemy_type(ENEMY_TYPE_BAT);
            break;
        case 1:
            load_enemy_type(ENEMY_TYPE_BAT);
            load_enemy_type(ENEMY_TYPE_SPIDER);
            break;
        case 2:
            load_enemy_type(ENEMY_TYPE_SPIDER);
            load_enemy_type(ENEMY_TYPE_ORC);
            break;
        case 3:
            load_enemy_type(ENEMY_TYPE_ORC);
            load_enemy_type(ENEMY_TYPE_SNIPER);
            load_enemy_type(ENEMY_TYPE_ARROW);
            break;
        case 4:
            load_enemy_type(ENEMY_TYPE_SNIPER);
            load_enemy_type(ENEMY_TYPE_ARROW);
            break;
        default:
            load_enemy_type(ENEMY_TYPE_GHOST);
            load_enemy_type(ENEMY_TYPE_FIREBALL);
            break;
    }
}

void init_level(char levnum) {
        level_number = levnum;
        switch_tileset(levnum);
        generate_map();
        load_enemies_for_level(level_number);
        ChangeRomBank(BANK_COMMON);
}

void next_level() {
    ++level_number;
    init_level(level_number);
}
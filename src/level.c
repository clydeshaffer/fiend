#include "enemies.h"
#include "tilemap.h"

unsigned char level_number = 0;

void load_enemies_for_level(char levnum) {
    clear_enemy_slots();
    switch(levnum) {
        case 0:
            load_enemy_type(ENEMY_TYPE_RAT);
            load_enemy_type(ENEMY_TYPE_BAT);
            break;
        case 1:
            load_enemy_type(ENEMY_TYPE_RAT);
            load_enemy_type(ENEMY_TYPE_SPIDER);
            break;
        case 2:
            load_enemy_type(ENEMY_TYPE_BAT);
            load_enemy_type(ENEMY_TYPE_SPIDER);
            break;
        case 3:
            load_enemy_type(ENEMY_TYPE_BAT);
            break;
        case 4:
            load_enemy_type(ENEMY_TYPE_SPIDER);
            load_enemy_type(ENEMY_TYPE_ORC);
            break;
        default:
            load_enemy_type(ENEMY_TYPE_ORC);
    }
}

void init_level(char levnum) {
        level_number = levnum;
        switch_tileset(level_number);
        generate_map();
        load_enemies_for_level(level_number);
}

void next_level() {
    ++level_number;
    init_level(level_number);
}
#include "enemies.h"
#include "tilemap.h"
#include "banking.h"
#include "music.h"
#include "globals.h"
#include "random.h"
#include "drawing_funcs.h"

unsigned char level_number = 0;

extern char BossMap;

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
        case 5:
            return MUSIC_TRACK_AREA4;
        default:
            return MUSIC_TRACK_NONE;
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
        case 5:
            load_enemy_type(ENEMY_TYPE_GHOST);
            load_enemy_type(ENEMY_TYPE_FIREBALL);
            break;
        case 6 :
            load_enemy_type(ENEMY_TYPE_CULTIST_BOSS);
            load_enemy_type(ENEMY_TYPE_GHOSTBAT);
            load_enemy_type(ENEMY_TYPE_SKELETON_BOSS);
            load_enemy_type(ENEMY_TYPE_GHOST);
            load_enemy_type(ENEMY_TYPE_FIREBALL);
            break;
    }
}

void init_level(char levnum) {
        level_number = levnum;
        switch_tileset(levnum);
        clear_spritebank(16);
        
        if(level_number == 6) {
            load_map(&BossMap);
        } else {
            ChangeRomBank(BANK_COMMON);
            generate_map();
        }
        
        load_enemies_for_level(level_number);
        ChangeRomBank(BANK_COMMON);

        if(!find_start_tile(&player_x, &player_y)) {
            do {
                player_x.i = (((((rnd() & 0x7FFF) % (MAP_W - 2)) + 1) << TILE_ORD)+16)<<3;
                player_y.i = (((((rnd() & 0x7FFF) % (MAP_H - 2)) + 1) << TILE_ORD)+16)<<3;
            } while(!character_tilemap_check(player_x, player_y));
        }
        camera_x.i = player_x.i - 512;
        camera_y.i = player_y.i - 512;

        clear_enemies(); 
        place_enemies();
        update_enemies();
}

void next_level() {
    ++level_number;
    init_level(level_number);
}
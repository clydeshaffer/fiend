/*
 * Functions for generating and collisionchecking the tilemap
 */

#ifndef TILEMAP_H
#define TILEMAP_H

#define MAP_SIZE 1024
#define MAP_W 32
#define MAP_H 32
#define MAP_ORD 5
#define TILE_SIZE 32
#define TILE_ORD 5
#define VISIBLE_W 5
#define VISIBLE_H 5

#define HITBOX_X -6
#define HITBOX_Y 4
#define HITBOX_W 10
#define HITBOX_H 3

#define EMPTY_TILE 0
#define GROUND_TILE 32
#define WALL_TILE 64
#define STAIRS_TILE 96

#define SPECIAL_TILE_MASK 31
#define SPECIAL_TILE_MUNDANE 0
#define SPECIAL_TILE_START 1
#define SPECIAL_TILE_WALLTRAP 2
#define SPECIAL_TILE_GATE 3
#define SPECIAL_TILE_SPAWN_ENEMY 16

extern unsigned int camera_x;
extern unsigned int camera_y;

void switch_tileset(unsigned char level_num);
void generate_map();
void load_map(char* data);
char tile_at(unsigned int pos_x, unsigned int pos_y);
char tile_at_cell(char x, char y);
void set_tile(unsigned int pos_x, unsigned int pos_y, char tile);
char character_tilemap_check(unsigned int pos_x, unsigned int pos_y);
void draw_world();
char find_start_tile(unsigned int* x, unsigned int* y);

#endif
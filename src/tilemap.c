#include "tilemap.h"
#include "gametank.h"
#include "drawing_funcs.h"
#include "random.h"
#include <zlib.h>

unsigned char tiles[MAP_SIZE];
char* tmpptr_char;
char tile_passmap[4] = { 0, 1, 0, 1};
unsigned int camera_x = 0;
unsigned int camera_y = 0;

extern const unsigned char* TestMap;

typedef struct Rect {
    unsigned char x, y, w, h;
} Rect;

Rect tmpRect;

unsigned char tilemap_offset = 0;

extern const unsigned char* Tileset_00;
extern const unsigned char* Tileset_01;
extern const unsigned char* Tileset_02;

void switch_tileset(unsigned char level_num) {
    switch(level_num) {
        case 0:
            load_spritesheet(&Tileset_00, 0);
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            load_spritesheet(&Tileset_01, 0);
            break;
        default:
            load_spritesheet(&Tileset_02, 0);
    }
}

void trim_edge_rects(Rect *r) {
    if(r->x == 0) {
        r->x++;
        r->w--;
    }
    if(r->y == 0) {
        r->y++;
        r->h--;
    }
    if((r->x + r->w) == (MAP_W - 1)) {
        r->w--;
    }
    if((r->y + r->h) == (MAP_H - 1)) {
        r->h--;
    }
}

void random_reduce_rect(Rect *r) {
    tmpRect = *r;
    r->w = rnd_range(2, tmpRect.w);
    r->h = rnd_range(2, tmpRect.h);
    r->x = rnd_range(tmpRect.x, tmpRect.x + tmpRect.w - r->w);
    r->y = rnd_range(tmpRect.y, tmpRect.y + tmpRect.h - r->h);
}

unsigned char min(a, b) {
    return (a < b) ? a : b;
}

unsigned char max(a, b) {
    return (a > b) ? a : b;
}

//assume src higher than dest
void map_path_v(Rect *src, Rect *dest) {
    unsigned char srcX, destX, turnY, i, j;
    turnY = rnd_range(src->y + src->h, dest->y);
    srcX = rnd_range(src->x, src->x + src->w);
    destX = rnd_range(dest->x, dest->x + dest->w);
    
    tmpptr_char = &tiles[min(srcX, destX) + (turnY << MAP_ORD)];
    j = max(srcX, destX);
    for(i = min(srcX, destX); i <= j; ++i) {
        *tmpptr_char = GROUND_TILE;
        tmpptr_char++;
    }

    tmpptr_char = &tiles[srcX + ((src->y + src->h) << MAP_ORD)];
    j = turnY - (src->y + src->h);
    for(i = 0; i < j; ++i) {
        *tmpptr_char = GROUND_TILE;
        tmpptr_char += MAP_W;
    }

    tmpptr_char = &tiles[destX + (turnY << MAP_ORD)];
    j = dest->y - turnY;
    for(i = 0; i < j; ++i) {
        *tmpptr_char = GROUND_TILE;
        tmpptr_char += MAP_W;
    }
}

//assume src left of dest
void map_path_h(Rect *src, Rect *dest) {
    unsigned char srcY, destY, turnX, i, j;
    turnX = rnd_range(src->x + src->w, dest->x);
    srcY = rnd_range(src->y, src->y + src->h);
    destY = rnd_range(dest->y, dest->y + dest->h);
    
    tmpptr_char = &tiles[turnX + (min(srcY, destY) << MAP_ORD)];
    j = max(srcY, destY);
    for(i = min(srcY, destY); i <= j; ++i) {
        *tmpptr_char = GROUND_TILE;
        tmpptr_char += MAP_W;
    }

    tmpptr_char = &tiles[(src->x + src->w) + (srcY << MAP_ORD)];
    j = turnX - (src->x + src->w);
    for(i = 0; i < j; ++i) {
        *tmpptr_char = GROUND_TILE;
        ++tmpptr_char;
    }

    tmpptr_char = &tiles[turnX + (destY << MAP_ORD)];
    j = dest->x - turnX;
    for(i = 0; i < j; ++i) {
        *tmpptr_char = GROUND_TILE;
        ++tmpptr_char;
    }
}

void map_fill_tile_rect(Rect *r) {
    unsigned char i, j;
    tmpptr_char = &tiles[r->x + (r->y << MAP_ORD)];
    for(i = 0; i < r->h; ++i) {
        for(j = 0; j < r->w; ++j) {
            *tmpptr_char = GROUND_TILE;
            ++tmpptr_char;
        }
        tmpptr_char += MAP_W;
        tmpptr_char -= r->w;
    }
}

void maybe_add_pillars(Rect *r) {
    char num_pillars, i, x, y;
    if(r->w < 5) return;
    if(r->h < 5) return;
    num_pillars = rnd_range(1,4);
    for(i = 0; i < num_pillars; ++i) {
        x = rnd_range(2, r->w - 1) + r->x;
        y = rnd_range(2, r->h - 1) + r->y;
        tiles[x + (y << MAP_ORD)] = EMPTY_TILE;
    }
}

void place_stairs(Rect *r) {
    char x, y;
    x = rnd_range(0, r->w) + r->x;
    y = rnd_range(0, r->h) + r->y;
    tiles[x + (y << MAP_ORD)] = STAIRS_TILE;
}

unsigned char scratchpad[256];

void generate_map() {
    //inflatemem(tiles, &TestMap);
    unsigned int i, j, st;
    Rect *mapRects = (Rect*) scratchpad;
    st = rnd_range(0, 16);

    for(i = 0; i < MAP_SIZE; i++) {
        tiles[i] = 0;
    }
    /*
        - divide 32x32 space into 8x8 rects
        - random reduce each rect
        - fill reduced rect
        - draw tile paths between adjacent rects
        - for every 0 tile above a walkable tile, set wall tile
    */
    for(i = 0; i < 32; i += 8) {
        for(j = 0; j < 32; j += 8) {
            mapRects->x = j;
            mapRects->y = i;
            mapRects->w = 8;
            mapRects->h = 8;
            trim_edge_rects(mapRects);
            random_reduce_rect(mapRects);
            map_fill_tile_rect(mapRects);
            maybe_add_pillars(mapRects);
            if(st == 0) {
                place_stairs(mapRects);
            }
            st--;
            mapRects++;
        }
    }

    mapRects = (Rect*) scratchpad;

    for(i = 0; i < 16; i+=4) {
        for(j = 0; j < 3; ++j) {
            map_path_h(&mapRects[i+j], &mapRects[i+j+1]);
        }
    }

    for(i = 0; i < 12; i+=4) {
        for(j = 0; j < 4; ++j) {
            map_path_v(&mapRects[i+j], &mapRects[i+j+4]);
        }
    }

    j = 32;
    for(i = 0; i < 992; ++i) {
        if(((i < MAP_W) || (tiles[i] == EMPTY_TILE)) && (tiles[j] != EMPTY_TILE)) {
            tiles[i] = WALL_TILE;
        }
        ++j;
    }
}

char tile_at(unsigned int pos_x, unsigned int pos_y) {
    pos_x += HITBOX_X;
    pos_y += HITBOX_Y;
    tmpptr_char = tiles + (pos_x >> TILE_ORD) + ((pos_y >> TILE_ORD) << MAP_ORD);
    return *tmpptr_char;
}

char character_tilemap_check(unsigned int pos_x, unsigned int pos_y) {
    pos_x += HITBOX_X;
    pos_y += HITBOX_Y;
    tmpptr_char = tiles + (pos_x >> TILE_ORD) + ((pos_y >> TILE_ORD) << MAP_ORD);
    pos_x &= (TILE_SIZE-1);
    pos_y &= (TILE_SIZE-1);
    if(!(*tmpptr_char & GROUND_TILE)) {
        return 0;
    }
    tmpptr_char++;
    if(pos_x + HITBOX_W >= TILE_SIZE) {
        if(!(*tmpptr_char & GROUND_TILE)) {
            return 0;
        }   
    }
    tmpptr_char += MAP_W - 1;
    if(pos_y + HITBOX_H >= TILE_SIZE) {
        if(!(*tmpptr_char & GROUND_TILE)) {
            return 0;
        }   
    }
    tmpptr_char++;
    if((pos_x + HITBOX_W >= TILE_SIZE) && (pos_y + HITBOX_H >= TILE_SIZE)) {
        if(!(*tmpptr_char & GROUND_TILE)) {
            return 0;
        }   
    }
    return 1;
}

void draw_world() {
    char r, c, tile_scroll_x, tile_scroll_y, cam_x, cam_y, r2, c2;
    int t;
    tile_scroll_x = camera_x & (TILE_SIZE-1);
    tile_scroll_y = camera_y & (TILE_SIZE-1);
    cam_x = camera_x >> TILE_ORD;
    cam_y = camera_y >> TILE_ORD;

    r = 0;
    c = 0;
    t = (cam_x + c) + ((cam_y + r) << MAP_ORD);
    //if(tiles[t] != 0) {
        SET_RECT(0, 0, TILE_SIZE - tile_scroll_x, TILE_SIZE - tile_scroll_y, tile_scroll_x + (tiles[t]), tile_scroll_y+tilemap_offset, 0, bankflip)
        queue_flags_param = DMA_GCARRY | DMA_OPAQUE;
        QueueSpriteRect();
    //}
    t++;
    c2 = TILE_SIZE;
    for(c = 1; c < VISIBLE_W; ++c) {
        if((cam_x + c) < MAP_W) {
           
            //if(tiles[t] != 0) {
                SET_RECT(c2 - tile_scroll_x, 0, TILE_SIZE, TILE_SIZE - tile_scroll_y, tiles[t], tile_scroll_y+tilemap_offset, 0, bankflip)
                queue_flags_param = DMA_GCARRY | DMA_OPAQUE;
                QueueSpriteRect();
            //}
        }
        c2 += TILE_SIZE;
        t++;
    }
    t += MAP_W - VISIBLE_W;

    c = 0;
    r = 1;
    c2 = 0;
    r2 = TILE_SIZE - tile_scroll_y;
    for(r = 1; r < VISIBLE_H; ++r) {
        if((cam_y + r) < MAP_H) {
            
            //if(tiles[t] != 0) {
                SET_RECT(0, r2, TILE_SIZE - tile_scroll_x, TILE_SIZE, tile_scroll_x + tiles[t], tilemap_offset, 0, bankflip)
                queue_flags_param = DMA_GCARRY | DMA_OPAQUE;
                QueueSpriteRect();
            //}

            t++;
            c2 = TILE_SIZE - tile_scroll_x;
            for(c = 1; c < VISIBLE_W; ++c) {
                if((cam_x + c) < MAP_W) {
                    //if(tiles[t] != 0) {
                        SET_RECT(c2, r2, TILE_SIZE, TILE_SIZE, tiles[t], tilemap_offset, 0, bankflip)
                        queue_flags_param = DMA_GCARRY | DMA_OPAQUE;
                        QueueSpriteRect();
                    //}
                }
                t++;
                c2 += TILE_SIZE;
            }
        } else { 
            t += VISIBLE_W;
        }
        t += MAP_W - VISIBLE_W;
        r2 += TILE_SIZE;
    }
    
}
#ifndef _MAP
#define _MAP
#include "genesis.h"

//u8 planwidth;
//u8 planheight;

// winX and winY -> tile location in tilemap
//u16 winX;
//u16 winY;

// tX and tY -> tile position in the plane
//u16 tX;
//u16 tY;

// maph and mapw -> height and width of tilemap
//u16 maph;
//u16 mapw;

typedef struct d_map{
    u8 planwidth;
    u8 planheight;
    // winX and winY -> tile location in tilemap
    u16 winX;
    u16 winY;
    // tX and tY -> tile position in the plane
    u16 tX;
    u16 tY;
    // maph and mapw -> height and width of tilemap
    u16 maph;
    u16 mapw;

    u16 tileoffset;
} datamap;

//datamap _blastdata = { 0, 0, 0, 0, 0, 0, 0, 0 };

//datamap* blastdata = &_blastdata;
extern datamap* blastdata;


// winH and winW -> height and width of visible window in tiles
#define winH 28
#define winW 40

typedef struct t_map{
    u16 width;
    u16 height;
    u8* data;
} tilemap;

typedef struct t_blastmap{
    datamap* data;
    tilemap* tiles;
} blastmap;

//void blast_init(datamap* blastdata);
void blast_init(blastmap* bmap);
void load_map(blastmap* bmap, int xoffset, int yoffset, u16 plan, u16 tileoffset);
void load_visible_map(blastmap* bmap, int xoffset, int yoffset, u16 plan, u16 tileoffset);
void load_map_row(blastmap* bmap, u16 plan, u8 row, u16 tileoffset);
void load_map_col(blastmap* bmap, u16 plan, u8 col, u16 tileoffset);
void screen_left(blastmap* bmap, u16* hScroll, u16 tile_offset, u16 plan);
void screen_right(blastmap* bmap, u16* hScroll, u16 tile_offset, u16 plan);
void screen_up(blastmap* bmap, u16* vScroll, u16 tile_offset, u16 plan);
void screen_down(blastmap* bmap, u16* vScroll, u16 tile_offset, u16 plan);
#endif

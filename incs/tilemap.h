#ifndef _MAP
#define _MAP
//#include "genesis.h"
//#include "blast.h"

// winH and winW -> height and width of visible window in tiles
#define winH 28
#define winW 40

//#define GET_TILE_XY(BMAP, X, Y) (BMAP->tiles->data[(Y * (BMAP->mapw)) + X] + BMAP->tileoffset - 1)

#define GET_COLL_XY(BMAP, X, Y) (((BMAP->tiles->data[(Y * (BMAP->mapw)) + X]) - 1) + BMAP->tileoffset)
#define GET_TILE_XY(BMAP, X, Y) (BMAP->tlookup[((BMAP->tiles->data[(Y * (BMAP->mapw)) + X]) - 1)] + BMAP->tileoffset)
#define NOCOLL 0
#define COLL 1
#define COLL_LEFT 2
#define COLL_RIGHT 4
#define COLL_UP 8
#define COLL_DOWN 16

u16 hs;
u16 vs;

struct _spritedef;
typedef struct _spritedef spritedef;

typedef struct t_map{
    u16 width;
    u16 height;
    u8* data;
    u8* coll;
} tilemap;

typedef struct d_map{
    // planwidth and planheight -> size of the plane
    u8 planwidth;
    u8 planheight;

    // winX and winY -> tile location(index) in tilemap
    u16 winX;
    u16 winY;

    // tX and tY -> tile position in the plane
    u16 tX;
    u16 tY;

    // maph and mapw -> height and width of tilemap
    u16 maph;
    u16 mapw;

    // tileoffset -> offset of tiledata in memory
    u16 tileoffset;

    // Which plane are we using?
    //u16 plane;
    VDPPlan plane;

    // tiles -> map of tiles
    tilemap* tiles;

    //
    u16* tlookup; 

} blastmap;

void blastmap_init(blastmap* bmap, tilemap* tmap, u16* tlookup, u16 tileoffset, VDPPlan plane);
void load_map(blastmap* bmap, int xoffset, int yoffset);
void load_visible_map(blastmap* bmap, int xoffset, int yoffset);
void load_map_row(blastmap* bmap, u8 row);
void load_map_col(blastmap* bmap, u8 col);
void screen_left(blastmap* bmap, u16* hScroll);
void screen_right(blastmap* bmap, u16* hScroll);
void screen_up(blastmap* bmap, u16* vScroll);
void screen_down(blastmap* bmap, u16* vScroll);
void center_screen(blastmap* bmap, u16 sprite_idx, u16* hscroll, u16* vscroll);

u8 check_left(blastmap* bmap, spritedef* sprite);
u8 check_right(blastmap* bmap, spritedef* sprite);
u8 check_up(blastmap* bmap, spritedef* sprite);
u8 check_down(blastmap* bmap, spritedef* sprite);

//u8 check_move(blastmap* bmap, spritedef* sprite);
u8 check_row(blastmap* bmap, spritedef* sprite, u8 tile_col);
u8 check_col(blastmap* bmap, spritedef* sprite, u8 tile_row);
#endif

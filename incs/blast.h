#include "genesis.h"
#include "gfx.h"
#include "tilemap.h"
#include "sprite.h"


#ifndef _BLAST
#define _BLAST

void vblank();
void hblank();
void blast_init();
void wait_vsync();

u8 coll_col [40];
u8 coll_row [28];
u8 spr_coll;
u16 coll_vcnt;
u32 coll_row_mask;
#endif

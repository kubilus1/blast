#include "genesis.h"
#include "gfx.h"
#include "tilemap.h"
#include "sprite.h"
#include "u8_stack.h"
#include "math_tables.h"
//#include "logic.h"
//#include "physics.h"

#ifndef _BLAST
#define _BLAST

#undef DEBUG
#undef ROWCHECK
#undef COLCHECK

//typedef int bool;
#define true 1
#define false 0

void vblank();
void hblank();
void blast_init();
void wait_vsync();
void BLAST_debugText(const char* inbuf, u16 x, u16 y); 
void showFPS(VDPPlane plan, u16 float_display);
/*
u8 coll_col [40];
u8 coll_row [28];
*/
u8 spr_coll;
blastmap* fore_map;
//u16 coll_vcnt;
//u32 coll_row_mask;
#endif

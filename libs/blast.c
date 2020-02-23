#include "genesis.h"
#include "blast.h"
//#include "math_tables.h"

char str[10];
void vblank() {
    //VDP_setHorizontalScroll(PLAN_B,hscroll*4);
    //VDP_setVerticalScroll(PLAN_B,vScroll*4);
    VDP_setHorizontalScroll(PLAN_A,hs);
    VDP_setVerticalScroll(PLAN_A,vs);

    //VDP_updateSprites(1,FALSE);
    BLAST_updateSprites();
    //check_collision();
}

void _hblank() {
    //check_collision();
#ifdef FOOBAR
    if(GET_VDPSTATUS(VDP_SPRCOLLISION_FLAG)) {
        //int i;
        //VDP_drawText(" COL ", 22, 20);
        //VDP_drawText(" COL ", 22, 20);
        //const u16 vcnt = GET_VCOUNTER;
#ifdef ROWCHECK
        coll_row_mask |= 1U << (GET_VCOUNTER >> 3);
#endif
        //coll_vcnt = GET_VCOUNTER;
        //coll_row[ vcnt/8 ] = 1;
        spr_coll = 1;
        //const u16 hcnt = GET_HCOUNTER;
        //uintToStr(coll_row_mask, str, 3);
        //VDP_drawText(str, 40, 16);
        //uintToStr(hcnt, str, 3);
        //VDP_drawText(str, 24, 25);
        
    }
#endif
}

void blast_init() {
    VDP_init();
    
    SYS_disableInts();
    
    //VDP_resetSprites();
    //VDP_setScreenWidth256();
    //VDP_setScreenWidth320();
    
    //VDP_setHIntCounter(2);
    //VDP_setHInterrupt(1);
    
    //VDP_setPlanSize(64,64);
    
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
    //VDP_setTextPalette(1);
    //VDP_clearPlan(VDP_PLAN_A, 0);
    
    //VDP_clearPlan(PLAN_A, 0);
    //VDP_clearPlan(PLAN_B, 0);
    
    //VDP_clearPlan(VDP_PLAN_B, 0);
    
    
    SYS_setVIntCallback(vblank);
    //SYS_setHIntCallback(hblank);
    //memset(coll_row, 0, sizeof(coll_row));

    spr_coll = 0;
    //coll_row_mask = 0;
    SYS_enableInts();

    //wait_vsync();
    VDP_waitVSync();
    VDP_waitVSync();
    VDP_waitVSync();
    VDP_waitVSync();
    VDP_waitVSync();
}

void wait_vsync()
{
    vu16 *pw;

    pw = (u16 *) GFX_CTRL_PORT;

    while (*pw & VDP_VBLANK_FLAG);
    while (!(*pw & VDP_VBLANK_FLAG));
}

void BLAST_debugText(const char* inbuf, u16 x, u16 y) { 
    //
#ifdef DEBUG_TEXT
    SYS_disableInts();
    VDP_drawText(inbuf, x, y);
    SYS_enableInts();
#endif
}

#include "genesis.h"
#include "blast.h"

#include "images.h"
#include "terrain_map1.h"

u16 terrain_tile_offset = 0;
blastmap terrain1map;
u16 asprite_idx;
spritedef asprite;
u16 hscroll;
u16 vscroll;
char str[10];

void vblank() {
    //VDP_setHorizontalScroll(PLAN_B,hscroll*4);
    //VDP_setVerticalScroll(PLAN_B,vScroll*4);
    VDP_setHorizontalScroll(PLAN_A,hs);
    VDP_setVerticalScroll(PLAN_A,vs);
}

void myJoyHandler(u16 joy, u16 changed, u16 state) {
    VDP_drawTextBG(VDP_PLAN_A,"JOY   ",0x8000,1,1);
    if (joy == JOY_1) {
        int mask = 1;
        u16 bitmask = state;
        u8 coll;
        while(bitmask) {
            switch(bitmask & mask) {
                case BUTTON_LEFT:
                    VDP_drawTextBG(VDP_PLAN_A,"LEFT  ",0x8000,1,1);
                    coll = check_left(&terrain1map, &asprite);
                    if(coll == NOCOLL || coll == COLL_LEFT) {
                        //asprite.posx--;
                        //asprite.vposx--;
                        sprite_left(&asprite, 1, 512);
                    }
                    break;
                case BUTTON_RIGHT:
                    VDP_drawTextBG(VDP_PLAN_A,"RIGHT ",0x8000,1,1);
                    coll = check_right(&terrain1map, &asprite);
                    if(coll == NOCOLL || coll == COLL_RIGHT) {
                        //asprite.posx++;
                        //asprite.vposx++;
                        sprite_right(&asprite, 1, 512);
                    }
                    break;
                case BUTTON_UP:
                    VDP_drawTextBG(VDP_PLAN_A,"UP    ",0x8000,1,1);
                    coll = check_up(&terrain1map, &asprite);
                    if(coll == NOCOLL || coll == COLL_UP) {
                        sprite_up(&asprite, 1, 512);
                        //asprite.posy--;
                        //asprite.vposy--;
                    }
                    break;
                case BUTTON_DOWN:
                    VDP_drawTextBG(VDP_PLAN_A,"DOWN  ",0x8000,1,1);
                    coll = check_down(&terrain1map, &asprite);
                    if(coll == NOCOLL || coll == COLL_DOWN) {
                        sprite_down(&asprite, 1, 512);
                        //asprite.posy++;
                        //asprite.vposy++;
                    }
                    break;
            }

            bitmask &= ~mask;
            mask <<= 1;
        }
        uintToStr(coll, str, 2);
        BMP_drawText(str, 12, 4);
        VDP_setSpriteP(asprite_idx, &asprite);
    }
}

int main() {

    /*
     *
     * Init background
     *
     */

    VDP_init();
    VDP_setPalette(PAL0, (u16*)terrain1.palette->data);
    terrain_tile_offset = load_bmp(
            (u32*) terrain1.image,
            terrain1.w,
            terrain1.h
    );

    VDP_clearPlan(VDP_PLAN_A, 0);
    VDP_clearPlan(VDP_PLAN_B, 0);
    blast_init(&terrain1map, &terrain1_map, terrain_tile_offset, APLAN);
    load_visible_map(&terrain1map,0,0);

    hscroll = -(terrain1map.tX * 8);
    vscroll = terrain1map.tY * 8;
    
    
    SYS_setVIntCallback(vblank);
    /*
     *
     * Init sprite
     *
     */
    
    asprite_idx = sprite_init(&asprite,terrain_tile_offset+7,1,8,8,1,1,PAL0);
    //asprite_idx = add_sprite(asprite);
    

    //spritedef bsprite;
    //u16 bsprite_idx; 
    //bsprite_idx = sprite_init(&bsprite,terrain_tile_offset+7,1,12,12,1,1,PAL0);
    //bsprite_idx = add_sprite(bsprite);

    uintToStr(asprite_idx, str, 2);
    BMP_drawText(str, 2, 2);
    //uintToStr(bsprite_idx, str, 2);
    //BMP_drawText(str, 2, 3);
    //VDP_setSpriteP(asprite_idx, &asprite);
    //VDP_setSpriteP(bsprite_idx, &bsprite);

    int i;
    for(i=0;i<8;i++){
        center_screen(&terrain1map, asprite_idx, &hs, &vs); 
        VDP_updateSprites();
        VDP_waitVSync();
    }

    while(1) {
        center_screen(&terrain1map, asprite_idx, &hs, &vs); 
        uintToStr(asprite.vposy, str, 3);
        BMP_drawText(str, 6, 0);
        uintToStr(asprite.vposx, str, 3);
        BMP_drawText(str, 10, 0);
        uintToStr(asprite.posy, str, 3);
        BMP_drawText(str, 14, 2);
        uintToStr(asprite.posx, str, 3);
        BMP_drawText(str, 18, 2);
        uintToStr(terrain1map.winX, str, 2);
        BMP_drawText(str, 0, 1);
        uintToStr(terrain1map.winY, str, 2);
        BMP_drawText(str, 3, 1);
        uintToStr(terrain1map.tX, str, 2);
        BMP_drawText(str, 6, 1);
        uintToStr(terrain1map.tY, str, 2);
        BMP_drawText(str, 9, 1);

        //vscroll_sprites(-2);
        myJoyHandler(JOY_1,0,JOY_readJoypad(JOY_1));
        VDP_updateSprites();
        VDP_waitVSync();
        //VDP_setSpriteP(asprite_idx, &asprite);
        //VDP_setSpriteP(bsprite_idx, &bsprite);
    }

    return 0;
}

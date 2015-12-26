#include "genesis.h"
#include "blast.h"

#include "images.h"
#include "terrain_map1.h"

u16 terrain_tile_offset = 0;
blastmap terrain1map;
u16 asprite_idx;
spritedef asprite;

void myJoyHandler(u16 joy, u16 changed, u16 state) {
    char str[10];
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
                    if(coll == NOCOLL || coll == COLL_LEFT)
                        asprite.posx--;
                    break;
                case BUTTON_RIGHT:
                    VDP_drawTextBG(VDP_PLAN_A,"RIGHT ",0x8000,1,1);
                    coll = check_right(&terrain1map, &asprite);
                    if(coll == NOCOLL || coll == COLL_RIGHT)
                        asprite.posx++;
                    break;
                case BUTTON_UP:
                    VDP_drawTextBG(VDP_PLAN_A,"UP    ",0x8000,1,1);
                    coll = check_up(&terrain1map, &asprite);
                    if(coll == NOCOLL || coll == COLL_UP)
                        asprite.posy--;
                    break;
                case BUTTON_DOWN:
                    VDP_drawTextBG(VDP_PLAN_A,"DOWN  ",0x8000,1,1);
                    coll = check_down(&terrain1map, &asprite);
                    if(coll == NOCOLL || coll == COLL_DOWN)
                        asprite.posy++;
                    break;
            }

            bitmask &= ~mask;
            mask <<= 1;
        }
        uintToStr(coll, str, 2);
        BMP_drawText(str, 12, 0);
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

    blast_init(&terrain1map, &terrain1_map, terrain_tile_offset, APLAN);
    VDP_clearPlan(VDP_PLAN_A, 0);
    load_map(&terrain1map,0,0);

    /*
     *
     * Init sprite
     *
     */
    
    sprite_init(&asprite,terrain_tile_offset+7,0,10,10,1,1,PAL0);
    asprite_idx = add_sprite(asprite);

    while(1) {
        myJoyHandler(JOY_1,0,JOY_readJoypad(JOY_1));
        VDP_updateSprites();
        VDP_waitVSync();
    }

    return 0;
}

#include "genesis.h"
#include "blast.h"

#include "images.h"
#include "terrain_map1.h"

u16 terrain_tile_offset = 0;
blastmap terrain1map;
u8 asprite_idx;
spritedef asprite;

u8 bsprite_idx;
spritedef bsprite;

u8 csprite_idx;
spritedef csprite;

//u8* colls;
u16 hscroll;
u16 vscroll;
char str[10];

u8 lista[1];
u8 listb[2];

void do_coll(spritedef* sprta, spritedef* sprtb) {
    switch(sprta->direction) {
        case 0:
            sprite_up(sprtb,1,512);
            //sprite_down(sprta,1,512);
            break;
        case 64:
            sprite_right(sprtb,1,512);
            //sprite_left(sprta,1,512);
            break;
        case 128:
            sprite_down(sprtb,1,512);
            //sprite_up(sprta,1,512);
            break;
        case 192:
            sprite_left(sprtb,1,512);
            //sprite_right(sprta,1,512);
            break;
    }
}
void terrain_coll(spritedef* sprt, u8 coll) {
    VDP_drawText(" T COLLIS    ", 2,2);
    switch(sprt->direction) {
        case 0:
            //sprite_up(sprt,2,512);
            sprite_down(sprt,2,512);
            break;
        case 64:
            //sprite_right(sprt,2,512);
            sprite_left(sprt,2,512);
            break;
        case 128:
            //sprite_down(sprt,2,512);
            sprite_up(sprt,2,512);
            break;
        case 192:
            //sprite_left(sprt,2,512);
            sprite_right(sprt,2,512);
            break;
    }
}

void (*coll_callback)(spritedef* sprta, spritedef* sprtb) = &do_coll;
void (*t_coll_callback)(spritedef* sprt, u8 coll) = &terrain_coll;

void myJoyHandler(u16 joy, u16 changed, u16 state) {
    //VDP_drawTextBG(VDP_PLAN_A,"JOY   ",0x8000,1,1);
    if (joy == JOY_1) {
        int mask = 1;
        u16 bitmask = state;
        u8 coll = 0;
        
        while(bitmask) {
            switch(bitmask & mask) {
                case BUTTON_LEFT:
                    //VDP_drawTextBG(VDP_PLAN_A,"LEFT  ",0x8000,1,1);
                    coll = check_left(&terrain1map, &asprite);
                    if(coll == NOCOLL || coll == COLL_LEFT) {
                        //asprite.posx--;
                        //asprite.vposx--;
                        sprite_left(&asprite, 1, 512);
                    }
                    break;
                case BUTTON_RIGHT:
                    //VDP_drawTextBG(VDP_PLAN_A,"RIGHT ",0x8000,1,1);
                    coll = check_right(&terrain1map, &asprite);
                    if(coll == NOCOLL || coll == COLL_RIGHT) {
                        //asprite.posx++;
                        //asprite.vposx++;
                        sprite_right(&asprite, 1, 512);
                    }
                    break;
                case BUTTON_UP:
                    //VDP_drawTextBG(VDP_PLAN_A,"UP    ",0x8000,1,1);
                    coll = check_up(&terrain1map, &asprite);
                    if(coll == NOCOLL || coll == COLL_UP) {
                        sprite_up(&asprite, 1, 512);
                        //asprite.posy--;
                        //asprite.vposy--;
                    }
                    break;
                case BUTTON_DOWN:
                    //VDP_drawTextBG(VDP_PLAN_A,"DOWN  ",0x8000,1,1);
                    coll = check_down(&terrain1map, &asprite);
                    if(coll == NOCOLL || coll == COLL_DOWN) {
                        sprite_down(&asprite, 1, 512);
                        //asprite.posy++;
                        //asprite.vposy++;
                    }
                    break;
            }
            
            //check_t_collision(lista, 1, &terrain1map, t_coll_callback);
            bitmask &= ~mask;
            mask <<= 1;
        }
        uintToStr(coll, str, 2);
        BMP_drawText(str, 12, 4);
        //VDP_setSpriteP(asprite_idx, &asprite);

    }
}

int main() {
    
    /*
     *
     * Init Blast
     *
     */

    blast_init();

    /*
     *
     * Init background
     *
     */

    VDP_setPalette(PAL0, (u16*)terrain1.palette->data);
    terrain_tile_offset = load_bmp(
            (u32*) terrain1.image,
            terrain1.w,
            terrain1.h
    );

    blastmap_init(&terrain1map, &terrain1_map, terrain_tile_offset, PLAN_A);
    load_visible_map(&terrain1map,0,0);

    hscroll = -(terrain1map.tX * 8);
    vscroll = terrain1map.tY * 8;
    
    
    /*
     *
     * Init sprites
     *
     */
    
    asprite_idx = sprite_init(&asprite,terrain_tile_offset+7,1,8,8,1,1,PAL0);
    bsprite_idx = sprite_init(&bsprite,terrain_tile_offset+7,1,32,32,1,1,PAL1);
    csprite_idx = sprite_init(&csprite,terrain_tile_offset+7,1,48,48,1,1,PAL2);

    u8 listb[2];

    lista[0] = asprite_idx;
    listb[0] = bsprite_idx;
    listb[1] = csprite_idx;

    uintToStr(asprite_idx, str, 2);
    BMP_drawText(str, 2, 2);
    //uintToStr(bsprite_idx, str, 2);
    //BMP_drawText(str, 2, 3);
    //VDP_setSpriteP(asprite_idx, &asprite);
    //VDP_setSpriteP(bsprite_idx, &bsprite);

    /*
    int i;
    for(i=0;i<8;i++){
        center_screen(&terrain1map, asprite_idx, &hs, &vs); 
        //VDP_updateSprites();
        VDP_waitVSync();
    }
    */


    while(1) {
        center_screen(&terrain1map, asprite_idx, &hs, &vs); 
        /*
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
        */

        //check_collision();
        //vscroll_sprites(-2);
        myJoyHandler(JOY_1,0,JOY_readJoypad(JOY_1));
    
        if(spr_coll == 1) {
            //VDP_drawText(" VDP COLLIS    ", 20,19);
            check_collision(lista, 1,  listb, 2, coll_callback);
            spr_coll = 0;
        } 
        check_t_collision(listb, 2, &terrain1map, t_coll_callback);
        //VDP_updateSprites();
        VDP_showFPS(0);
        VDP_waitVSync();
        //VDP_setSpriteP(asprite_idx, &asprite);
        //VDP_setSpriteP(bsprite_idx, &bsprite);
    }

    return 0;
}

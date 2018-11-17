#include "genesis.h"
#include "blast.h"

#include "images.h"
#include "arena.h"
//#include "terrain_map1.h"
//#include "shot_0.h"

u16 terrain_tile_offset = 0;
u16 shot_offset = 0;
blastmap arena_map;
u8 asprite_idx;
spritedef asprite;

u8 bsprite_idx;
spritedef bsprite;

u8 pow_idx;
spritedef pow;
u8 coly_idx;
spritedef coly;

//u16 hscroll;
//u16 vscroll;
char str[10];

void myJoyHandler(u16 joy, u16 changed, u16 state) {
    //VDP_drawTextBG(VDP_PLAN_A,"JOY   ",0x8000,1,1);
    if (joy == JOY_1) {
        int mask = 1;
        u16 bitmask = state;
        u8 coll;
        while(bitmask) {
            switch(bitmask & mask) {
                case BUTTON_LEFT:
                    //VDP_drawTextBG(VDP_PLAN_A,"LEFT  ",0x8000,1,1);
                    //coll = check_left(&arena_map, &asprite);
                    if(coll == NOCOLL || coll == COLL_LEFT) {
                        //asprite.posx--;
                        //asprite.vposx--;
                        sprite_left(&asprite, 1, 512);
                    }
                    break;
                case BUTTON_RIGHT:
                    //VDP_drawTextBG(VDP_PLAN_A,"RIGHT ",0x8000,1,1);
                    //coll = check_right(&arena_map, &asprite);
                    if(coll == NOCOLL || coll == COLL_RIGHT) {
                        //asprite.posx++;
                        //asprite.vposx++;
                        sprite_right(&asprite, 1, 512);
                    }
                    break;
                case BUTTON_UP:
                    //VDP_drawTextBG(VDP_PLAN_A,"UP    ",0x8000,1,1);
                    //coll = check_up(&arena_map, &asprite);
                    if(coll == NOCOLL || coll == COLL_UP) {
                        sprite_up(&asprite, 1, 512);
                        //asprite.posy--;
                        //asprite.vposy--;
                    }
                    break;
                case BUTTON_DOWN:
                    //VDP_drawTextBG(VDP_PLAN_A,"DOWN  ",0x8000,1,1);
                    //coll = check_down(&arena_map, &asprite);
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
        //uintToStr(coll, str, 2);
        //BMP_drawText(str, 12, 4);
        //BLAST_setSpriteP(asprite_idx, &asprite);
        //VDP_setSpritePosition(asprite_idx, asprite.posx, asprite.posy);
        //check_collision();
    }
}

void do_coll(spritedef* sprta, spritedef* sprtb) {
    VDP_drawText(" CALLBACK    ", 20,18);
    //sprt->pal = PAL1;
    //sprt->posx -= 2;
    //sprite_left(sprtb,2,512);
    //sprite_right(sprta,1,512);
    

    switch(sprta->direction) {
        case 0:
            sprite_up(sprtb,2,512);
            //sprite_down(sprta,1,512);
            break;
        case 64:
            sprite_right(sprtb,2,512);
            //sprite_left(sprta,1,512);
            break;
        case 128:
            sprite_down(sprtb,2,512);
            //sprite_up(sprta,1,512);
            break;
        case 192:
            sprite_left(sprtb,2,512);
            //sprite_right(sprta,1,512);
            break;
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


    shot_offset = load_bmp(
            (u32*) shot_0.image,
            shot_0.w,
            shot_0.h
    );


    blastmap_init(&arena_map, &terrain1_map, terrain_tile_offset, PLAN_A);
    load_visible_map(&arena_map,0,0);

   // hscroll = -(arena_map.tX * 8);
   // vscroll = arena_map.tY * 8;
    
    
    /*
     *
     * Init sprites
     *
     */


    //VDP_resetSprites();

/*
    u8 good_list[42];
    memset(good_list, 0xFF, 42);
    u8 bad_list[42]; 
    memset(bad_list, 0xFF, 42);

*/
    u8 lista[1];
    u8 listb[2];

    asprite_idx = sprite_init(&asprite,terrain_tile_offset+7,1,256,256,1,1,PAL0);

    lista[0] = asprite_idx;
    //BLAST_updateSprites();
    //VDP_updateSprites(asprite_idx,TRUE);
    //uintToStr(asprite_idx, str, 2);
    //BMP_drawText(str, 2, 2);
    //uintToStr(bsprite_idx, str, 2);
    //BMP_drawText(str, 2, 3);
    //VDP_setSpriteP(asprite_idx, &asprite);
    //VDP_setSpriteP(bsprite_idx, &bsprite);


    void (*coll_callback)(spritedef* sprta, spritedef* sprtb) = &do_coll;

    int i;
    for(i=0;i<32;i++){
        center_screen(&arena_map, asprite_idx, &hs, &vs); 
        center_screen(&arena_map, asprite_idx, &hs, &vs); 
        center_screen(&arena_map, asprite_idx, &hs, &vs); 
        center_screen(&arena_map, asprite_idx, &hs, &vs); 
        //BLAST_updateSprites();
        VDP_waitVSync();
    }

    bsprite_idx = sprite_init(&bsprite,terrain_tile_offset+7,1,64,64,1,1,PAL0);
    pow_idx = sprite_init(&pow,shot_offset,1,120,120,1,1,PAL0);

    listb[0] = bsprite_idx;
    listb[1] = pow_idx;

    coly_idx = sprite_init(&coly,shot_offset,1,5,5,1,1,PAL0);
   
    //BLAST_updateSprites(); 
    //VDP_updateSprites(bsprite_idx,TRUE);

    while(1) {
        /*
        while (*pw & VDP_VBLANK_FLAG);
        
        do {
            gcp = *pw;
            //check_collision();

            if(gcp & VDP_SPRCOLLISION_FLAG) {
                //VDP_drawText("  COLL  ", 2, 2);
                //hint = VDP_getHIntCounter();
                //uintToStr(hint, str, 3);
                //VDP_drawText(str, 4, 4);
            } 

        } while (!(gcp & VDP_VBLANK_FLAG));
        */
        //check_collision();
        //while (!(*pw & VDP_VBLANK_FLAG));


        //check_collision();
        //SYS_disableInts();
        //
        //


        // Move player
        myJoyHandler(JOY_1,0,JOY_readJoypad(JOY_1));

        // Move other sprites

        if(spr_coll == 1) {
            coly.posy = (coll_vcnt - 4);
            VDP_drawText(" VDP COLLIS    ", 20,19);
            check_collision(lista, 1, listb, 2, coll_callback);
            spr_coll = 0;
        } else {
            VDP_drawText("               ", 20,19);
            VDP_drawText("               ", 30,35);
            VDP_drawText("               ", 30,36);
        };
        //check_collision(&asprite, bad_list, do_coll);
        //VDP_updateSprites();
        //SYS_enableInts();
        //load_map_row(&arena_map,0);
        
        
        //do_shit(&arena_map, 0);
        /*
        VDP_setTileMapXY(
                arena_map.plane, 
                (u16*)arena_map.tiles->data[(arena_map.winY * (arena_map.mapw)) + arena_map.winX] + arena_map.tileoffset - 1,
                arena_map.tX,
                arena_map.winX
        );
        */


        //VDP_updateSprites();
        //center_screen(&arena_map, asprite_idx, &hs, &vs); 
        //VDP_updateSprites();
        
        //Reading GFX_CTRL_PORT clears spr coll bit
        //GET_VDPSTATUS(VDP_FIFOEMPTY_FLAG);
        //check_collision();


        /*
        uintToStr(asprite.vposy, str, 3);
        BMP_drawText(str, 6, 0);
        uintToStr(asprite.vposx, str, 3);
        BMP_drawText(str, 10, 0);
        uintToStr(asprite.posy, str, 3);
        BMP_drawText(str, 14, 2);
        uintToStr(asprite.posx, str, 3);
        BMP_drawText(str, 18, 2);
        uintToStr(arena_map.winX, str, 2);
        BMP_drawText(str, 0, 1);
        uintToStr(arena_map.winY, str, 2);
        BMP_drawText(str, 3, 1);
        uintToStr(arena_map.tX, str, 2);
        BMP_drawText(str, 6, 1);
        uintToStr(arena_map.tY, str, 2);
        BMP_drawText(str, 9, 1);
        */

        //check_collision();
        //vscroll_sprites(-2);
    
        //VDP_updateSprites(2,TRUE);
        //VDP_setSpriteP(asprite_idx, &asprite);
        //check_collision();
        //VDP_setSpriteP(bsprite_idx, &bsprite);
        //check_collision();
        //VDP_waitVSync();
        //vblank();
        //check_collision();
        //VDP_setSpriteP(asprite_idx, &asprite);
        //VDP_setSpriteP(bsprite_idx, &bsprite);
        VDP_showFPS(0);
        //VDP_setSpritePosition(asprite_idx, 40,40);
        //VDP_setSpritePosition(bsprite_idx, 120,90);
        //BLAST_updateSprites();
        //VDP_updateSprites(80,TRUE);
        VDP_waitVSync();
    }

    //SYS_enableInts();
    return 0;
}

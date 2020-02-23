#include "blast.h"
//#include "genesis.h"

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
u8 pow1_idx;
spritedef pow1;
u8 pow2_idx;
spritedef pow2;
u8 coly_idx;
spritedef coly;

#define NUMSPRITES 20

spritedef spr_list[NUMSPRITES];

manifold coll_manifold;
//u16 hscroll;
//u16 vscroll;
char str[10];
u8 frame = 0;

void myJoyHandler(u16 joy, u16 changed, u16 state) {
    //VDP_drawTextBG(VDP_PLAN_A,"JOY   ",0x8000,1,1);
    if (joy == JOY_1) {
        if((state & BUTTON_UP)) {
            asprite.velocity.y += -16;
        } else if ((state & BUTTON_DOWN)) {
            asprite.velocity.y += 16;
        } 
            
        if ((state & BUTTON_RIGHT)) {
            asprite.velocity.x += 16;
        } else if ((state & BUTTON_LEFT)) {
            asprite.velocity.x += -16;
        }
    }
    asprite.velocity.x = min(asprite.velocity.x, 256);
    asprite.velocity.x = max(asprite.velocity.x, -256);
    asprite.velocity.y = min(asprite.velocity.y, 256);
    asprite.velocity.y = max(asprite.velocity.y, -256);
}


void force_sprite(spritedef* sprt, u8 force, u8 direction) {
    s8 x_delta;
    s8 y_delta;

    y_delta = ((force) * cos_table[direction])>>6;
    x_delta = ((force) * sin_table[direction])>>6;

    sprt->velocity.x = x_delta;
    sprt->velocity.y = (-1 * y_delta);
    //
    return;
}


void do_coll(spritedef* sprta, spritedef* sprtb) {
    //VDP_drawText(" COL CALLBACK  ", 20,18);
    if(get_circle_manifold(sprta, sprtb, &coll_manifold)) {
        sprite_bounce(sprta, sprtb, &coll_manifold);
    }

    return;
}

void terrain_coll(spritedef* sprt, u8 coll_dir) {
    //VDP_drawText(" TERRAIN COLLIS  ", 20,19);
    /*if(coll != 0){
        sprt->force=0;
    }*/

    //sprt->direction += 128;
    //sprt->force = 2;
}

void (*coll_callback)(spritedef* sprta, spritedef* sprtb) = &do_coll;
void (*t_coll_callback)(spritedef* sprt, u8 coll) = &terrain_coll;

int main() {
    
    int i;
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
    shot_offset = load_img(&shot_0);
    terrain_tile_offset = load_img(&terrain1);


    //VDP_setPalette(PAL0, (u16*)terrain1.palette->data);


    /*
    shot_offset = load_bmp(
            (u32*) shot_0.image,
            shot_0.w,
            shot_0.h
    );

    terrain_tile_offset = load_bmp(
            (u32*) terrain1.image,
            terrain1.w,
            terrain1.h
    );
    */

    //blastmap_init(&arena_map, &terrain1_map, terrain_tile_offset, PLAN_A);
    blastmap_init(&arena_map, &terrain1_map, (u16*)terrain1.map->tilemap, terrain_tile_offset, PLAN_A);
    load_visible_map(&arena_map,0,0);
    SYS_enableInts(); 
    /*
     *
     * Init sprites
     *
     */

    
    u8 lista[1];
    u8 listb[2];
    u8 listc[5];
    u8 listd[NUMSPRITES+2];

    asprite_idx = sprite_init(&asprite,terrain_tile_offset+5,1,256,256,1,1,PAL0);
    lista[0] = asprite_idx;

    bsprite_idx = sprite_init(&bsprite,terrain_tile_offset+5,1,200,200,1,1,PAL1);
    /*
    pow_idx = sprite_init(&pow,shot_offset,1,276,265,1,1,PAL0);
    pow1_idx = sprite_init(&pow1,shot_offset,1,226,285,1,1,PAL0);
    pow2_idx = sprite_init(&pow2,shot_offset,1,270,265,1,1,PAL0);
    */
    asprite.inv_mass = FIX16(0.5);
    bsprite.inv_mass = FIX16(0.5);
    //pow.inv_mass = FIX16(0.5);
    //pow1.inv_mass = FIX16(0.2);
    //pow2.inv_mass = FIX16(0.25);


    listb[0] = bsprite_idx;
    listb[1] = pow_idx;

    listc[0] = asprite_idx;
    listc[1] = bsprite_idx;
    //listc[2] = pow_idx;
    //listc[3] = pow1_idx;
    //listc[4] = pow2_idx;

    

    coly_idx = sprite_init(&coly,shot_offset,1,5,5,1,1,PAL0);
    for(i=0;i<NUMSPRITES;i++){
        u8 x =
        sprite_init(
                &spr_list[i],
                shot_offset,
                //terrain_tile_offset+7,
                1,
                random() % 64 + 200,
                random() % 64 + 200,
                1,1,PAL2);

        spr_list[i].aabb.min.x += 2;
        spr_list[i].aabb.max.x -= 2;
        spr_list[i].aabb.min.y += 2;
        spr_list[i].aabb.max.y -= 2;
        spr_list[i].circle.radius = 3;

        listd[i] = spr_list[i].idx;
        spr_list[i].velocity.x = random()>>9;
        spr_list[i].velocity.y = random()>>9;
        spr_list[i].inv_mass = FIX16(0.25);
    }

   
    listd[NUMSPRITES] = asprite_idx;
    listd[NUMSPRITES+1] = bsprite_idx;

    //bsprite.direction = 160;
    //bsprite.force = 16;
    //force_sprite(&bsprite, 128, 192);
    //force_sprite(&asprite, 64, 128);
    //force_sprite(&bsprite, 64, 0);
    //force_sprite(&pow, 0, 73);
    asprite.velocity.y = -22;
    asprite.velocity.x = -22;
    /*
    bsprite.velocity.y = -48;
    bsprite.velocity.x = 128;
    pow.velocity.x = -98;
    pow.velocity.y = 98;
    pow1.velocity.x = 128;
    pow1.velocity.y = 183;
    pow2.velocity.x = -64;
    pow2.velocity.y = -132;
    */
   

    for(i=0;i<32;i++){
        center_screen(&arena_map, asprite_idx, &hs, &vs); 
        center_screen(&arena_map, asprite_idx, &hs, &vs); 
        center_screen(&arena_map, asprite_idx, &hs, &vs); 
        center_screen(&arena_map, asprite_idx, &hs, &vs); 
        //BLAST_updateSprites();
        VDP_waitVSync();
    }

    while(1) {
        
        uintToStr(getFPS(), str, 1);
        //center_screen(&arena_map, asprite_idx, &hs, &vs); 
        /*
        uintToStr(terrain_tile_offset, str, 3);
        VDP_drawText(str, 15, 18);
        uintToStr(shot_offset, str, 3);
        VDP_drawText(str, 15, 19);
        */

        // Move player
        myJoyHandler(JOY_1,0,JOY_readJoypad(JOY_1));
        //check_t_collision(lista, 1, &arena_map, t_coll_callback);
        // Move other sprites

        move_sprites(&arena_map, t_coll_callback);
        //check_collision(listc, 2, listc, 2, coll_callback);
        if(frame == 2) {
            check_collision(listd, (NUMSPRITES+2), listd, (NUMSPRITES+2), coll_callback);
            frame = 0;
        } else {
            VDP_clearText(24, 18, 3);
            VDP_drawText(str, 24, 18);

            VDP_waitVSync();
        }
        frame += 1;
        //check_t_collision(listb, 1, &arena_map, t_coll_callback);
        //VDP_clearText(20,19, 20);
        
        //drag_sprites();

        //VDP_showFPS(0);
    }

    return 0;
}

#include "genesis.h"
#include "blast.h"

#include "images.h"
#include "arena.h"
#include "math_tables.h"
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
        //int mask = 1;
        //u16 bitmask = state;
        //u8 coll = 0;
      
        u8 force = 0;
        u8 direction = 0;

        if((state & BUTTON_UP) && (state &  BUTTON_RIGHT)) {
            asprite.direction = 32;
            force = 32;
        } else if ((state & BUTTON_DOWN) && (state & BUTTON_RIGHT)) {
            asprite.direction = 96;
            force = 32;
        } else if ((state & BUTTON_DOWN) && (state & BUTTON_LEFT)) {
            asprite.direction = 160;
            force = 32;
        } else if ((state & BUTTON_UP) && (state & BUTTON_LEFT)) {
            asprite.direction = 224;
            force = 32;
        } else if((state & BUTTON_UP)) {
            asprite.direction = 0;
            force = 32;
        } else if ((state & BUTTON_RIGHT)) {
            asprite.direction = 64;
            force = 32;
        } else if ((state & BUTTON_DOWN)) {
            asprite.direction = 128;
            force = 16;
        } else if ((state & BUTTON_LEFT)) {
            asprite.direction = 192;
            force = 16;
        }
        if(force != 0)
            force_sprite(&asprite, force, asprite.direction);
    }

}


void force_sprite(spritedef* sprt, u8 force, u8 direction) {
    s8 x_delta;
    s8 y_delta;

    y_delta = ((force) * cos_table[direction])>>6;
    x_delta = ((force) * sin_table[direction])>>6;

    sprt->x_vec = x_delta;
    sprt->y_vec = y_delta;
}




void drag_sprites() {
    u8 cur_idx = 0;
    u8 next_idx = 0;
    spritedef* tsprite;

    while(1) {
        tsprite = _sprite_all[cur_idx];
        
        
        switch(tsprite->x_vec) {
            case -127 ... -1:
                tsprite->x_vec += 1;
                break;
            case 1 ... 128:
                tsprite->x_vec -= 1;
                break;
        }

        switch(tsprite->y_vec) {
            case -127 ... -1:
                tsprite->y_vec += 1;
                break;
            case 1 ... 128:
                tsprite->y_vec -= 1;
                break;
        }
        
        
        next_idx = tsprite->link;
        if(next_idx == 0) {
            break;
        }
        cur_idx = next_idx;
    }

}



void move_sprite(spritedef* sprt, blastmap* map, void (*callback)(spritedef* insprt, u8 coll)){
    if((sprt->x_vec == 0) && (sprt->y_vec == 0))
        return;

    /*
    s8 x_delta;
    s8 y_delta;

    y_delta = ((sprt->force) * cos_table[sprt->direction])>>6;
    x_delta = ((sprt->force) * sin_table[sprt->direction])>>6;
    */

    u8 coll = 0;
    if(sprt->x_vec>0) {
        coll = check_right(map, sprt);
        if(coll == NOCOLL || coll == COLL_RIGHT) {
            sprite_right(sprt, (sprt->x_vec>>4), 512);
            //sprt->x_vec -= 1;
        } else {
            sprt->x_vec *= -1;    
            (*callback)(sprt, coll);
        }
    } else if (sprt->x_vec<0) {
        coll = check_left(map, sprt);
        if(coll == NOCOLL || coll == COLL_LEFT) {
            sprite_left(sprt, ((sprt->x_vec*-1)>>4), 512);
            //sprt->x_vec += 1;
        } else {
            sprt->x_vec *= -1;    
            (*callback)(sprt, coll);
        }
    }

    if(sprt->y_vec>0) {
        coll = check_up(map, sprt);
        if(coll == NOCOLL || coll == COLL_UP) {
            sprite_up(sprt, (sprt->y_vec>>4), 512);
            //sprt->y_vec -= 1;
        } else {
            sprt->y_vec *= -1;
            (*callback)(sprt, coll);
        }
    } else if (sprt->y_vec<0) {
        coll = check_down(map, sprt);
        if(coll == NOCOLL || coll == COLL_DOWN) {
            sprite_down(sprt, ((sprt->y_vec*-1)>>4), 512);
            //sprt->y_vec += 1;
        } else {
            sprt->y_vec *= -1;
            (*callback)(sprt, coll);
        }
    }

    //sprt->force -= 1;
    //sprt->force = sprt->force>>1;
}

void move_sprites(blastmap* map, void (*callback)(spritedef* sprt, u8 coll)) {
    u8 cur_idx = 0;
    u8 next_idx = 0;
    spritedef* tsprite;

    while(1) {
        tsprite = _sprite_all[cur_idx];
        //if(tsprite->force != 0) {
        move_sprite(tsprite, map, (*callback));
        //}
        next_idx = tsprite->link;
        if(next_idx == 0) {
            break;
        }
        cur_idx = next_idx;
    }
}

void do_coll(spritedef* sprta, spritedef* sprtb) {
    VDP_drawText(" COL CALLBACK  ", 20,18);
    //sprt->pal = PAL1;
    //sprt->posx -= 2;
    //sprite_left(sprtb,2,512);
    //sprite_right(sprta,1,512);
    


    //   Swap vectors.  Works poorly at low speed

    /*
    s8 temp_x = sprtb->x_vec;
    s8 temp_y = sprtb->y_vec;


    sprtb->x_vec = sprta->x_vec;
    sprtb->y_vec = sprta->y_vec;


    sprta->x_vec = temp_x;
    sprta->y_vec = temp_y;

    */


    s8 xvec = (sprta->x_vec + sprtb->x_vec)/2;
    s8 yvec = (sprta->y_vec + sprtb->y_vec)/2;


    sprtb->x_vec = xvec*=-1;
    sprtb->y_vec = yvec*=-1;

    sprta->x_vec = xvec;
    sprta->y_vec = yvec;

    /*
    if(sprta->posx > sprtb->posx) {
        // Left of
        if((sprta->posx + sprta->width) < sprtb->posx) {
            // Left collision
            force_sprite(sprtb, 32, 64);
        }
    } else {
        // Right of
        if((sprtb->posx + sprtb->width) > sprta->posx) {
            // Right collision
            force_sprite(sprtb, 32, 192);
        } 
    }

    if(sprta->posy > sprtb->posy) {
        // Above
        if((sprta->posy + sprta->height) < sprtb->posy) {
            // Above collision
            force_sprite(sprtb, 32, 128);
        }
    } else {
        // Below
        if((sprtb->posy + sprtb->height) > sprta->posy) {
            // Below collision
            force_sprite(sprtb, 32, 0);
        }
    }
    
    */

    //sprtb->direction = sprta->direction;
    //sprta->x_vec = 0;
    //sprta->y_vec = 0;
    
    //sprta->x_vec *= -1;
    //sprta->y_vec *= -1;



    /*sprtb->direction = sprta->direction;
    if(sprta->force != 0) {
        sprtb->force = sprta->force;
        sprta->force -= 1;
    }
    */


    /*

    switch(sprta->direction) {
        case 0 ... 63:
            //sprite_up(sprtb,2,512);
            force_sprite(sprtb, 32, 0);
            //sprite_down(sprta,1,512);
            break;
        case 64 ... 127:
            force_sprite(sprtb, 32, 64);
            //sprite_right(sprtb,2,512);
            //sprite_left(sprta,1,512);
            break;
        case 128 ... 191:
            force_sprite(sprtb, 32, 128);
            //sprite_down(sprtb,2,512);
            //sprite_up(sprta,1,512);
            break;
        case 192 ... 255:
            force_sprite(sprtb, 32, 192);
            //sprite_left(sprtb,2,512);
            //sprite_right(sprta,1,512);
            break;
    }

    */
}

void terrain_coll(spritedef* sprt, u8 coll_dir) {
    VDP_drawText(" TERRAIN COLLIS  ", 20,19);
    /*if(coll != 0){
        sprt->force=0;
    }*/

    //sprt->direction += 128;
    //sprt->force = 2;

}

void (*coll_callback)(spritedef* sprta, spritedef* sprtb) = &do_coll;
void (*t_coll_callback)(spritedef* sprt, u8 coll) = &terrain_coll;

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
    
    /*
     *
     * Init sprites
     *
     */

    
    u8 lista[1];
    u8 listb[2];
    u8 listc[3];

    asprite_idx = sprite_init(&asprite,terrain_tile_offset+7,1,256,256,1,1,PAL0);
    lista[0] = asprite_idx;


    bsprite_idx = sprite_init(&bsprite,terrain_tile_offset+7,1,220,240,1,1,PAL1);
    pow_idx = sprite_init(&pow,shot_offset,1,276,265,1,1,PAL0);

    listb[0] = bsprite_idx;
    listb[1] = pow_idx;

    listc[0] = asprite_idx;
    listc[1] = bsprite_idx;
    listc[2] = pow_idx;

    coly_idx = sprite_init(&coly,shot_offset,1,5,5,1,1,PAL0);
   
    int i;
    for(i=0;i<32;i++){
        center_screen(&arena_map, asprite_idx, &hs, &vs); 
        center_screen(&arena_map, asprite_idx, &hs, &vs); 
        center_screen(&arena_map, asprite_idx, &hs, &vs); 
        center_screen(&arena_map, asprite_idx, &hs, &vs); 
        //BLAST_updateSprites();
        VDP_waitVSync();
    }

    bsprite.direction = 160;
    bsprite.force = 16;
    force_sprite(&bsprite, 60, 160);
    force_sprite(&asprite, 36, 160);
    force_sprite(&pow, 86, 73);
    while(1) {

        // Move player
        myJoyHandler(JOY_1,0,JOY_readJoypad(JOY_1));
        //check_t_collision(lista, 1, &arena_map, t_coll_callback);

        // Move other sprites

        if(spr_coll == 1) {
            coly.posy = (coll_vcnt - 4);
            VDP_drawText(" VDP COLLIS    ", 20,19);
            check_collision(lista, 1, listb, 2, coll_callback);
            //check_collision(listc, 3, listc, 3, coll_callback);
            //check_collision(listb, 2, lista, 1, coll_callback);
            spr_coll = 0;
        } else {
            VDP_drawText("               ", 20,18);
            VDP_drawText("               ", 20,19);
            VDP_drawText("               ", 30,35);
            VDP_drawText("               ", 30,36);
        };

        //check_t_collision(listb, 1, &arena_map, t_coll_callback);
        move_sprites(&arena_map, t_coll_callback);
        drag_sprites();

        //force_sprite(&asprite, 1, 128);

        VDP_showFPS(0);
        VDP_waitVSync();
    }

    return 0;
}

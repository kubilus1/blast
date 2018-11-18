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
    // Move sprite in the same direction
    switch(sprta->direction) {
        case 0:
        case 32:
            // UP
            if(sprta->posy > sprtb->posy){
                sprtb->direction = 0;
                sprtb->force = 1;
                sprta->force = 0;
            }
            break;
        case 64:
        case 96:
            // RIGHT
            if(sprta->posx < sprtb->posx){
                sprtb->direction = 64;
                sprtb->force = 1;
                sprta->force = 0;
            }
            break;
        case 128:
        case 160:
            // DOWN
            if(sprta->posy < sprtb->posy){
                sprtb->direction = 128;
                sprtb->force = 1;
                sprta->force = 0;
            }
            break;
        case 192:
        case 224:
            // LEFT
            if(sprta->posx > sprtb->posx){
                sprtb->direction = 192;
                sprtb->force = 1;
                sprta->force = 0;
            }
            break;
    }
    //sprtb->direction = sprta->direction;
    //sprtb->force = 1;
    //sprta->force = 0;
}
void terrain_coll(spritedef* sprt, u8 coll) {
    VDP_drawText(" T COLLIS    ", 2,2);
    switch(sprt->direction) {
        case 0:
            if(coll != COLL_UP)
                sprt->force = 0;
            break;
        case 64:
            if(coll != COLL_RIGHT)
                sprt->force = 0;
            break;
        case 128:
            if(coll != COLL_DOWN)
                sprt->force = 0;
            break;
        case 192:
            if(coll != COLL_LEFT)
                sprt->force = 0;
            break;
        default:
            sprt->force = 0;
    }
}

void (*coll_callback)(spritedef* sprta, spritedef* sprtb) = &do_coll;
void (*t_coll_callback)(spritedef* sprt, u8 coll) = &terrain_coll;

void myJoyHandler(u16 joy, u16 changed, u16 state) {
    //VDP_drawTextBG(VDP_PLAN_A,"JOY   ",0x8000,1,1);
    if (joy == JOY_1) {
        //int mask = 1;
        //u16 bitmask = state;
        //u8 coll = 0;
       
        if((state & BUTTON_UP) && (state &  BUTTON_RIGHT)) {
            asprite.direction = 32;
            asprite.force = 1;
        } else if ((state & BUTTON_DOWN) && (state & BUTTON_RIGHT)) {
            asprite.direction = 96;
            asprite.force = 1;
        } else if ((state & BUTTON_DOWN) && (state & BUTTON_LEFT)) {
            asprite.direction = 160;
            asprite.force = 1;
        } else if ((state & BUTTON_UP) && (state & BUTTON_LEFT)) {
            asprite.direction = 224;
            asprite.force = 1;
        } else if((state & BUTTON_UP)) {
            asprite.direction = 0;
            asprite.force = 1;
        } else if ((state & BUTTON_RIGHT)) {
            asprite.direction = 64;
            asprite.force = 1;
        } else if ((state & BUTTON_DOWN)) {
            asprite.direction = 128;
            asprite.force = 1;
        } else if ((state & BUTTON_LEFT)) {
            asprite.direction = 192;
            asprite.force = 1;
        }
    }
}

void move_sprite(spritedef* sprt) {
    switch(sprt->direction) {
        case 0:
            // UP
            sprite_up(sprt,sprt->force,512);
            break;
        case 32:
            // UP RIGHT
            sprite_up(sprt,sprt->force,512);
        case 64:
            // RIGHT
            sprite_right(sprt,sprt->force,512);
            break;
        case 96:
            // DOWN RIGHT
            sprite_right(sprt,sprt->force,512);
        case 128:
            // DOWN
            sprite_down(sprt,sprt->force,512);
            break;
        case 160:
            // DOWN LEFT
            sprite_down(sprt,sprt->force,512);
        case 192:
            // LEFT
            sprite_left(sprt,sprt->force,512);
            break;
        case 224:
            // UP LEFT
            sprite_left(sprt,sprt->force,512);
            sprite_up(sprt,sprt->force,512);
            break;

    }
    sprt->force = 0;
}


void move_sprites() {
    u8 cur_idx = 0;
    u8 next_idx = 0;
    spritedef* tsprite;

    while(1) {
        tsprite = _sprite_all[cur_idx];
        if(tsprite->force != 0) {
            move_sprite(tsprite);
        }
        next_idx = _sprite_all[cur_idx]->link;
        if(next_idx == 0) {
            break;
        }
        cur_idx = next_idx;
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

    //uintToStr(asprite_idx, str, 2);
    //BMP_drawText(str, 2, 2);

    while(1) {
        // Keep screen centered around a sprite
        center_screen(&terrain1map, asprite_idx, &hs, &vs); 
        
        // Get joystick input
        myJoyHandler(JOY_1,0,JOY_readJoypad(JOY_1));

        // Check for primary sprite collisions
        check_t_collision(lista, 1, &terrain1map, t_coll_callback);

        if(spr_coll == 1) {
            //VDP_drawText(" VDP COLLIS    ", 20,19);
            // If we have any VDP collision, dig deeper
            check_collision(lista, 1,  listb, 2, coll_callback);
            spr_coll = 0;
        }
        // Check other sprite collisions 
        check_t_collision(listb, 2, &terrain1map, t_coll_callback);

        // Move all sprites to new positions
        move_sprites(); 

        VDP_showFPS(0);
        VDP_waitVSync();
    }

    return 0;
}

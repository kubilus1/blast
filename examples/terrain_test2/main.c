//#include "genesis.h"
#include "blast.h"

#include "images.h"
//#include "terrain_map1.h"
#include "terrain1.h"

u16 terrain_tile_offset = 0;
blastmap terrain1map;
u8 asprite_idx;
spritedef asprite;

u8 bsprite_idx;
spritedef bsprite;

u8 csprite_idx;
spritedef csprite;

manifold coll_manifold;

//u8* colls;
u16 hscroll;
u16 vscroll;
char str[10];

u8 lista[3];
//u8 listb[2];

void do_coll(spritedef* sprta, spritedef* sprtb) {
    // Move sprite in the same direction
    VDP_drawText(" COL CALLBACK  ", 1,1);
    if(get_circle_manifold(sprta, sprtb, &coll_manifold)) {
        sprite_bounce(sprta, sprtb, &coll_manifold);
    }

    return;
}
void terrain_coll(spritedef* sprt, u8 coll) {
    // Some kind of terrain collision detected
#ifdef FOO
    VDP_drawText(" T COLLIS    ", 2,2);
    uintToStr(coll, str, 3);
    VDP_drawText(str, 10, 2); 
    return;
#endif
}

void (*coll_callback)(spritedef* sprta, spritedef* sprtb) = &do_coll;
void (*t_coll_callback)(spritedef* sprt, u8 coll) = &terrain_coll;

void myJoyHandler(u16 joy, u16 changed, u16 state) {
    //VDP_drawTextBG(VDP_PLAN_A,"JOY   ",0x8000,1,1);
    if (joy == JOY_1) {
        if((state & BUTTON_UP)) {
            asprite.velocity.y = -64;
        } else if ((state & BUTTON_DOWN)) {
            asprite.velocity.y = 64;
        } 
            
        if ((state & BUTTON_RIGHT)) {
            asprite.velocity.x = 64;
        } else if ((state & BUTTON_LEFT)) {
            asprite.velocity.x = -64;
        }
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
    /*
    terrain_tile_offset = load_bmp(
            (u32*) terrain1.image,
            terrain1.w,
            terrain1.h
    );
    */

    SYS_disableInts();
    terrain_tile_offset = load_img(&terrain1);

    blastmap_init(&terrain1map, &terrain1_map, (u16*)terrain1.map->tilemap, terrain_tile_offset, PLAN_A);
    load_visible_map(&terrain1map,0,0);

    SYS_enableInts();
    hscroll = -(terrain1map.tX * 8);
    vscroll = terrain1map.tY * 8;
    
    /*
     *
     * Init sprites
     *
     */
    
    asprite_idx = sprite_init(&asprite,terrain_tile_offset+5,1,8,8,1,1,PAL0);
    bsprite_idx = sprite_init(&bsprite,terrain_tile_offset+5,1,32,32,1,1,PAL1);
    csprite_idx = sprite_init(&csprite,terrain_tile_offset+5,1,48,48,1,1,PAL2);

    /*
    while(1) {
        VDP_waitVSync();
    }
    */
    //u8 listb[2];

    lista[0] = asprite_idx;
    lista[1] = bsprite_idx;
    lista[2] = csprite_idx;

    //uintToStr(asprite_idx, str, 2);
    //BMP_drawText(str, 2, 2);
    asprite.inv_mass = FIX16(0.5);
    bsprite.inv_mass = FIX16(0.5);
    csprite.inv_mass = FIX16(0.5);
   
#ifdef FOO
    int i; 
    for(i=0;i<32;i++){
        center_screen(&terrain1map, asprite_idx, &hs, &vs); 
        VDP_waitVSync();
    }
#endif

    while(1) {
        // Keep screen centered around a sprite
        center_screen(&terrain1map, asprite_idx, &hs, &vs); 
        
        // Get joystick input
        myJoyHandler(JOY_1,0,JOY_readJoypad(JOY_1));

        // Check for primary sprite collisions
        //check_t_collision(lista, 1, &terrain1map, t_coll_callback);

            //VDP_drawText(" VDP COLLIS    ", 20,19);
            // If we have any VDP collision, dig deeper
        check_collision(lista, 3,  lista, 3, coll_callback);
        move_sprites(&terrain1map, t_coll_callback); 
        // Check other sprite collisions 
        // Move all sprites to new positions
        drag_sprites(8);

        SYS_disableInts();
        VDP_showFPS(0);
        SYS_enableInts();

        VDP_waitVSync();
    }

    return 0;
}

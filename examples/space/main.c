#include "blast.h"
#include "images.h"
#include "newspace.h"

u8 frame = 0;

u16 newspace_tile_offset;
u16 shot_tile_offset;
u16 enemy_tile_offset;
u16 enterprise_tile_offset;

blastmap newspacemap;

spritedef enterprise_sprite;
spritedef shot_sprite;
spritedef ship_sprite;
u8 enterprise_sprite_idx;
u8 shot_sprite_idx;
u8 ship_sprite_idx;

#define MAX_SHOTS 20
#define MAX_ENEMIES 8

typedef struct {
    u8 maxshots;
    u8 numshots;
    u8 curshot;
    spritedef shots[MAX_SHOTS];
    u8 ttls[MAX_SHOTS];
    u8 sprite_ids[MAX_SHOTS];
} shotlistdef;

shotlistdef myshots;
shotlistdef theirshots;

u8 num_badships = 0; 
//u8_Stack* badship_ids;
u8_Stack* badship_free_ids;
//u8* badship_ids = {0,1,2,3};
spritedef bad_shiplist[MAX_ENEMIES];

//u8 myshots_coll_list[MAX_SHOTS];
//u8 badshots_coll_list[MAX_SHOTS];
u8 me_coll_list[1];
u8 bad_coll_list[MAX_ENEMIES];

typedef enum {
    SHIP_ALIVE, SHIP_DEAD, SHIP_REMOVED
} ship_state;

typedef enum {
    UP, DOWN, LEFT, RIGHT, UPLEFT, UPRIGHT, DOWNLEFT, DOWNRIGHT
} direction;

typedef struct {
    u8 hitpoints;
    u8 cooldown;
    u8 cooldown_max;
    ship_state state;
} objprops;

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

char str[10];
void myJoyHandler(u16 joy, u16 changed, u16 state) {
    //VDP_drawTextBG(VDP_PLAN_A,"JOY   ",0x8000,1,1);
    if (joy == JOY_1) {

        if((state & BUTTON_UP) && (state & BUTTON_RIGHT)) {
            enterprise_sprite.velocity.y += -8;
            enterprise_sprite.velocity.x += 8;
            enterprise_sprite.curstep = 1;
            enterprise_sprite.direction = 32;
            flip_sprite(&enterprise_sprite, 0, 0);
        } else if ((state & BUTTON_UP) && (state & BUTTON_LEFT)) {
            enterprise_sprite.velocity.y += -8;
            enterprise_sprite.velocity.x += -8;
            enterprise_sprite.curstep = 1;
            enterprise_sprite.direction = 224;
            flip_sprite(&enterprise_sprite, 1, 0);
        } else if ((state & BUTTON_DOWN) && (state & BUTTON_RIGHT)) {
            enterprise_sprite.velocity.y += 8;
            enterprise_sprite.velocity.x += 8;
            enterprise_sprite.curstep = 1;
            enterprise_sprite.direction = 96;
            flip_sprite(&enterprise_sprite, 0, 1);
        } else if ((state & BUTTON_DOWN) && (state & BUTTON_LEFT)) {
            enterprise_sprite.velocity.y += 8;
            enterprise_sprite.velocity.x += -8;
            enterprise_sprite.curstep = 1;
            enterprise_sprite.direction = 160;
            flip_sprite(&enterprise_sprite, 1, 1);

        } else if((state & BUTTON_UP)) {
            enterprise_sprite.velocity.y += -8;
            enterprise_sprite.curstep = 0;
            enterprise_sprite.direction = 0;
            flip_sprite(&enterprise_sprite, 0, 0);
        } else if ((state & BUTTON_DOWN)) {
            enterprise_sprite.velocity.y += 8;
            enterprise_sprite.curstep = 0;
            enterprise_sprite.direction = 128;
            flip_sprite(&enterprise_sprite, 0, 1);
            
        } else if ((state & BUTTON_RIGHT)) {
            enterprise_sprite.velocity.x += 8;
            enterprise_sprite.curstep = 2;
            enterprise_sprite.direction = 64;
            flip_sprite(&enterprise_sprite, 0, 0);
        } else if ((state & BUTTON_LEFT)) {
            enterprise_sprite.velocity.x += -8;
            enterprise_sprite.curstep = 2;
            enterprise_sprite.direction = 192;
            flip_sprite(&enterprise_sprite, 1, 0);
        }

        if (enterprise_sprite.velocity.x > 512) {
            enterprise_sprite.velocity.x = 512;
        } else if (enterprise_sprite.velocity.x < -512) {
            enterprise_sprite.velocity.x = -512;
        }
        if (enterprise_sprite.velocity.y > 512) {
            enterprise_sprite.velocity.y = 512;
        } else if(enterprise_sprite.velocity.y < -512) {
            enterprise_sprite.velocity.y = -512;
        }

        if (state & BUTTON_C) {
            shoot(&enterprise_sprite, &myshots);
        }
    }
}

void force_sprite(spritedef* sprt, s16 force, u8 direction) {
    s16 x_delta;
    s16 y_delta;

    y_delta = ((force) * cos_table[direction])>>6;
    x_delta = ((force) * sin_table[direction])>>6;

    sprt->velocity.x += x_delta;
    sprt->velocity.y += (-1 * y_delta);
    //sprt->velocity.y = (y_delta);
    //
    return;
}

u8 vec_2_deg(s16 x, s16 y) {

    u16 ax = abs(x);
    u16 ay = abs(y);
    u8 atan_lookup;

    if(x>0 && y<0) {
        if(ay>ax) {
            atan_lookup = (u8)((ax<<4)/ay);
            return atan_table[atan_lookup];
        } else if(x>y) { 
            atan_lookup = ((ay<<4)/ax);
            //return (64 - atan_table((ay<<4)/ax));
            return (64 - atan_table[atan_lookup]);
        } else {
            return 32;
        }
    } else if (x>0 && y>0) {
        if(ay>ax) {
            atan_lookup = ((ax<<4)/ay);
            //return (128 - atan_table((ax<<4)/ay));
            return (128 - atan_table[atan_lookup]);
        } else if(ax>ay) {    
            atan_lookup = ((ay<<4)/ax);
            return (64 + atan_table[atan_lookup]);
        } else {
            return 96;
        }
    } else if (x<0 && y>0) {
        if(ay>ax) {
            return (128 + atan_table[(ax<<4)/ay]);
        } else if(ax>ay) {    
            return (192 - atan_table[(ay<<4)/ax]);
        } else {
            return 160;
        }
    } else if (x<0 && y<0) {
        if(ay>ax) {
            return (255 - atan_table[(ax<<4)/ay]);
        } else if(ax>ay) {    
            return (192 + atan_table[(ay<<4)/ax]);
        } else {
            return 224;
        }
    } else if (x==0 && y<0) {
        return 0;
    } else if (x==0 && y>0) {
        return 128;
    } else if (y==0 && x>0) {
        return 64;
    } else if (y==0 && x<0) {
        return 192;
    } else {
        return 0;
    }

};

void expire_shots(shotlistdef* shotlist) {
    u8 remainingshots; 
    remainingshots = shotlist->numshots;
    u8 startidx; 
    startidx = shotlist->curshot;
    u8 collidx = 0;

    while(remainingshots > 0) {
        if(startidx == 0) {
            startidx = shotlist->maxshots;
        }
        startidx--;
        shotlist->ttls[startidx]--;
        remainingshots--;
        
        /*
        uintToStr(shotlist->ttls[startidx], str, 3);
        VDP_drawText(str, 15, 12+startidx);
        */
        //uintToStr(shotlist->shots[startidx].idx, str, 3);
        //VDP_drawText(str, 15, 12+startidx);
        if(shotlist->ttls[startidx] == 0){
            //shotlist->ttls[startidx] = 15;
            //shotlist->sprite_ids[collidx++] = shotlist->shots[startidx].idx;
            //Expired, remove it
            drop_sprite(shotlist->shots[startidx].idx);
            shotlist->numshots--;
            //VDP_drawText("D", 14, 12+startidx);
        } else {
           // uintToStr(shotlist->shots[startidx].idx, str, 3);
           // VDP_drawText(str, 20, 12+collidx);
           // VDP_drawText(" ", 19, 12+collidx);
           // If we aren't expiring it, add to the collision check list
           shotlist->sprite_ids[collidx++] = shotlist->shots[startidx].idx;
           // VDP_drawText("C", 19, 12+collidx);
        }

    }
}

void debug() {
    return;
    int i = 0;
    int cur_idx = 0;
    int next_idx = 0;
    spritedef* sprite;

    while(1) {
        sprite = _sprite_all[cur_idx];

        uintToStr(sprite->link, str, 3);
        VDP_drawText("links", 5, 7);
        VDP_drawText(str, 5, 8+i);

        next_idx = sprite->link;
        i++;
        if(next_idx == 0)
        {
            VDP_drawText("done", 5, 8+i);
            break;
        }
        cur_idx = next_idx;
    }
}

void shoot(spritedef* sprt, shotlistdef* shotlist) {

     
    if (((objprops*) sprt->properties)->cooldown > 0) {
        ((objprops*) sprt->properties)->cooldown--;
        return;
    }

    if (shotlist->numshots >= shotlist->maxshots) {
        return;
        //shotlist->shots[shotlist->curshot].posx = sprt->posx;
        //shotlist->shots[shotlist->curshot].posy = sprt->posy;
    } else {
        sprite_init(
            &shotlist->shots[shotlist->curshot],
            shot_tile_offset,
            1,
            sprt->posx,
            sprt->posy,
            1,1,PAL0
        );
        shotlist->numshots++;
        //VDP_drawText(" ", 14, 12+shotlist->curshot);
    }

    ((objprops*) sprt->properties)->cooldown = ((objprops*) sprt->properties)->cooldown_max;
    shotlist->ttls[shotlist->curshot] = 15;
    force_sprite(&shotlist->shots[shotlist->curshot], 792, sprt->direction);
    shotlist->curshot++;
    if (shotlist->curshot >= shotlist->maxshots) {
        shotlist->curshot = 0;
    }
}

void start_bad_ship() {

    if(num_badships >= MAX_ENEMIES) {
        return;
    }

    //u8 badship_idx = badship_ids[((MAX_ENEMIES - num_badships)-1)];

    // Get a free id
    u8 badship_idx = u8_stack_pop(badship_free_ids);
    //uintToStr(badship_idx, str, 3);
    //VDP_drawText(str, 15, 17+badship_idx);

    //u8 badship_idx = num_badships;

    //bad_coll_list[badship_idx] = 
    
    sprite_init(
        &bad_shiplist[badship_idx],
        enemy_tile_offset,
        3,
        random() & 64 + 150,
        random() & 64 + 150,
        2,2,PAL0
    );
    num_badships++;


    //uintToStr(bad_coll_list[badship_idx], str, 3);
    //VDP_drawText(str, 8, 12+badship_idx);
    //objprops tprops;
    //tprops.hitpoints = 3;
    spritedef* tspr;
    tspr = &bad_shiplist[badship_idx];
    //objprops* (tspr.properties) = (objprops*)malloc(sizeof(objprops));
    //tspr->properties = (objprops*)MEM_alloc(sizeof(objprops));
    ((objprops*) tspr->properties)->hitpoints = 15;
    ((objprops*) tspr->properties)->cooldown = 0;;
    ((objprops*) tspr->properties)->cooldown_max = 30;;
    ((objprops*) tspr->properties)->state = SHIP_ALIVE;
    //bad_shiplist[badship_idx].properties->hitpoints = 3;
    //uintToStr(((objprops*) tspr->properties)->hitpoints, str, 3);
    //VDP_drawText(str, 15, 17);
    u8 i;
    u8 j=0;
    for (i=0;i<MAX_ENEMIES;i++) {
        tspr = &bad_shiplist[i];
        if( ((objprops*) tspr->properties)->hitpoints > 0 ) {
            bad_coll_list[j] = tspr->idx;
            j++;
        }
    }
    process_badships(bad_coll_list, num_badships);
}

void stop_bad_ships(spritedef* inspr) {

    spritedef* tspr;
    u8 i;
    u8 j=0;
    for (i=0;i<MAX_ENEMIES;i++) {
        tspr = &bad_shiplist[i];
        if( ((objprops*) tspr->properties)->state == SHIP_ALIVE ) {
            // Save the live ship on the collision list
            bad_coll_list[j] = tspr->idx;
            j++;
        } else if( ((objprops*) tspr->properties)->state == SHIP_DEAD ) {
            // Indicate the ID is now free
            u8_stack_push(badship_free_ids, i);
            ((objprops*) tspr->properties)->state=SHIP_REMOVED;
            // Drop the sprite
            //MEM_free(tspr->properties);
            drop_sprite(tspr->idx);
            num_badships--;
        }
    }
    
}

void attract(spritedef* spritea, spritedef* spriteb) {
    // Point A towards B

    s16 dy = spriteb->posy - spritea->posy;
    s16 dx = spriteb->posx - spritea->posx;
   
    u8 deg = vec_2_deg(dx, dy);
    if (abs(dx)>50 | abs(dy)>50) {
        spritea->direction = deg;
        force_sprite(spritea, 4, deg);
    }
    
    if (deg > 16 && deg <= 48) {
        // up right
        spritea->curstep = 1;
        flip_sprite(spritea, 0, 0);
    } else if (deg > 48 && deg <= 80) {
        // right
        spritea->curstep = 2;
        flip_sprite(spritea, 0, 0);
    } else if (deg > 80 && deg <= 112) {
        // down right
        spritea->curstep = 1;
        flip_sprite(spritea, 0, 1);
    } else if (deg > 112 && deg <= 134) {
        // down
        spritea->curstep = 0;
        flip_sprite(spritea, 0, 1);
    } else if (deg > 134 && deg <= 176) {
        // down left
        spritea->curstep = 1;
        flip_sprite(spritea, 1, 1);
    } else if (deg > 176 && deg <= 208) {
        // left
        spritea->curstep = 2;
        flip_sprite(spritea, 1, 0);
    } else if (deg > 208 && deg <= 240) {
        // up left
        spritea->curstep = 1;
        flip_sprite(spritea, 1, 0);
    } else {
        // up
        spritea->curstep = 0;
        flip_sprite(spritea, 0, 0);
    }
}

void attract2(spritedef* spritea, spritedef* spriteb) {
    direction dir;

    if(spritea->posy > spriteb->posy) {
        // above
        dir=UP;
        spritea->velocity.y -= 6;
    } else {
        dir=DOWN;
        // below
        spritea->velocity.y += 6;
    }
    if (spritea->posx > spriteb->posx) {
        // left of
        flip_sprite(spritea, 1, 0);
        spritea->velocity.x -= 6;
        spritea->direction = 64;
    } else {
        // right of
        flip_sprite(spritea, 0, 0);
        spritea->velocity.x += 6;
        spritea->direction = 192;
    }
    if(abs(spritea->velocity.x) > abs(spritea->velocity.y)){
        spritea->curstep = 2;
    } else {
        spritea->curstep = 0;
        if(dir==UP) {
            spritea->direction=0;
            flip_sprite(spritea, 0, 0);
        } else {
            spritea->direction=128;
            flip_sprite(spritea, 0, 1);
        }
    }

    if (spritea->velocity.x > 512) {
        spritea->velocity.x = 512;
    } else if (spritea->velocity.x < -512) {
        spritea->velocity.x = -512;
    }
    if (spritea->velocity.y > 512) {
        spritea->velocity.y = 512;
    } else if(spritea->velocity.y < -512) {
        spritea->velocity.y = -512;
    }

}


void process_badships(u8* in_badships, u8 numships) {
    spritedef* tspr;
    u8 i;
    s16 dy;
    s16 dx;
    //uintToStr(numships, str, 3);
    //VDP_drawText(str, 10, 11);
    for(i=0;i<numships;i++) {
        tspr = _sprite_all[in_badships[i]];
        attract(tspr, &enterprise_sprite);
        dy = tspr->posy - enterprise_sprite.posy;
        dx = tspr->posx - enterprise_sprite.posx;
        if((abs(dy)<100) && (abs(dx)<100)) {
            shoot(tspr, &theirshots);
        }
    }
}

void do_coll(spritedef* sprta, spritedef* sprtb) {

    //objprops* tprops;
    //tprops = &(sprtb->properties);
    //uintToStr(((objprops*) sprtb->properties)->hitpoints, str, 3);
    //VDP_drawText(str, 15, 18);

    if(((objprops*) sprtb->properties)->hitpoints == 0) {
        if( ((objprops*) sprtb->properties)->state == SHIP_ALIVE ) {
            ((objprops*) sprtb->properties)->state=SHIP_DEAD;
            stop_bad_ships(sprtb);
        }
    } else if(((objprops*) sprtb->properties)->hitpoints < 5) {
        sprtb->tile_attr = TILE_ATTR_FULL(
            PAL1,
            1,
            sprtb->v_attr,
            sprtb->h_attr,
            sprtb->startaddr+(sprtb->tilesize*(sprtb->curstep % sprtb->steps))
        );
        ((objprops*) sprtb->properties)->hitpoints--;
    } else {
        ((objprops*) sprtb->properties)->hitpoints--;
        //tprops->hitpoints--;
    }
      
    // Move sprite in the same direction
/*    VDP_drawText(" COL CALLBACK  ", 1,1);
    if(get_circle_manifold(sprta, sprtb, &coll_manifold)) {
        sprite_bounce(sprta, sprtb, &coll_manifold);
    }

    return;
    */
}
void terrain_coll(spritedef* sprt, u8 coll) {
    // Some kind of terrain collision detected
}

void (*coll_callback)(spritedef* sprta, spritedef* sprtb) = &do_coll;
void (*t_coll_callback)(spritedef* sprt, u8 coll) = &terrain_coll;

void showFPS(VDPPlan plan, u16 float_display)
{
    char str[16];
    SYS_disableInts();
    if (float_display)
    {
        fix32ToStr(getFPS_f(), str, 1);
        VDP_clearText(2, 1, 5);
    }
    else
    {
        uintToStr(getFPS(), str, 1);
        VDP_clearText(2, 1, 2);
    }

    // display FPS
    VDP_drawTextBG(plan,"     ",1,1);
    VDP_drawTextBG(plan,str,1,1);
    SYS_enableInts();
}

void init() {
    blast_init();
    SYS_disableInts();

    VDP_setPalette(PAL0, (u16*)space.palette->data);
    newspace_tile_offset = load_img(&space);
    enemy_tile_offset = load_img(&spaceship);
    shot_tile_offset = load_img(&shot_0);
    enterprise_tile_offset = load_img(&enterprise);

    blastmap_init(
            &newspacemap, 
            &space1_map, 
            (u16*)space.map->tilemap, 
            newspace_tile_offset, 
            PLAN_A
    );
    load_visible_map(&newspacemap,32,32);
    
    //Window Plane 2 columns from top
    VDP_setReg(18,2);
    VDP_clearPlan(PLAN_B, 0);

    SYS_enableInts();
}

int main() {

    init();

    enterprise_sprite_idx = sprite_init(&enterprise_sprite, enterprise_tile_offset, 3,32, 32, 2, 2, PAL0);
    enterprise_sprite.properties = (objprops*)MEM_alloc(sizeof(objprops));
    ((objprops*) enterprise_sprite.properties)->hitpoints = 30;
    ((objprops*) enterprise_sprite.properties)->cooldown = 0;
    ((objprops*) enterprise_sprite.properties)->cooldown_max = 5;
    ((objprops*) enterprise_sprite.properties)->state = SHIP_ALIVE;
    me_coll_list[0] = enterprise_sprite_idx;

    myshots.numshots = 0;
    myshots.curshot = 0;
    myshots.maxshots = 8;

    theirshots.numshots = 0;
    theirshots.curshot = 0;
    theirshots.maxshots = 8;

    badship_free_ids = u8_stack_new();

    int i;
    spritedef* tspr;
    for(i=0;i<MAX_ENEMIES;i++) {
        u8_stack_push(badship_free_ids, i); 
        tspr = &bad_shiplist[i];
        //objprops* (tspr.properties) = (objprops*)malloc(sizeof(objprops));
        tspr->properties = (objprops*)MEM_alloc(sizeof(objprops));
        ((objprops*) tspr->properties)->hitpoints = 0;
        ((objprops*) tspr->properties)->state = SHIP_REMOVED;
    }

    while(1) {
        //uintToStr(getFPS(), str, 1);
        //VDP_showFPS(0);
        showFPS(PLAN_WINDOW,1);
        // Keep screen centered around a sprite
        //center_screen(&terrain1map, asprite_idx, &hs, &vs); 

        //u8 bfi_s = u8_stack_size(badship_free_ids);
        //uintToStr(bfi_s, str, 3);
        //VDP_drawText(str, 15, 12);
        
        center_screen(&newspacemap, enterprise_sprite_idx, &hs, &vs); 
        
        expire_shots(&myshots);
        expire_shots(&theirshots);
        // Get joystick input
        myJoyHandler(JOY_1,0,JOY_readJoypad(JOY_1));
        
        if(frame == 2) {
            process_badships(bad_coll_list, num_badships);
            frame = 0;
        } else {
            check_collision(myshots.sprite_ids, myshots.numshots, bad_coll_list, num_badships, coll_callback); 
            check_collision(theirshots.sprite_ids, theirshots.numshots, me_coll_list, 1, coll_callback); 
        }

        // Check for primary sprite collisions
        //check_t_collision(lista, 1, &terrain1map, t_coll_callback);

            //VDP_drawText(" VDP COLLIS    ", 20,19);
            // If we have any VDP collision, dig deeper
        //check_collision(lista, 3,  lista, 3, coll_callback);
        
        move_sprites(&newspacemap, t_coll_callback); 
        
        // Check other sprite collisions 
        // Move all sprites to new positions
        drag_sprites(1);

        //VDP_showFPS(0);
        //screen_up(&newspacemap, &vs);
        //screen_left(&newspacemap, &hs);
        if(num_badships < MAX_ENEMIES) {
        //if(num_badships < 1) {
            if(random() % 90 == 0){
                start_bad_ship(&bad_shiplist);
            }
        }
        //debug();
        VDP_waitVSync();
        frame++;
    }

    return 0;
}

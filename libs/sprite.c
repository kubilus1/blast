#include "genesis.h"
#include "blast.h"

char abuf[50];
// Serve as the sprite counter
u8 _sprite_count = 0;
u16 spriteNum;

//SpriteDef vdpSpriteCache[MAX_SPRITE];

void BLAST_setSpriteP(u16 index, const spritedef *sprite)
{
}

void BLAST_updateSprites()
{
    vu32 *plctrl;
    vu16 *pwdata;
    //SpriteDef *sprite;
    spritedef *sprite;
    u16 i;
    int cur_idx = 0;
    int next_idx = 0;


    if (_sprite_count == 0) return;

    VDP_setAutoInc(2);

    /* Point to vdp port */
    plctrl = (u32 *) GFX_CTRL_PORT;
    pwdata = (u16 *) GFX_DATA_PORT;

    *plctrl = GFX_WRITE_VRAM_ADDR(SLIST);



    while(1)
    {
        sprite = _sprite_all[cur_idx];
        // y position
        *pwdata = 0x80 + sprite->posy;
        // size & link
        *pwdata = (sprite->size << 8) | sprite->link;
        // tile attribut
        *pwdata = sprite->tile_attr;
        // x position
        *pwdata = 0X80 + sprite->posx;

        next_idx = sprite->link;
        if(next_idx == 0)
        {
            break;
        }
        cur_idx = next_idx;
    }


    /*
    sprite = &vdpSpriteCache[0];
    i = spriteNum;
    while(i--)
    {
        // y position
        *pwdata = 0x80 + sprite->posy;
        // size & link
        *pwdata = (sprite->size << 8) | sprite->link;
        // tile attribut
        *pwdata = sprite->tile_attr;
        // x position
        *pwdata = 0X80 + sprite->posx;

        // next sprite
        sprite++;
    }

    // we won't upload unmodified sprite
    spriteNum = 0;
    */
}

void sprite_setrowcol(spritedef* sprite) {
    
    sprite->columns[0] = sprite->posx/8;
    sprite->columns[1] = (sprite->posx + sprite->width)/8;
    //sprite->rows[0] = sprite->posy/8;
    //sprite->rows[1] = (sprite->posy + sprite->height)/8;

    sprite->row_mask = 0;
    sprite->row_mask |= 1U << (sprite->posy/8);
    sprite->row_mask |= 1U << ((sprite->posy + sprite->height)/8);
    /*     
    VDP_drawText("           ", 18, 16);
    VDP_drawText("           ", 18, 17);
    //uintToStr(coll_row_mask, abuf, 3);
    uintToStr(sprite->posx, abuf, 3);
    VDP_drawText(abuf, 18, 16);
    //uintToStr(sprite->row_mask, abuf, 3);
    uintToStr((sprite->posx + sprite->width), abuf, 3);
    VDP_drawText(abuf, 18, 17);
    */
}

u16 sprite_init(spritedef* sprite, u16 addr, u16 steps, s16 x, s16 y, u8 w, u8 h, u8 pal) {
    sprite->posx = x;
    sprite->posy = y;
    sprite->size = SPRITE_SIZE(w,h);
    sprite->width = w*8;
    sprite->height = h*8;
    sprite->tile_width = w;
    sprite->tile_height = h;
    sprite->startaddr = addr;
    sprite->tile_attr = TILE_ATTR_FULL(pal, 1, 0, 0, addr);
    sprite->link = 0;
    sprite->prev_link = 0;
    sprite->curstep = 0;
    sprite->steps = steps;
    sprite->tilesize = w*h;
    sprite->pal = pal;
    

    // This doesn't appear to be correct after moving screen
    sprite->vposx = x + hs;
    sprite->vposy = y + vs;
    
    sprite->idx = add_sprite(sprite);
    //sprite->coll_list = coll_list;
    
    sprite->direction=0;
    sprite->force=0;

    sprite->column_mask = 0;

    /*
    sprite->minx = x;
    sprite->maxx = x+w;
    sprite->miny = y;
    sprite->maxy = y+h;
    */


    sprite_setrowcol(sprite);

    /*
    u16 i;
    if(((sprite->posx) % 8) == 0) {
        for(i=0;i<sprite->width;i++) {
            sprite->coll_list[(sprite->posx/8) + i] = sprite->idx;
        }
    } else {
        // In both spots
        for(i=0;i<sprite->width+1;i++) {
            sprite->coll_list[(sprite->posx/8) + i] = sprite->idx;
        }
    }

    uintToStr((sprite->posx/8)+i,abuf,3);
    VDP_drawText(abuf, 14, 16+sprite->idx);
    */
    sprite->aabb.min.x = x;
    sprite->aabb.min.y = y;
    sprite->aabb.max.x = (w*8)+x;
    sprite->aabb.max.y = (h*8)+y;

    sprite->inv_mass = FIX16(0.1);
    sprite->velocity.x = 0;
    sprite->velocity.y = 0;



    return sprite->idx;
}


bool check_aabb(AABB a, AABB b) {
    // Exit with no intersection if found separated along an axis
    if(a.max.x < b.min.x || a.min.x > b.max.x) return false;
    if(a.max.y < b.min.y || a.min.y > b.max.y) return false;
        
    // No separating axis found, therefor there is at least one overlapping axis
    return true;
}

bool get_manifold(spritedef* a, spritedef* b, manifold *m) {
    vec2 n;
    n.x = b->posx - a->posx;
    n.y = b->posy - a->posy;

    AABB abox = a->aabb;
    AABB bbox = b->aabb;

    u16 a_extent = (abox.max.x - abox.min.x)>>1;
    u16 b_extent = (bbox.max.x - bbox.min.x)>>1;

    u16 x_overlap = a_extent + b_extent - abs(n.x);

    if(x_overlap > 0) {

        a_extent = (abox.max.y - abox.min.y)>>1;
        b_extent = (bbox.max.y - bbox.min.y)>>1;

        u16 y_overlap = a_extent + b_extent - abs(n.y);
        if(y_overlap > 0) {
            if(x_overlap < y_overlap) {
                //VDP_drawText(" X min ", 14, 19);
                if(n.x < 0){
                    m->normal.x = -1;
                    m->normal.y = 0;
                } else {
                    m->normal.x = 1;
                    m->normal.y = 0;
                }
                m->penetration = x_overlap;
            } else {
                //VDP_drawText(" Y min  ", 14, 19);
                if(n.y < 0) {
                    //VDP_drawText(" y>0 ", 19, 18);
                    m->normal.x = 0;
                    m->normal.y = -1;
                } else {
                    //VDP_drawText(" y>0 ", 19, 18);
                    m->normal.x = 0;
                    m->normal.y = 1;
                }
                m->penetration = y_overlap;
            }
            return true;
        }
    }

    return false;
}

//#define TESTCOLL 1

void sprite_bounce(spritedef* sprt_a, spritedef* sprt_b, manifold* m) {
#ifdef TESTCOLL
    if ( sprt_a->velocity.y == 0 ){
        return;
    }
#endif
    vec2 rv;

    rv.x = sprt_b->velocity.x - sprt_a->velocity.x;
    rv.y = sprt_b->velocity.y - sprt_a->velocity.y;
 
#ifdef DEBUG   
    intToStr(rv.x,abuf,3);
    VDP_drawText(abuf, 14,15);
    intToStr(rv.y,abuf,3);
    VDP_drawText(abuf, 14,16);
#endif

    s16 vel_normal = ((rv.x * m->normal.x) + (rv.y * m->normal.y));

#ifdef DEBUG
    intToStr(vel_normal,abuf,3);
    VDP_drawText(abuf, 14,17);
#endif 

    if(vel_normal > 0) {
#ifdef DEBUG
        VDP_drawText(" SKIP  ", 19, 16);
#endif
#ifdef TESTCOLL
        intToStr(sprt_a->velocity.x,abuf,3);
        VDP_drawText(abuf, 14,21);
        intToStr(sprt_a->velocity.y,abuf,3);
        VDP_drawText(abuf, 14,22);

        intToStr(sprt_b->velocity.x,abuf,3);
        VDP_drawText(abuf, 14,23);
        intToStr(sprt_b->velocity.y,abuf,3);
        VDP_drawText(abuf, 14,24);
        sprt_a->velocity.x = 0;
        sprt_a->velocity.y = 0;
        sprt_b->velocity.x = 0;
        sprt_b->velocity.y = 0;
#endif 
        //VDP_drawText(" SKIP  ", 19, 17);
        return;
    }
    // Restitution == bounciness
    //u16 e = 1;

    fix16 j = fix16Mul(FIX16(-1.4), intToFix16(vel_normal));
   
    //fix16 a_inv_mass = fix16Div(FIX16(1), intToFix16(sprt_a->mass));
    //fix16 b_inv_mass = fix16Div(FIX16(1), intToFix16(sprt_b->mass));
    //fix16 a_inv_mass = FIX16(0.5);
    //fix16 b_inv_mass = FIX16(0.5);

    // j /= (1/a.mass + 1/b.mass);
    j = fix16Div(j, fix16Add(sprt_a->inv_mass, sprt_b->inv_mass));
    //j = -2; 
    //

#ifdef DEBUG
    intToStr(fix16ToInt(j),abuf,3);
    VDP_drawText(abuf, 14,18);
    intToStr(m->normal.x,abuf,3);
    VDP_drawText(abuf, 14,19);
    intToStr(m->normal.y,abuf,3);
    VDP_drawText(abuf, 14,20);
#endif

    //j = j>>6;
    //u16 mass_sum = sprt_a->mass + sprt_b->mass;

    vec2 impulse;
    impulse.x = fix16ToInt(fix16Mul(j, intToFix16(m->normal.x)));
    impulse.y = fix16ToInt(fix16Mul(j, intToFix16(m->normal.y)));

    /* Max impulse */
    impulse.x = min(impulse.x, 256);
    impulse.x = max(impulse.x, -256);
    impulse.y = min(impulse.y, 256);
    impulse.y = max(impulse.y, -256);
    /*
    if (impulse.x > 256 || impulse.y > 256) {
        VDP_drawText("BOUNDS EXCEEDED", 14, 17);
    }
    */
#ifdef DEBUG
    intToStr(impulse.x,abuf,3);
    VDP_drawText(abuf, 19,16);
    intToStr(impulse.y,abuf,3);
    VDP_drawText(abuf, 19,17);
#endif

    //u16 mass_sum = sprt_a->mass + sprt_b->mass;
    //u8 ratio = sprt_a->mass / mass_sum;
   
    sprt_a->velocity.x -= fix16ToInt(fix16Mul(intToFix16(impulse.x), sprt_a->inv_mass));
    sprt_a->velocity.y -= fix16ToInt(fix16Mul(intToFix16(impulse.y), sprt_a->inv_mass));

    sprt_b->velocity.x += fix16ToInt(fix16Mul(intToFix16(impulse.x), sprt_b->inv_mass));
    sprt_b->velocity.y += fix16ToInt(fix16Mul(intToFix16(impulse.y), sprt_b->inv_mass));
   
#ifdef DEBUG
    intToStr(sprt_a->velocity.x,abuf,3);
    VDP_drawText(abuf, 14,21);
    intToStr(sprt_a->velocity.y,abuf,3);
    VDP_drawText(abuf, 14,22);

    intToStr(sprt_b->velocity.x,abuf,3);
    VDP_drawText(abuf, 14,23);
    intToStr(sprt_b->velocity.y,abuf,3);
    VDP_drawText(abuf, 14,24);
#endif 
#ifdef TESTCOLL
    sprt_a->velocity.x = 0;
    sprt_a->velocity.y = 0;
    sprt_b->velocity.x = 0;
    sprt_b->velocity.y = 0;
#endif 


    sprt_a->velocity.x = min(sprt_a->velocity.x, 256);
    sprt_a->velocity.x = max(sprt_a->velocity.x, -256);
    sprt_a->velocity.y = min(sprt_a->velocity.y, 256);
    sprt_a->velocity.y = max(sprt_a->velocity.y, -256);
    //ratio = sprt_b->mass / mass_sum;
    
    sprt_b->velocity.x = min(sprt_b->velocity.x, 256);
    sprt_b->velocity.x = max(sprt_b->velocity.x, -256);
    sprt_b->velocity.y = min(sprt_b->velocity.y, 256);
    sprt_b->velocity.y = max(sprt_b->velocity.y, -256);
    
    return;
}




// Probably just have a universal direction setter.

void sprite_left(spritedef *sprt, u8 amnt, u16 max) {
    //int i;
    sprt->posx-=amnt;
    sprt->aabb.min.x-=amnt;
    sprt->aabb.max.x-=amnt;
    //sprt->direction=192;

    /*
    if(((sprt->posx) % 8) == 0) {
        // Crossing a boundary 
        for(i=0;i<sprt->width;i++) {
            uintToStr((sprt->posx/8)+i,abuf,3);
            //uintToStr((sprt->posx/8)+i,abuf,3);
            VDP_drawText(abuf, 19,16);
            uintToStr((sprt->vposx/8)+i,abuf,3);
            //uintToStr((sprt->posx/8)+i,abuf,3);
            VDP_drawText(abuf, 19,17);
            sprt->coll_list[(sprt->posx/8) + i] = sprt->idx;
        }
        // Clear out old spot
        sprt->coll_list[(sprt->posx/8) + i] = 0xFF;
    } else {
        // In both spots
        for(i=0;i<sprt->width+1;i++) {
            uintToStr((sprt->posx/8)+i,abuf,3);
            //uintToStr((sprt->posx/8)+i,abuf,3);
            VDP_drawText(abuf, 19,16);
            uintToStr((sprt->vposx/8)+i,abuf,3);
            //uintToStr((sprt->posx/8)+i,abuf,3);
            VDP_drawText(abuf, 19,17);
            sprt->coll_list[(sprt->posx/8) + i] = sprt->idx;
        }
    }
    */
    sprite_setrowcol(sprt);
    if(sprt->vposx  <= amnt) {
        //sprt->vposx = (sprt->vposx - amnt) + 65536;
        sprt->vposx = max - (amnt - 1);
    } else {
        sprt->vposx -= amnt;
    }
    //BLAST_setSpriteP(sprt->idx, sprt);
}

void sprite_right(spritedef *sprt, u8 amnt, u16 max) {
    //int i;
    sprt->posx+=amnt;
    sprt->aabb.min.x+=amnt;
    sprt->aabb.max.x+=amnt;
    //sprt->direction=64;
    sprite_setrowcol(sprt);

    /*
    if(((sprt->posx) % 8) == 0) {
        for (i=0;i<sprt->width;i++) {
            uintToStr((sprt->posx/8)+i,abuf,3);
            VDP_drawText(abuf, 19,16);
            //uintToStr((sprt->vposx/8)+i,abuf,3);
            //uintToStr((sprt->posx/8)+i,abuf,3);
            //VDP_drawText(abuf, 19,17);
            sprt->coll_list[(sprt->posx/8) + i] = sprt->idx;
        }
        // Clear old spot
        sprt->coll_list[(sprt->posx/8) - 1] = 0xFF;
    } else {
        // In both spots
        for (i=0;i<sprt->width+1;i++) {
            uintToStr((sprt->posx/8)+i,abuf,3);
            VDP_drawText(abuf, 19,16);
            //uintToStr((sprt->vposx/8)+i,abuf,3);
            //uintToStr((sprt->posx/8)+i,abuf,3);
            //VDP_drawText(abuf, 19,17);
            sprt->coll_list[(sprt->posx/8) + i] = sprt->idx;
        }
    }
    */

    if(sprt->vposx + amnt > max) {
        sprt->vposx = 0 + (amnt);
    } else {
        sprt->vposx += amnt;
    }
}
void sprite_up(spritedef *sprt, u8 amnt, u16 max) {
    sprt->posy-=amnt;
    sprt->aabb.min.y-=amnt;
    sprt->aabb.max.y-=amnt;
    //sprt->direction=0;
    sprite_setrowcol(sprt);
    //uintToStr((sprt->posy),abuf,3);
    //VDP_drawText(abuf, 30,16);
/*    if(((sprt->vposy) % 8) == 0) {
        int i;
        for (i=0;i<sprt->height;i++) {
            spr_row[(sprt->vposy/8) + i] = sprt->idx;
        }
    }*/
    if(sprt->vposy <= amnt) {
        //sprt->vposx = (sprt->vposx - amnt) + 65536;
        sprt->vposy = max - (amnt - 1);
    } else {
        sprt->vposy -= amnt;
    }
    //BLAST_setSpriteP(sprt->idx, sprt);
}
void sprite_down(spritedef *sprt, u8 amnt, u16 max) {
    sprt->posy+=amnt;
    sprt->aabb.min.y+=amnt;
    sprt->aabb.max.y+=amnt;
    //sprt->direction=128;
    sprite_setrowcol(sprt);
    //uintToStr((sprt->posy),abuf,3);
    //VDP_drawText(abuf, 30,16);
/*    if(((sprt->vposy) % 8) == 0) {
        int i;
        for (i=0;i<sprt->height;i++) {
            spr_row[(sprt->vposy/8) + i] = sprt->idx;
        }
    }*/
    if((sprt->vposy + amnt) > max) {
        sprt->vposy = 0 + (amnt);
    } else {
        sprt->vposy += amnt;
    }
    //BLAST_setSpriteP(sprt->idx, sprt);
}


void animate_sprite(spritedef *sprt)
{
    sprt->tile_attr = TILE_ATTR_FULL(
        sprt->pal,
        1,0,0,
        sprt->startaddr+(sprt->tilesize*(sprt->curstep % sprt->steps)));
    sprt->curstep++;
}

u8 add_sprite(spritedef* sprite)
{
    // Somehow detect if the sprite_list is new.  Put something into position 0.

    if(_sprite_count == 0)
    {
        _sprite_count = 1;
        _sprite_all[0] = sprite;
        _sprite_all[0]->link = 0;
        return 0;
    }

    // Increment sprite counter
    _sprite_count++;

    //VDP_drawText("Searching ...", 0, 2);
    // Search the list
    int cur_idx = 0;
    int last_idx = 0;
    int next_idx = 0;

    while(1)
    {
        // If the next link is 0, we are at the end
        next_idx = _sprite_all[cur_idx]->link;
        last_idx = cur_idx;
        if(next_idx == 0)
        {
            if(cur_idx == (MAX_SPRITE - 1))
            {
                //Check if sprite list is full
                //VDP_drawText("FULL!", 10, 3);
                return -1;
            }
            else
            {
                //Otherwise the next is free
                last_idx = cur_idx;
                cur_idx += 1;
                
                //sprintf(abuf, "CIDX: %d", cur_idx);
                //VDP_drawText(abuf, 0, 3);
                break;
            }
        }

        if((cur_idx + 1) != next_idx)
        {
            //We have a gap, use it
            last_idx = cur_idx;
            cur_idx += 1;
            //sprintf(abuf, "GAP CIDX:(%d), NIDX:(%d)", cur_idx, next_idx);
            //VDP_drawText(abuf, 0, 4);
            break;
        }
        cur_idx = next_idx;
    }

    // Setup new sprite
    _sprite_all[cur_idx] = sprite;
    _sprite_all[cur_idx]->link = next_idx;

    // Update link of last_idx
    _sprite_all[last_idx]->link = cur_idx;

    return cur_idx;
}

int drop_sprite(u8 del_idx)
{
    int last_idx=0;
    int idx = 0;
    //sprintf(abuf, "DROP: %d", del_idx);
    //VDP_drawText(abuf, 20, 0);
    while(1)
    {
        //VDP_drawText("deleting...", 20, 1);
        // If the next link is 0, we are at the end
        last_idx = idx;
        idx = _sprite_all[idx]->link;
        if(idx == 0)
            break;
        
        if(idx == del_idx)
        {
            //Found the item to delete
            _sprite_all[last_idx]->link = _sprite_all[idx]->link;
            break;
        }

    }
    _sprite_count--;
    return 0;
}

void hscroll_sprites(s8 hscroll) 
{
    int cur_idx = 0;
    int next_idx = 0;
    spritedef* tsprite;

    while(1)
    {
        // If the next link is 0, we are at the end
        tsprite = _sprite_all[cur_idx];
        tsprite->posx += hscroll;
        sprite_setrowcol(tsprite);
        //BLAST_setSpriteP(
        //    cur_idx, 
        //    _sprite_all[cur_idx]);
        /*BLAST_setSpritePosition(
            cur_idx, 
            _sprite_all[cur_idx]->posx,
            _sprite_all[cur_idx]->posy
        );*/
        next_idx = _sprite_all[cur_idx]->link;
        if(next_idx == 0)
        {
            break;
        }
        cur_idx = next_idx;
    }
}

void vscroll_sprites(s8 vscroll) 
{
    int cur_idx = 0;
    int next_idx = 0;
    spritedef* tsprite;

    while(1)
    {
        //VDP_drawTextBG(VDP_PLAN_A, "SCROLL", 0x8000, 4,4);
        // If the next link is 0, we are at the end
        tsprite = _sprite_all[cur_idx];
        tsprite->posy += vscroll;
        sprite_setrowcol(tsprite);
        //BLAST_setSpriteP(
        //    cur_idx, 
        //    _sprite_all[cur_idx]);
        /*
        BLAST_setSpritePosition(
            cur_idx, 
            _sprite_all[cur_idx]->posx,
            _sprite_all[cur_idx]->posy
        );
        */
        next_idx = _sprite_all[cur_idx]->link;
        if(next_idx == 0)
        {
            break;
        }
        cur_idx = next_idx;
    }
}


void check_t_collision(u8* slist, u8 list_len, blastmap* map, void (*callback)(spritedef* sprt, u8 coll)) {
    u8 i;
    spritedef* sprt;
    u8 coll = 0;
    for(i=0;i<list_len;i++){
        sprt = _sprite_all[slist[i]];
        coll = 0;



        /*
        // Check diagonals
        switch(sprt->direction) {
            case 32:
                if(check_up(map, sprt))
                    sprt->direction = 64;
                break;
            case 96:
                if(check_right(map, sprt))
                    sprt->direction = 128;
                break;
            case 160:
                if(check_down(map, sprt))
                    sprt->direction = 192;
                break;
            case 224:
                if(check_left(map, sprt))
                    sprt->direction = 0;
                break;
        }

        switch(sprt->direction) {
            case 224:
            case 0:
                coll |= check_up(map, sprt);
                if(coll && sprt->direction == 224){
                    sprt->direction = 192;
                    coll = 0;
                }
                break;
            case 32:
                // Already checked 
                //coll = check_up(map, sprt);
            case 64:
                coll |= check_right(map, sprt);
                if(coll && sprt->direction == 32){
                    sprt->direction = 0;
                    coll = 0;
                }
                break;
            case 96:
                //coll = check_right(map, sprt);
            case 128:
                coll |= check_down(map, sprt);
                if(coll && sprt->direction == 96){
                    sprt->direction = 64;
                    coll = 0;
                }
                break;
            case 160:
                //coll = check_down(map, sprt);
            case 192:
                coll |= check_left(map, sprt);
                if(coll && sprt->direction == 160){
                    sprt->direction = 128;
                    coll = 0;
                }
                break;
        }
        if(coll != 0) {
            (*callback) (sprt, coll);
        }*/
    }
    
}

void do_noop(){
    u8 dummy = 0;
    return;
}

void check_collision(u8* lista, u8 lista_len, u8* listb, u8 listb_len, void (*callback)(spritedef* sprta, spritedef* sprtb)){

    /*
    for sprite_a in lista:
        for sprite_b in listb:
            column_collisions = sprite_a.column_mask & sprite_b_.column_mask
            if column_collisions != 0:
                // Column collision
                for row in sprite_a.rows:
                    if coll_rows[row] == collision:
                        // Row collision
                        //  Do collision callback
                        //

    */

    u8 a_idx;
    u8 b_idx;
    u8 a_row_idx;
    u8 a_col_idx;
    u8 b_col_idx;

    u32 column_collision = 0;
    u32 row_collision;

    spritedef* sprite_a;
    spritedef* sprite_b;
    manifold* coll_manifold;
    
#ifdef DEBUG
    VDP_drawText("             ", 20,20);
    VDP_drawText("             ", 20,21);
#endif
    for(a_idx=0; a_idx < lista_len; a_idx++) {
        // For each sprite_a in lista
        sprite_a = _sprite_all[lista[a_idx]];
        
        if((sprite_a->row_mask & coll_row_mask) == 0) {
            // If not on a row with a collision, skip
            continue;
        }
#ifdef DEBUG
        // Row collision
        VDP_drawText("Row Coll     ", 20,20);
#endif 
        for(b_idx=0; b_idx < listb_len; b_idx++) {
            // For each sprite_b in listb

            sprite_b = _sprite_all[listb[b_idx]]; 
            if(sprite_b->idx == sprite_a->idx) {
                // Don't check sprite against itself!
                continue;
            }

            if((sprite_b->row_mask & coll_row_mask) == 0) {
                // If not on a row with a collision, skip
                continue;
            }
        
            for(a_col_idx=0; a_col_idx < MAX_SPRITE_COL; a_col_idx++) {
            // For each column of sprite_a
            //
                for(b_col_idx=0; b_col_idx < MAX_SPRITE_COL; b_col_idx++) {
                    // For each colum of spriteb

                    if(sprite_a->columns[a_col_idx] == sprite_b->columns[b_col_idx]) {
                        if(check_aabb(sprite_a->aabb, sprite_b->aabb)) {
                            // Collision!
                            // Do a callback
                            //column_collision += 1;
                            //do_noop();
                            (*callback) (sprite_a, sprite_b);
                            //if(get_manifold(sprite_a, sprite_b, coll_manifold)) {
                                //sprite_bounce(sprite_a, sprite_b, coll_manifold);
                            //}
                        }
                    }
                }
            }
        }
    }
    // Clear collision mask
    coll_row_mask = 0;
}


void move_sprite(spritedef* sprt, blastmap* map, void (*callback)(spritedef* insprt, u8 coll)){
    if((sprt->velocity.x == 0) && (sprt->velocity.y == 0))
        return;

    u8 coll = 0;
    if(sprt->velocity.x>0) {
        coll = check_right(map, sprt);
        if(coll == NOCOLL || coll == COLL_RIGHT) {
            sprite_right(sprt, (sprt->velocity.x>>6), 512);
            //sprt->velocity.x -= 1;
        } else {
            sprt->velocity.x *= -1;    
            (*callback)(sprt, coll);
        }
    } else if (sprt->velocity.x<0) {
        coll = check_left(map, sprt);
        if(coll == NOCOLL || coll == COLL_LEFT) {
            sprite_left(sprt, ((sprt->velocity.x*-1)>>6), 512);
            //sprt->velocity.x += 1;
        } else {
            sprt->velocity.x *= -1;    
            (*callback)(sprt, coll);
        }
    }

    if(sprt->velocity.y<0) {
        coll = check_up(map, sprt);
        if(coll == NOCOLL || coll == COLL_UP) {
            sprite_up(sprt, (abs(sprt->velocity.y)>>6), 512);
            //sprt->velocity.y -= 1;
        } else {
            sprt->velocity.y *= -1;
            (*callback)(sprt, coll);
        }
    } else if (sprt->velocity.y>0) {
        coll = check_down(map, sprt);
        if(coll == NOCOLL || coll == COLL_DOWN) {
            sprite_down(sprt, (sprt->velocity.y>>6), 512);
            sprt->velocity.y += 1;
        } else {
            sprt->velocity.y *= -1;
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


void drag_sprites() {
    u8 cur_idx = 0;
    u8 next_idx = 0;
    spritedef* tsprite;

    u8 drag = 1;

    while(1) {
        tsprite = _sprite_all[cur_idx];
        
        if(tsprite->velocity.x > 0) {
            tsprite->velocity.x -= drag;
        } else if (tsprite->velocity.x < 0) {
            tsprite->velocity.x += drag;
        }

        if(tsprite->velocity.y > 0) {
            tsprite->velocity.y -= drag;
        } else if (tsprite->velocity.y < 0) {
            tsprite->velocity.y += drag;
        }

        next_idx = tsprite->link;
        if(next_idx == 0) {
            break;
        }
        cur_idx = next_idx;
    }
}

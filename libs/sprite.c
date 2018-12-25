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
    return sprite->idx;
}


// Probably just have a universal direction setter.

void sprite_left(spritedef *sprt, u8 amnt, u16 max) {
    //int i;
    sprt->posx-=amnt;
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

    //u8 lista_len;
    //u8 listb_len;
    
    u32 column_collision;
    u32 row_collision;

    spritedef* sprite_a;
    spritedef* sprite_b;

    //lista_len = sizeof(lista)/8;
    //listb_len = sizeof(listb)/8;

    //lista_len = 1;
    //listb_len = 2;
    

    VDP_drawText("             ", 20,20);
    VDP_drawText("             ", 20,21);
    for(a_idx=0; a_idx < lista_len; a_idx++) {
        // For each sprite_a in lista
        sprite_a = _sprite_all[lista[a_idx]];
        
        if((sprite_a->row_mask & coll_row_mask) == 0) {
            // If not on a row with a collision, skip
            continue;
        }

        // Row collision
        VDP_drawText("Row Coll     ", 20,20);

        for(b_idx=0; b_idx < listb_len; b_idx++) {
            // For each sprite_b in listb

            sprite_b = _sprite_all[listb[b_idx]]; 

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
                        // Collision!
                        VDP_drawText("COLLISION!!     ", 20,21);
                        // Do a callback
                        (*callback) (sprite_a, sprite_b);
                        //sprite_b->pal = PAL1;
                        //sprite_b->posx-=2;
                    }
                }
            }
        }
    }
    // Clear collision mask
    coll_row_mask = 0;



#ifdef nope
    VDP_drawText("             ", 30,17);
    VDP_drawText("             ", 30,18);
    for(a_idx=0; a_idx < lista_len; a_idx++) {
        sprite_a = _sprite_all[a_idx];

        for(b_idx=0; b_idx < listb_len; b_idx++) {
            sprite_b = _sprite_all[b_idx];

            column_collision = sprite_a->column_mask & sprite_b->column_mask;
            if(column_collision != 0) {
                // Column collision
                
                VDP_drawText("Column Coll   ", 30,17);
                for(a_row_idx=0; a_row_idx < MAX_SPRITE_ROW; a_row_idx++){
                    if(coll_row[sprite_a->rows[a_row_idx]] == 0xFF) {
                        VDP_drawText("COLLISION    ", 30,18);
                    }    
                }
            }
        }
    }
#endif

}

#ifdef BAR
void check_collision(u8 groupa, u8 groupb) {
    // Check for collisions between two groups


    for sprite in all_sprites:
        

}


void check_collision(u8* lista, u8* listb){
    
    for coll_row in coll_rows:
        if coll_row.collision:
            for sprite_a in lista:
               for row in sprite_a.rows:
                   if row == coll_row:
                      for coll_col in sprite_a.columns:
                         for sprite_b in listb:
                            if sprite_b.column == coll_col:
                               COLLISION! 



    for sprite_a in lista:
        for row in sprite_a.rows:
            if coll_rows[row] == row_coll:
                for spritea_col in sprite_a.columns:
                    for sprite_b in listb:
                        if sprite_b.column == spritea_col:
                            COLLISION!


    u16 a_idx;
    u16 b_idx;
    u8 a_row_idx;
    u8 a_col_idx;
    u8 b_col_idx;

    u8 lista_len;
    u8 listb_len;
    
    spritdef* sprite_a;
    spritdef* sprite_b;

    lista_len = sizeof(lista)/8;
    listb_len = sizeof(listb)/8;

    for(a_idx=0;a_idx<lista_len;a_idx++) {
        // For each sprite_a in list_a
        //
        sprite_a = _sprite_all[a_idx];
        for(a_row_idx=0;a_row_idx<MAX_SPRITE_ROW;a_row_idx++){
            // For each row in sprite_a
            //
            if(coll_row[sprite_a->rows[a_row_idx]] == 0xFF) {
                // Possible row collision
                //
                for(a_col_idx=0;a_col_idx<MAX_SPRITE_COL;a_col_idx++){
                    // For each col in sprite_a
                    //
                    for(b_idx=0;b_idx<listb_len;b_idx++){
                        // For each sprite_b in list_b
                        //
                        sprite_b = _sprite_all[b_idx];
                        for(b_col_idx=0;b_col_idx<MAX_SPRITE_COL;b_col_idx++){
                            // For each col in sprite_b
                            if(sprite_b->columns[b_col_idx] == sprite_a->columns[a_col_idx]){
                                //
                                //COLLISION
                                //
                                //NOTE: may detect multiple collisions for same sprite pair
                                //
                            }
                        }
                    }
                }
            }
        }   
    }
}
#endif

#ifdef FOO
void check_collision(u8* lista, u8* listb) {
//void check_collision(spritedef* sprt, spritdef** spr_list) {
    int i;
    int j;
    int k;
    char str[10];

    spritedef* sprta;
    spritedef* sprtb;

    for(i=0;i<40;i++){

        //VDP_drawText(str, 20, 20+i);

        if(lista[i] != 0xFF) {

            //uintToStr(i, str, 3);
            //VDP_drawText(str, 30,30);

            if(listb[i] != 0xFF) {
                VDP_drawText("possible Coll", 20,21);
                // Possible collision on column
                sprta = _sprite_all[lista[i]];

                // Flaw, sprite may span rows
                u8 row = sprta->posy/8;
                if(coll_row[row] != 0) {
                    // Very likey collision between sprt A and sprt B
                    VDP_drawText("COLLISION    ", 20,22);
                    uintToStr(i, str, 3);
                    VDP_drawText(str, 24, 24);
                    
                    uintToStr(row, str, 3);
                    VDP_drawText(str, 30,22);
                    coll_row[row] = 0;
                }
                /*
                // Flaw, sprite may span rows
                u8 row = (sprta->posy+sprta->h)/8;
                if(coll_row[row] != 0) {
                    // Very likey collision between sprt A and sprt B
                    VDP_drawText("COLLISION    ", 20,22);
                    uintToStr(i, str, 3);
                    VDP_drawText(str, 24, 24);
                    
                    uintToStr(row, str, 3);
                    VDP_drawText(str, 30,22);
                    coll_row[row] = 0;
                }
                */

            } else {
                VDP_drawText("               ", 20,21);
                VDP_drawText("               ", 20,22);
            }
        }
    }

    /*
    for(i=0;i<28;i++){
        if(coll_row[i] != 0) {
            uintToStr(i, str, 3);
            VDP_drawText(str, 24, 24);
            //coll_row[i] = 0;
            //sprt = _sprite_all[i];        
        }
    } 
    */
}

#endif

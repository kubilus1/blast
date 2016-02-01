#include "genesis.h"
#include "blast.h"

char abuf[50];
u8 _sprite_init = 0;

u16 sprite_init(spritedef* sprite, u16 addr, u16 steps, u8 x, u8 y, u8 w, u8 h, u8 pal) {
    sprite->posx = x;
    sprite->posy = y;
    sprite->size = SPRITE_SIZE(w,h);
    sprite->width = w;
    sprite->height = h;
    sprite->startaddr = addr;
    sprite->tile_attr = TILE_ATTR_FULL(pal, 1, 0, 0, addr);
    sprite->link = 0;
    sprite->prev_link = 0;
    sprite->curstep = 0;
    sprite->steps = steps;
    sprite->tilesize = w*h;
    sprite->pal = pal;
    sprite->vposx = x;
    sprite->vposy = y;
    return add_sprite(sprite);
}


void sprite_left(spritedef *sprt, u8 amnt, u16 max) {
    sprt->posx-=amnt;
    if(sprt->vposx  <= amnt) {
        //sprt->vposx = (sprt->vposx - amnt) + 65536;
        sprt->vposx = max - (amnt - 1);
    } else {
        sprt->vposx -= amnt;
    }
}

void sprite_right(spritedef *sprt, u8 amnt, u16 max) {
    sprt->posx+=amnt;
    if(sprt->vposx + amnt > max) {
        sprt->vposx = 0 + (amnt);
    } else {
        sprt->vposx += amnt;
    }
}
void sprite_up(spritedef *sprt, u8 amnt, u16 max) {
    sprt->posy-=amnt;
    if(sprt->vposy <= amnt) {
        //sprt->vposx = (sprt->vposx - amnt) + 65536;
        sprt->vposy = max - (amnt - 1);
    } else {
        sprt->vposy -= amnt;
    }
}
void sprite_down(spritedef *sprt, u8 amnt, u16 max) {
    sprt->posy+=amnt;
    if((sprt->vposy + amnt) > max) {
        sprt->vposy = 0 + (amnt);
    } else {
        sprt->vposy += amnt;
    }
}


void animate_sprite(spritedef *sprt)
{
    sprt->tile_attr = TILE_ATTR_FULL(
        sprt->pal,
        1,0,0,
        sprt->startaddr+(sprt->tilesize*(sprt->curstep % sprt->steps)));
    sprt->curstep++;
}

int add_sprite(spritedef* sprite)
{
    // Somehow detect if the sprite_list is new.  Put something into position 0.

    if(_sprite_init == 0)
    {
        _sprite_init = 1;
        _sprite_all[0] = sprite;
        _sprite_all[0]->link = 0;
        VDP_setSpriteP(0, sprite);
        return 0;
    }

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

    //Tell the VDP about the updates.
    VDP_setSpriteP(
        cur_idx, 
        _sprite_all[cur_idx]
    );
    VDP_setSpriteP(
        last_idx, 
        _sprite_all[last_idx]
    );

    return cur_idx;
}

int drop_sprite(int del_idx)
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
    //VDP_drawText("done...", 20, 1);
    //Tell the VDP about the updates.
    VDP_setSpriteP(
        last_idx, 
        _sprite_all[last_idx]
    );
   
    return 0;
}

void hscroll_sprites(s8 hscroll) 
{
    int cur_idx = 0;
    int next_idx = 0;

    while(1)
    {
        // If the next link is 0, we are at the end
        next_idx = _sprite_all[cur_idx]->link;
        _sprite_all[cur_idx]->posx += hscroll;
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

    while(1)
    {
        //VDP_drawTextBG(VDP_PLAN_A, "SCROLL", 0x8000, 4,4);
        // If the next link is 0, we are at the end
        next_idx = _sprite_all[cur_idx]->link;
        _sprite_all[cur_idx]->posy += vscroll;
        VDP_setSpriteP(
            cur_idx, 
            _sprite_all[cur_idx]
        );
        if(next_idx == 0)
        {
            break;
        }
        cur_idx = next_idx;
    }
}

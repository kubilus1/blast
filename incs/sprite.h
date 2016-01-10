#include "genesis.h"

#ifndef _SPRITE
#define _SPRITE

typedef struct
{
    // Position on plane?
    s16 posx;
    s16 posy;
    // Tile attr
    u16 tile_attr;
    // Sprite size
    u8 size;
    // Sprite link
    u8 link;
    // Sprite tile starting address in memory
    u16 startaddr;
    // # Animation steps
    u8 steps;
    // Current animation step
    u8 curstep;
    // ??
    u8 tilesize;
    // Which palette we are using
    u8 pal;
    // Previous tile link
    u8 prev_link;
    // width & height 
    u8 height;
    u8 width;
    //
    u16 vposx;
    u16 vposy;
} spritedef;

// List of all current sprites
spritedef* _sprite_all[MAX_SPRITE];

u8 _sprite_init;

void animate_sprite(spritedef * sprt);

u16 sprite_init(spritedef* sprite, u16 addr, u16 steps, u8 x, u8 y, u8 w, u8 h, u8 pal);
//int add_sprite(spritedef sprite);
int add_sprite(spritedef* sprite);
int drop_sprite(int index);
void hscroll_sprites(s8 hscroll); 
void vscroll_sprites(s8 vscroll);
void sprite_left(spritedef *sprt, u8 amnt, u16 max);
void sprite_right(spritedef *sprt, u8 amnt, u16 max);
void sprite_up(spritedef *sprt, u8 amnt, u16 max);
void sprite_down(spritedef *sprt, u8 amnt, u16 max);

#endif

#ifndef _SPRITE
#define _SPRITE
//#include "math_tables.h"
//#include "genesis.h"
//#include "blast.h"
//#include "tilemap.h"

#define MAX_SPRITE 80
//#define SLIST 0xF400
#define SLIST slist_addr
#define MAX_SPRITE_COL 2
#define MAX_SPRITE_ROW 2

//typedef int bool;

typedef struct _vec2 {
    s16 x;
    s16 y;
} vec2;

// Axis aligned bounding box
typedef struct _AABB {
    vec2 min;
    vec2 max;
} AABB;


typedef struct _BLAST_Circle {
    u8 radius;
    vec2 position;
} BLAST_Circle;

typedef struct _spritedef
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
    // 
    u8 tilesize;
    // Which palette we are using
    u8 pal;
    // Previous tile link
    u8 prev_link;
    // pixel width & height 
    u8 height;
    u8 width;
    // Tile width & height
    u8 tile_height;
    u8 tile_width;
    // virtual position in map 
    u16 vposx;
    u16 vposy;
    // VDP sprite list index
    u8 idx;

    // horizontal and vertical attrs
    u8 h_attr;
    u8 v_attr;

    // Direction of motion
    // 0 up
    // 64 right
    // 128 down
    // 192 left
    u8 direction;

    // Force
    u8 force;
    //u8 col_group;


    //s8 x_vec;
    //s8 y_vec;

#ifdef COLCHECK
    u8 columns[MAX_SPRITE_COL];
    u32 column_mask;
#endif

#ifdef ROWCHECK
    u32 row_mask;
    u8 rows[MAX_SPRITE_ROW];
#endif 

    // Sprites axis aligned bounding box
    AABB aabb;
    fix16 inv_mass;
    vec2 velocity;

    // Remainder velocity:w
    vec2 r_vel;

    BLAST_Circle circle;
    //u8* coll_list;

    // Extra properties
    void* properties;
} spritedef;

typedef struct _manifold {
    u16 penetration;
    vec2 normal; 
} manifold;

u8 spr_col [40];
u8 spr_row [28];

// List of all current sprites
spritedef* _sprite_all[MAX_SPRITE];

typedef struct
{
    s16 posx;
    s16 posy;
    u16 tile_attr;
    u8 size;
    u8 link;
} SpriteDef;


/**
 *  \brief VDP sprite definition cache.
 */
//extern SpriteDef vdpSpriteCache[MAX_SPRITE];


u8 _sprite_init;

void animate_sprite(spritedef * sprt);

void sprite_setrowcol(spritedef* sprite);
u16 sprite_init(spritedef* sprite, u16 addr, u16 steps, s16 x, s16 y, u8 w, u8 h, u8 pal);
//int add_sprite(spritedef sprite);
u8 add_sprite(spritedef* sprite);
int drop_sprite(u8 index);
void hscroll_sprites(s8 hscroll); 
void vscroll_sprites(s8 vscroll);
void sprite_left(spritedef *sprt, u8 amnt, u16 max);
void sprite_right(spritedef *sprt, u8 amnt, u16 max);
void sprite_up(spritedef *sprt, u8 amnt, u16 max);
void sprite_down(spritedef *sprt, u8 amnt, u16 max);
//void check_collision();
void check_t_collision(u8* lista, u8 lista_len, blastmap* map, void (*callback)(spritedef* sprt, u8 coll));
void check_collision(u8* lista, u8 lista_len, u8* listb, u8 listb_len, void (*callback)(spritedef* sprta, spritedef* sprtb));
void BLAST_updateSprites(); 
void BLAST_setSpriteP(u16 index, const spritedef *sprite);
void sprite_bounce(spritedef* sprt_a, spritedef* sprt_b, manifold* m);
bool get_box_manifold(spritedef* a, spritedef* b, manifold *m);
bool get_circle_manifold(spritedef* a, spritedef* b, manifold *m);
void move_sprite(spritedef* sprt, blastmap* map, void (*callback)(spritedef* insprt, u8 coll));
void move_sprites(blastmap* map, void (*callback)(spritedef* sprt, u8 coll));
void drag_sprites(u8 drag);
void flip_sprite(spritedef* sprt, u8 h, u8 v);

#endif

#include "blast.h"

#define SPR_NBTILES 4   //tiles needed for our sprite

//const is used to keep tile in ROM, not in RAM
const u32 spriteTiles[SPR_NBTILES*8]=
{
        0x00001111, //Tile Top Left
        0x00001111,
        0x00111144,
        0x00111144,
        0x11112244,
        0x11112244,
        0x11112244,
        0x11112244,

        0x11112222, //Tile Bottom Left
        0x11112222,
        0x11112222,
        0x11112222,
        0x00111122,
        0x00111122,
        0x00001111,
        0x00001111,

        0x11110000, //Tile Top Right
        0x11110000,
        0x44111100,
        0x44111100,
        0x44221111,
        0x44221111,
        0x44221111,
        0x44221111,

        0x22221111, //Tile Bottom Right
        0x22221111,
        0x22221111,
        0x22221111,
        0x22111100,
        0x22111100,
        0x11110000,
        0x11110000
};

int main( )
{
    blast_init();
    
    spritedef mySprite;
    u16 sprite_idx = 0;;
    u8* colls;
    u16 sprite_bmp_offset = 1;
    
    sprite_bmp_offset = load_tile(
           (u32*) spriteTiles,
           32,
           8
    );

    sprite_idx = sprite_init(
            &mySprite, 
            sprite_bmp_offset, 
            1, 
            0, 
            64,  
            2, 
            2, 
            PAL0, 
            colls
    );

    while(1)
    {
        //sprite_down(&mySprite, 1, 72);
        mySprite.posy++;
        wait_vsync();

    }
    return 0;
}

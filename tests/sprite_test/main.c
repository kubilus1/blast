#include "blast.h"
//#include "images.h"
//#include "terrain_map1.h"

//u16 terrain_tile_offset;
//blastmap terrain1map;

char str[10];

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

#define DEBUG
#undef DEBUG
int main( )
{
    spritedef mySprite;
    u16 sprite_idx = 0;;
    //u8* colls;
    u16 sprite_bmp_offset = 1;

    blast_init();
  
    //ind = TILE_USERINDEX+sprite_bmp_offset;
   
    //uintToStr(terrain1.tileset->compression, str, 3);
    //VDP_drawText(str, 5, 5);

    u16 ind = TILE_USERINDEX;
    //VDP_drawImageEx(PLAN_B, &terrain1, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    
    /*
    VDP_loadTileData(terrain1.tileset->tiles, 
            TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, ind), 
            terrain1.tileset->numTile, 0);
    //VDP_setMap(PLAN_B, terrain1.map, ind, 0, 0);
    VDP_loadTileSet((TileSet*)terrain1.tileset, ind & TILE_INDEX_MASK, TRUE);
    blastmap_init(&terrain1map, &terrain1_map, (u16*)terrain1.map->tilemap, ind, PLAN_B);
    load_visible_map(&terrain1map,0,0);
    


    */ 
    
    //VDP_setPalette(PAL0, (u16*)terrain1.palette->data);
    
    //ind += terrain1.tileset->numTile;
    //terrain_tile_offset = ind;
  
    //VDP_drawText(str, 5, 5);

    //VDP_loadTileSet(&(terrain1.tileset), ind, COMPRESSION_NONE);
    //ind += terrain1.tileset->numTile;
    //terrain_tile_offset = ind;

    /*
    terrain_tile_offset = load_bmp(
            (u32*) terrain1.tileset->tiles,
            terrain1.map->w,
            terrain1.map->h
    );
    */
#ifdef DEBUG
    terrain_tile_offset = load_img(&terrain1);
    VDP_setPalette(PAL0, (u16*)terrain1.palette->data);

    blastmap_init(&terrain1map, &terrain1_map, (u16*)terrain1.tilemap->tilemap, terrain_tile_offset, BG_A);
    load_visible_map(&terrain1map,0,0);
   
    uintToStr(terrain1.tileset->numTile,str,3);
   
#endif
    VDP_drawText("Sprite Test", 1, 1);
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
            PAL0 
    );

    while(1)
    {
        //sprite_down(&mySprite, 1, 72);
        mySprite.posy++;
        VDP_waitVSync();
        //wait_vsync();

    }
    return 0;
}

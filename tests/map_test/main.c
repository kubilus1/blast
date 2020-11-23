#include "blast.h"
#include "images.h"
#include "terrain_map1.h"

u16 terrain_tile_offset;
blastmap terrain1map;

char str[10];

#define DEBUG 1
int main( )
{
    blast_init();
    //ind = TILE_USERINDEX+sprite_bmp_offset;
   
    uintToStr(terrain1.tileset->compression, str, 3);
    VDP_drawText(str, 5, 5);

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
    VDP_setPalette(PAL0, (u16*)terrain1.palette->data);
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
    terrain_tile_offset = load_img(&terrain1);
    VDP_setPalette(PAL0, (u16*)terrain1.palette->data);

    blastmap_init(&terrain1map, &terrain1_map, (u16*)terrain1.tilemap->tilemap, terrain_tile_offset, BG_A);
    load_visible_map(&terrain1map,0,0);
   
    uintToStr(terrain1.tileset->numTile,str,3);
   
    while(1)
    {
        VDP_drawText("map test", 2, 2);
        VDP_waitVSync();
    }
    return 0;
}

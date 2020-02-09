#include "gfx.h"

static u16 vdp_offset = TILE_USERINDEX;
//static u16 vdp_offset = 1;

u16 draw_img(Image *img) {
    u16 tmp_offset = vdp_offset;
    VDP_drawImageEx(PLAN_B, img, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, vdp_offset), 0, 0, FALSE, TRUE);
    vdp_offset += img->tileset->numTile;
    return tmp_offset;
}

u16 load_img(Image* img) {
    u16 tmp_offset = vdp_offset;

    VDP_loadTileSet(img->tileset, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, vdp_offset) & TILE_INDEX_MASK, 1);
    //VDP_loadTileData(img->tileset->tiles, vdp_offset, img->tileset->numTile, TRUE);
    vdp_offset += img->tileset->numTile;
    return tmp_offset;
}

u16 load_bmp(u32* data, u16 w, u16 h) {
    u16 tmp_offset = vdp_offset;
    
    VDP_loadBMPTileData(
            data,
            vdp_offset,
            w/8,
            h/8,
            w/8
    );

    vdp_offset += ((w/8) * (h/8));
    return tmp_offset;
}


u16 load_tile(u32 *data, u16 w, u16 h) {
    u16 tmp_offset = vdp_offset;
    
    VDP_loadTileData(
            data,
            vdp_offset,
            ((w/8) * (h/8)),
            1
    );

    vdp_offset += ((w/8) * (h/8));
    return tmp_offset;
}

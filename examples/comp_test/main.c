#include <genesis.h>

#define TILE1	1

//const is used to keep tile in ROM, not in RAM


u32 grey_patterns0[8][8];
u32 grey_patterns1[8][8];

#define COLOR_PATTERNS 8
u32 color_patterns0[COLOR_PATTERNS][8];
u32 color_patterns1[COLOR_PATTERNS][8];


u32 tile_patterns0[64][8][8];
u32 tile_patterns1[64][8][8];


void gen_pattern(u8 pattern, u32 tile[]) {
    int i;
    u32 pline;
    u8* p = &pline;
    p[0] = pattern;
    p[1] = pattern;
    p[2] = pattern;
    p[3] = pattern;

    for(i=0;i<8;i++) {
        tile[i] = pline;
    }
}

int main( )
{
	//This will fill the background with this tile
	//VDP_loadTileData( (const u32 *)tile, 0, 1, 0);
    
    int i;
    int j;
    int k;
    int tile = TILE1;

    VDP_setPaletteColor(8,RGB24_TO_VDPCOLOR(0xFF0000));
    VDP_setPaletteColor(9,RGB24_TO_VDPCOLOR(0x00FF00));
    VDP_setPaletteColor(10,RGB24_TO_VDPCOLOR(0x0000FF));
    VDP_setPaletteColor(11,RGB24_TO_VDPCOLOR(0xFFFF00));
    VDP_setPaletteColor(12,RGB24_TO_VDPCOLOR(0x00FFFF));
    VDP_setPaletteColor(13,RGB24_TO_VDPCOLOR(0xFF00FF));
    VDP_setPaletteColor(14,RGB24_TO_VDPCOLOR(0x880088));
    VDP_setPaletteColor(15,RGB24_TO_VDPCOLOR(0x008888));

    for(i=0;i<8;i++){
        gen_pattern(i, grey_patterns1[i]);
        gen_pattern(i<<4, grey_patterns0[i]);
    }

    for(i=0;i<8;i++){
        gen_pattern((i+8), color_patterns0[i]);
        gen_pattern((i+8)<<4, color_patterns1[i]);
    }

    int num_patterns = COLOR_PATTERNS;

    for(k=0;k<COLOR_PATTERNS;k++) {
        // k color pattern
        for(i=0;i<8;i++){
            // i tile
            for(j=0;j<8;j++)
            {
                // j line in tile
                tile_patterns0[k][i][j] = color_patterns0[k][j] | grey_patterns0[i][j];
                tile_patterns1[k][i][j] = color_patterns1[k][j] | grey_patterns1[i][j];
            }
        }
    }

    for(k=0;k<COLOR_PATTERNS;k++){
        // k color pattern
        for(i=0;i<8;i++){
            // i tile
            for(j=0;j<8;j++){
                // j line in tile

                // hold one color tile still, iterate over other colors
                tile_patterns0[k+num_patterns][i][j] = color_patterns0[k][j] | color_patterns1[i][j];
                tile_patterns1[k+num_patterns][i][j] = grey_patterns0[i][j] | grey_patterns1[i][j];
                //
                // The reverse will be 100% duplicates
                //tile_patterns1[k+num_patterns][i][j] = color_patterns1[k][j] | color_patterns0[i][j];
            }
        }              
    }

    //num_patterns += COLOR_PATTERNS;

	//load the tile in VRAM (check it using GensKMod CPU>Debug>Genesis>VDP)
	VDP_loadTileData( (const u32 *)grey_patterns0, tile, 8, 0);
	tile+=8;
    VDP_loadTileData( (const u32 *)grey_patterns1, tile, 8, 0);
	tile+=8;

    for(i=0;i<num_patterns;i++) {
        VDP_loadTileData( (const u32 *)(tile_patterns0[i]), tile, 8, 0);
        tile+=8;
        VDP_loadTileData( (const u32 *)(tile_patterns1[i]), tile, 8, 0);
        tile+=8;
    }
    for(i=num_patterns;i<(num_patterns*2);i++) {
        VDP_loadTileData( (const u32 *)(tile_patterns0[i]), tile, 8, 0);
        tile+=8;
    }
    VDP_loadTileData( (const u32 *)(tile_patterns1[num_patterns]), tile, 8, 0);
    tile+=8;


    VDP_drawText(" Approx 216 Colors, One Palette ",0,0);
        
    while(1)
	{
        int col=0;
        int row=1;
        for(i=0;i<tile;i++) {
            VDP_setTileMapXY(APLAN, TILE_ATTR_FULL(PAL0, 0, 0, 0, TILE1+i), col, row);
            col+=1;
            if(col == 32) {
                row+=1;
                col=0;
            }
        }

		VDP_waitVSync();
		VDP_waitVSync();
	}

	return 0;
}

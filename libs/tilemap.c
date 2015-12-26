#include "genesis.h"
#include "blast.h"


void blast_init(blastmap* bmap, tilemap* tmap, u16 tileoffset, u16 plane) {
    bmap->tX = 64; 
    bmap->tY = 64;
    bmap->winX = bmap->tX;
    bmap->winY = bmap->tY;
    bmap->maph = 0;
    bmap->mapw = 0;    
    bmap->planwidth = VDP_getPlanWidth();
    bmap->planheight = VDP_getPlanHeight();
    bmap->tileoffset = tileoffset;
    bmap->plane = plane;
    bmap->tiles = tmap;
}

u8 check_up(blastmap* bmap, spritedef* sprite) {
    char str[10];
    // Check if we are at a tile boundary, if not skip.
    if(((sprite->posy) % 8) != 0) {
        return 0;
    }
    // 
    u8 tile_row = (sprite->posy - 1) / 8;
    return check_col(bmap, sprite, tile_row);
}
u8 check_down(blastmap* bmap, spritedef* sprite) {
    char str[10];
    // Check if we are at a tile boundary, if not skip.
    if(((sprite->posy) % 8) != 0) {
        return 0;
    }
    // 
    u8 tile_row = (sprite->posy + 8) / 8;
    return check_col(bmap, sprite, tile_row);
}

u8 check_col(blastmap* bmap, spritedef* sprite, u8 tile_row) {
    u8 coll;
    u16 tile;
    // How many rows to test?
    u8 tile_col = sprite->posx / 8;
    u8 end_col = tile_col + sprite->width;
    if(((sprite->posx) % 8) != 0) {
        // We are crossing rows
        end_col++;
    }

    coll = 0;
    for(; tile_col<end_col;tile_col++) {
        tile = GET_TILE_XY(bmap, tile_col, tile_row);
        coll |= bmap->tiles->coll[(tile-1)];
    }

    return coll;
}

u8 check_row(blastmap* bmap, spritedef* sprite, u8 tile_col) {
    u8 coll;
    u16 tile;
    // How many rows to test?
    u8 tile_row = sprite->posy / 8;
    u8 end_row = tile_row + sprite->height;
    if(((sprite->posy) % 8) != 0) {
        // We are crossing rows
        end_row++;
    }

    coll = 0;
    for(; tile_row<end_row;tile_row++) {
        tile = GET_TILE_XY(bmap, tile_col, tile_row);
        coll |= bmap->tiles->coll[(tile-1)];
                
    }

    return coll;
}

u8 check_left(blastmap* bmap, spritedef* sprite) {
    char str[10];
    // Check if we are at a tile boundary, if not skip.
    if(((sprite->posx) % 8) != 0) {
        return 0;
    }
    // 
    u8 tile_col = (sprite->posx - 1) / 8;
    return check_row(bmap, sprite, tile_col);
}

u8 check_right(blastmap* bmap, spritedef* sprite) {
    char str[10];
    
    // Check if we are at a tile boundary, if not skip.
    if(((sprite->posx) % 8) != 0) {
        return 0;
    }

    // Which column are we testing? 
    u8 tile_col = (sprite->posx + (sprite->width*8)) / 8;
    return check_row(bmap, sprite, tile_col);
}

void screen_left(blastmap* bmap, u16* hScroll) {
    *hScroll+=1;
    if(*hScroll % 8 == 0) {
        bmap->tX-=1;
        bmap->winX-=1;
        if(bmap->tX>=bmap->planwidth){
            bmap->tX = bmap->planwidth - 1;
        }
        if(bmap->winX>=bmap->mapw) {
            bmap->winX = bmap->mapw - 1;
        }
        load_map_col(bmap,0);
    }
}

void screen_right(blastmap* bmap, u16* hScroll) {
    *hScroll-=1;
    if(*hScroll % 8 == 0) {
        bmap->tX+=1;
        bmap->winX+=1;
        if(bmap->tX>=bmap->planwidth){
            bmap->tX = 0;
        }
        if(bmap->winX>=bmap->mapw) {
            bmap->winX = 0;
        }
        load_map_col(bmap,winW+1);
    }
}

void screen_up(blastmap* bmap, u16* vScroll) {
    *vScroll-=1;
    if(*vScroll % 8 == 0) {
        bmap->tY-=1;
        bmap->winY-=1;
        if(bmap->tY>=bmap->planheight){
            bmap->tY = bmap->planheight - 1;
        }
        if(bmap->winY>=bmap->maph){
            bmap->winY = bmap->maph - 1;
        }
        load_map_row(bmap,0);
    }
}
void screen_down(blastmap* bmap, u16* vScroll) {
    *vScroll+=1;
    if(*vScroll % 8 == 0) {
        bmap->tY+=1;
        bmap->winY+=1;
        if(bmap->tY>=bmap->planheight){
            bmap->tY = 0;
        }
        if(bmap->winY>=bmap->maph){
            bmap->winY = 0;
        }
        load_map_row(bmap,winH+1);
    }
}

void load_map(blastmap* bmap, int xoffset, int yoffset) {
    /*
     * load_map
     *
     * Loads an entire map
     *
     */

    int i;
    int j;

    //int start_index = 1;
    bmap->maph = bmap->tiles->height;
    bmap->mapw = bmap->tiles->width;

    int tilex;
    int tiley;
    
    for(i=0;i<bmap->maph;i++) {
        if(i+yoffset >= bmap->planheight) {
            tiley = (i + yoffset) - bmap->planheight;
        } else {
            tiley = i + yoffset;
        }

        for(j=0;j<bmap->mapw;j++) {
            if(j+xoffset >= bmap->planwidth) {
                tilex = (j + xoffset) - bmap->planwidth;

            } else {
                tilex = j + xoffset;

            }
            VDP_setTileMapXY(
                    bmap->plane, 
                    //(u16*)bmap->tiles->data[(i * (bmap->mapw)) + j] + bmap->tileoffset - 1,
                    (u16*)GET_TILE_XY(bmap, j, i), 
                    tilex,
                    tiley
            );
        }
    }
}

void load_visible_map(blastmap* bmap, int xoffset, int yoffset) {
    /*
     * load_visible_map
     *
     * Load only the visible portion of a map
     *
     */

    u16 i;
    u16 j;
    u16 iW;
    u16 jW;
    u8 ycount;
    u8 xcount;

    bmap->mapw = bmap->tiles->width;
    bmap->maph = bmap->tiles->height;

    i = bmap->tY;
    iW = bmap->winY;
    for(ycount=0;ycount<winH;ycount++) {

        // wrap around
        if(i == bmap->planheight) {
            i -= bmap->planheight;
        } 
        if(iW >= bmap->maph) {
            iW -= bmap->maph;
        }

        j = bmap->tX;
        jW = bmap->winX;
        for(xcount=0;xcount<winW;xcount++){
            if(j == bmap->planwidth) {
                j -= bmap->planwidth;
            } 
            if(jW == bmap->mapw) {
                jW = 0;
            }
            VDP_setTileMapXY(
                    bmap->plane, 
                    (u16*)(bmap->tiles->data[(iW * (bmap->mapw)) + jW]) + bmap->tileoffset - 1,
                    j,
                    i
            );
            j++;
            jW++;
        }
        i++;
        iW++;
    }

}

void load_map_row(blastmap* bmap, u8 row) {
    /*
     * load_map_row
     *
     * Load a single row of the map
     *
     */

    u8 i;
    u8 j;
    u16 iW;
    u16 jW;
    u8 xcount;

    i = bmap->tY + row;
    iW = bmap->winY + row;

    // wrap around
    if(i >= bmap->planheight) {
        i -= bmap->planheight;
    } 
    if(iW >= bmap->maph) {
        iW -= bmap->maph;
    }

    j = bmap->tX;
    jW = bmap->winX;
    for(xcount=0;xcount<winW;xcount++){
        if(j == bmap->planwidth) {
            j = 0;
        } 
        if(jW == bmap->mapw) {
            jW = 0;
        }
        VDP_setTileMapXY(
                bmap->plane, 
                (u16*)bmap->tiles->data[(iW * (bmap->mapw)) + jW] + bmap->tileoffset - 1,
                j,
                i
        );
        j++;
        jW++;
    }
}

void load_map_col(blastmap* bmap, u8 col) {
    /*
     * load_map_col
     *
     * Load a single column of the map
     *
     */

    u8 i;
    u8 j;
    u16 iW;
    u16 jW;
    u8 ycount;

    i = bmap->tX + col;
    iW = bmap->winX + col;

    // wrap around
    if(i >= bmap->planwidth) {
        i -= bmap->planwidth;
    } 
    if(iW >= bmap->mapw) {
        iW -= bmap->mapw;
    }

    j = bmap->tY;
    jW = bmap->winY;
    for(ycount=0;ycount<winH;ycount++){
        if(j == bmap->planheight) {
            j = 0;
        } 
        if(jW == bmap->maph) {
            jW = 0;
        }
        VDP_setTileMapXY(
                bmap->plane, 
                (u16*)bmap->tiles->data[(jW * (bmap->mapw)) + iW] + bmap->tileoffset - 1,
                i,
                j
        );
        jW++;
        j++;
    }
}

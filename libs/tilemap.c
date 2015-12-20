#include "genesis.h"
#include "tilemap.h"


//void blast_init(datamap* blastdata) {
void blast_init(blastmap* bmap) {
    //blastdata = malloc(sizeof(datamap));
    bmap->data->tX = 64; 
    bmap->data->tY = 64;
    bmap->data->winX = bmap->data->tX;
    bmap->data->winY = bmap->data->tY;
    bmap->data->maph = 0;
    bmap->data->mapw = 0;    
    bmap->data->planwidth = VDP_getPlanWidth();
    bmap->data->planheight = VDP_getPlanHeight();
}

void screen_left(blastmap* bmap, u16* hScroll, u16 tile_offset, u16 plan) {
    *hScroll+=1;
    if(*hScroll % 8 == 0) {
        bmap->data->tX-=1;
        bmap->data->winX-=1;
        if(bmap->data->tX>=bmap->data->planwidth){
            bmap->data->tX = bmap->data->planwidth - 1;
        }
        if(bmap->data->winX>=bmap->data->mapw) {
            bmap->data->winX = bmap->data->mapw - 1;
        }
        load_map_col(bmap,plan,0,tile_offset);
    }
}

void screen_right(blastmap* bmap, u16* hScroll, u16 tile_offset, u16 plan) {
    *hScroll-=1;
    if(*hScroll % 8 == 0) {
        bmap->data->tX+=1;
        bmap->data->winX+=1;
        if(bmap->data->tX>=bmap->data->planwidth){
            bmap->data->tX = 0;
        }
        if(bmap->data->winX>=bmap->data->mapw) {
            bmap->data->winX = 0;
        }
        load_map_col(bmap,plan,winW+1,tile_offset);
    }
}

void screen_up(blastmap* bmap, u16* vScroll, u16 tile_offset, u16 plan) {
    *vScroll-=1;
    if(*vScroll % 8 == 0) {
        bmap->data->tY-=1;
        bmap->data->winY-=1;
        if(bmap->data->tY>=bmap->data->planheight){
            bmap->data->tY = bmap->data->planheight - 1;
        }
        if(bmap->data->winY>=bmap->data->maph){
            bmap->data->winY = bmap->data->maph - 1;
        }
        load_map_row(bmap,plan,0,tile_offset);
    }
}
void screen_down(blastmap* bmap, u16* vScroll, u16 tile_offset, u16 plan) {
    *vScroll+=1;
    if(*vScroll % 8 == 0) {
        bmap->data->tY+=1;
        bmap->data->winY+=1;
        if(bmap->data->tY>=bmap->data->planheight){
            bmap->data->tY = 0;
        }
        if(bmap->data->winY>=bmap->data->maph){
            bmap->data->winY = 0;
        }
        load_map_row(bmap,plan,winH+1,tile_offset);
    }
}

void load_map(blastmap* bmap, int xoffset, int yoffset, u16 plan, u16 tileoffset) {
    int i;
    int j;

    //int start_index = 1;
    bmap->data->maph = bmap->tiles->height;
    bmap->data->mapw = bmap->tiles->width;

    int tilex;
    int tiley;
    
    for(i=0;i<bmap->data->maph;i++) {
        if(i+yoffset >= bmap->data->planheight) {
            tiley = (i + yoffset) - bmap->data->planheight;
        } else {
            tiley = i + yoffset;
        }

        for(j=0;j<bmap->data->mapw;j++) {
            if(j+xoffset >= bmap->data->planwidth) {
                tilex = (j + xoffset) - bmap->data->planwidth;

            } else {
                tilex = j + xoffset;

            }
            VDP_setTileMapXY(
                    plan, 
                    (u16*)bmap->tiles->data[(i * (bmap->data->mapw)) + j] + tileoffset - 1,
                    tilex,
                    tiley
            );
        }
    }
}

void load_visible_map(blastmap* bmap, int xoffset, int yoffset, u16 plan, u16 tileoffset) {
    u16 i;
    u16 j;
    u16 iW;
    u16 jW;
    u8 ycount;
    u8 xcount;

    bmap->data->mapw = bmap->tiles->width;
    bmap->data->maph = bmap->tiles->height;

    i = bmap->data->tY;
    iW = bmap->data->winY;
    for(ycount=0;ycount<winH;ycount++) {

        // wrap around
        if(i == bmap->data->planheight) {
            i -= bmap->data->planheight;
        } 
        if(iW >= bmap->data->maph) {
            iW -= bmap->data->maph;
        }

        j = bmap->data->tX;
        jW = bmap->data->winX;
        for(xcount=0;xcount<winW;xcount++){
            if(j == bmap->data->planwidth) {
                j -= bmap->data->planwidth;
            } 
            if(jW == bmap->data->mapw) {
                jW = 0;
            }
            VDP_setTileMapXY(
                    plan, 
                    (u16*)(bmap->tiles->data[(iW * (bmap->data->mapw)) + jW]) + tileoffset - 1,
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

void load_map_row(blastmap* bmap, u16 plan, u8 row, u16 tileoffset) {
    u8 i;
    u8 j;
    u16 iW;
    u16 jW;
    u8 xcount;

    i = bmap->data->tY + row;
    iW = bmap->data->winY + row;

    // wrap around
    if(i >= bmap->data->planheight) {
        i -= bmap->data->planheight;
    } 
    if(iW >= bmap->data->maph) {
        iW -= bmap->data->maph;
    }

    j = bmap->data->tX;
    jW = bmap->data->winX;
    for(xcount=0;xcount<winW;xcount++){
        if(j == bmap->data->planwidth) {
            j = 0;
        } 
        if(jW == bmap->data->mapw) {
            jW = 0;
        }
        VDP_setTileMapXY(
                plan, 
                (u16*)bmap->tiles->data[(iW * (bmap->data->mapw)) + jW] + tileoffset - 1,
                j,
                i
        );
        j++;
        jW++;
    }
}

void load_map_col(blastmap* bmap, u16 plan, u8 col, u16 tileoffset) {
    u8 i;
    u8 j;
    u16 iW;
    u16 jW;
    u8 ycount;

    i = bmap->data->tX + col;
    iW = bmap->data->winX + col;

    // wrap around
    if(i >= bmap->data->planwidth) {
        i -= bmap->data->planwidth;
    } 
    if(iW >= bmap->data->mapw) {
        iW -= bmap->data->mapw;
    }

    j = bmap->data->tY;
    jW = bmap->data->winY;
    for(ycount=0;ycount<winH;ycount++){
        if(j == bmap->data->planheight) {
            j = 0;
        } 
        if(jW == bmap->data->maph) {
            jW = 0;
        }
        VDP_setTileMapXY(
                plan, 
                (u16*)bmap->tiles->data[(jW * (bmap->data->mapw)) + iW] + tileoffset - 1,
                i,
                j
        );
        jW++;
        j++;
    }
}

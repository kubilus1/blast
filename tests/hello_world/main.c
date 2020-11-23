#include "blast.h"

int main( )
{
    blast_init();
    VDP_drawText("Hello World!", 1, 1);

    while(1)
    {
        VDP_waitVSync();
    }
    return 0;
}

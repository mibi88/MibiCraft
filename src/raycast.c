#include <raycast.h>
#include <math.h>
#include <gfx.h>

/* I'll use DDA soon. */
#define STEP 0.01

void raycast(World *world, int *cx, int *cy, int *cz, float sx, float sy,
             float sz, float rx, float ry, float length) {
    float x = sx, y = sy, z = sz;
    float len = 0;
    float sin_rx = sin((rx+90)/180*PI);
    while(len < length){
        x += cos((ry-90)/180*PI)*0.25*sin_rx*STEP;
        y -= sin(rx/180*PI)*0.25*STEP;
        z += sin((ry-90)/180*PI)*0.25*sin_rx*STEP;
        if(world_get_tile(world, x, y, z)){
            *cx = (int)x;
            *cy = (int)y;
            *cz = (int)z;
            return;
        }
    }
}

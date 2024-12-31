#include <raycast.h>
#include <math.h>
#include <stdlib.h>
#include <gfx.h>

#define STEP 0.0001

void raycast(World *world, Entity *player, float len,
             int voxel(int x, int y, int z, void *data)) {
    /*float fx, fy, fz, sin_rx;
    int x, y, z, step_x, step_y, step_z;
    sin_rx = sin((player->rx+90)/180*PI);
    fx = player->x+cos((player->ry-90)/180*PI)*sin_rx*len;
    fy = player->y-sin(player->rx/180*PI)*len;
    fz = player->z+sin((player->ry-90)/180*PI)*sin_rx*len;
    x = player->x;
    step_x = x-fx > 0 ? -1 : 1;
    y = player->y;
    step_y = y-fy > 0 ? -1 : 1;*/
    float cos_rx = cos((player->rx)/180*PI);
    float xinc = cos((player->ry-90)/180*PI)*cos_rx;
    float yinc = -sin(player->rx/180*PI);
    float zinc = sin((player->ry-90)/180*PI)*cos_rx;
    float x = player->x;
    float y = player->y;
    float z = player->z;
    int old_x = x;
    int old_y = y;
    int old_z = z;
    float l = 0;
    while(l < len){
        x += xinc*STEP;
        y += yinc*STEP;
        z += zinc*STEP;
        l += STEP;
        if((int)x != old_x || (int)y != old_y || (int)z != old_z){
            if(voxel(x, y+CHUNK_HEIGHT/2, z, world)){
                break;
            }
            old_x = x;
            old_y = y;
            old_z = z;
        }
    }
}

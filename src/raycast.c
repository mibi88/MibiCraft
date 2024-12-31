#include <raycast.h>
#include <math.h>
#include <stdlib.h>
#include <gfx.h>

#define STEP 0.05

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
    double cos_rx = cos((player->rx)/180*PI);
    double xinc = cos((player->ry-90)/180*PI)*cos_rx;
    double yinc = -sin(player->rx/180*PI);
    double zinc = sin((player->ry-90)/180*PI)*cos_rx;
    double x = player->x;
    double y = player->y+CHUNK_HEIGHT/2;
    double z = player->z;
    double old_x = floor(x);
    double old_y = floor(y);
    double old_z = floor(z);
    double l = 0;
    puts("Raycasting");
    while(l < len){
        x += xinc*STEP;
        y += yinc*STEP;
        z += zinc*STEP;
        l += STEP;
        if(floor(x) != old_x || floor(y) != old_y || floor(z) != old_z){
            if(voxel(floor(x+0.5), floor(y+0.5), floor(z+0.5), world)){
                printf("Point at: %f, %f, %f\n", x, y, z);
                gfx_render_wire_cube(x, y, z, 0.5);
                break;
            }
            old_x = floor(x);
            old_y = floor(y);
            old_z = floor(z);
        }
    }
}

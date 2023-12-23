#include <raycast.h>
#include <math.h>
#include <stdlib.h>
#include <gfx.h>

void raycast(World *world, Entity *player, float len,
             int voxel(int x, int y, int z, void *data)) {
    float fx, fy, fz, sin_rx;
    int x, y, z, step_x, step_y, step_z;
    sin_rx = sin((player->rx+90)/180*PI);
    fx = player->x+cos((player->ry-90)/180*PI)*sin_rx*len;
    fy = player->y-sin(player->rx/180*PI)*len;
    fz = player->z+sin((player->ry-90)/180*PI)*sin_rx*len;
    x = player->x;
    step_x = x-fx > 0 ? -1 : 1;
    y = player->y;
    step_y = y-fy > 0 ? -1 : 1;
}

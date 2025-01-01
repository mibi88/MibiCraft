#include <raycast.h>
#include <math.h>
#include <stdlib.h>
#include <gfx.h>

#include <float.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

void raycast(World *world, Entity *player, float len,
             int voxel(int x, int y, int z, void *data)) {
    /*double cos_rx = cos((player->rx)/180*PI);
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
    while(l < len){
        x += xinc*STEP;
        y += yinc*STEP;
        z += zinc*STEP;
        l += STEP;
        if(floor(x) != old_x || floor(y) != old_y || floor(z) != old_z){
            if(voxel(floor(x+0.5), floor(y+0.5), floor(z+0.5), world)){
                break;
            }
            old_x = floor(x);
            old_y = floor(y);
            old_z = floor(z);
        }
    }*/
    float cos_rx = cos((player->rx)/180*PI);
    float xinc = cos((player->ry-90)/180*PI)*cos_rx;
    float yinc = -sin(player->rx/180*PI);
    float zinc = sin((player->ry-90)/180*PI)*cos_rx;
    float x = player->x+0.5;
    float y = player->y+CHUNK_HEIGHT/2+0.5;
    float z = player->z+0.5;
    int px = (int)x;
    int py = (int)y;
    int pz = (int)z;
    float raystep_x, raystep_y;
    float start_x, start_y;
    float steplen_x, steplen_y;
    float rays_x, rays_y;
    float end_len;
    raystep_x = fabs((yinc*len)/(xinc*len != 0 ? xinc*len : FLT_MIN));
    raystep_y = fabs((xinc*len)/(yinc*len != 0 ? yinc*len : FLT_MIN));
    steplen_x = sqrt(raystep_x*raystep_x+1);
    steplen_y = sqrt(raystep_y*raystep_y+1);
    if(xinc*len > 0){
        if(x > 0) start_x = 1-(x-floor(x));
        else start_x = 1-(x-ceil(x));
    }else{
        if(x > 0) start_x = x-floor(x);
        else start_x = x-ceil(x);
    }
    if(yinc*len > 0){
        if(y > 0) start_y = 1-(y-floor(y));
        else start_y = 1-(y-ceil(y));
    }else{
        if(y > 0) start_y = y-floor(y);
        else start_y = y-ceil(y);
    }
    rays_x = steplen_x*start_x;
    rays_y = steplen_y*start_y;
    if(rays_x < rays_y){
        end_len = rays_x;
    }else{
        end_len = rays_y;
    }
    while(end_len < len){
        if(rays_x < rays_y){
            px += xinc*len > 0 ? 1 : -1;
            rays_x += steplen_x;
        }else{
            py += yinc*len > 0 ? 1 : -1;
            rays_y += steplen_y;
        }
        if(voxel(px, py, pz, world)){
            break;
        }
        end_len = rays_x < rays_y ? rays_x : rays_y;
    }
}

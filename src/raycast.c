#include <raycast.h>
#include <math.h>
#include <stdlib.h>
#include <gfx.h>

#include <float.h>

#define STEP 0.000005

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
    float raystep_x, raystep_y, raystep_z;
    float start_x, start_y, start_z;
    float steplen_x, steplen_y, steplen_z;
    float rays_x, rays_y, rays_z;
    float end_len;
    raystep_x = 1/(xinc != 0 ? xinc : FLT_MIN);
    raystep_y = 1/(yinc != 0 ? yinc : FLT_MIN);
    raystep_z = 1/(zinc != 0 ? zinc : FLT_MIN);
    steplen_x = sqrt(raystep_x*raystep_x+1);
    steplen_y = sqrt(raystep_y*raystep_y+1);
    steplen_z = sqrt(raystep_z*raystep_z+1);
    if(xinc*len > 0){
        start_x = 1-(x-floor(x));
    }else{
        start_x = x-floor(x);
    }
    if(yinc*len > 0){
        start_y = 1-(y-floor(y));
    }else{
        start_y = y-floor(y);
    }
    if(zinc*len > 0){
        start_z = 1-(z-floor(z));
    }else{
        start_z = z-floor(z);
    }
    rays_x = steplen_x*start_x;
    rays_y = steplen_y*start_y;
    rays_z = steplen_z*start_z;
    if(rays_x < rays_y){
        end_len = rays_z < rays_x ? rays_z : rays_x;
    }else{
        end_len = rays_z < rays_y ? rays_z : rays_y;
    }
    while(end_len < len){
        if(rays_x < rays_y){
            if(rays_z < rays_x){
                pz += zinc*len > 0 ? 1 : -1;
                rays_z += steplen_z;
            }else{
                px += xinc*len > 0 ? 1 : -1;
                rays_x += steplen_x;
            }
        }else{
            if(rays_z < rays_y){
                pz += zinc*len > 0 ? 1 : -1;
                rays_z += steplen_z;
            }else{
                py += yinc*len > 0 ? 1 : -1;
                rays_y += steplen_y;
            }
        }
        if(voxel(px, py, pz, world)){
            break;
        }
        if(rays_x < rays_y){
            end_len = rays_x < rays_z ? rays_x : rays_z;
        }else{
            end_len = rays_y < rays_z ? rays_y : rays_z;
        }
    }
}

#include <raycast.h>
#include <math.h>
#include <stdlib.h>
#include <gfx.h>

#include <float.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

void raycast(Entity *player, float len,
             int voxel(int x, int y, int z, void *data), void *data) {
    float cos_rx = cos((player->rx)/180*PI);
    float xinc = cos((player->ry-90)/180*PI)*cos_rx;
    float yinc = -sin(player->rx/180*PI);
    float zinc = sin((player->ry-90)/180*PI)*cos_rx;
    float x = player->x+0.5;
    float y = player->y+CHUNK_HEIGHT/2+0.5;
    float z = player->z+0.5;
    int px = (int)floor(x);
    int py = (int)floor(y);
    int pz = (int)floor(z);
    float start_x, start_y, start_z;
    float steplen_x, steplen_y, steplen_z;
    float rays_x, rays_y, rays_z;
    float end_len;
    steplen_x = (xinc > 0 ? 1 : (xinc != 0 ? -1 : 0))/(xinc*len);
    steplen_y = (yinc > 0 ? 1 : (yinc != 0 ? -1 : 0))/(yinc*len);
    steplen_z = (zinc > 0 ? 1 : (zinc != 0 ? -1 : 0))/(zinc*len);
    if(xinc >= 0){
        if(x > 0) start_x = 1-(x-floor(x));
        else start_x = 1-(-floor(x)+x);
    }else{
        if(x > 0) start_x = x-floor(x);
        else start_x = -floor(x)+x;
    }
    if(yinc >= 0){
        if(y > 0) start_y = 1-(y-floor(y));
        else start_y = 1-(-floor(y)+y);
    }else{
        if(y > 0) start_y = y-floor(y);
        else start_y = -floor(y)+y;
    }
    if(zinc >= 0){
        if(z > 0) start_z = 1-(z-floor(z));
        else start_z = 1-(-floor(z)+z);
    }else{
        if(z > 0) start_z = z-floor(z);
        else start_z = -floor(z)+z;
    }
    rays_x = steplen_x*start_x;
    rays_y = steplen_y*start_y;
    rays_z = steplen_z*start_z;
    if(rays_x < rays_y){
        end_len = rays_x < rays_z ? rays_x : rays_z;
    }else{
        end_len = rays_y < rays_z ? rays_y : rays_z;
    }
    while(end_len < len){
        if(rays_x < rays_y){
            if(rays_x < rays_z){
                px += xinc*len > 0 ? 1 : -1;
                rays_x += steplen_x;
            }else{
                pz += zinc*len > 0 ? 1 : -1;
                rays_z += steplen_z;
            }
        }else{
            if(rays_y < rays_z){
                py += yinc*len > 0 ? 1 : -1;
                rays_y += steplen_y;
            }else{
                pz += zinc*len > 0 ? 1 : -1;
                rays_z += steplen_z;
            }
        }
        if(voxel(px, py, pz, data)){
            break;
        }
        if(rays_x < rays_z){
            end_len = rays_x < rays_y ? rays_x : rays_y;
        }else{
            end_len = rays_z < rays_y ? rays_z : rays_y;
        }
    }
}

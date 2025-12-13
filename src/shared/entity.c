/*
 * MibiCraft - A small game in a world of cubes
 * Copyright (C) 2023  Mibi88
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see https://www.gnu.org/licenses/.
 */

#include <entity.h>

#include <math.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int entity_on_floor(Entity *entity, World *world) {
    float x, z;
    float xmin, xmax;
    float zmin, zmax;
    xmin = MIN(entity->hitbox[0], entity->hitbox[3]);
    xmax = MAX(entity->hitbox[0], entity->hitbox[3]);
    zmin = MIN(entity->hitbox[2], entity->hitbox[5]);
    zmax = MAX(entity->hitbox[2], entity->hitbox[5]);
    for(z=entity->z+zmin;z<entity->z+zmax;z+=0.5){
        for(x=entity->x+xmin;x<entity->x+xmax;x+=0.5){
            if(blocks[world_get_tile(world, floor(0.5+x), floor(entity->y+0.5+
                              MIN(entity->hitbox[1], entity->hitbox[4])+
                              CHUNK_HEIGHT/2-0.1), floor(0.5+z))].solid){
                return 1;
            }
        }
    }
    return 0;
}

int entity_on_ceiling(Entity *entity, World *world) {
    float x, z;
    float xmin, xmax;
    float zmin, zmax;
    xmin = MIN(entity->hitbox[0], entity->hitbox[3]);
    xmax = MAX(entity->hitbox[0], entity->hitbox[3]);
    zmin = MIN(entity->hitbox[2], entity->hitbox[5]);
    zmax = MAX(entity->hitbox[2], entity->hitbox[5]);
    for(z=entity->z+zmin;z<entity->z+zmax;z+=0.5){
        for(x=entity->x+xmin;x<entity->x+xmax;x+=0.5){
            if(blocks[world_get_tile(world, floor(0.5+x), floor(entity->y+0.5+
                              MAX(entity->hitbox[1], entity->hitbox[4])+
                              CHUNK_HEIGHT/2+0.1), floor(0.5+z))].solid){
                return 1;
            }
        }
    }
    return 0;
}

int entity_colliding(Entity *entity, World *world) {
    float x, y, z;
    float xmin, xmax;
    float ymin, ymax;
    float zmin, zmax;
    xmin = MIN(entity->hitbox[0], entity->hitbox[3]);
    xmax = MAX(entity->hitbox[0], entity->hitbox[3]);
    ymin = MIN(entity->hitbox[1], entity->hitbox[4]);
    ymax = MAX(entity->hitbox[1], entity->hitbox[4]);
    zmin = MIN(entity->hitbox[2], entity->hitbox[5]);
    zmax = MAX(entity->hitbox[2], entity->hitbox[5]);
    for(z=entity->z+zmin;z<entity->z+zmax;z+=0.5){
        for(y=entity->y+ymin;y<entity->y+ymax;y+=0.5){
            for(x=entity->x+xmin;x<entity->x+xmax;x+=0.5){
                if(blocks[world_get_tile(world, floor(0.5+x),
                                  floor(0.5+y+CHUNK_HEIGHT/2),
                                  floor(0.5+z))].solid){
                    return 1;
                }
            }
        }
    }
    return 0;
}

float entity_space(Entity *entity, World *world, int ix, int iy, int iz,
                   float max) {
    float x, y, z;
    float xmin, xmax;
    float ymin, ymax;
    float zmin, zmax;
    xmin = MIN(entity->hitbox[0], entity->hitbox[3]);
    xmax = MAX(entity->hitbox[0], entity->hitbox[3]);
    ymin = MIN(entity->hitbox[1], entity->hitbox[4]);
    ymax = MAX(entity->hitbox[1], entity->hitbox[4]);
    zmin = MIN(entity->hitbox[2], entity->hitbox[5]);
    zmax = MAX(entity->hitbox[2], entity->hitbox[5]);
    if(ix != 0){
        for(z=entity->z+zmin;z<entity->z+zmax;z+=0.5){
            for(y=entity->y+ymin;y<entity->y+ymax;y+=0.5){
                for(x=entity->x+(ix < 0 ? xmin : xmax);
                    x<entity->x+max+xmax && x>entity->x-max+xmin;
                    x+=ix > 0 ? 0.5 : -0.5){
                    if(blocks[world_get_tile(world, floor(0.5+x),
                                      floor(0.5+y+CHUNK_HEIGHT/2),
                                      floor(0.5+z))].solid){
                        return fabs(entity->x-floor(x)-0.5)-
                               fabs(ix > 0 ? xmax : xmin);
                    }
                }
            }
        }
    }else if(iy != 0){
        for(z=entity->z+zmin;z<entity->z+zmax;z+=0.5){
            for(y=entity->y+(iy < 0 ? ymin : ymax);
                y<entity->y+max+ymax && y>entity->y-max+ymin;
                y+=iy > 0 ? 0.5 : -0.5){
                for(x=entity->x+xmin;x<entity->x+xmax;x+=0.5){
                    if(blocks[world_get_tile(world, floor(0.5+x),
                                      floor(0.5+y+CHUNK_HEIGHT/2),
                                      floor(0.5+z))].solid){
                        return fabs(entity->y-floor(y)-0.5)-
                               fabs(iy > 0 ? ymax : ymin);
                    }
                }
            }
        }
    }else if(iz != 0){
        for(z=entity->z+(iz < 0 ? zmin : zmax);
            z<entity->z+max+zmax && z>entity->z-max+zmin;
            z+=iz > 0 ? 0.5 : -0.5){
            for(y=entity->y+ymin;y<entity->y+ymax;y+=0.5){
                for(x=entity->x+xmin;x<entity->x+xmax;x+=0.5){
                    if(blocks[world_get_tile(world, floor(0.5+x),
                                      floor(0.5+y+CHUNK_HEIGHT/2),
                                      floor(0.5+z))].solid){
                        return fabs(entity->z-floor(z)-0.5)-
                               fabs(iz > 0 ? zmax : zmin);
                    }
                }
            }
        }
    }
    return max;
}

int entity_is_inside(Entity *entity, World *world, float x, float y, float z) {
    float xmin, xmax;
    float ymin, ymax;
    float zmin, zmax;

    (void)world;

    xmin = MIN(entity->hitbox[0], entity->hitbox[3]);
    xmax = MAX(entity->hitbox[0], entity->hitbox[3]);
    ymin = MIN(entity->hitbox[1], entity->hitbox[4]);
    ymax = MAX(entity->hitbox[1], entity->hitbox[4]);
    zmin = MIN(entity->hitbox[2], entity->hitbox[5]);
    zmax = MAX(entity->hitbox[2], entity->hitbox[5]);

    if(x >= entity->x+xmin && x <= entity->x+xmax &&
       y >= entity->y+ymin && y <= entity->y+ymax &&
       z >= entity->z+zmin && z <= entity->z+zmax){
        return 1;
    }

    return 0;
}

int entity_is_block_inside(Entity *entity, World *world, int sx, int sy,
                           int sz) {
    float xmin, xmax;
    float ymin, ymax;
    float zmin, zmax;
    float x, y, z;

    (void)world;

    x = sx-0.5;
    y = sy-0.5-CHUNK_HEIGHT/2;
    z = sz-0.5;

    xmin = MIN(entity->hitbox[0], entity->hitbox[3]);
    xmax = MAX(entity->hitbox[0], entity->hitbox[3]);
    ymin = MIN(entity->hitbox[1], entity->hitbox[4]);
    ymax = MAX(entity->hitbox[1], entity->hitbox[4]);
    zmin = MIN(entity->hitbox[2], entity->hitbox[5]);
    zmax = MAX(entity->hitbox[2], entity->hitbox[5]);

    if((x > entity->x+xmin || x+1 > entity->x+xmin) &&
       (x < entity->x+xmax || x+1 < entity->x+xmax) &&
       (y > entity->y+ymin || y+1 > entity->y+ymin) &&
       (y < entity->y+ymax || y+1 < entity->y+ymax) &&
       (z > entity->z+zmin || z+1 > entity->z+zmin) &&
       (z < entity->z+xmax || z+1 < entity->z+zmax)){
        return 1;
    }
    return 0;
}

int entity_contains_block(Entity *entity, World *world, Tile block,
                          float hitbox[HITBOX_SZ*3]) {
    float x, y, z;
    float xmin, xmax;
    float ymin, ymax;
    float zmin, zmax;
    xmin = MIN(hitbox[0], hitbox[3]);
    xmax = MAX(hitbox[0], hitbox[3]);
    ymin = MIN(hitbox[1], hitbox[4]);
    ymax = MAX(hitbox[1], hitbox[4]);
    zmin = MIN(hitbox[2], hitbox[5]);
    zmax = MAX(hitbox[2], hitbox[5]);
    for(z=entity->z+zmin;z<entity->z+zmax;z+=0.5){
        for(y=entity->y+ymin;y<entity->y+ymax;y+=0.5){
            for(x=entity->x+xmin;x<entity->x+xmax;x+=0.5){
                if(world_get_tile(world, floor(0.5+x),
                                  floor(0.5+y+CHUNK_HEIGHT/2),
                                  floor(0.5+z)) == block){
                    return 1;
                }
            }
        }
    }
    return 0;
}

void entity_update(Entity *entity, World *world, float delta) {
    float xmov, ymov, zmov;
    float dist;
    xmov = entity->x_velocity;
    zmov = entity->z_velocity;
    if(xmov < 0){
        dist = entity_space(entity, world, -1, 0, 0, ceil(fabs(xmov)));
        if(dist < -xmov){
            xmov = -dist;
        }
    }else if(xmov > 0){
        dist = entity_space(entity, world, 1, 0, 0, ceil(fabs(xmov)));
        if(dist < xmov){
            xmov = dist;
        }
    }
    entity->x += xmov;
    if(zmov < 0){
        dist = entity_space(entity, world, 0, 0, -1, ceil(fabs(zmov)));
        if(dist < -zmov){
            zmov = -dist;
        }
    }else if(zmov > 0){
        dist = entity_space(entity, world, 0, 0, 1, ceil(fabs(zmov)));
        if(dist < zmov){
            zmov = dist;
        }
    }
    entity->z += zmov;
    ymov = entity->y_velocity*delta;
    if(ymov < 0){
        dist = entity_space(entity, world, 0, -1, 0, ceil(fabs(ymov)));
        if(dist < -ymov){
            ymov = -dist;
            entity->y_velocity = 0;
        }
    }else if(ymov > 0){
        dist = entity_space(entity, world, 0, 1, 0, ceil(fabs(ymov)));
        if(dist < ymov){
            ymov = dist;
            entity->y_velocity = 0;
        }
    }
    entity->y += ymov;
    entity->y_velocity -= entity->gravity*delta;
}

int entity_in_water(Entity *entity, World *world) {
    return world_get_tile(world, floor(entity->x+0.5),
                          floor(entity->y+0.5+CHUNK_HEIGHT/2),
                          floor(entity->z+0.5)) == T_WATER;
}


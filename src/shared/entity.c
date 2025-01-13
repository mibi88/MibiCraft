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

extern const Block_property blocks[T_AMOUNT];

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

int entity_is_inside(Entity *entity, World *world, float x, float y, float z) {
    float xmin, xmax;
    float ymin, ymax;
    float zmin, zmax;
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
    x = sx-0.5;
    y = sy-0.5-CHUNK_HEIGHT/2;
    z = sz-0.5;
    xmin = MIN(entity->hitbox[0], entity->hitbox[3]);
    xmax = MAX(entity->hitbox[0], entity->hitbox[3]);
    ymin = MIN(entity->hitbox[1], entity->hitbox[4]);
    ymax = MAX(entity->hitbox[1], entity->hitbox[4]);
    zmin = MIN(entity->hitbox[2], entity->hitbox[5]);
    zmax = MAX(entity->hitbox[2], entity->hitbox[5]);
    if((x >= entity->x+xmin || x+1 >= entity->x+xmin) &&
       (x <= entity->x+xmax || x+1 <= entity->x+xmax) &&
       (y >= entity->y+ymin || y+1 >= entity->y+ymin) &&
       (y <= entity->y+ymax || y+1 <= entity->y+ymax) &&
       (z >= entity->z+zmin || z+1 >= entity->z+zmin) &&
       (z <= entity->z+xmax || z+1 <= entity->z+zmax)){
        return 1;
    }
    return 0;
}

void entity_update(Entity *entity, World *world, float delta) {
    float xmov, zmov;
    xmov = cos((entity->ry-90)/180*PI)*entity->velocity*entity->acceleration;
    zmov = sin((entity->ry-90)/180*PI)*entity->velocity*entity->acceleration;
    if(entity_colliding(entity, world)){
        entity->x -= xmov;
        entity->z -= zmov;
        entity->velocity = 0;
    }
    entity->x += xmov;
    if(entity_colliding(entity, world)){
        entity->x -= xmov;
    }
    entity->z += zmov;
    if(entity_colliding(entity, world)){
        entity->z -= zmov;
    }
    entity->y += entity->y_velocity*delta;
    if(entity->velocity > 0){
        entity->velocity -= entity->deceleration;
        if(entity->velocity < 0){
            entity->velocity = 0;
        }
    }else if(entity->velocity < 0){
        entity->velocity += entity->deceleration;
        if(entity->velocity > 0){
            entity->velocity = 0;
        }
    }
    entity->y_velocity -= entity->gravity;
    if(entity_on_floor(entity, world) && entity->y_velocity < 0){
        entity->y_velocity = 0;
        if(entity_colliding(entity, world)){
            entity->y = floor(entity->y)-fmod(MIN(entity->hitbox[1],
                              entity->hitbox[4]), 1)-0.5;
        }
    }
    if(entity_on_ceiling(entity, world) && entity->y_velocity > 0){
        entity->y_velocity = -entity->y_velocity;
        if(entity_colliding(entity, world)){
            entity->y = floor(entity->y)-1-fmod(MAX(entity->hitbox[1],
                              entity->hitbox[4]), 1)+0.01;
        }
    }
}

int entity_in_water(Entity *entity, World *world) {
    return world_get_tile(world, floor(entity->x+0.5),
                          floor(entity->y+0.5+CHUNK_HEIGHT/2),
                          floor(entity->z+0.5)) == T_WATER;
}


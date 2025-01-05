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

int entity_on_floor(Entity *entity, World *world) {
    float x, z;
    for(z=entity->hitbox[2];z<entity->hitbox[5];z++){
        for(x=entity->hitbox[1];x<entity->hitbox[4];x++){
            if(world_get_tile(world, entity->x+0.5+x,
                              entity->y+0.5-entity->hitbox[4]+CHUNK_HEIGHT/2,
                              entity->z+0.5+z) != T_VOID){
                return 1;
            }
        }
    }
    return 0;
}

int entity_can_move(Entity *entity, World *world, float *hitbox,
                    int hitbox_size, float x, float z) {
    int i;
    for(i=0;i<hitbox_size;i++){
        if(world_get_tile(world, x+hitbox[i*2], entity->y-1.5,
                          z+hitbox[i*2+1]) != T_VOID){
            return 0;
        }
        if(world_get_tile(world, x+hitbox[i*2], entity->y-0.5,
                          z+hitbox[i*2+1]) != T_VOID){
            return 0;
        }
    }
    return 1;
}

void entity_update(Entity *entity, World *world, float delta) {
    entity->x += cos((entity->ry-90)/180*PI)*entity->velocity*
            entity->acceleration;
    entity->z += sin((entity->ry-90)/180*PI)*entity->velocity*
            entity->acceleration;
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
        /* entity->y = floor(entity->y); */
    }
}


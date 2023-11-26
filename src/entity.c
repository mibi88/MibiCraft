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

int entity_on_floor(Entity *entity, World *world, float *hitbox,
                    int hitbox_size) {
    int i;
    for(i=0;i<hitbox_size;i++){
        if(world_get_tile(world, entity->x+hitbox[i*2], entity->y-2,
                          entity->z+hitbox[i*2+1]) != T_VOID){
            return 1;
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


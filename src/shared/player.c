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

#include <player.h>

#include <math.h>

#include <raycast.h>

void player_init(Player *player) {
    Entity entity = {
            CHUNK_WIDTH, CHUNK_HEIGHT/2, CHUNK_DEPTH,
            0, 0, 0,
            0, 0, 0,
            PLAYER_GRAVITY,
            {
                -0.4, -1.5, -0.4,
                0.4, 0.2, 0.4
            }
    };
    float swimming_hitbox[HITBOX_SZ*3] = {
        -0.4, -0.4, -0.4,
        0.4, 0.4, 0.4
    };
    player->entity = entity;
    player->speed = PLAYER_WALKING_SPEED;
    player->jump_force = PLAYER_JUMP_FORCE;
    player->friction = PLAYER_DEFAULT_FRICTION;
    player->water_force = PLAYER_WATER_FORCE;
    player->deceleration = PLAYER_DEFAULT_DECELERATION;
    player->current_block = T_SPRUCE_PLANKS;
    player->mode = M_CREATIVE;
    player->sneak = 0;
    player->gravity = PLAYER_GRAVITY;
    player->swimming_down_speed = PLAYER_SWIMMING_DOWN_SPEED;
    player->acceleration = 2.5;
    player->adherence = 1;
    player->moved = 0;
    memcpy(player->swimming_hitbox, swimming_hitbox, HITBOX_SZ*3*sizeof(float));
}

void player_move(Player *player, float delta, float direction) {
    /* TODO: Add sneaking */
    float cos_rx;
    cos_rx = cos((player->entity.rx)/180*PI);
    if(player->mode == M_SPECTATOR){
        player->entity.x += cos((player->entity.ry-90)/180*PI)*
                          cos_rx*player->velocity*direction;
        player->entity.y += -sin(player->entity.rx/180*PI)*
                          player->velocity*direction;
        player->entity.z += sin((player->entity.ry-90)/180*PI)*
                          cos_rx*player->velocity*direction;
    }else{
        /* Survival or creative mode. */
        player->velocity += player->acceleration*delta*direction;
        player->moved = 1;
    }
}

void player_jump(Player *player, World *world) {
    if(entity_contains_block(&player->entity, world, T_WATER,
                             player->swimming_hitbox)){
        player->entity.y_velocity = player->jump_force;
    }else if(entity_on_floor(&player->entity, world)){
        player->entity.y_velocity = player->jump_force;
    }
}

void player_sneak(Player *player) {
    player->sneak = 1;
}

typedef struct {
    int old_x;
    int old_y;
    int old_z;
    Player *player;
    World *world;
} RaycastData;

int _player_break_block(int x, int y, int z, void *vdata) {
    RaycastData *data = vdata;
    if(!blocks[world_get_tile(data->world, x, y, z)].replaceable){
        printf("Break block at %d, %d, %d\n", x, y, z);
        world_set_tile(data->world, T_VOID, x, y, z);
        return 1;
    }
    return 0;
}

void player_break_block(Player *player, World *world) {
    RaycastData data;
    data.old_x = 0;
    data.old_y = 0;
    data.old_z = 0;
    data.player = player;
    data.world = world;
    raycast(&player->entity, RAYCAST_DISTANCE, _player_break_block, &data);
}

int _player_place_block(int x, int y, int z, void *vdata) {
    RaycastData *data = vdata;
    if(!blocks[world_get_tile(data->world, x, y, z)].replaceable){
        if(!entity_is_block_inside(&data->player->entity, data->world,
                                   data->old_x, data->old_y, data->old_z)){
            world_set_tile(data->world, data->player->current_block,
                           data->old_x, data->old_y, data->old_z);
        }
        return 1;
    }
    printf("Place block at %d, %d, %d\n", x, y, z);
    data->old_x = x;
    data->old_y = y;
    data->old_z = z;
    return 0;
}

void player_place_block(Player *player, World *world) {
    RaycastData data;
    data.old_x = 0;
    data.old_y = 0;
    data.old_z = 0;
    data.player = player;
    data.world = world;
    raycast(&player->entity, RAYCAST_DISTANCE, _player_place_block, &data);
}

void player_update(Player *player, World *world, float delta) {
    float deceleration;
    if(player->mode != M_SPECTATOR){
        player->entity.x_velocity = cos((player->entity.ry-90)/180*PI)*
                player->velocity*player->adherence+player->entity.x_velocity*
                (1-player->adherence);
        player->entity.z_velocity = sin((player->entity.ry-90)/180*PI)*
                player->velocity*player->adherence+player->entity.z_velocity*
                (1-player->adherence);
        if(player->moved) deceleration = player->friction;
        else deceleration = player->deceleration;
        if(player->velocity > player->speed*delta){
            player->velocity = player->speed*delta;
        }
        if(player->velocity < -player->speed*delta){
            player->velocity = -player->speed*delta;
        }
        player->moved = 0;
        if(entity_contains_block(&player->entity, world, T_WATER,
                                 player->swimming_hitbox)){
            if(player->sneak){
                player->entity.gravity = player->swimming_down_speed;
                if(player->entity.y_velocity <
                        -player->swimming_down_speed){
                    player->entity.y_velocity =
                            -player->swimming_down_speed;
                }
            }else{
                player->entity.gravity = player->water_force;
                if(player->entity.y_velocity < -player->water_force){
                    player->entity.y_velocity = -player->water_force;
                }
            }
        }else{
            player->entity.gravity = player->gravity;
        }
        player->sneak = 0;
        entity_update(&player->entity, world, delta);
        if(player->velocity > 0){
            player->velocity -= deceleration*delta;
            if(player->velocity < 0){
                player->velocity = 0;
            }
        }else if(player->velocity < 0){
            player->velocity += deceleration*delta;
            if(player->velocity > 0){
                player->velocity = 0;
            }
        }
    }else{
        player->velocity = 10*delta;
    }
}

void player_respawn(Player *player, World *world) {
    /* TODO: Determine the spawn position correctly */
    player->entity.x = 0;
    if(player->mode == M_SPECTATOR){
        player->entity.y = 0;
    }else{
        player->entity.y = CHUNK_HEIGHT/2;
    }
    player->entity.z = 0;
    world->x = -(world->width/2*CHUNK_WIDTH+CHUNK_WIDTH/2);
    world->y = -(world->height/2*CHUNK_DEPTH+CHUNK_DEPTH/2);
    player->entity.ry = 0;
    player->entity.rx = 0;
    player->velocity = 0;
    player->entity.y_velocity = 0;
    /* TODO: Refactor chunk loading to avoid having to reload the world to
     * avoid issues */
    world_init_data(world);
}

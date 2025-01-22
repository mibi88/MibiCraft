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

#ifndef STRUCTS_H
#define STRUCTS_H

#include <config.h>

#define HITBOX_SZ 2

typedef struct {
    float x, y, z;
    float rx, ry, rz;
    float x_velocity, y_velocity, z_velocity;
    float gravity;
    float hitbox[HITBOX_SZ*3];
} Entity;

typedef struct {
    Entity entity;
    float swimming_hitbox[HITBOX_SZ*3];
    float speed;
    float friction;
    float deceleration;
    float water_force;
    float velocity;
    float acceleration;
    float jump_force;
    float gravity;
    float swimming_down_speed;
    char moved;
    char sneak;
    unsigned char mode;
    int current_block;
    float adherence;
} Player;

typedef struct {
    Player *players;
    int player_num;
    Chunk *chunks;
    int width, height;
    int x, y;
    int seed;
    unsigned int texture;
    /* World update */
    int new_x, new_z;
    int finished;
    int update_required;
    /****************/
} World;

#endif

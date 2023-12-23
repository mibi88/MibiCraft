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

#ifndef ENTITY_H
#define ENTITY_H

#include <world.h>

typedef struct {
    float x, y, z;
    float rx, ry, rz;
    float x_velocity, y_velocity, z_velocity;
} Entity;

int entity_on_floor(Entity *entity, World *world, float *hitbox,
                    int hitbox_size);

int entity_can_move(Entity *entity, World *world, float *hitbox,
                    int hitbox_size, float x, float z);

#endif

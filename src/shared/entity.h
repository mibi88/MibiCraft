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

#define HITBOX_SZ 2

#include <world.h>

typedef struct {
    float x, y, z;
    float rx, ry, rz;
    float velocity, y_velocity;
    float acceleration, deceleration, gravity;
    float hitbox[HITBOX_SZ*3];
} Entity;

int entity_on_floor(Entity *entity, World *world);

int entity_is_inside(Entity *entity, World *world, float x, float y, float z);

void entity_update(Entity *entity, World *world, float delta);

#endif

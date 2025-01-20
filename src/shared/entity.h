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

int entity_on_floor(Entity *entity, World *world);

int entity_is_inside(Entity *entity, World *world, float x, float y, float z);

int entity_is_block_inside(Entity *entity, World *world, int sx, int sy,
                           int sz);

int entity_contains_block(Entity *entity, World *world, Tile block,
                          float hitbox[HITBOX_SZ*3]);

void entity_update(Entity *entity, World *world, float delta);

int entity_in_water(Entity *entity, World *world);

#endif

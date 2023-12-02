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

#ifndef WORLD_H
#define WORLD_H

#define THREADING (1)

#include <chunk.h>

typedef struct {
    Chunk *chunks;
    int width, height;
    int x, y;
    int seed;
    unsigned int texture;
    /* World update */
    int new_x, new_z;
    int finished;
    /****************/
} World;

void world_init(World *world, int width, int height, int seed,
                unsigned int texture);

void world_update(World *world, float x, float z);

void world_render(World *world);

void world_update(World *world, float x, float z);

Tile world_get_tile(World *world, float sx, float sy, float sz);
void world_set_tile(World *world, Tile tile, float sx, float sy, float sz);
void world_update_chunk_model_at(World *world, float sx, float sz);

void world_free(World *world);

#endif

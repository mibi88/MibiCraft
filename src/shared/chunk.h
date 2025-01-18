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

#ifndef CHUNK_H
#define CHUNK_H

#define CHUNK_WIDTH       8
#define CHUNK_HEIGHT      128
#define CHUNK_DEPTH       8

#define TEX_WIDTH         256
#define TEX_HEIGHT        256

#define TILE_WIDTH        16
#define TILE_HEIGHT       16

#define TILE_AWIDTH       (TEX_WIDTH/TILE_WIDTH)
#define TILE_AHEIGHT      (TEX_HEIGHT/TILE_HEIGHT)

#define TILE_FWIDTH       (1.0/(float)TILE_AWIDTH)
#define TILE_FHEIGHT      (1.0/(float)TILE_AHEIGHT)

#define VERTICES_AMOUNT   4
#define TRIANGLES_AMOUNT  2

/* Data size for one face */
#define SZ_VERTICES       (VERTICES_AMOUNT*3)
#define SZ_INDICES        6
#define SZ_TEX_COORDS     8

#define TREE_WIDTH        5
#define TREE_HEIGHT       8
#define TREE_DEPTH        5

#define PLANTS            4

#include <gfx.h>
#include <string.h>

typedef enum {
    T_VOID,
    T_GRASS,
    T_DIRT,
    T_STONE,
    T_COAL,
    T_IRON,
    T_GOLD,
    T_REDSTONE,
    T_LAPIS,
    T_DIAMOND,
    T_BEDROCK,
    T_WATER,
    T_SAND,
    T_OAK_LOG,
    T_OAK_LEAVES,
    T_PODZOL,
    T_SPRUCE_LOG,
    T_SPRUCE_LEAVES,
    T_SNOW,
    T_DRIED_GRASS,
    T_ACACIA_LOG,
    T_ACACIA_LEAVES,
    T_SAND_STONE,
    T_PALM_LOG,
    T_PALM_LEAVES,
    T_CACTUS,
    T_ROSE,
    T_GRASS_PLANT,
    T_DEAD_BUSH,
    T_BROWN_MUSHROOM,
    T_RED_MUSHROOM,
    T_SPRUCE_PLANKS,
    T_GLASS,
    T_DANDELION,
    T_BERRY_BUSH,
    T_FRUIT_BERRY_BUSH,
    T_AMOUNT
} Tile;

typedef enum {
    B_PLAINS,
    B_TAIGA,
    B_SAVANNA,
    B_DESERT,
    B_AMOUNT
} Biome;

typedef struct {
    unsigned char chunk_data[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_DEPTH];
    float chunk_vertices[8*6*CHUNK_WIDTH*CHUNK_HEIGHT*CHUNK_DEPTH];
    int chunk_indices[8*6*CHUNK_WIDTH*CHUNK_HEIGHT*CHUNK_DEPTH];
    float chunk_texture_coords[8*6*CHUNK_WIDTH*CHUNK_HEIGHT*CHUNK_DEPTH];
    GFXModel chunk_model;
    int x, z;
    /* For model generation */
    float *vert_ptr;
    float *tex_ptr;
    int *indices_ptr;
    int triangles;
    int _x, _y, _z;
    int tex_x, tex_y;

    float vertices[SZ_VERTICES*2];
    int indices[SZ_VERTICES*2];

    float texture_coords[SZ_TEX_COORDS];

    int ready;

    int remesh;

    int regenerate;
} Chunk;

typedef enum {
    S_CUBE,
    S_CROSS,
    S_AMOUNT
} Block_shape;

typedef struct {
    Block_shape shape;
    int transparent;
    int solid;
    int replaceable;
} Block_property;

typedef struct {
    float amplitude;
    int tree_probability;
    Tile layer_top;
    Tile layer_material;
    int plants[PLANTS];
    int plant_probability[PLANTS];
} Biome_property;

void chunk_generate_data(Chunk *chunk, int sx, int sz, int seed);
void chunk_generate_texture_coords(Chunk *chunk, int tex_x, int tex_y);
Tile chunk_get_tile(Chunk *chunk, int x, int y, int z, int rx, int ry, int rz);
void chunk_set_tile(Chunk *chunk, Tile tile, int x, int y, int z);
void chunk_generate_model(Chunk *chunk, unsigned int texture,
                          Tile get_tile(Chunk *chunk, int x, int y, int z,
                                        int rx, int ry, int rz, void *extra),
                          void *extra);

#endif


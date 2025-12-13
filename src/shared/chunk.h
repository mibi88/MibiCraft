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

#define CHUNK_WIDTH           8
#define CHUNK_HEIGHT          128
#define CHUNK_DEPTH           8

#define TEX_WIDTH             256
#define TEX_HEIGHT            256

#define TILE_WIDTH            16
#define TILE_HEIGHT           16

#define TILE_AWIDTH           (TEX_WIDTH/TILE_WIDTH)
#define TILE_AHEIGHT          (TEX_HEIGHT/TILE_HEIGHT)

#define BLOCK_VERTEX_MAX      (6*4*3/2)
#define BLOCK_INDEX_MAX       (6*6/2)
#define BLOCK_TEXTURE_MAX     (6*4*2/2)

#define TREE_WIDTH            5
#define TREE_HEIGHT           8
#define TREE_DEPTH            5

#define PLANTS                4

#define DEBUG_EXTREME_TERRAIN 0

#include <gfx.h>
#include <string.h>

/* It's unfortunate that OpenGL doesn't support storing vertices in chars */
typedef short int vertex_t;
#define TYPE_VERTEX TYPE_SHORT
typedef unsigned short int index_t;
#define TYPE_INDEX TYPE_USHORT
typedef short int texture_t;
#define TYPE_TEXTURE TYPE_SHORT

typedef enum {
    I_VOID,
    I_GRASS,
    I_DIRT,
    I_STONE,
    I_COAL,
    I_IRON,
    I_GOLD,
    I_REDSTONE,
    I_LAPIS,
    I_DIAMOND,
    I_BEDROCK,
    I_WATER,
    I_SAND,
    I_OAK_LOG,
    I_OAK_LEAVES,
    I_PODZOL,
    I_SPRUCE_LOG,
    I_SPRUCE_LEAVES,
    I_SNOW,
    I_DRIED_GRASS,
    I_ACACIA_LOG,
    I_ACACIA_LEAVES,
    I_SAND_STONE,
    I_PALM_LOG,
    I_PALM_LEAVES,
    I_CACTUS,
    I_ROSE,
    I_GRASS_PLANT,
    I_DEAD_BUSH,
    I_BROWN_MUSHROOM,
    I_RED_MUSHROOM,
    I_SPRUCE_PLANKS,
    I_GLASS,
    I_DANDELION,
    I_BERRY_BUSH,
    I_FRUIT_BERRY_BUSH,
    I_AMOUNT
} Texture;

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
    vertex_t chunk_vertices[BLOCK_VERTEX_MAX*CHUNK_WIDTH*CHUNK_HEIGHT*
                            CHUNK_DEPTH];
    index_t chunk_indices[BLOCK_INDEX_MAX*CHUNK_WIDTH*CHUNK_HEIGHT*CHUNK_DEPTH];
    texture_t chunk_texture_coords[BLOCK_TEXTURE_MAX*CHUNK_WIDTH*CHUNK_HEIGHT*
                                   CHUNK_DEPTH];
    GFXModel chunk_model;
    int x, z;

    int ready;

    int remesh;

    int regenerate;
} Chunk;

typedef struct {
    vertex_t *vertices;
    texture_t *tex_coordinates;
    index_t *indices;
    size_t vertex_count;
    size_t index_count;
    unsigned char blocking;
} Face;

typedef struct {
    /* XXX: Is it good to make the faces pointer or is it more beneficial to
     * waste a few bytes and process them even if they are empty? */
    Face *face_left;
    Face *face_right;
    Face *face_front;
    Face *face_back;
    Face *face_top;
    Face *face_bottom;

    Face *face_middle;
} BlockShape;

typedef struct {
    BlockShape *shape;

    struct {
        unsigned char left;
        unsigned char right;
        unsigned char front;
        unsigned char back;
        unsigned char top;
        unsigned char bottom;

        unsigned char middle;
    } texture;

    unsigned char transparent;
    unsigned char solid;
    unsigned char replaceable;
} BlockProperty;

typedef struct {
    float amplitude;
    int tree_probability;
    Tile layer_top;
    Tile layer_material;
    int plants[PLANTS];
    int plant_probability[PLANTS];
} BiomeProperty;

extern const BlockProperty blocks[T_AMOUNT];

void chunk_generate_data(Chunk *chunk, int sx, int sz, int seed);
void chunk_generate_texture_coords(Chunk *chunk, int tex_x, int tex_y);
Tile chunk_get_tile(Chunk *chunk, int x, int y, int z, int rx, int ry, int rz);
void chunk_set_tile(Chunk *chunk, Tile tile, int x, int y, int z);
void chunk_generate_model(Chunk *chunk, unsigned int texture,
                          Tile get_surrounding_tile(Chunk *chunk, int x, int y,
                                                    int z, int rx, int ry,
                                                    int rz, void *extra),
                          void *extra);

#endif


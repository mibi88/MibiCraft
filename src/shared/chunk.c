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

#include <chunk.h>

#include <noise.h>

#include <stddef.h>

/* Biomes */
const BiomeProperty biomes[B_AMOUNT] = {
    {(CHUNK_HEIGHT/2), 5, T_GRASS, T_DIRT,
     {T_GRASS_PLANT, T_DANDELION, T_ROSE, T_VOID}, {8, 12, 20, 0}},
    {(CHUNK_HEIGHT/1.8), 10, T_PODZOL, T_DIRT,
     {T_BROWN_MUSHROOM, T_RED_MUSHROOM, T_BERRY_BUSH, T_FRUIT_BERRY_BUSH},
     {8, 16, 20, 24}},
    {(CHUNK_HEIGHT/2.2), 5, T_DRIED_GRASS, T_DIRT,
     {T_GRASS_PLANT, T_ROSE, T_VOID, T_VOID},
     {4, 8, 0, 0}},
    {(CHUNK_HEIGHT/2.2), 7, T_SAND, T_SAND_STONE,
     {T_DEAD_BUSH, T_CACTUS, T_VOID, T_VOID}, {4, 5, 0, 0}}
};

/* Cube shape */
static const vertex_t left_vertices[] = {
    1, 0, 0,
    1, 1, 0,
    1, 1, 1,
    1, 0, 1
};

static const vertex_t right_vertices[] = {
    0, 0, 0,
    0, 1, 0,
    0, 1, 1,
    0, 0, 1
};

static const vertex_t front_vertices[] = {
    0, 1, 1,
    0, 0, 1,
    1, 0, 1,
    1, 1, 1
};

static const vertex_t back_vertices[] = {
    0, 1, 0,
    0, 0, 0,
    1, 0, 0,
    1, 1, 0
};

static const vertex_t top_vertices[] = {
    0, 1, 1,
    0, 1, 0,
    1, 1, 0,
    1, 1, 1
};

static const vertex_t bottom_vertices[] = {
    0, 0, 1,
    0, 0, 0,
    1, 0, 0,
    1, 0, 1
};

static const texture_t base_tex[] = {
    0, 0,
    0, 1,
    1, 1,
    1, 0
};

static const index_t base_indices[] = {
    0, 1, 3,
    3, 1, 2
};

static const Face left_face = {
    (vertex_t*)left_vertices, (texture_t*)base_tex, (index_t*)base_indices,
    sizeof(left_vertices)/sizeof(vertex_t)/3,
    sizeof(base_indices)/sizeof(index_t),
    1
};

static const Face right_face = {
    (vertex_t*)right_vertices, (texture_t*)base_tex, (index_t*)base_indices,
    sizeof(right_vertices)/sizeof(vertex_t)/3,
    sizeof(base_indices)/sizeof(index_t),
    1
};

static const Face front_face = {
    (vertex_t*)front_vertices, (texture_t*)base_tex, (index_t*)base_indices,
    sizeof(front_vertices)/sizeof(vertex_t)/3,
    sizeof(base_indices)/sizeof(index_t),
    1
};

static const Face back_face = {
    (vertex_t*)back_vertices, (texture_t*)base_tex, (index_t*)base_indices,
    sizeof(back_vertices)/sizeof(vertex_t)/3,
    sizeof(base_indices)/sizeof(index_t),
    1
};

static const Face top_face = {
    (vertex_t*)top_vertices, (texture_t*)base_tex, (index_t*)base_indices,
    sizeof(top_vertices)/sizeof(vertex_t)/3,
    sizeof(base_indices)/sizeof(index_t),
    1
};

static const Face bottom_face = {
    (vertex_t*)bottom_vertices, (texture_t*)base_tex, (index_t*)base_indices,
    sizeof(bottom_vertices)/sizeof(vertex_t)/3,
    sizeof(base_indices)/sizeof(index_t),
    1
};

static const BlockShape cube = {
    (Face*)&left_face,
    (Face*)&right_face,
    (Face*)&front_face,
    (Face*)&back_face,
    (Face*)&top_face,
    (Face*)&bottom_face,
    NULL
};

/* Cross shape */
static const vertex_t cross_vertices[] = {
    0, 1, 0,
    0, 0, 0,
    1, 0, 1,
    1, 1, 1,
    0, 1, 1,
    0, 0, 1,
    1, 0, 0,
    1, 1, 0
};

static const texture_t cross_tex[] = {
    0, 0,
    0, 1,
    1, 1,
    1, 0,
    0, 0,
    0, 1,
    1, 1,
    1, 0
};

static const index_t cross_indices[] = {
    0, 1, 3,
    3, 1, 2,
    4, 5, 7,
    7, 5, 6
};

static const Face cross_face = {
    (vertex_t*)cross_vertices, (texture_t*)cross_tex, (index_t*)cross_indices,
    sizeof(cross_vertices)/sizeof(vertex_t)/3,
    sizeof(cross_indices)/sizeof(index_t),
    0
};

static const BlockShape cross = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,

    (Face*)&cross_face
};

/* Blocks */
const BlockProperty blocks[T_AMOUNT] = {
    {NULL,
     {I_VOID, I_VOID, I_VOID, I_VOID, I_VOID, I_VOID, I_VOID},
     1, 0, 1},  /* T_VOID */
    {(BlockShape*)&cube,
     {I_GRASS, I_GRASS, I_GRASS, I_GRASS, I_GRASS, I_DIRT, I_VOID},
     0, 1, 0},  /* T_GRASS */
    {(BlockShape*)&cube,
     {I_SAND_STONE, I_DIRT, I_DIRT, I_DIRT, I_DIRT, I_DIRT, I_VOID},
     0, 1, 0},  /* T_DIRT */
    {(BlockShape*)&cube,
     {I_STONE, I_STONE, I_STONE, I_STONE, I_STONE, I_STONE, I_VOID},
     0, 1, 0},  /* T_STONE */
    {(BlockShape*)&cube,
     {I_COAL, I_COAL, I_COAL, I_COAL, I_COAL, I_COAL, I_VOID},
     0, 1, 0},  /* T_COAL */
    {(BlockShape*)&cube,
     {I_IRON, I_IRON, I_IRON, I_IRON, I_IRON, I_IRON, I_VOID},
     0, 1, 0},  /* T_IRON */
    {(BlockShape*)&cube,
     {I_GOLD, I_GOLD, I_GOLD, I_GOLD, I_GOLD, I_GOLD, I_VOID},
     0, 1, 0},  /* T_GOLD */
    {(BlockShape*)&cube,
     {I_REDSTONE, I_REDSTONE, I_REDSTONE, I_REDSTONE, I_REDSTONE, I_REDSTONE,
      I_VOID},
     0, 1, 0},  /* T_REDSTONE */
    {(BlockShape*)&cube,
     {I_LAPIS, I_LAPIS, I_LAPIS, I_LAPIS, I_LAPIS, I_LAPIS, I_VOID},
     0, 1, 0},  /* T_LAPIS */
    {(BlockShape*)&cube,
     {I_DIAMOND, I_DIAMOND, I_DIAMOND, I_DIAMOND, I_DIAMOND, I_DIAMOND,
      I_VOID},
     0, 1, 0},  /* T_DIAMOND */
    {(BlockShape*)&cube,
     {I_BEDROCK, I_BEDROCK, I_BEDROCK, I_BEDROCK, I_BEDROCK, I_BEDROCK,
      I_VOID},
     0, 1, 0},  /* T_BEDROCK */
    {(BlockShape*)&cube,
     {I_WATER, I_WATER, I_WATER, I_WATER, I_WATER, I_WATER, I_VOID},
     0, 0, 1},  /* T_WATER */
    {(BlockShape*)&cube,
     {I_SAND, I_SAND, I_SAND, I_SAND, I_SAND, I_SAND, I_VOID},
     0, 1, 0},  /* T_SAND */
    {(BlockShape*)&cube,
     {I_OAK_LOG, I_OAK_LOG, I_OAK_LOG, I_OAK_LOG, I_OAK_LOG, I_OAK_LOG,
      I_VOID},
     0, 1, 0},  /* T_OAK_LOG */
    {(BlockShape*)&cube,
     {I_OAK_LEAVES, I_OAK_LEAVES, I_OAK_LEAVES, I_OAK_LEAVES, I_OAK_LEAVES,
      I_OAK_LEAVES, I_VOID},
     0, 1, 0},  /* T_OAK_LEAVES */
    {(BlockShape*)&cube,
     {I_PODZOL, I_PODZOL, I_PODZOL, I_PODZOL, I_PODZOL, I_PODZOL, I_VOID},
     0, 1, 0},  /* T_PODZOL */
    {(BlockShape*)&cube,
     {I_SPRUCE_LOG, I_SPRUCE_LOG, I_SPRUCE_LOG, I_SPRUCE_LOG, I_SPRUCE_LOG,
      I_SPRUCE_LOG, I_VOID},
     0, 1, 0},  /* T_SPRUCE_LOG */
    {(BlockShape*)&cube,
     {I_SPRUCE_LEAVES, I_SPRUCE_LEAVES, I_SPRUCE_LEAVES, I_SPRUCE_LEAVES,
      I_SPRUCE_LEAVES, I_SPRUCE_LEAVES, I_VOID},
     0, 1, 0},  /* T_SPRUCE_LEAVES */
    {(BlockShape*)&cube,
     {I_SNOW, I_SNOW, I_SNOW, I_SNOW, I_SNOW, I_SNOW, I_VOID},
     0, 1, 0},  /* T_SNOW */
    {(BlockShape*)&cube,
     {I_DRIED_GRASS, I_DRIED_GRASS, I_DRIED_GRASS, I_DRIED_GRASS,
      I_DRIED_GRASS, I_DRIED_GRASS, I_VOID},
     0, 1, 0},  /* T_DRIED_GRASS */
    {(BlockShape*)&cube,
     {I_ACACIA_LOG, I_ACACIA_LOG, I_ACACIA_LOG, I_ACACIA_LOG, I_ACACIA_LOG,
      I_ACACIA_LOG, I_VOID},
     0, 1, 0},  /* T_ACACIA_LOG */
    {(BlockShape*)&cube,
     {I_ACACIA_LEAVES, I_ACACIA_LEAVES, I_ACACIA_LEAVES, I_ACACIA_LEAVES,
      I_ACACIA_LEAVES, I_ACACIA_LEAVES, I_VOID},
     0, 1, 0},  /* T_ACACIA_LEAVES */
    {(BlockShape*)&cube,
     {I_SAND_STONE, I_SAND_STONE, I_SAND_STONE, I_SAND_STONE, I_SAND_STONE,
      I_SAND_STONE, I_VOID},
     0, 1, 0},  /* T_SAND_STONE */
    {(BlockShape*)&cube,
     {I_PALM_LOG, I_PALM_LOG, I_PALM_LOG, I_PALM_LOG, I_PALM_LOG, I_PALM_LOG,
      I_VOID},
     0, 1, 0},  /* T_PALM_LOG */
    {(BlockShape*)&cube,
     {I_PALM_LEAVES, I_PALM_LEAVES, I_PALM_LEAVES, I_PALM_LEAVES,
      I_PALM_LEAVES, I_PALM_LEAVES, I_VOID},
     0, 1, 0},  /* T_PALM_LEAVES */
    {(BlockShape*)&cube,
     {I_CACTUS, I_CACTUS, I_CACTUS, I_CACTUS, I_CACTUS, I_CACTUS, I_VOID},
     0, 1, 0},  /* T_CACTUS */
    {(BlockShape*)&cross,
     {I_VOID, I_VOID, I_VOID, I_VOID, I_VOID, I_VOID, I_ROSE},
     1, 0, 0}, /* T_ROSE */
    {(BlockShape*)&cross,
     {I_VOID, I_VOID, I_VOID, I_VOID, I_VOID, I_VOID, I_GRASS_PLANT},
     1, 0, 0}, /* T_GRASS_PLANT */
    {(BlockShape*)&cross,
     {I_VOID, I_VOID, I_VOID, I_VOID, I_VOID, I_VOID, I_DEAD_BUSH},
     1, 0, 0}, /* T_DEAD_BUSH */
    {(BlockShape*)&cross,
     {I_VOID, I_VOID, I_VOID, I_VOID, I_VOID, I_VOID, I_BROWN_MUSHROOM},
     1, 0, 0}, /* T_BROWN_MUSHROOM */
    {(BlockShape*)&cross,
     {I_VOID, I_VOID, I_VOID, I_VOID, I_VOID, I_VOID, I_RED_MUSHROOM},
     1, 0, 0}, /* T_RED_MUSHROOM */
    {(BlockShape*)&cube,
     {I_SPRUCE_PLANKS, I_SPRUCE_PLANKS, I_SPRUCE_PLANKS, I_SPRUCE_PLANKS,
      I_SPRUCE_PLANKS, I_SPRUCE_PLANKS, I_VOID},
     0, 1, 0},  /* T_SPRUCE_PLANKS */
    {(BlockShape*)&cube,
     {I_GLASS, I_GLASS, I_GLASS, I_GLASS, I_GLASS, I_GLASS, I_VOID},
     1, 1, 0},  /* T_GLASS */
    {(BlockShape*)&cross,
     {I_VOID, I_VOID, I_VOID, I_VOID, I_VOID, I_VOID, I_DANDELION},
     1, 0, 0}, /* T_DANDELION */
    {(BlockShape*)&cross,
     {I_VOID, I_VOID, I_VOID, I_VOID, I_VOID, I_VOID, I_BERRY_BUSH},
     1, 0, 0}, /* T_BERRY_BUSH */
    {(BlockShape*)&cross,
     {I_VOID, I_VOID, I_VOID, I_VOID, I_VOID, I_VOID, I_FRUIT_BERRY_BUSH},
     1, 0, 0}  /* T_FRUIT_BERRY_BUSH */
};
/**********/

/* Trees */
const Tile oak_tree[TREE_WIDTH*TREE_HEIGHT*TREE_DEPTH] = {
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LEAVES, T_VOID,
    T_VOID, T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LEAVES, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_OAK_LEAVES, T_VOID, T_VOID,
    T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LEAVES,
    T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LEAVES,
    T_VOID, T_VOID, T_OAK_LEAVES, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_OAK_LEAVES, T_VOID, T_VOID,
    T_VOID, T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LEAVES, T_VOID,
    T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LOG, T_OAK_LEAVES, T_OAK_LEAVES,
    T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LOG, T_OAK_LEAVES, T_OAK_LEAVES,
    T_VOID, T_OAK_LEAVES, T_OAK_LOG, T_OAK_LEAVES, T_VOID,
    T_VOID, T_VOID, T_OAK_LOG, T_VOID, T_VOID,
    T_VOID, T_VOID, T_OAK_LOG, T_VOID, T_VOID,
    T_VOID, T_VOID, T_OAK_LOG, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_OAK_LEAVES, T_VOID, T_VOID,
    T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LEAVES,
    T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LEAVES,
    T_VOID, T_VOID, T_OAK_LEAVES, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LEAVES, T_VOID,
    T_VOID, T_OAK_LEAVES, T_OAK_LEAVES, T_OAK_LEAVES, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID
};

const Tile spruce_tree[TREE_WIDTH*TREE_HEIGHT*TREE_DEPTH] = {
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES,
        T_SPRUCE_LEAVES,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_SPRUCE_LEAVES, T_VOID, T_VOID,
    T_VOID, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_VOID,
    T_VOID, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_VOID,
    T_VOID, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_VOID,
    T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES,
        T_SPRUCE_LEAVES,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_SPRUCE_LEAVES, T_VOID, T_VOID,
    T_VOID, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_VOID,
    T_VOID, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_VOID,
    T_VOID, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_VOID,
    T_VOID, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_VOID,
    T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LOG, T_SPRUCE_LEAVES,
        T_SPRUCE_LEAVES,
    T_VOID, T_VOID, T_SPRUCE_LOG, T_VOID, T_VOID,
    T_VOID, T_VOID, T_SPRUCE_LOG, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_SPRUCE_LEAVES, T_VOID, T_VOID,
    T_VOID, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_VOID,
    T_VOID, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_VOID,
    T_VOID, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_VOID,
    T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES,
        T_SPRUCE_LEAVES,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES, T_SPRUCE_LEAVES,
        T_SPRUCE_LEAVES,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
};

const Tile acacia_tree[TREE_WIDTH*TREE_HEIGHT*TREE_DEPTH] = {
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_ACACIA_LEAVES, T_ACACIA_LEAVES, T_ACACIA_LEAVES, T_ACACIA_LEAVES,
        T_ACACIA_LEAVES,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_ACACIA_LEAVES, T_ACACIA_LEAVES, T_ACACIA_LEAVES, T_VOID,
    T_ACACIA_LEAVES, T_ACACIA_LEAVES, T_ACACIA_LEAVES, T_ACACIA_LEAVES,
        T_ACACIA_LEAVES,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_ACACIA_LEAVES, T_ACACIA_LEAVES, T_ACACIA_LEAVES, T_VOID,
    T_ACACIA_LEAVES, T_ACACIA_LEAVES, T_ACACIA_LEAVES, T_ACACIA_LEAVES,
        T_ACACIA_LEAVES,
    T_VOID, T_VOID, T_ACACIA_LOG, T_VOID, T_VOID,
    T_VOID, T_VOID, T_ACACIA_LOG, T_VOID, T_VOID,
    T_VOID, T_VOID, T_ACACIA_LOG, T_VOID, T_VOID,
    T_VOID, T_VOID, T_ACACIA_LOG, T_VOID, T_VOID,
    T_VOID, T_VOID, T_ACACIA_LOG, T_VOID, T_VOID,
    T_VOID, T_VOID, T_ACACIA_LOG, T_VOID, T_VOID,
    T_VOID, T_ACACIA_LEAVES, T_ACACIA_LEAVES, T_ACACIA_LEAVES, T_VOID,
    T_ACACIA_LEAVES, T_ACACIA_LEAVES, T_ACACIA_LEAVES, T_ACACIA_LEAVES,
        T_ACACIA_LEAVES,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_ACACIA_LEAVES, T_ACACIA_LEAVES, T_ACACIA_LEAVES, T_ACACIA_LEAVES,
        T_ACACIA_LEAVES,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID
};
const Tile fallen_oak_tree[TREE_WIDTH*TREE_HEIGHT*TREE_DEPTH] = {
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_BROWN_MUSHROOM, T_BROWN_MUSHROOM, T_RED_MUSHROOM, T_VOID,
    T_OAK_LOG, T_OAK_LOG, T_OAK_LOG, T_OAK_LOG, T_OAK_LOG
};
const Tile fallen_spruce_tree[TREE_WIDTH*TREE_HEIGHT*TREE_DEPTH] = {
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
    T_VOID, T_BROWN_MUSHROOM, T_BROWN_MUSHROOM, T_VOID, T_BROWN_MUSHROOM,
    T_SPRUCE_LOG, T_SPRUCE_LOG, T_SPRUCE_LOG, T_SPRUCE_LOG, T_SPRUCE_LOG
};
/*********/

/**************/

Biome chunk_get_biome(int sx, int sz, int x, int z, int seed) {
    int i;
    float biome_height;
    float biome_step = 0.5/B_AMOUNT;
    biome_height = noise_2d((float)(x+sx)/512, (float)(z+sz)/512, seed);
    for(i=0;i<B_AMOUNT;i++){
        if(biome_height < (i+1)*biome_step){
            return i;
            break;
        }
    }

    return B_PLAINS;
}

float chunk_get_height(int sx, int sz, int x, int z, float amplitude,
                       int seed) {
    return noise_2d((float)(x+sx)/256, (float)(z+sz)/256, seed)*amplitude+
           (CHUNK_HEIGHT/2);
}

unsigned int xorshift(int *seed) {
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    *seed ^= *seed << 13;
    *seed ^= *seed >> 17;
    *seed ^= *seed << 5;
    return *seed;
}

int chunk_rand(int sx, int sz, int x, int z, int seed, int probability) {
    seed = seed+(xorshift(&sz)+z)*(xorshift(&sx)+x)+(xorshift(&sx)+x);
    return xorshift(&seed)%probability;
}

void chunk_place_structure(Chunk *chunk, int sx, int sy, int sz,
                           const Tile *structure, int width, int height,
                           int depth) {
    int x, y, z;
    for(z=0;z<depth;z++){
        for(x=0;x<width;x++){
            for(y=0;y<height;y++){
                if(sx+x >= 0 && sx+x < CHUNK_WIDTH &&
                   sy+(height-y)-1 >= 0 &&
                   sy+(height-y)-1 < CHUNK_HEIGHT &&
                   sz+z >= 0 && sz+z < CHUNK_DEPTH &&
                   structure[z*(width*height)+y*width+x] != T_VOID){
                    chunk->chunk_data[sx+x][sy+(height-y)-1][sz+z] =
                                         structure[z*(width*height)+y*width+x];
                }
            }
        }
    }
}

void chunk_generate_structure(Chunk *chunk, int sx, int sz,
                              const Tile *structure, int width, int height,
                              int depth, int probability, int seed, Biome biome,
                              int dx, int dz) {
    int x, y, z;
    int i;
    int amount;
    Biome real_biome;
    BiomeProperty *properties;
    int pos_seed = seed;
    amount = probability/2+chunk_rand(sx, sz, 0, 0, seed, probability/2);
    for(i=0;i<amount;i++){
        x = chunk_rand(sx, sz, 0, 0, xorshift(&pos_seed), CHUNK_WIDTH);
        z = chunk_rand(sx, sz, 0, 0, xorshift(&pos_seed), CHUNK_DEPTH);
        real_biome = chunk_get_biome(sx, sz, x, z, seed);
        properties = (BiomeProperty*)&biomes[real_biome];
        y = chunk_get_height(sx, sz, x, z, properties->amplitude, seed);
        real_biome = chunk_get_biome(sx, sz, x, z, seed);
        properties = (BiomeProperty*)&biomes[real_biome];
        if(y >= CHUNK_HEIGHT/2){
            if(real_biome == biome){
                chunk_place_structure(chunk, x-dx, y, z-dz, structure,
                                      width, height, depth);
            }
        }
    }
}

#define ABS(x) (x < 0 ? -x : x)

void chunk_generate_ores(Chunk *chunk, int sx, int sz, int height,
                         Tile material, Tile ore, int probability, int area,
                         int num, int seed) {
    int i, p;
    int n, s;
    int x, y, z;
    int dx, dy, dz;
    n = (probability/2)+xorshift(&seed)%(probability/2);
    seed += sx*sz;
    for(i=0;i<n;i++){
        x = xorshift(&seed)%CHUNK_WIDTH;
        y = xorshift(&seed)%height;
        z = xorshift(&seed)%CHUNK_DEPTH;
        if(chunk->chunk_data[x][y][z] == material){
            chunk->chunk_data[x][y][z] = ore;
        }
        s = xorshift(&seed)%num;
        for(p=0;p<s;p++){
            dx = xorshift(&seed)%(area*2)-area;
            dy = xorshift(&seed)%(area*2)-area;
            dz = xorshift(&seed)%(area*2)-area;
            if(x+dx >= 0 && x+dx < CHUNK_WIDTH &&
               y+dy >= 0 && y+dy < CHUNK_HEIGHT &&
               z+dz >= 0 && z+dz < CHUNK_DEPTH){
                if(chunk->chunk_data[x+dx][y+dy][z+dz] == material){
                    chunk->chunk_data[x+dx][y+dy][z+dz] = ore;
                }
            }
        }
    }
}

void chunk_generate_layer(Chunk *chunk, int sx, int sz, int x, int y, int z,
                          float height, float min, float step, float amplitude,
                          Tile top, Tile material, int seed) {
    float layer;
    layer = noise_2d((float)(x+sx)/step, (float)(z+sz)/step, seed)*
            amplitude+min;
    if(y<(int)height && y>=(int)(height-layer)){
        chunk->chunk_data[x][y][z] = (int)height-y == 1 ? top : material;
    }
}

void chunk_generate_beaches(Chunk *chunk, int sx, int sz, int x, int y, int z,
                            float height, float min, float step,
                            float amplitude, Tile material, int seed) {
    float layer;
    layer = noise_2d((float)(x+sx)/step, (float)(z+sz)/step, seed)*amplitude+
            min;
    if(y>(CHUNK_HEIGHT/2)-layer && y<(CHUNK_HEIGHT/2)+layer &&
       (int)height+layer > CHUNK_HEIGHT/2){
        chunk->chunk_data[x][y][z] = material;
    }
}

void chunk_generate_snow(Chunk *chunk, int sx, int sz, float amplitude,
                         int seed) {
    int x, y, z;
    float height;
    Biome biome;
    for(x=0;x<CHUNK_WIDTH;x++){
        for(z=0;z<CHUNK_DEPTH;z++){
            biome = chunk_get_biome(sx, sz, x, z, seed);
            height = chunk_get_height(sx, sz, x, z, amplitude, seed);
            if(biome == B_TAIGA){
                for(y=CHUNK_HEIGHT;y>0;y--){
                    if(height > CHUNK_HEIGHT/4*3 &&
                       chunk->chunk_data[x][y-1][z] != T_VOID &&
                       chunk->chunk_data[x][y][z] == T_VOID){
                        chunk->chunk_data[x][y][z] = T_SNOW;
                        break;
                    }
                }
            }
        }
    }
}

void chunk_generate_plants(Chunk *chunk, int sx, int sz, int seed) {
    int x, y, z;
    int i, n, amount;
    Tile plant = T_ROSE;
    int plant_max = 0;
    Biome real_biome;
    BiomeProperty *properties;
    int pos_seed = seed;
    int plant_rand;
    real_biome = chunk_get_biome(sx, sz, 0, 0, seed);
    properties = (BiomeProperty*)&biomes[real_biome];
    plant_max = 0;
    for(n=0;n<PLANTS;n++){
        if(properties->plants[n] > T_VOID &&
           properties->plants[n] < T_AMOUNT){
            plant_max += properties->plant_probability[n];
        }
    }
    amount = plant_max/2+chunk_rand(sx, sz, 0, 0, seed, plant_max/2);
    xorshift(&pos_seed);
    for(i=0;i<amount;i++){
        x = chunk_rand(sx, sz, 0, 0, xorshift(&pos_seed), CHUNK_WIDTH);
        z = chunk_rand(sx, sz, 0, 0, xorshift(&pos_seed), CHUNK_DEPTH);
        real_biome = chunk_get_biome(sx, sz, x, z, seed);
        properties = (BiomeProperty*)&biomes[real_biome];
        y = chunk_get_height(sx, sz, x, z, properties->amplitude, seed);
        if(y >= CHUNK_HEIGHT/2){
            properties = (BiomeProperty*)&biomes[real_biome];
            plant_max = 0;
            for(n=0;n<PLANTS;n++){
                if(properties->plants[n] > T_VOID &&
                   properties->plants[n] < T_AMOUNT){
                    plant_max += properties->plant_probability[n];
                }
            }
            plant_rand = chunk_rand(sx, sz, 0, 0, xorshift(&pos_seed),
                               plant_max);
            plant = properties->plants[0];
            for(n=0;n<PLANTS;n++){
                if(properties->plants[n] > T_VOID &&
                   properties->plants[n] < T_AMOUNT){
                    plant_rand -= properties->plant_probability[n];
                    if(plant_rand < properties->plant_probability[n]){
                        plant = properties->plants[n];
                        break;
                    }
                }
            }
            if(chunk_get_tile(chunk, x, y, z, 0, 0, 0) == T_VOID){
                chunk_set_tile(chunk, plant, x, y, z);
            }
        }
    }
}

void chunk_generate_caves(Chunk *chunk, int sx, int sz, int x, int y, int z,
                          float step, float probability, Tile material,
                          int seed) {
    if(chunk->chunk_data[x][y][z] == material &&
       noise_3d((float)(x+sx)/step, (float)y/step,
               (float)(z+sz)/step, seed) > probability){
        chunk->chunk_data[x][y][z] = T_VOID;
    }
}

void chunk_generate_data(Chunk *chunk, int sx, int sz, int seed) {
    int x, y, z;
    float height;
    Biome biome = B_PLAINS;
    BiomeProperty *properties;

    for(x=0;x<CHUNK_WIDTH;x++){
        for(z=0;z<CHUNK_DEPTH;z++){
            biome = chunk_get_biome(sx, sz, x, z, seed);

            properties = (BiomeProperty*)&biomes[biome];

            height = chunk_get_height(sx, sz, x, z, properties->amplitude,
                    seed);

            for(y=0;y<CHUNK_HEIGHT;y++){
                if(y < (CHUNK_HEIGHT/2) && y > (int)height-1){
                    chunk->chunk_data[x][y][z] = T_WATER;
                }else if(y < (int)height){
                    chunk->chunk_data[x][y][z] = T_STONE;
                }else{
                    chunk->chunk_data[x][y][z] = T_VOID;
                }

                chunk_generate_layer(chunk, sx, sz, x, y, z, height, 3, 128, 3,
                                     properties->layer_top,
                                     properties->layer_material, seed);
                chunk_generate_beaches(chunk, sx, sz, x, y, z, height, 0, 16, 3,
                                       T_SAND, seed);
                chunk_generate_caves(chunk, sx, sz, x, y, z, 8, 0.25, T_STONE,
                                     seed);
                chunk_generate_caves(chunk, sx, sz, x, y, z, 32, 0.75, T_STONE,
                                     seed);
                if(y == 0){
                    chunk->chunk_data[x][y][z] = T_BEDROCK;
                }
            }
        }
    }
    chunk_generate_ores(chunk, sx, sz, 8, T_STONE,
                        T_DIAMOND, 10, 3, 3, seed);
    chunk_generate_ores(chunk, sx, sz, 12, T_STONE,
                        T_LAPIS, 20, 8, 8, seed);
    chunk_generate_ores(chunk, sx, sz, 12, T_STONE,
                        T_REDSTONE, 30, 4, 3, seed);
    chunk_generate_ores(chunk, sx, sz, 16, T_STONE,
                        T_GOLD, 30, 5, 5, seed);
    chunk_generate_ores(chunk, sx, sz, 32, T_STONE,
                        T_IRON, 50, 5, 5, seed);
    chunk_generate_ores(chunk, sx, sz, 64, T_STONE,
                        T_COAL, 60, 10, 25, seed);
    chunk->x = sx;
    chunk->z = sz;
    chunk_generate_structure(chunk, sx, sz, fallen_oak_tree, TREE_WIDTH,
                             TREE_HEIGHT, TREE_DEPTH,
                             properties->tree_probability*3, seed, B_PLAINS,
                             TREE_WIDTH/2, TREE_DEPTH/2);
    chunk_generate_structure(chunk, sx, sz, oak_tree, TREE_WIDTH, TREE_HEIGHT,
                             TREE_DEPTH, properties->tree_probability, seed,
                             B_PLAINS, TREE_WIDTH/2, TREE_DEPTH/2);
    chunk_generate_structure(chunk, sx, sz, fallen_spruce_tree, TREE_WIDTH,
                             TREE_HEIGHT, TREE_DEPTH,
                             properties->tree_probability*3, seed, B_TAIGA,
                             TREE_WIDTH/2, TREE_DEPTH/2);
    chunk_generate_structure(chunk, sx, sz, spruce_tree, TREE_WIDTH,
                             TREE_HEIGHT, TREE_DEPTH,
                             properties->tree_probability, seed, B_TAIGA,
                             TREE_WIDTH/2, TREE_DEPTH/2);
    chunk_generate_structure(chunk, sx, sz, acacia_tree, TREE_WIDTH,
                             TREE_HEIGHT, TREE_DEPTH,
                             properties->tree_probability, seed, B_SAVANNA,
                             TREE_WIDTH/2, TREE_DEPTH/2);
    chunk_generate_snow(chunk, sx, sz, properties->amplitude, seed);
    chunk_generate_plants(chunk, sx, sz, seed);

#if DEBUG_EXTREME_TERRAIN
    {
        size_t x, y, z, c = 0;
        for(z=0;z<CHUNK_DEPTH;z++){
            for(y=0;y<CHUNK_HEIGHT;y++){
                for(x=0;x<CHUNK_WIDTH;x++){
                    chunk->chunk_data[x][y][z] = c ? T_VOID : T_GRASS;
                    c ^= 1;
                }
                c ^= 1;
            }
            c ^= 1;
        }
    }
#endif
}

Tile chunk_get_tile(Chunk *chunk, int x, int y, int z, int rx, int ry, int rz) {
    x += rx;
    y += ry;
    z += rz;
    if(x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT && z >= 0 &&
       z < CHUNK_DEPTH) {
       return chunk->chunk_data[x][y][z];
   }
   return T_VOID;
}

void chunk_set_tile(Chunk *chunk, Tile tile, int x, int y, int z) {
    if(x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT && z >= 0 &&
       z < CHUNK_DEPTH) {
       chunk->chunk_data[x][y][z] = tile;
   }
}

#define CHUNK_ADD_FACE(x, y, z, face, tex) \
    { \
        size_t i; \
        ptrdiff_t offset = vert_ptr-chunk->chunk_vertices; \
 \
        const size_t vertex_size = face->vertex_count*3; \
        const size_t tex_size = face->vertex_count*2; \
 \
        int u = (tex-1)%TILE_AWIDTH; \
        int v = (tex-1)/TILE_AWIDTH; \
 \
        for(i=0;i<vertex_size;i+=3){ \
            *(vert_ptr++) = face->vertices[i]+x; \
            *(vert_ptr++) = face->vertices[i+1]+y; \
            *(vert_ptr++) = face->vertices[i+2]+z; \
        } \
 \
        for(i=0;i<tex_size;i+=2){ \
            *(tex_ptr++) = face->tex_coordinates[i]+u; \
            *(tex_ptr++) = face->tex_coordinates[i+1]+v; \
        } \
 \
        for(i=0;i<face->index_count;i++){ \
            *(index_ptr++) = face->indices[i]+offset/3; \
        } \
 \
        indices += face->index_count; \
    }

#define CULL 1

#define NEXT_BLOCKING(face) \
    (blocks[next].shape != NULL && \
     blocks[next].shape->face != NULL && \
     blocks[next].shape->face->blocking && \
     ((!blocks[tile].transparent == !blocks[next].transparent && \
       !blocks[tile].solid == !blocks[next].solid) || \
       (!blocks[tile].transparent && blocks[next].solid)))

void chunk_generate_model(Chunk *chunk, unsigned int texture,
                          Tile get_surrounding_tile(Chunk *chunk, int x, int y,
                                                    int z, int rx, int ry,
                                                    int rz, void *extra),
                          void *extra) {
    size_t i;
    size_t x, y, z;

    vertex_t *vert_ptr = chunk->chunk_vertices;
    texture_t *tex_ptr = chunk->chunk_texture_coords;
    index_t *index_ptr = chunk->chunk_indices;
    size_t indices = 0;

    /* Left faces */
    for(z=0;z<CHUNK_DEPTH;z++){
        for(y=0;y<CHUNK_HEIGHT;y++){
            for(x=0;x<CHUNK_WIDTH-1;x++){
                register Tile tile = chunk->chunk_data[x][y][z];
                register Tile next;

                if(blocks[tile].shape == NULL ||
                   blocks[tile].shape->face_left == NULL) continue;

#if CULL
                next = chunk->chunk_data[x+1][y][z];

                if(NEXT_BLOCKING(face_right)) continue;
#endif

                /* Add a left face for this block */
                CHUNK_ADD_FACE(x, y, z, blocks[tile].shape->face_left,
                               blocks[tile].texture.left);
            }
        }
    }

    /* Right faces */
    for(z=0;z<CHUNK_DEPTH;z++){
        for(y=0;y<CHUNK_HEIGHT;y++){
            for(x=1;x<CHUNK_WIDTH;x++){
                register Tile tile = chunk->chunk_data[x][y][z];
                register Tile next;

                if(blocks[tile].shape == NULL ||
                   blocks[tile].shape->face_right == NULL) continue;

#if CULL
                next = chunk->chunk_data[x-1][y][z];

                if(NEXT_BLOCKING(face_left)) continue;
#endif

                /* Add a right face for this block */
                CHUNK_ADD_FACE(x, y, z, blocks[tile].shape->face_right,
                               blocks[tile].texture.right);
            }
        }
    }

    /* Front faces */
    for(x=0;x<CHUNK_WIDTH;x++){
        for(y=0;y<CHUNK_HEIGHT;y++){
            for(z=0;z<CHUNK_DEPTH-1;z++){
                register Tile tile = chunk->chunk_data[x][y][z];
                register Tile next;

                if(blocks[tile].shape == NULL ||
                   blocks[tile].shape->face_front == NULL) continue;

#if CULL
                next = chunk->chunk_data[x][y][z+1];

                if(NEXT_BLOCKING(face_back)) continue;
#endif

                /* Add a front face for this block */
                CHUNK_ADD_FACE(x, y, z, blocks[tile].shape->face_front,
                               blocks[tile].texture.front);
            }
        }
    }

    /* Back faces */
    for(x=0;x<CHUNK_WIDTH;x++){
        for(y=0;y<CHUNK_HEIGHT;y++){
            for(z=1;z<CHUNK_DEPTH;z++){
                register Tile tile = chunk->chunk_data[x][y][z];
                register Tile next;

                if(blocks[tile].shape == NULL ||
                   blocks[tile].shape->face_back == NULL) continue;

#if CULL
                next = chunk->chunk_data[x][y][z-1];

                if(NEXT_BLOCKING(face_front)) continue;
#endif

                /* Add a back face for this block */
                CHUNK_ADD_FACE(x, y, z, blocks[tile].shape->face_back,
                               blocks[tile].texture.back);
            }
        }
    }

    /* Top faces */
    for(z=0;z<CHUNK_DEPTH;z++){
        for(x=0;x<CHUNK_WIDTH;x++){
            for(y=0;y<CHUNK_HEIGHT;y++){
                register Tile tile = chunk->chunk_data[x][y][z];
                register Tile next;

                if(blocks[tile].shape == NULL ||
                   blocks[tile].shape->face_top == NULL) continue;

#if CULL
                if(y < CHUNK_HEIGHT-1){
                    next = chunk->chunk_data[x][y+1][z];

                    if(NEXT_BLOCKING(face_bottom)) continue;
                }
#endif

                /* Add a top face for this block */
                CHUNK_ADD_FACE(x, y, z, blocks[tile].shape->face_top,
                               blocks[tile].texture.top);
            }
        }
    }

    /* Bottom faces */
    for(z=0;z<CHUNK_DEPTH;z++){
        for(x=0;x<CHUNK_WIDTH;x++){
            for(y=0;y<CHUNK_HEIGHT;y++){
                register Tile tile = chunk->chunk_data[x][y][z];
                register Tile next;

                if(blocks[tile].shape == NULL ||
                   blocks[tile].shape->face_bottom == NULL) continue;

#if CULL
                if(y){
                    next = chunk->chunk_data[x][y-1][z];

                    if(NEXT_BLOCKING(face_top)) continue;
                }
#endif

                /* Add a bottom face for this block */
                CHUNK_ADD_FACE(x, y, z, blocks[tile].shape->face_bottom,
                               blocks[tile].texture.bottom);
            }
        }
    }

    /* Centered model */
    for(z=0;z<CHUNK_DEPTH;z++){
        for(x=0;x<CHUNK_WIDTH;x++){
            for(y=0;y<CHUNK_HEIGHT;y++){
                register Tile tile = chunk->chunk_data[x][y][z];
                register Tile next;

                if(blocks[tile].shape == NULL ||
                   blocks[tile].shape->face_middle == NULL) continue;

                CHUNK_ADD_FACE(x, y, z, blocks[tile].shape->face_middle,
                               blocks[tile].texture.middle);
            }
        }
    }

    /* Sides to other chunks */

    /* Front side */
    for(x=0;x<CHUNK_WIDTH;x++){
        for(y=0;y<CHUNK_HEIGHT;y++){
            register Tile tile;
            register Tile next;

            tile = chunk->chunk_data[x][y][CHUNK_DEPTH-1];

            if(blocks[tile].shape == NULL ||
               blocks[tile].shape->face_front == NULL) continue;

            next = get_surrounding_tile(chunk, chunk->x, 0, chunk->z, x, y,
                                        CHUNK_DEPTH, extra);

            if(NEXT_BLOCKING(face_back)) continue;

            /* Add a left face for this block */
            CHUNK_ADD_FACE(x, y, CHUNK_DEPTH-1, blocks[tile].shape->face_front,
                           blocks[tile].texture.front);
        }
    }

    /* Back side */
    for(x=0;x<CHUNK_WIDTH;x++){
        for(y=0;y<CHUNK_HEIGHT;y++){
            register Tile tile;
            register Tile next;

            tile = chunk->chunk_data[x][y][0];

            if(blocks[tile].shape == NULL ||
               blocks[tile].shape->face_back == NULL) continue;

            next = get_surrounding_tile(chunk, chunk->x, 0, chunk->z, x, y,
                                        -1, extra);

            if(NEXT_BLOCKING(face_front)) continue;

            /* Add a left face for this block */
            CHUNK_ADD_FACE(x, y, 0, blocks[tile].shape->face_back,
                           blocks[tile].texture.back);
        }
    }

    /* Left side */
    for(z=0;z<CHUNK_DEPTH;z++){
        for(y=0;y<CHUNK_HEIGHT;y++){
            register Tile tile;
            register Tile next;

            tile = chunk->chunk_data[CHUNK_WIDTH-1][y][z];

            if(blocks[tile].shape == NULL ||
               blocks[tile].shape->face_left == NULL) continue;

            next = get_surrounding_tile(chunk, chunk->x, 0, chunk->z,
                                        CHUNK_WIDTH, y, z, extra);

            if(NEXT_BLOCKING(face_right)) continue;

            /* Add a left face for this block */
            CHUNK_ADD_FACE(CHUNK_WIDTH-1, y, z, blocks[tile].shape->face_left,
                           blocks[tile].texture.left);
        }
    }

    /* Right side */
    for(z=0;z<CHUNK_DEPTH;z++){
        for(y=0;y<CHUNK_HEIGHT;y++){
            register Tile tile;
            register Tile next;

            tile = chunk->chunk_data[0][y][z];

            if(blocks[tile].shape == NULL ||
               blocks[tile].shape->face_right == NULL) continue;

            next = get_surrounding_tile(chunk, chunk->x, 0, chunk->z,
                                        -1, y, z, extra);

            if(NEXT_BLOCKING(face_left)) continue;

            /* Add a left face for this block */
            CHUNK_ADD_FACE(0, y, z, blocks[tile].shape->face_right,
                           blocks[tile].texture.right);
        }
    }

    gfx_init_model(&chunk->chunk_model, chunk->chunk_vertices,
                   chunk->chunk_indices, chunk->chunk_texture_coords, texture,
                   1, 1, indices/3, TYPE_VERTEX, TYPE_INDEX, TYPE_TEXTURE);
    chunk->remesh = 0;
}

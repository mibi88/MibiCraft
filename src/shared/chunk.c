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

/* Biomes */
const Biome_property biomes[B_AMOUNT] = {
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

/* Blocks */
const Block_property blocks[T_AMOUNT] = {
    {S_CUBE, 1, 0, 1},  /* T_VOID */
    {S_CUBE, 0, 1, 0},  /* T_GRASS */
    {S_CUBE, 0, 1, 0},  /* T_DIRT */
    {S_CUBE, 0, 1, 0},  /* T_STONE */
    {S_CUBE, 0, 1, 0},  /* T_COAL */
    {S_CUBE, 0, 1, 0},  /* T_IRON */
    {S_CUBE, 0, 1, 0},  /* T_GOLD */
    {S_CUBE, 0, 1, 0},  /* T_REDSTONE */
    {S_CUBE, 0, 1, 0},  /* T_LAPIS */
    {S_CUBE, 0, 1, 0},  /* T_DIAMOND */
    {S_CUBE, 0, 1, 0},  /* T_BEDROCK */
    {S_CUBE, 0, 0, 1},  /* T_WATER */
    {S_CUBE, 0, 1, 0},  /* T_SAND */
    {S_CUBE, 0, 1, 0},  /* T_OAK_LOG */
    {S_CUBE, 0, 1, 0},  /* T_OAK_LEAVES */
    {S_CUBE, 0, 1, 0},  /* T_PODZOL */
    {S_CUBE, 0, 1, 0},  /* T_SPRUCE_LOG */
    {S_CUBE, 0, 1, 0},  /* T_SPRUCE_LEAVES */
    {S_CUBE, 0, 1, 0},  /* T_SNOW */
    {S_CUBE, 0, 1, 0},  /* T_DRIED_GRASS */
    {S_CUBE, 0, 1, 0},  /* T_ACACIA_LOG */
    {S_CUBE, 0, 1, 0},  /* T_ACACIA_LEAVES */
    {S_CUBE, 0, 1, 0},  /* T_SAND_STONE */
    {S_CUBE, 0, 1, 0},  /* T_PALM_LOG */
    {S_CUBE, 0, 1, 0},  /* T_PALM_LEAVES */
    {S_CUBE, 0, 1, 0},  /* T_CACTUS */
    {S_CROSS, 1, 0, 0}, /* T_ROSE */
    {S_CROSS, 1, 0, 0}, /* T_GRASS_PLANT */
    {S_CROSS, 1, 0, 0}, /* T_DEAD_BUSH */
    {S_CROSS, 1, 0, 0}, /* T_BROWN_MUSHROOM */
    {S_CROSS, 1, 0, 0}, /* T_RED_MUSHROOM */
    {S_CUBE, 0, 1, 0},  /* T_SPRUCE_PLANKS */
    {S_CUBE, 1, 1, 0},  /* T_GLASS */
    {S_CROSS, 1, 0, 0}, /* T_DANDELION */
    {S_CROSS, 1, 0, 0}, /* T_BERRY_BUSH */
    {S_CROSS, 1, 0, 0}  /* T_FRUIT_BERRY_BUSH */
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

/* Front face */
const vertex_t front_vertices[SZ_VERTICES] = {
    0, 1, 1,
    0, 0, 1,
    1, 0, 1,
    1, 1, 1
};
const vertex_t top_vertices[SZ_VERTICES] = {
    0, 1, 1,
    0, 1, 0,
    1, 1, 0,
    1, 1, 1
};
const vertex_t left_vertices[SZ_VERTICES] = {
    1, 0, 0,
    1, 1, 0,
    1, 1, 1,
    1, 0, 1
};

const index_t base_indices[SZ_INDICES] = {
    0, 1, 3,
    3, 1, 2
};

const vertex_t cross_vertices[SZ_VERTICES*2] = {
    0, 1, 0,
    0, 0, 0,
    1, 0, 1,
    1, 1, 1,
    0, 1, 1,
    0, 0, 1,
    1, 0, 0,
    1, 1, 0
};
const index_t cross_indices[SZ_INDICES*2] = {
    0, 1, 3,
    3, 1, 2,
    4, 5, 7,
    7, 5, 6
};
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
    Biome_property *properties;
    int pos_seed = seed;
    amount = probability/2+chunk_rand(sx, sz, 0, 0, seed, probability/2);
    for(i=0;i<amount;i++){
        x = chunk_rand(sx, sz, 0, 0, xorshift(&pos_seed), CHUNK_WIDTH);
        z = chunk_rand(sx, sz, 0, 0, xorshift(&pos_seed), CHUNK_DEPTH);
        real_biome = chunk_get_biome(sx, sz, x, z, seed);
        properties = (Biome_property*)&biomes[real_biome];
        y = chunk_get_height(sx, sz, x, z, properties->amplitude, seed);
        real_biome = chunk_get_biome(sx, sz, x, z, seed);
        properties = (Biome_property*)&biomes[real_biome];
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
    Biome_property *properties;
    int pos_seed = seed;
    int plant_rand;
    real_biome = chunk_get_biome(sx, sz, 0, 0, seed);
    properties = (Biome_property*)&biomes[real_biome];
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
        properties = (Biome_property*)&biomes[real_biome];
        y = chunk_get_height(sx, sz, x, z, properties->amplitude, seed);
        if(y >= CHUNK_HEIGHT/2){
            properties = (Biome_property*)&biomes[real_biome];
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
    Biome_property *properties;

    for(x=0;x<CHUNK_WIDTH;x++){
        for(z=0;z<CHUNK_DEPTH;z++){
            biome = chunk_get_biome(sx, sz, x, z, seed);

            properties = (Biome_property*)&biomes[biome];

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
}

void chunk_generate_texture_coords(Chunk *chunk, int tex_x, int tex_y) {
    /* Position of the vertices for a square facing the player */
    /* Bottom left */
    chunk->texture_coords[0] = tex_x;
    chunk->texture_coords[1] = tex_y;
    /* Top left */
    chunk->texture_coords[2] = tex_x;
    chunk->texture_coords[3] = tex_y+1;
    /* Top right */
    chunk->texture_coords[4] = tex_x+1;
    chunk->texture_coords[5] = tex_y+1;
    /* Bottom right */
    chunk->texture_coords[6] = tex_x+1;
    chunk->texture_coords[7] = tex_y;
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

void chunk_add_face_to_model(Chunk *chunk, const vertex_t *face_vertices,
                             int rx, int ry, int rz) {
    int i;
    chunk_generate_texture_coords(chunk, chunk->tex_x, chunk->tex_y);
    /* Front face */
    memcpy(chunk->vertices, face_vertices, SZ_VERTICES*sizeof(vertex_t));
    for(i=0;i<VERTICES_AMOUNT;i++){
        chunk->vertices[i*3] += chunk->_x+rx;
        chunk->vertices[i*3+1] += chunk->_y+ry;
        chunk->vertices[i*3+2] += chunk->_z+rz;
    }
    memcpy(chunk->vert_ptr, chunk->vertices, SZ_VERTICES*sizeof(vertex_t));
    chunk->vert_ptr += SZ_VERTICES;
    memcpy(chunk->indices, base_indices, SZ_INDICES*sizeof(index_t));
    for(i=0;i<SZ_INDICES;i++){
        chunk->indices[i] += chunk->triangles/TRIANGLES_AMOUNT*
                      VERTICES_AMOUNT;
    }
    memcpy(chunk->indices_ptr, chunk->indices, SZ_INDICES*sizeof(index_t));
    chunk->indices_ptr += SZ_INDICES;

    memcpy(chunk->tex_ptr, chunk->texture_coords, SZ_TEX_COORDS*
                                                  sizeof(texture_t));
    chunk->tex_ptr += SZ_TEX_COORDS;

    chunk->triangles += 2;
}

void chunk_generate_model(Chunk *chunk, unsigned int texture,
                          Tile get_tile(Chunk *chunk, int x, int y, int z,
                                        int rx, int ry, int rz, void *extra),
                          void *extra) {
    int i;
    Tile tile = T_VOID, real_tile, side_tile;
    chunk->vert_ptr = chunk->chunk_vertices;
    chunk->tex_ptr = chunk->chunk_texture_coords;
    chunk->indices_ptr = chunk->chunk_indices;
    chunk->triangles = 0;

    for(chunk->_x=0;chunk->_x<CHUNK_WIDTH;chunk->_x++){
        for(chunk->_y=0;chunk->_y<CHUNK_HEIGHT;chunk->_y++){
            for(chunk->_z=0;chunk->_z<CHUNK_DEPTH;chunk->_z++){
                tile = chunk->chunk_data[chunk->_x][chunk->_y][chunk->_z];
                if(blocks[tile].shape == S_CUBE && !blocks[tile].transparent) {
                    real_tile = tile-1;
                    chunk->tex_x = real_tile%TILE_AWIDTH;
                    chunk->tex_y = real_tile/TILE_AWIDTH;
                    side_tile = get_tile(chunk, chunk->_x, chunk->_y, chunk->_z,
                                         0, 0, 1, extra);
                    if(blocks[side_tile].transparent ||
                       (blocks[side_tile].replaceable && side_tile != tile)){
                        chunk_add_face_to_model(chunk, front_vertices, 0, 0, 0);
                    }
                    side_tile = get_tile(chunk, chunk->_x, chunk->_y, chunk->_z,
                                         0, 0, -1, extra);
                    if(blocks[side_tile].transparent ||
                       (blocks[side_tile].replaceable && side_tile != tile)){
                        chunk_add_face_to_model(chunk, front_vertices, 0, 0,
                                                -1);
                    }
                    side_tile = get_tile(chunk, chunk->_x, chunk->_y, chunk->_z,
                                         0, 1, 0, extra);
                    if(blocks[side_tile].transparent ||
                       (blocks[side_tile].replaceable && side_tile != tile)){
                        chunk_add_face_to_model(chunk, top_vertices, 0, 0, 0);
                    }
                    side_tile = get_tile(chunk, chunk->_x, chunk->_y, chunk->_z,
                                         0, -1, 0, extra);
                    if(blocks[side_tile].transparent ||
                       (blocks[side_tile].replaceable && side_tile != tile)){
                        chunk_add_face_to_model(chunk, top_vertices, 0, -1, 0);
                    }
                    side_tile = get_tile(chunk, chunk->_x, chunk->_y, chunk->_z,
                                         1, 0, 0, extra);
                    if(blocks[side_tile].transparent ||
                       (blocks[side_tile].replaceable && side_tile != tile)){
                        chunk_add_face_to_model(chunk, left_vertices, 0, 0, 0);
                    }
                    side_tile = get_tile(chunk, chunk->_x, chunk->_y, chunk->_z,
                                         -1, 0, 0, extra);
                    if(blocks[side_tile].transparent ||
                       (blocks[side_tile].replaceable && side_tile != tile)){
                        chunk_add_face_to_model(chunk, left_vertices, -1, 0, 0);
                    }
                }else if(blocks[tile].shape == S_CUBE && tile != T_VOID){
                    real_tile = tile-1;
                    chunk->tex_x = real_tile%TILE_AWIDTH;
                    chunk->tex_y = real_tile/TILE_AWIDTH;
                    side_tile = get_tile(chunk, chunk->_x, chunk->_y, chunk->_z,
                                         0, 0, 1, extra);
                    if((side_tile != tile || side_tile == T_VOID)){
                        chunk_add_face_to_model(chunk, front_vertices, 0, 0, 0);
                    }
                    side_tile = get_tile(chunk, chunk->_x, chunk->_y, chunk->_z,
                                         0, 0, -1, extra);
                    if(side_tile != tile || side_tile == T_VOID){
                        chunk_add_face_to_model(chunk, front_vertices, 0, 0,
                                                -1);
                    }
                    side_tile = get_tile(chunk, chunk->_x, chunk->_y, chunk->_z,
                                         0, 1, 0, extra);
                    if(side_tile != tile || side_tile == T_VOID){
                        chunk_add_face_to_model(chunk, top_vertices, 0, 0, 0);
                    }
                    side_tile = get_tile(chunk, chunk->_x, chunk->_y, chunk->_z,
                                         0, -1, 0, extra);
                    if(side_tile != tile || side_tile == T_VOID){
                        chunk_add_face_to_model(chunk, top_vertices, 0, -1, 0);
                    }
                    side_tile = get_tile(chunk, chunk->_x, chunk->_y, chunk->_z,
                                         1, 0, 0, extra);
                    if(side_tile != tile || side_tile == T_VOID){
                        chunk_add_face_to_model(chunk, left_vertices, 0, 0, 0);
                    }
                    side_tile = get_tile(chunk, chunk->_x, chunk->_y, chunk->_z,
                                         -1, 0, 0, extra);
                    if(side_tile != tile || side_tile == T_VOID){
                        chunk_add_face_to_model(chunk, left_vertices, -1, 0, 0);
                    }
                }else if(blocks[tile].shape == S_CROSS && tile != T_VOID){
                    real_tile = tile-1;
                    chunk->tex_x = real_tile%TILE_AWIDTH;
                    chunk->tex_y = real_tile/TILE_AWIDTH;
                    chunk_generate_texture_coords(chunk, chunk->tex_x,
                                                  chunk->tex_y);
                    /* Front face */
                    memcpy(chunk->vertices, cross_vertices,
                           SZ_VERTICES*2*sizeof(vertex_t));
                    for(i=0;i<VERTICES_AMOUNT*2;i++){
                        chunk->vertices[i*3] += chunk->_x;
                        chunk->vertices[i*3+1] += chunk->_y;
                        chunk->vertices[i*3+2] += chunk->_z;
                    }
                    memcpy(chunk->vert_ptr, chunk->vertices,
                           SZ_VERTICES*2*sizeof(vertex_t));
                    chunk->vert_ptr += SZ_VERTICES*2;
                    memcpy(chunk->indices, cross_indices,
                           SZ_INDICES*2*sizeof(index_t));
                    for(i=0;i<SZ_INDICES*2;i++){
                        chunk->indices[i] += chunk->triangles/TRIANGLES_AMOUNT*
                                      VERTICES_AMOUNT;
                    }
                    memcpy(chunk->indices_ptr, chunk->indices,
                           SZ_INDICES*2*sizeof(index_t));
                    chunk->indices_ptr += SZ_INDICES*2;

                    memcpy(chunk->tex_ptr, chunk->texture_coords,
                           SZ_TEX_COORDS*sizeof(texture_t));
                    chunk->tex_ptr += SZ_TEX_COORDS;
                    memcpy(chunk->tex_ptr, chunk->texture_coords,
                           SZ_TEX_COORDS*sizeof(texture_t));
                    chunk->tex_ptr += SZ_TEX_COORDS;

                    chunk->triangles += 4;
                }
            }
        }
    }

    gfx_init_model(&chunk->chunk_model, chunk->chunk_vertices,
                   chunk->chunk_indices, chunk->chunk_texture_coords, texture,
                   1, 1, chunk->triangles, TYPE_VERTEX, TYPE_INDEX,
                   TYPE_TEXTURE);
    chunk->remesh = 0;
}


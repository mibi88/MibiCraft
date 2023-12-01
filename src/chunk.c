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

#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>

/* Biomes */
Biome_property biomes[B_AMOUNT] = {
    {(CHUNK_HEIGHT/2), 32, T_GRASS, T_DIRT, {T_GRASS_PLANT, T_ROSE}},
    {(CHUNK_HEIGHT/1.8), 16, T_PODZOL, T_DIRT, {T_BROWN_MUSHROOM,
                                                T_RED_MUSHROOM}},
    {(CHUNK_HEIGHT/2.2), 64, T_DRIED_GRASS, T_DIRT, {T_GRASS_PLANT, T_ROSE}},
    {(CHUNK_HEIGHT/2.2), 96, T_SAND, T_SAND_STONE, {T_DEAD_BUSH, T_CACTUS}}
};

/* Blocks */
Block_property blocks[T_AMOUNT] = {
    {S_CUBE, 1},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CUBE, 0},
    {S_CROSS, 1},
    {S_CROSS, 1},
    {S_CROSS, 1},
    {S_CROSS, 1},
    {S_CROSS, 1},
    {S_CUBE, 0},
    {S_CUBE, 1}
};
/**********/

/* Trees */
Tile oak_tree[TREE_WIDTH*TREE_HEIGHT*TREE_DEPTH] = {
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

Tile spruce_tree[TREE_WIDTH*TREE_HEIGHT*TREE_DEPTH] = {
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

Tile acacia_tree[TREE_WIDTH*TREE_HEIGHT*TREE_DEPTH] = {
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
Tile fallen_oak_tree[TREE_WIDTH*TREE_HEIGHT*TREE_DEPTH] = {
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
Tile fallen_spruce_tree[TREE_WIDTH*TREE_HEIGHT*TREE_DEPTH] = {
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
float front_vertices[SZ_VERTICES] = {
    -0.5, 0.5, 0,
    -0.5, -0.5, 0,
    0.5, -0.5, 0,
    0.5, 0.5, 0
};
float top_vertices[SZ_VERTICES] = {
    -0.5, 0.5, 0,
    -0.5, 0.5, -1,
    0.5, 0.5, -1,
    0.5, 0.5, 0
};
float left_vertices[SZ_VERTICES] = {
    0.5, -0.5, -1,
    0.5, 0.5, -1,
    0.5, 0.5, 0,
    0.5, -0.5, 0
};

int base_indices[SZ_INDICES] = {
    0, 1, 3,
    3, 1, 2
};

float cross_vertices[SZ_VERTICES*2] = {
    -0.5, 0.5, -1,
    -0.5, -0.5, -1,
    0.5, -0.5, 0,
    0.5, 0.5, 0,
    -0.5, 0.5, 0,
    -0.5, -0.5, 0,
    0.5, -0.5, -1,
    0.5, 0.5, -1
};
int cross_indices[SZ_INDICES*2] = {
    0, 1, 3,
    3, 1, 2,
    4, 5, 7,
    7, 5, 6
};
/**************/

float vertices[SZ_VERTICES*2];
int indices[SZ_VERTICES*2];

float texture_coords[SZ_TEX_COORDS];

Biome chunk_get_biome(int sx, int sz, int x, int z, int seed) {
    int i;
    float biome_height;
    float biome_step = 0.5/B_AMOUNT;
    biome_height = stb_perlin_noise3_seed((float)(x+sx)/512,
                                          (float)(z+sz)/512, 0, 0, 0,
                                          0, seed);
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
    return stb_perlin_noise3_seed((float)(x+sx)/256, (float)(z+sz)/256, 0, 0, 0,
                                  0, seed)*amplitude+(CHUNK_HEIGHT/2);
}

int chunk_should_add(int sx, int sz, int x, int z, int seed, int probability) {
    srand(seed+(sz+z)*(sx+x)+(sx+x));
    return !(rand()%probability);
}

void chunk_place_structure(Chunk *chunk, int sx, int sy, int sz,
                           Tile *structure, int width, int height,
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
                              Tile *structure, int width, int height,
                              int depth, int probability, int amplitude,
                              int seed, Biome biome, int dx, int dz) {
    int x, y, z;
    Biome real_biome;
    for(x=-width;x<CHUNK_WIDTH+width;x++){
        for(z=-depth;z<CHUNK_DEPTH+depth;z++){
            srand(seed*((sz+z)*CHUNK_WIDTH+(sx+x)));
            if(y >= CHUNK_HEIGHT/2){
                y = chunk_get_height(sx, sz, x, z, amplitude, seed);
                if(chunk_should_add(sx, sz ,x, z, seed, probability)){
                    real_biome = chunk_get_biome(sx, sz, x, z, seed);
                    if(real_biome == biome){
                        chunk_place_structure(chunk, x-dx, y, z-dz, structure,
                                              width, height, depth);
                    }
                }
            }
        }
    }
}

void chunk_generate_ores(Chunk *chunk, int sx, int sz, int x, int y, int z,
                         float height, Tile material, Tile ore,
                         float amplitude, float probability, int depth,
                         int seed) {
    if(stb_perlin_noise3_seed((float)(x+sx)/amplitude, (float)y/amplitude,
                              (float)(z+sz)/amplitude, 0, 0, 0,
                              seed) > probability && y < (int)height/depth &&
                              chunk->chunk_data[x][y][z] == material) {
        chunk->chunk_data[x][y][z] = ore;
    }
}

void chunk_generate_layer(Chunk *chunk, int sx, int sz, int x, int y, int z,
                          float height, float min, float step, float amplitude,
                          Tile top, Tile material, int seed) {
    float layer;
    layer = stb_perlin_noise3_seed((float)(x+sx)/step, (float)(z+sz)/step, 0, 0,
                                   0, 0, seed)*amplitude+min;
    if(y<(int)height && y>=(int)(height-layer)){
        chunk->chunk_data[x][y][z] = (int)height-y == 1 ? top : material;
    }
}

void chunk_generate_beaches(Chunk *chunk, int sx, int sz, int x, int y, int z,
                            float height, float min, float step,
                            float amplitude, Tile material, int seed) {
    float layer;
    layer = stb_perlin_noise3_seed((float)(x+sx)/step, (float)(z+sz)/step, 0, 0,
                                   0, 0, seed)*amplitude+min;
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

void chunk_generate_plants(Chunk *chunk, int sx, int sz, int x, int z,
        float height, Tile plant, int probability, int seed) {
    if(height >= CHUNK_HEIGHT/2 && height < CHUNK_HEIGHT){
        if(chunk->chunk_data[x][(int)height][z] != plant){
            if(chunk_should_add(sx, sz, x, z, seed, probability)){
                chunk->chunk_data[x][(int)height][z] = plant;
            }
        }
    }
}

void chunk_generate_caves(Chunk *chunk, int sx, int sz, int x, int y, int z,
                          float step, float probability, Tile material,
                          int seed) {
    if(chunk->chunk_data[x][y][z] == material &&
       stb_perlin_noise3_seed((float)(x+sx)/step, (float)y/step,
               (float)(z+sz)/step, 0, 0, 0, seed) > probability){
        chunk->chunk_data[x][y][z] = T_VOID;
    }
}

void chunk_generate_data(Chunk *chunk, int sx, int sz, int seed) {
    int x, y, z;
    float height;
    Biome biome = B_PLAINS;
    Biome_property *properties;
    srand(seed+sx+sz*CHUNK_WIDTH);
    for(x=0;x<CHUNK_WIDTH;x++){
        for(z=0;z<CHUNK_DEPTH;z++){
            biome = chunk_get_biome(sx, sz, x, z, seed);

            properties = &biomes[biome];

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
                chunk_generate_plants(chunk, sx, sz, x, z, height,
                                      properties->plants[0], 32, seed);
                chunk_generate_plants(chunk, sx, sz, x, z, height,
                                      properties->plants[1], 24, seed);

                chunk_generate_layer(chunk, sx, sz, x, y, z, height, 3, 128, 3,
                                     properties->layer_top,
                                     properties->layer_material, seed);
                chunk_generate_beaches(chunk, sx, sz, x, y, z, height, 0, 16, 3,
                                       T_SAND, seed);
                chunk_generate_caves(chunk, sx, sz, x, y, z, 8, 0.25, T_STONE,
                                     seed);
                chunk_generate_caves(chunk, sx, sz, x, y, z, 32, 0.75, T_STONE,
                                     seed);
                chunk_generate_ores(chunk, sx, sz, x, y, z, height, T_STONE,
                                    T_DIAMOND, 2, 0.7, 6, seed);
                chunk_generate_ores(chunk, sx, sz, x, y, z, height, T_STONE,
                                    T_LAPIS, 3, 0.6, 5, seed);
                chunk_generate_ores(chunk, sx, sz, x, y, z, height, T_STONE,
                                    T_REDSTONE, 4, 0.5, 4, seed);
                chunk_generate_ores(chunk, sx, sz, x, y, z, height, T_STONE,
                                    T_GOLD, 5, 0.4, 3, seed);
                chunk_generate_ores(chunk, sx, sz, x, y, z, height, T_STONE,
                                    T_IRON, 5, 0.3, 2, seed);
                chunk_generate_ores(chunk, sx, sz, x, y, z, height, T_STONE,
                                    T_COAL, 6, 0.2, 1, seed);
                if(y == 0){
                    chunk->chunk_data[x][y][z] = T_BEDROCK;
                }
            }
        }
    }
    chunk->x = sx;
    chunk->z = sz;
    chunk_generate_structure(chunk, sx, sz, fallen_oak_tree, TREE_WIDTH,
                             TREE_HEIGHT, TREE_DEPTH,
                             properties->tree_probability*3,
                             properties->amplitude, seed, B_PLAINS,
                             TREE_WIDTH/2, TREE_DEPTH/2);
    chunk_generate_structure(chunk, sx, sz, oak_tree, TREE_WIDTH, TREE_HEIGHT,
                             TREE_DEPTH, properties->tree_probability,
                             properties->amplitude, seed, B_PLAINS,
                             TREE_WIDTH/2, TREE_DEPTH/2);
    chunk_generate_structure(chunk, sx, sz, fallen_spruce_tree, TREE_WIDTH,
                             TREE_HEIGHT, TREE_DEPTH,
                             properties->tree_probability*3,
                             properties->amplitude, seed, B_TAIGA, TREE_WIDTH/2,
                             TREE_DEPTH/2);
    chunk_generate_structure(chunk, sx, sz, spruce_tree, TREE_WIDTH,
                             TREE_HEIGHT, TREE_DEPTH,
                             properties->tree_probability,
                             properties->amplitude, seed, B_TAIGA, TREE_WIDTH/2,
                             TREE_DEPTH/2);
    chunk_generate_structure(chunk, sx, sz, acacia_tree, TREE_WIDTH,
                             TREE_HEIGHT, TREE_DEPTH,
                             properties->tree_probability,
                             properties->amplitude, seed, B_SAVANNA,
                             TREE_WIDTH/2, TREE_DEPTH/2);
    chunk_generate_snow(chunk, sx, sz, properties->amplitude, seed);
}

void chunk_generate_texture_coords(int tex_x, int tex_y) {
    float x = (float)tex_x, y = (float)tex_y;
    /* Position of the vertices for a square facing the player */
    /* Bottom left */
    texture_coords[0] = x*TILE_FWIDTH;
    texture_coords[1] = y*TILE_FHEIGHT;
    /* Top left */
    texture_coords[2] = x*TILE_FWIDTH;
    texture_coords[3] = (y+1)*TILE_FHEIGHT;
    /* Top right */
    texture_coords[4] = (x+1)*TILE_FWIDTH;
    texture_coords[5] = (y+1)*TILE_FHEIGHT;
    /* Bottom right */
    texture_coords[6] = (x+1)*TILE_FWIDTH;
    texture_coords[7] = y*TILE_FHEIGHT;
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

float *vert_ptr;
float *tex_ptr;
int *indices_ptr;
int triangles;
int _x, _y, _z;
int tex_x, tex_y;

void chunk_add_face_to_model(float *face_vertices, float rx,
                             float ry, float rz) {
    int i;
    chunk_generate_texture_coords(tex_x, tex_y);
    /* Front face */
    memcpy(vertices, face_vertices,
           SZ_VERTICES*sizeof(float));
    for(i=0;i<VERTICES_AMOUNT;i++){
        vertices[i*3] += (float)_x+rx;
        vertices[i*3+1] += (float)_y+ry;
        vertices[i*3+2] += (float)_z+rz;
    }
    memcpy(vert_ptr, vertices, SZ_VERTICES*sizeof(float));
    vert_ptr += SZ_VERTICES;
    memcpy(indices, base_indices, SZ_INDICES*sizeof(int));
    for(i=0;i<SZ_INDICES;i++){
        indices[i] += triangles/TRIANGLES_AMOUNT*
                      VERTICES_AMOUNT;
    }
    memcpy(indices_ptr, indices, SZ_INDICES*sizeof(int));
    indices_ptr += SZ_INDICES;
    
    memcpy(tex_ptr, texture_coords,
           SZ_TEX_COORDS*sizeof(float));
    tex_ptr += SZ_TEX_COORDS;
    
    triangles += 2;
}

void chunk_generate_model(Chunk *chunk, unsigned int texture,
                          Tile get_tile(Chunk *chunk, int x, int y, int z,
                                        int rx, int ry, int rz)) {
    int i;
    Tile tile = T_VOID, real_tile, side_tile;
    vert_ptr = chunk->chunk_vertices;
    tex_ptr = chunk->chunk_texture_coords;
    indices_ptr = chunk->chunk_indices;
    triangles = 0;
    for(_x=0;_x<CHUNK_WIDTH;_x++){
        for(_y=0;_y<CHUNK_HEIGHT;_y++){
            for(_z=0;_z<CHUNK_DEPTH;_z++){
                tile = chunk->chunk_data[_x][_y][_z];
                if(blocks[tile].shape == S_CUBE && !blocks[tile].transparent) {
                    real_tile = tile-1;
                    tex_x = real_tile%TILE_AWIDTH;
                    tex_y = real_tile/TILE_AWIDTH;
                    side_tile = get_tile(chunk, _x, _y, _z, 0, 0, 1);
                    if(blocks[side_tile].transparent){
                        chunk_add_face_to_model(front_vertices, 0, 0, 0);
                    }
                    side_tile = get_tile(chunk, _x, _y, _z, 0, 0, -1);
                    if(blocks[side_tile].transparent){
                        chunk_add_face_to_model(front_vertices, 0, 0, -1);
                    }
                    side_tile = get_tile(chunk, _x, _y, _z, 0, 1, 0);
                    if(blocks[side_tile].transparent){
                        chunk_add_face_to_model(top_vertices, 0, 0, 0);
                    }
                    side_tile = get_tile(chunk, _x, _y, _z, 0, -1, 0);
                    if(blocks[side_tile].transparent){
                        chunk_add_face_to_model(top_vertices, 0, -1, 0);
                    }
                    side_tile = get_tile(chunk, _x, _y, _z, 1, 0, 0);
                    if(blocks[side_tile].transparent){
                        chunk_add_face_to_model(left_vertices, 0, 0, 0);
                    }
                    side_tile = get_tile(chunk, _x, _y, _z, -1, 0, 0);
                    if(blocks[side_tile].transparent){
                        chunk_add_face_to_model(left_vertices, -1, 0, 0);
                    }
                }else if(blocks[tile].shape == S_CUBE && tile != T_VOID){
                    real_tile = tile-1;
                    tex_x = real_tile%TILE_AWIDTH;
                    tex_y = real_tile/TILE_AWIDTH;
                    side_tile = get_tile(chunk, _x, _y, _z, 0, 0, 1);
                    if(!blocks[side_tile].transparent || side_tile == T_VOID){
                        chunk_add_face_to_model(front_vertices, 0, 0, 0);
                    }
                    side_tile = get_tile(chunk, _x, _y, _z, 0, 0, -1);
                    if(!blocks[side_tile].transparent || side_tile == T_VOID){
                        chunk_add_face_to_model(front_vertices, 0, 0, -1);
                    }
                    side_tile = get_tile(chunk, _x, _y, _z, 0, 1, 0);
                    if(!blocks[side_tile].transparent || side_tile == T_VOID){
                        chunk_add_face_to_model(top_vertices, 0, 0, 0);
                    }
                    side_tile = get_tile(chunk, _x, _y, _z, 0, -1, 0);
                    if(!blocks[side_tile].transparent || side_tile == T_VOID){
                        chunk_add_face_to_model(top_vertices, 0, -1, 0);
                    }
                    side_tile = get_tile(chunk, _x, _y, _z, 1, 0, 0);
                    if(!blocks[side_tile].transparent || side_tile == T_VOID){
                        chunk_add_face_to_model(left_vertices, 0, 0, 0);
                    }
                    side_tile = get_tile(chunk, _x, _y, _z, -1, 0, 0);
                    if(!blocks[side_tile].transparent || side_tile == T_VOID){
                        chunk_add_face_to_model(left_vertices, -1, 0, 0);
                    }
                }else if(blocks[tile].shape == S_CROSS && tile != T_VOID){
                    real_tile = tile-1;
                    tex_x = real_tile%TILE_AWIDTH;
                    tex_y = real_tile/TILE_AWIDTH;
                    chunk_generate_texture_coords(tex_x, tex_y);
                    /* Front face */
                    memcpy(vertices, cross_vertices,
                           SZ_VERTICES*2*sizeof(float));
                    for(i=0;i<VERTICES_AMOUNT*2;i++){
                        vertices[i*3] += (float)_x;
                        vertices[i*3+1] += (float)_y;
                        vertices[i*3+2] += (float)_z;
                    }
                    memcpy(vert_ptr, vertices, SZ_VERTICES*2*sizeof(float));
                    vert_ptr += SZ_VERTICES*2;
                    memcpy(indices, cross_indices, SZ_INDICES*2*sizeof(int));
                    for(i=0;i<SZ_INDICES*2;i++){
                        indices[i] += triangles/TRIANGLES_AMOUNT*
                                      VERTICES_AMOUNT;
                    }
                    memcpy(indices_ptr, indices, SZ_INDICES*2*sizeof(int));
                    indices_ptr += SZ_INDICES*2;

                    memcpy(tex_ptr, texture_coords,
                           SZ_TEX_COORDS*sizeof(float));
                    tex_ptr += SZ_TEX_COORDS;
                    memcpy(tex_ptr, texture_coords,
                           SZ_TEX_COORDS*sizeof(float));
                    tex_ptr += SZ_TEX_COORDS;

                    triangles += 4;
                }
            }
        }
    }

    gfx_init_model(&chunk->chunk_model, chunk->chunk_vertices,
                   chunk->chunk_indices, chunk->chunk_texture_coords, texture,
                   1, 1, triangles);
}


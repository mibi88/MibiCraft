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

#include <world.h>

World *_world;
int _cx, _cy;
int _ncx, _ncy;

Chunk *world_get_chunk(World *world, int x, int y) {
    int chunk_x = x/CHUNK_WIDTH, chunk_y = y/CHUNK_DEPTH;
    _ncx = chunk_x;
    _ncy = chunk_y;
    if(chunk_x >= 0 && chunk_x < world->width && chunk_y >= 0 &&
       chunk_y < world->height){
        return &world->chunks[chunk_y*world->width+chunk_x];
    }
    return NULL;
}

Tile _modelgen_get_tile(Chunk *chunk, int x, int y, int z, int rx, int ry,
                        int rz) {
    int i;
    Chunk *tile_chunk;

    x += rx+chunk->x;
    y += ry;
    z += rz+chunk->z;
    if(x >= chunk->x && x < chunk->x+CHUNK_WIDTH &&
       z >= chunk->z && z < chunk->z+CHUNK_DEPTH){
        return chunk_get_tile(chunk, x-chunk->x, y, z-chunk->z, 0, 0, 0);
    }
    for(i=0;i<_world->width*_world->height;i++){
        tile_chunk = &_world->chunks[i];
        if(x >= tile_chunk->x && x < tile_chunk->x+CHUNK_WIDTH &&
           z >= tile_chunk->z && z < tile_chunk->z+CHUNK_DEPTH){
               return chunk_get_tile(tile_chunk, x-chunk->x, y, z-chunk->z, 0,
                                     0, 0);
           }
    }
    return T_GRASS;
}

#define GET_TILE chunk_get_tile

void world_generate_data(World *world) {
    int x, y, pos;
    _world = world;
    for(y=0;y<world->height;y++) {
        for(x=0;x<world->width;x++) {
            _cx = x;
            _cy = y;
            pos = y*world->width+x;
            chunk_generate_data(&world->chunks[pos], world->x+x*CHUNK_WIDTH,
                                world->y+y*CHUNK_DEPTH, world->seed);
            chunk_generate_model(&world->chunks[pos], world->texture,
                                 GET_TILE);
        }
    }
}

void world_generate_models(World *world) {
    int x, y;
    _world = world;
    for(y=0;y<world->height;y++) {
        for(x=0;x<world->width;x++) {
            _cx = x;
            _cy = y;
            chunk_generate_model(&world->chunks[y*world->width+x],
                                 world->texture, GET_TILE);
        }
    }
}

void world_init(World *world, int width, int height, int seed,
                unsigned int texture) {
    world->width = width;
    world->height = height;
    world->chunks = malloc(width*height*sizeof(Chunk));
    if(!world->chunks){
        puts("World init failed");
        exit(1);
    }
    world->x = 0;
    world->y = 0;
    world->seed = seed;
    world->texture = texture;
    world_generate_data(world);
}

void world_update(World *world, float sx, float sz) {
    int x, y;
    int chunk_x = (sx-world->x)/CHUNK_WIDTH;
    int chunk_y = (sz-world->y)/CHUNK_DEPTH;
    int center_x = world->width/2, center_y = world->height/2;
    int old_x, old_y;
    Chunk *chunk;
    while(chunk_x != center_x || chunk_y != center_y){
        old_x = world->x;
        old_y = world->y;
        if(chunk_y < center_y){
            world->y -= CHUNK_DEPTH;
        }
        if(chunk_y > center_y){
            world->y += CHUNK_DEPTH;
        }
        if(chunk_x < center_x){
            world->x -= CHUNK_WIDTH;
        }
        if(chunk_x > center_x){
            world->x += CHUNK_WIDTH;
        }
        for(y=0;y<world->height;y++){
            for(x=0;x<world->width;x++){
                chunk = &world->chunks[y*world->width+x];
                if(chunk->x < world->x){
                    chunk_generate_data(chunk,
                                        old_x+world->width*CHUNK_WIDTH,
                                        chunk->z, world->seed);
                    chunk_generate_model(chunk, world->texture, GET_TILE);
                }else if(chunk->x >= world->x+world->width*CHUNK_WIDTH){
                    chunk_generate_data(chunk, world->x, chunk->z,
                                        world->seed);
                    chunk_generate_model(chunk, world->texture, GET_TILE);
                }
                if(chunk->z < world->y){
                    chunk_generate_data(chunk, chunk->x,
                                old_y+world->height*CHUNK_DEPTH,
                                        world->seed);
                    chunk_generate_model(chunk, world->texture, GET_TILE);
                }else if(chunk->z >= world->y+world->height*CHUNK_DEPTH){
                    chunk_generate_data(chunk, chunk->x, world->y,
                                        world->seed);
                    chunk_generate_model(chunk, world->texture, GET_TILE);
                }
            }
        }
        chunk_x = (sx-world->x)/CHUNK_WIDTH;
        chunk_y = (sz-world->y)/CHUNK_DEPTH;
        center_x = world->width/2, center_y = world->height/2;
    }
}

void world_render(World *world) {
    int i;
    for(i=0;i<world->width*world->height;i++) {
        gfx_draw_model(&world->chunks[i].chunk_model,
                       world->chunks[i].x, -(CHUNK_HEIGHT/2),
                       world->chunks[i].z, 0, 0, 0);
    }
}

Tile world_get_tile(World *world, float sx, float sy, float sz) {
    int x, y, z;
    int chunk_x, chunk_y;
    sx -= 0.5;
    sy -= 0.5;
    sz -= 0.5;
    x = (int)sx-world->x;
    y = (int)sy;
    z = (int)sz-world->y;
    chunk_x = x/CHUNK_WIDTH;
    chunk_y = z/CHUNK_DEPTH;
    if(chunk_x >= 0 && chunk_x < world->width && chunk_y >= 0 &&
       chunk_y < world->height) {
        Chunk *chunk = &world->chunks[chunk_y*world->width+chunk_x];
        return chunk_get_tile(chunk, x%CHUNK_WIDTH, y+(CHUNK_HEIGHT/2),
                              z%CHUNK_DEPTH, 0, 0, 0);
    }
    return T_VOID;
}

void world_set_tile(World *world, Tile tile, float sx, float sy, float sz) {
    int x, y, z;
    int chunk_x, chunk_y;
    sx -= 0.5;
    sy -= 0.5;
    sz -= 0.5;
    x = (int)sx-world->x;
    y = (int)sy;
    z = (int)sz-world->y;
    chunk_x = x/CHUNK_WIDTH;
    chunk_y = z/CHUNK_DEPTH;
    if(chunk_x >= 0 && chunk_x < world->width && chunk_y >= 0 &&
       chunk_y < world->height) {
        Chunk *chunk = &world->chunks[chunk_y*world->width+chunk_x];
        chunk_set_tile(chunk, tile, x%CHUNK_WIDTH, y+(CHUNK_HEIGHT/2),
                       z%CHUNK_DEPTH);
    }
}

void world_update_chunk_model_at(World *world, float sx, float sz) {
    int x = (int)sx-world->x, z = (int)sz-world->y;
    int chunk_x = x/CHUNK_WIDTH, chunk_y = z/CHUNK_DEPTH;
    if(chunk_x >= 0 && chunk_x < world->width && chunk_y >= 0 &&
       chunk_y < world->height) {
        Chunk *chunk = &world->chunks[chunk_y*world->width+chunk_x];
        chunk_generate_model(chunk, world->texture, chunk_get_tile);
    }
}

void world_free(World *world) {
    free(world->chunks);
}


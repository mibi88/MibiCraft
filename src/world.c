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

#if THREADING
#include <pthread.h>
#endif

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
                        int rz, void *vworld) {
    int i;
    Chunk *tile_chunk;
    World *world = (World*)vworld;

    x += rx+chunk->x;
    y += ry;
    z += rz+chunk->z;
    if(x >= chunk->x && x < chunk->x+CHUNK_WIDTH &&
       z >= chunk->z && z < chunk->z+CHUNK_DEPTH){
        return chunk_get_tile(chunk, x-chunk->x, y, z-chunk->z, 0, 0, 0);
    }
    for(i=0;i<world->width*world->height;i++){
        tile_chunk = &world->chunks[i];
        if(x >= tile_chunk->x && x < tile_chunk->x+CHUNK_WIDTH &&
           z >= tile_chunk->z && z < tile_chunk->z+CHUNK_DEPTH){
               return chunk_get_tile(tile_chunk, x-tile_chunk->x, y,
                                     z-tile_chunk->z, 0, 0, 0);
           }
    }
    return T_GRASS;
}

#define GET_TILE _modelgen_get_tile

void world_generate_models(World *world) {
    int x, y;
    world->finished = 0;
    for(y=0;y<world->height;y++) {
        for(x=0;x<world->width;x++) {
            _cx = x;
            _cy = y;
            world->chunks[y*world->width+x].ready = 0;
            chunk_generate_model(&world->chunks[y*world->width+x],
                                 world->texture, GET_TILE, world);
            world->chunks[y*world->width+x].ready = 1;
        }
    }
    world->finished = 1;
}

void world_generate_data(World *world) {
    int x, y, pos;
    world->finished = 0;
    for(y=0;y<world->height;y++) {
        for(x=0;x<world->width;x++) {
            _cx = x;
            _cy = y;
            pos = y*world->width+x;
            world->chunks[pos].ready = 0;
            chunk_generate_data(&world->chunks[pos], world->x+x*CHUNK_WIDTH,
                                world->y+y*CHUNK_DEPTH, world->seed);
            world->chunks[pos].ready = 1;
        }
    }
    world_generate_models(world);
    world->finished = 1;
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
    world->seed = seed;
    world->texture = texture;
    world_generate_data(world);
}

void *_world_update(void *vworld) {
    World *world = (World*)vworld;
    int x, y;
    int chunk_x = (world->new_x-world->x)/CHUNK_WIDTH;
    int chunk_y = (world->new_z-world->y)/CHUNK_DEPTH;
    int center_x = world->width/2, center_y = world->height/2;
    int old_x, old_y;
    Chunk *chunk;
#if THREADING
    if(!world->finished){
        pthread_exit(NULL);
    }
#endif
    world->finished = 0;
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
                /* Generate new chunks */
                if(chunk->x < world->x){
                    chunk->ready = 0;
                    chunk_generate_data(chunk, old_x+world->width*CHUNK_WIDTH,
                                        chunk->z, world->seed);
                    chunk_generate_model(chunk, world->texture, GET_TILE,
                                         world);
                    chunk->ready = 1;
                }else if(chunk->x >= world->x+world->width*CHUNK_WIDTH){
                    chunk->ready = 0;
                    chunk_generate_data(chunk, world->x, chunk->z, world->seed);
                    chunk_generate_model(chunk, world->texture, GET_TILE,
                                         world);
                    chunk->ready = 1;
                }
                if(chunk->z < world->y){
                    chunk->ready = 0;
                    chunk_generate_data(chunk, chunk->x,
                                old_y+world->height*CHUNK_DEPTH, world->seed);
                    chunk_generate_model(chunk, world->texture, GET_TILE,
                                         world);
                    chunk->ready = 1;
                }else if(chunk->z >= world->y+world->height*CHUNK_DEPTH){
                    chunk->ready = 0;
                    chunk_generate_data(chunk, chunk->x, world->y, world->seed);
                    chunk_generate_model(chunk, world->texture, GET_TILE,
                                         world);
                    chunk->ready = 1;
                }
                /* Update old chunks */
                if(chunk->x == world->x+CHUNK_WIDTH){
                    chunk->ready = 0;
                    chunk_generate_model(chunk, world->texture, GET_TILE,
                                         world);
                    chunk->ready = 1;
                }else if(chunk->x == world->x+(world->width-2)*CHUNK_WIDTH){
                    chunk->ready = 0;
                    chunk_generate_model(chunk, world->texture, GET_TILE,
                                         world);
                    chunk->ready = 1;
                }
                if(chunk->z == world->y+CHUNK_DEPTH){
                    chunk->ready = 0;
                    chunk_generate_model(chunk, world->texture, GET_TILE,
                                         world);
                    chunk->ready = 1;
                }else if(chunk->z == world->y+(world->height-2)*CHUNK_DEPTH){
                    chunk->ready = 0;
                    chunk_generate_model(chunk, world->texture, GET_TILE,
                                         world);
                    chunk->ready = 1;
                }
            }
        }
        chunk_x = (world->new_x-world->x)/CHUNK_WIDTH;
        chunk_y = (world->new_z-world->y)/CHUNK_DEPTH;
        center_x = world->width/2, center_y = world->height/2;
    }
    world->finished = 1;
#if THREADING
    pthread_exit(NULL);
#else
    return NULL;
#endif
}

void world_update(World *world, float sx, float sz) {
#if THREADING
    pthread_t id;
    world->new_x = sx;
    world->new_z = sz;
    pthread_create(&id, NULL, _world_update, (void*)world);
    pthread_detach(id);
#else
    world->new_x = sx;
    world->new_z = sz;
    _world_update((void*)world);
#endif
}

void world_render(World *world) {
    int i;
    for(i=0;i<world->width*world->height;i++) {
        if(!world->chunks[i].ready) continue;
        gfx_draw_model(&world->chunks[i].chunk_model,
                       world->chunks[i].x, -(CHUNK_HEIGHT/2),
                       world->chunks[i].z, 0, 0, 0);
    }
}

Tile world_get_tile(World *world, float sx, float sy, float sz) {
    int i, x, y, z;
    Chunk *tile_chunk;
    sx -= 0.5;
    sy -= 0.5;
    sz -= 1;
    x = (int)sx;
    y = (int)sy;
    z = (int)sz;
    for(i=0;i<world->width*world->height;i++){
        tile_chunk = &world->chunks[i];
        if(x >= tile_chunk->x && x < tile_chunk->x+CHUNK_WIDTH &&
           z >= tile_chunk->z && z < tile_chunk->z+CHUNK_DEPTH){
               return chunk_get_tile(tile_chunk, x-tile_chunk->x, y,
                                     z-tile_chunk->z, 0, 0, 0);
           }
    }
    return T_VOID;
}

void world_set_tile(World *world, Tile tile, float sx, float sy, float sz) {
    int i, x, y, z;
    Chunk *tile_chunk;
    sx -= 0.5;
    sy -= 0.5;
    sz -= 1;
    x = (int)sx;
    y = (int)sy;
    z = (int)sz;
    for(i=0;i<world->width*world->height;i++){
        tile_chunk = &world->chunks[i];
        if(x >= tile_chunk->x && x < tile_chunk->x+CHUNK_WIDTH &&
           z >= tile_chunk->z && z < tile_chunk->z+CHUNK_DEPTH){
               chunk_set_tile(tile_chunk, tile, x-tile_chunk->x, y,
                              z-tile_chunk->z);
           }
    }
}

void world_update_chunk_model_at(World *world, float sx, float sz) {
    int x = (int)sx-world->x, z = (int)sz-world->y;
    int chunk_x = x/CHUNK_WIDTH, chunk_y = z/CHUNK_DEPTH;
    if(chunk_x >= 0 && chunk_x < world->width && chunk_y >= 0 &&
       chunk_y < world->height) {
        Chunk *chunk = &world->chunks[chunk_y*world->width+chunk_x];
        chunk_generate_model(chunk, world->texture, GET_TILE, world);
    }
}

void world_free(World *world) {
    free(world->chunks);
}


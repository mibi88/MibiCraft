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

#include <player.h>

#include <threading.h>

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

void world_init_data(World *world) {
    int x, y, pos, start;
    world->finished = 0;
    y = world->height/2;
    x = world->width/2;
    pos = y*world->width+x;
    world->chunks[pos].ready = 0;
    chunk_generate_data(world->chunks+pos, world->x+x*CHUNK_WIDTH,
                        world->y+y*CHUNK_DEPTH, world->seed);
    chunk_generate_model(world->chunks+pos, world->texture, GET_TILE, world);
    world->chunks[pos].regenerate = 0;
    world->chunks[pos].remesh = 1;
    world->chunks[pos].ready = 1;
    start = pos;
    for(y=0;y<world->height;y++) {
        for(x=0;x<world->width;x++) {
            _cx = x;
            _cy = y;
            pos = y*world->width+x;
            if(pos != start){
                world->chunks[pos].ready = 0;
                world->chunks[pos].x = world->x+x*CHUNK_WIDTH;
                world->chunks[pos].z = world->y+y*CHUNK_DEPTH;
                world->chunks[pos].regenerate = 1;
            }
        }
    }
    world->finished = 1;
    world->update_required = 1;
}

int world_init(World *world, int width, int height, int seed,
               unsigned int texture, int player_num) {
    int i;
    world->width = width;
    world->height = height;
    world->players = malloc(sizeof(Player)*player_num);
    world->player_num = player_num;
    if(!world->players){
        puts("Player init failed");
        return 1;
    }
    world->chunks = malloc(width*height*sizeof(Chunk));
    if(!world->chunks){
        puts("World init failed");
        free(world->players);
        world->players = NULL;
        return 1;
    }
    world->seed = seed;
    world->texture = texture;
    for(i=0;i<player_num;i++){
        player_init(world->players+i);
    }
    world->x = world->players[0].entity.x;
    world->y = world->players[0].entity.z;
    world_init_data(world);
    return 0;
}

THREAD_CALL(_world_update, vworld) {
    World *world = (World*)vworld;
    int x, y;
    int chunk_x = (world->new_x-world->x)/CHUNK_WIDTH;
    int chunk_y = (world->new_z-world->y)/CHUNK_DEPTH;
    int center_x = world->width/2, center_y = world->height/2;
    int old_x, old_y;
    Chunk *chunk;

    if(!world->finished){
        THREAD_EXIT();
    }
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
                    chunk->x = old_x+world->width*CHUNK_WIDTH;
                    chunk->regenerate = 1;
                }else if(chunk->x >= world->x+world->width*CHUNK_WIDTH){
                    chunk->ready = 0;
                    chunk->x = world->x;
                    chunk->regenerate = 1;
                }
                if(chunk->z < world->y){
                    chunk->ready = 0;
                    chunk->z = old_y+world->height*CHUNK_DEPTH;
                    chunk->regenerate = 1;
                }else if(chunk->z >= world->y+world->height*CHUNK_DEPTH){
                    chunk->ready = 0;
                    chunk->z = world->y;
                    chunk->regenerate = 1;
                }
                /* Update old chunks */
                if(chunk->x == world->x+CHUNK_WIDTH){
                    chunk->ready = 0;
                    chunk->remesh = 1;
                }else if(chunk->x == world->x+(world->width-2)*CHUNK_WIDTH){
                    chunk->ready = 0;
                    chunk->remesh = 1;
                }
                if(chunk->z == world->y+CHUNK_DEPTH){
                    chunk->ready = 0;
                    chunk->remesh = 1;
                }else if(chunk->z == world->y+(world->height-2)*CHUNK_DEPTH){
                    chunk->ready = 0;
                    chunk->remesh = 1;
                }
            }
        }
        chunk_x = (world->new_x-world->x)/CHUNK_WIDTH;
        chunk_y = (world->new_z-world->y)/CHUNK_DEPTH;
        center_x = world->width/2, center_y = world->height/2;
    }
    for(y=0;y<world->width*world->height;y++){
        chunk = &world->chunks[y];
        if(chunk->regenerate){
            chunk->ready = 0;
            chunk_generate_data(chunk, chunk->x, chunk->z, world->seed);
            chunk->remesh = 1;
            chunk->regenerate = 0;
        }
    }
    for(y=0;y<world->width*world->height;y++){
        chunk = &world->chunks[y];
        if(chunk->remesh){
            chunk_generate_model(chunk, world->texture,
                                 _modelgen_get_tile, world);
            chunk->ready = 1;
        }
    }
    world->finished = 1;
    THREAD_EXIT();
}

void world_update(World *world) {
    /* TODO: Support multiple players */
    THREAD_ID(id);
    int i;
    int update_required = 0;
    if(!world->finished) return;
    for(i=0;i<world->player_num;i++){
        if(((int)world->players[i].entity.x-world->x)/CHUNK_WIDTH !=
                world->width/2 ||
           ((int)world->players[i].entity.z-world->y)/CHUNK_DEPTH !=
                   world->height/2){
            update_required = 1;
            break;
        }
    }
    if(world->update_required || update_required){
        puts("update required");
        world->new_x = world->players[0].entity.x;
        world->new_z = world->players[0].entity.z;
        if(world->finished){
            THREAD_CREATE(id, _world_update, world);
        }
        world->update_required = 0;
    }
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

Tile world_get_tile(World *world, float x, float y, float z) {
    int i;
    Chunk *tile_chunk;
    for(i=0;i<world->width*world->height;i++){
        tile_chunk = &world->chunks[i];
        if(x >= tile_chunk->x && x < tile_chunk->x+CHUNK_WIDTH &&
           z >= tile_chunk->z && z < tile_chunk->z+CHUNK_DEPTH){
#if DEBUG_WORLD
            if(chunk_get_tile(tile_chunk,  x-tile_chunk->x, y,
                                  z-tile_chunk->z, 0, 0, 0) != T_VOID){
                gfx_render_wire_cube(x, y-CHUNK_HEIGHT/2, z, 1.01);
            }
#endif
            return chunk_get_tile(tile_chunk,  x-tile_chunk->x, y,
                                  z-tile_chunk->z, 0, 0, 0);
        }
    }
    return T_VOID;
}

void world_set_tile(World *world, Tile tile, int x, int y, int z) {
    int i, n;
    Chunk *tile_chunk;
    Chunk *neighbor_chunk;
    puts("Set tile");
    for(i=0;i<world->width*world->height;i++){
        tile_chunk = &world->chunks[i];
        if(x >= tile_chunk->x && x < tile_chunk->x+CHUNK_WIDTH &&
           z >= tile_chunk->z && z < tile_chunk->z+CHUNK_DEPTH){
            chunk_set_tile(tile_chunk, tile, x-tile_chunk->x, y,
                           z-tile_chunk->z);
            tile_chunk->remesh = 1;
            world->update_required = 1;
            break;
        }
        /* Check if we need to update a neighboring chunk */
        if(x == tile_chunk->x){
            puts("Update neighboring chunk < x");
            for(n=0;n<world->width*world->height;n++){
                neighbor_chunk = &world->chunks[n];
                if(x-1 >= neighbor_chunk->x &&
                   x-1 < neighbor_chunk->x+CHUNK_WIDTH &&
                   z >= neighbor_chunk->z && z < neighbor_chunk->z+CHUNK_DEPTH){
                    neighbor_chunk->remesh = 1;
                    printf("Chunk at %d, %d updated!\n", neighbor_chunk->x,
                           neighbor_chunk->z);
                    break;
                }
            }
        }else if(x == tile_chunk->x+CHUNK_WIDTH-1){
            puts("Update neighboring chunk > x");
            for(n=0;n<world->width*world->height;n++){
                neighbor_chunk = &world->chunks[n];
                if(x+1 >= neighbor_chunk->x &&
                   x+1 < neighbor_chunk->x+CHUNK_WIDTH &&
                   z >= neighbor_chunk->z && z < neighbor_chunk->z+CHUNK_DEPTH){
                    neighbor_chunk->remesh = 1;
                    printf("Chunk at %d, %d updated!\n", neighbor_chunk->x,
                           neighbor_chunk->z);
                    break;
                }
            }
        }
        if(z == tile_chunk->z){
            puts("Update neighboring chunk < z");
            for(n=0;n<world->width*world->height;n++){
                neighbor_chunk = &world->chunks[n];
                if(z-1 >= neighbor_chunk->z &&
                   z-1 < neighbor_chunk->z+CHUNK_DEPTH &&
                   x >= neighbor_chunk->x && x < neighbor_chunk->x+CHUNK_WIDTH){
                    neighbor_chunk->remesh = 1;
                    printf("Chunk at %d, %d updated!\n", neighbor_chunk->x,
                           neighbor_chunk->z);
                    break;
                }
            }
        }else if(z == tile_chunk->z+CHUNK_DEPTH-1){
            puts("Update neighboring chunk > z");
            for(n=0;n<world->width*world->height;n++){
                neighbor_chunk = &world->chunks[n];
                if(z+1 >= neighbor_chunk->z &&
                   z+1 < neighbor_chunk->z+CHUNK_DEPTH &&
                   x >= neighbor_chunk->x && x < neighbor_chunk->x+CHUNK_WIDTH){
                    neighbor_chunk->remesh = 1;
                    printf("Chunk at %d, %d updated!\n", neighbor_chunk->x,
                           neighbor_chunk->z);
                    break;
                }
            }
        }
    }
}

void world_update_chunk_model_at(World *world, float sx, float sz) {
    int x = (int)sx-world->x, z = (int)sz-world->y;
    int chunk_x = x/CHUNK_WIDTH, chunk_y = z/CHUNK_DEPTH;
    if(chunk_x >= 0 && chunk_x < world->width && chunk_y >= 0 &&
       chunk_y < world->height) {
        Chunk *chunk = &world->chunks[chunk_y*world->width+chunk_x];
        chunk->remesh = 1;
    }
}

int world_change_size(World *world, int width, int height) {
    Chunk *chunks;
    if(!world->finished) return 1;
    world->finished = 0;
    /* TODO: Add support for multiple players. */
    world->x -= (width-world->width)/2*CHUNK_WIDTH;
    world->y -= (height-world->height)/2*CHUNK_DEPTH;
    world->width = width;
    world->height = height;
    /* TODO: Do not reload all the chunks: keep the chunks that are already
     * loaded. */
    chunks = realloc(world->chunks, world->width*world->height*sizeof(Chunk));
    if(!chunks){
        puts("Failed to change the world size!");
        return 2;
    }
    world->chunks = chunks;
    world_init_data(world);
    world->finished = 1;
    world->update_required =  1;
    return 0;
}

void world_free(World *world) {
    free(world->players);
    world->players = NULL;
    free(world->chunks);
    world->chunks = NULL;
}


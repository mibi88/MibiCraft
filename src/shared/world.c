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

#include <shared/world.h>

#include <shared/player.h>

#include <shared/threading.h>

#include <shared/chunk_queue.h>

#include <limits.h>

int _cx, _cy;
int _ncx, _ncy;

int world_init(World *world, size_t width, size_t height, size_t player_num,
               size_t queue_num, int seed, unsigned int texture) {
    size_t i;

    world->width = width;
    world->height = height;
    world->players = malloc(sizeof(Player)*player_num);
    world->player_num = player_num;

    if(world->players == NULL){
        fputs("Player init failed!\n", stderr);
        return 1;
    }

    world->chunk_data = malloc(width*height*player_num*sizeof(Chunk));
    world->chunks = malloc(width*height*player_num*sizeof(Chunk*));
    world->queues = malloc(queue_num*sizeof(ChunkQueue));
    world->empty = malloc(width*height*player_num*sizeof(Chunk*));

    if(world->chunk_data == NULL ||
       world->chunks == NULL ||
       world->queues == NULL ||
       world->empty == NULL){
        fputs("World init failed!\n", stderr);

        free(world->chunk_data);
        world->chunk_data = NULL;

        free(world->chunks);
        world->chunks = NULL;

        free(world->queues);
        world->queues = NULL;

        free(world->players);
        world->players = NULL;

        free(world->empty);
        world->empty = NULL;

        return 1;
    }

    for(i=0;i<queue_num;i++){
        if(chunk_queue_init(world->queues+i, width*height*player_num)){
            size_t n;

            fputs("Queue init failed!\n", stderr);

            for(n=i;n--;) chunk_queue_free(world->queues+n);

            free(world->chunk_data);
            world->chunk_data = NULL;

            free(world->chunks);
            world->chunks = NULL;

            free(world->queues);
            world->queues = NULL;

            free(world->players);
            world->players = NULL;

            return 1;
        }
    }

    world->seed = seed;
    world->texture = texture;

    for(i=0;i<player_num;i++){
        player_init(world->players+i);
    }

    world_init_data(world);

    return 0;
}

Chunk *world_get_chunk(World *world, int x, int y) {
    /* FIXME */
}

void world_generate_models(World *world) {
    /* FIXME */
}

void world_generate_data(World *world) {
    /* FIXME */
}

static Tile no_surrounding(Chunk *chunk, int x, int y, int z, int rx, int ry,
                           int rz, void *vworld) {
    return T_VOID;
}

void world_init_data(World *world) {
    int x, y, pos, start;

    world->finished = 0;

    y = world->height/2;
    x = world->width/2;

    start = y*world->width+x;

    for(y=0;y<world->height;y++) {
        for(x=0;x<world->width;x++) {
            _cx = x;
            _cy = y;
            pos = y*world->width+x;
            if(pos != start){
                world->chunks[pos].ready = 0;
                world->chunks[pos].x = world->x+x*CHUNK_WIDTH;
                world->chunks[pos].z = world->y+y*CHUNK_DEPTH;
                world->chunks[pos].last_hit = NULL;
                world->chunks[pos].regenerate = 1;
            }
        }
    }

    y = world->height/2;
    x = world->width/2;

    world->chunks[start].ready = 0;
    world->chunks[start].last_hit = NULL;
    chunk_generate_data(world->chunks+start, world->x+x*CHUNK_WIDTH,
                        world->y+y*CHUNK_DEPTH, world->seed);
    chunk_generate_model(world->chunks+start, world->texture, no_surrounding,
                         world);
    world->chunks[start].regenerate = 0;
    world->chunks[start].remesh = 1;
    world->chunks[start].ready = 1;

    world->finished = 1;
    world->update_required = 1;
}

struct update_data {
    World *w;
    size_t queue_id;
};

static THREAD_CALL(update_thread, vupdate_data) {
    struct update_data *d = vupdate_data;

    /* TODO: Process a queue */

    THREAD_EXIT();
}

void world_update(World *world) {
    /* FIXME */
}

void world_render(World *world) {
    int i;

    gfx_reset_texture_transforms();
    gfx_set_texture_scale(TILE_WIDTH/(float)TEX_WIDTH,
                          TILE_HEIGHT/(float)TEX_WIDTH);

    for(i=0;i<world->width*world->height;i++) {
        if(!world->chunks[i].ready) continue;
        gfx_draw_model(&world->chunks[i].chunk_model,
                       world->chunks[i].x-0.5, -(CHUNK_HEIGHT/2)-0.5,
                       world->chunks[i].z-0.5, 0, 0, 0);
    }

    gfx_reset_texture_transforms();
}

Tile world_get_tile(World *world, float x, float y, float z) {
    int i;
    Chunk *tile_chunk;
    for(i=0;i<world->width*world->height;i++){
        tile_chunk = &world->chunks[i];
        if(x >= tile_chunk->x && x < tile_chunk->x+CHUNK_WIDTH &&
           z >= tile_chunk->z && z < tile_chunk->z+CHUNK_DEPTH &&
           !tile_chunk->regenerate){
#if DEBUG_WORLD
            if(chunk_get_tile(tile_chunk, x-tile_chunk->x, y,
                                  z-tile_chunk->z, 0, 0, 0) != T_VOID){
                gfx_set_color(0, 0, 0, 1);
                gfx_render_wire_cube(x, y-CHUNK_HEIGHT/2, z, 1.01);
            }
#endif
            return chunk_get_tile(tile_chunk, x-tile_chunk->x, y,
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
    /* TODO: Do not reload all the chunks: keep the chunks that are already
     * loaded. */
    chunks = realloc(world->chunks, width*height*sizeof(Chunk));
    if(!chunks){
        puts("Failed to change the world size!");
        return 2;
    }

    /* TODO: Add support for multiple players. */
    world->x -= (width-world->width)/2*CHUNK_WIDTH;
    world->y -= (height-world->height)/2*CHUNK_DEPTH;
    world->width = width;
    world->height = height;
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

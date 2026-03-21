/*
 * MibiCraft -- A small game in a world of cubes
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
    world->chunk_locks = malloc(width*height*player_num*sizeof(thread_lock_t));
    world->queues = malloc(queue_num*sizeof(ChunkQueue));
    world->empty = malloc(width*height*player_num*sizeof(Chunk*));

    if(world->chunk_data == NULL ||
       world->chunks == NULL ||
       world->chunk_locks == NULL ||
       world->queues == NULL ||
       world->empty == NULL){
        fputs("World init failed!\n", stderr);

        free(world->chunk_data);
        world->chunk_data = NULL;

        free(world->chunks);
        world->chunks = NULL;

        free(world->chunk_locks);
        world->chunk_locks = NULL;

        free(world->queues);
        world->queues = NULL;

        free(world->players);
        world->players = NULL;

        free(world->empty);
        world->empty = NULL;

        return 1;
    }

    world->queue_num = queue_num;

    for(i=0;i<queue_num;i++){
        if(chunk_queue_init(world->queues+i, width*height*player_num)){
            size_t n;

            fputs("Queue init failed!\n", stderr);

            free(world->chunk_data);
            world->chunk_data = NULL;

            free(world->chunks);
            world->chunks = NULL;

            free(world->chunk_locks);
            world->chunk_locks = NULL;

            free(world->queues);
            world->queues = NULL;

            free(world->players);
            world->players = NULL;

            free(world->empty);
            world->empty = NULL;

            for(n=0;n<i;n++){
                chunk_queue_free(world->queues+n);
            }

            return 1;
        }
    }

    world->seed = seed;
    world->texture = texture;

    for(i=0;i<player_num;i++){
        size_t x, y;

        player_init(world->players+i);

        for(y=0;y<height;y++){
            for(x=0;x<width;x++){
                Chunk *c = world->chunk_data+i*width*height+y*width+x;

                chunk_init(c);
                world->chunks[i*width*height+y*width+x] = c;

                THREAD_LOCK_INIT(world->chunk_locks[i*width*height+y*width+x]);
            }
        }
    }

    world_init_data(world);

    return 0;
}

void world_init_data(World *world) {
    /* FIXME */
}

struct update_data {
    World *w;
    ChunkQueue *queue;
};

static THREAD_CALL(update_thread, vupdate_data) {
    struct update_data *d = vupdate_data;

    /* TODO: Process a queue */

    THREAD_EXIT();
}

void world_update(World *world) {
    /* FIXME */
}

static Chunk *world_get_chunk(World *world, int x, int y) {
    size_t i;

    for(i=0;i<world->player_num;i++){
        /* TODO */
    }

    return NULL;
}

void world_set_tile(World *world, Tile tile, int x, int y, int z) {
    /* FIXME */
}

Tile world_get_tile(World *world, float x, float y, float z) {
    /* FIXME */
    return T_VOID;
}

int world_change_size(World *world, int width, int height) {
    /* FIXME */

    return 0;
}

void world_free(World *world) {
    size_t i;

    for(i=0;i<world->queue_num;i++){
        chunk_queue_free(world->queues+i);
    }

    free(world->chunk_data);
    world->chunk_data = NULL;

    free(world->chunks);
    world->chunks = NULL;

    free(world->chunk_locks);
    world->chunk_locks = NULL;

    free(world->queues);
    world->queues = NULL;

    free(world->players);
    world->players = NULL;

    free(world->empty);
    world->empty = NULL;

    world->queue_num = 0;
}

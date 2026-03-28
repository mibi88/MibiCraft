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
#include <math.h>

int world_init(World *world, size_t width, size_t height, size_t player_num,
               size_t queue_num, int seed, unsigned int texture) {
    size_t i;

    world->width = width;
    world->height = height;
    world->players = malloc(sizeof(Player)*player_num);
    world->player_num = player_num;

    world->stop = 0;

    if(world->players == NULL){
        fputs("Player init failed!\n", stderr);
        return 1;
    }

    world->chunk_data = malloc(width*height*player_num*sizeof(Chunk));
    world->chunks = malloc(width*height*player_num*sizeof(Chunk*));
    world->queues = malloc(queue_num*sizeof(ChunkQueue));
    world->threads = malloc(queue_num*sizeof(thread_t));
    world->thread_data = malloc(queue_num*sizeof(UpdateData));
    world->empty = malloc(width*height*player_num*sizeof(Chunk*));

    if(world->chunk_data == NULL ||
       world->chunks == NULL ||
       world->queues == NULL ||
       world->threads == NULL ||
       world->thread_data == NULL ||
       world->empty == NULL){
        fputs("World init failed!\n", stderr);

        free(world->chunk_data);
        world->chunk_data = NULL;

        free(world->chunks);
        world->chunks = NULL;

        free(world->queues);
        world->queues = NULL;

        free(world->threads);
        world->threads = NULL;

        free(world->thread_data);
        world->thread_data = NULL;

        free(world->players);
        world->players = NULL;

        free(world->empty);
        world->empty = NULL;

        return 2;
    }

    world->queue_num = queue_num;
    world->last_queue = 0;

    for(i=0;i<queue_num;i++){
        world->thread_data[i].w = NULL;
        if(chunk_queue_init(world->queues+i, width*height*player_num)){
            size_t n;

            fputs("Queue init failed!\n", stderr);

            for(n=0;n<i;n++){
                chunk_queue_free(world->queues+n);
            }

            free(world->chunk_data);
            world->chunk_data = NULL;

            free(world->chunks);
            world->chunks = NULL;

            free(world->queues);
            world->queues = NULL;

            free(world->threads);
            world->threads = NULL;

            free(world->thread_data);
            world->thread_data = NULL;

            free(world->players);
            world->players = NULL;

            free(world->empty);
            world->empty = NULL;

            return 3;
        }
    }

    world->seed = seed;
    world->texture = texture;

    for(i=0;i<player_num;i++){
        size_t n;

        player_init(world->players+i);

        for(n=0;n<player_num*width*height;n++){
            world->chunks[n] = world->chunk_data+n;

            if(chunk_init(world->chunk_data+n)){
                size_t m;

                fputs("Chunk init failed!\n", stderr);

                for(m=0;m<queue_num;m++){
                    chunk_queue_free(world->queues+m);
                }

                for(m=0;m<n;m++){
                    chunk_free(world->chunks[m]);
                }

                free(world->chunk_data);
                world->chunk_data = NULL;

                free(world->chunks);
                world->chunks = NULL;

                free(world->queues);
                world->queues = NULL;

                free(world->threads);
                world->threads = NULL;

                free(world->thread_data);
                world->thread_data = NULL;

                free(world->players);
                world->players = NULL;

                free(world->empty);
                world->empty = NULL;

                return 4;
            }
        }
    }

    world_init_data(world);

    return 0;
}

void world_update_chunk(World *world, Chunk *chunk, unsigned char flags) {
    ChunkUpdate update;

    update.chunk = chunk;
    update.flags = flags;

    chunk_queue_push(world->queues+world->last_queue, update);

    world->last_queue = (world->last_queue+1)%world->queue_num;
}

void world_update_chunk_fast(World *world, Chunk *chunk, unsigned char flags) {
    ChunkUpdate update;

    update.chunk = chunk;
    update.flags = flags;

    chunk_queue_bypass(world->queues+world->last_queue, update);

    world->last_queue = (world->last_queue+1)%world->queue_num;
}

void world_update_all(World *world, size_t player, unsigned char flags) {
    size_t b = player*world->width*world->height;

    if(world->width == world->height && (world->width&1)){
        size_t s = world->width/2;
        size_t i;

        world_update_chunk(world, world->chunks[b+s*world->width+s], flags);

        for(i=1;i<=s;i++){
            size_t n;

            for(n=0;n<i;n++){
                world_update_chunk(world,
                                   world->chunks[b+(s-i+n)*world->width+s-n],
                                   flags);
            }

            for(n=0;n<i;n++){
                world_update_chunk(world,
                                   world->chunks[b+(s+n)*world->width+s-i+n],
                                   flags);
            }

            for(n=0;n<i;n++){
                world_update_chunk(world,
                                   world->chunks[b+(s+i-n)*world->width+s+n],
                                   flags);
            }

            for(n=0;n<i;n++){
                world_update_chunk(world,
                                   world->chunks[b+(s-n)*world->width+s+i-n],
                                   flags);
            }

        }
        i--;
        for(;i--;){
            size_t n;

            for(n=0;n<i+1;n++){
                world_update_chunk(world,
                                   world->chunks[b+n*world->width+i-n],
                                   flags);
            }

            for(n=0;n<i+1;n++){
                world_update_chunk(world,
                                   world->chunks[b+(world->height-1-i+n)*
                                                 world->width+n],
                                   flags);
            }

            for(n=0;n<i+1;n++){
                world_update_chunk(world,
                                   world->chunks[b+(world->height-1-n)*
                                                 world->width+
                                                 world->width-1-i+n],
                                   flags);
            }

            for(n=0;n<i+1;n++){
                world_update_chunk(world,
                                   world->chunks[b+(i-n)*world->width+
                                                 world->width-1-n],
                                   flags);
            }
        }
    }else{
        /* TODO: Add the chunks closest to the player first */
        size_t x, y;

        for(y=0;y<world->height;y++){
            for(x=0;x<world->width;x++,b++){
                world_update_chunk(world, world->chunks[b], flags);
            }
        }
    }
}

static void world_set_chunk_positions(World *world, size_t player) {
    size_t i = 0;
    size_t x, y, cx;

    size_t px = (px = world->players[player].entity.x,
                 px -= world->width*CHUNK_WIDTH/2,
                 px-px%CHUNK_WIDTH);
    size_t py = (py = world->players[player].entity.z,
                 py -= world->height*CHUNK_DEPTH/2,
                 py-py%CHUNK_DEPTH);

    for(y=0;y<world->height;y++,py+=CHUNK_DEPTH){
        for(x=0,cx=px;x<world->width;x++,cx+=CHUNK_WIDTH,i++){
            world->chunks[i]->x = cx;
            world->chunks[i]->z = py;
        }
    }
}

void world_init_data(World *world) {
    size_t i;

    for(i=0;i<world->player_num;i++){
        world_set_chunk_positions(world, i);
        world_update_all(world, i, CU_DATA|CU_MESH);
        /*world_update_all(world, i, CU_MESH);*/
    }
}

static Tile get_tile(Chunk *ch, int x, int y, int z, int rx, int ry, int rz,
                     void *extra) {
    World *world = extra;

    long min_x, min_z;
    long ix = x+rx;
    long iy = y+ry;
    long iz = z+rz;

    if(iy < 0 || iy >= CHUNK_HEIGHT) return T_VOID;

    /* TODO: Support multiple players */
    if(ch != world->chunks[0]){
        while(THREAD_LOCK_TRYLOCK(world->chunks[0]->lock)){
            THREAD_LOCK_UNLOCK(ch->lock);
            THREAD_LOCK_LOCK(ch->lock);
        }
    }

    min_x = world->chunks[0]->x;
    min_z = world->chunks[0]->z;

    if(ch != world->chunks[0]) THREAD_LOCK_UNLOCK(world->chunks[0]->lock);

    if(ix >= min_x && ix < min_x+(long)world->width*CHUNK_WIDTH &&
       iz >= min_z && iz < min_z+(long)world->height*CHUNK_DEPTH){
        Chunk *c;

        long cx = ix;
        long cz = iz;

        Tile t;

        ix -= min_x;
        iz -= min_z;

        c = world->chunks[iz/CHUNK_DEPTH*world->width+ix/CHUNK_WIDTH];

        if(ch != c){
            while(THREAD_LOCK_TRYLOCK(c->lock)){
                THREAD_LOCK_UNLOCK(ch->lock);
                THREAD_LOCK_LOCK(ch->lock);
            }
        }

        if(!c->initialized) t = T_VOID;
        else t = c->chunk_data[cx-c->x][iy][cz-c->z];

        if(ch != c) THREAD_LOCK_UNLOCK(c->lock);

        return t;
    }

    return T_VOID;
}

static THREAD_CALL(update_thread, vupdate_data) {
    UpdateData *d = vupdate_data;
    ChunkUpdate update;

    /*puts("thread");*/

    while((update = chunk_queue_pop(d->queue)).chunk != NULL){
        if(d->w->stop) break;
        THREAD_LOCK_LOCK(update.chunk->lock);

#if 0
        printf("%d, %d\n", update.chunk->x, update.chunk->z);
#endif

        if(update.flags&CU_DATA){
            chunk_generate_data(update.chunk, update.chunk->x,
                                update.chunk->z, d->w->seed);
        }
        if(update.flags&CU_MESH) chunk_generate_model(update.chunk,
                                                      d->w->texture,
                                                      get_tile, d->w);

        THREAD_LOCK_UNLOCK(update.chunk->lock);
    }

    THREAD_EXIT();
}

void world_update(World *world) {
    size_t i;

    for(i=0;i<world->queue_num;i++){
        char e;

        if(!chunk_queue_empty(world->queues+i)){
            UpdateData *d = world->thread_data+i;

            if(world->thread_data[i].w != NULL){
                printf("join: %lu\n", world->threads[i]);
                THREAD_JOIN(world->threads[i]);
            }

            d->w = world;
            d->queue = world->queues+i;

            THREAD_CREATE(world->threads+i, update_thread, d);
                printf("create: %lu\n", world->threads[i]);
            }
    }
}

void world_set_tile(World *world, Tile tile, int x, int y, int z) {
    long min_x, min_z;
    long ix = floor(x+0.5);
    long iy = floor(y+0.5);
    long iz = floor(z+0.5);

    if(iy < 0 || iy >= CHUNK_HEIGHT) return;

    /* TODO: Support multiple players */
    THREAD_LOCK_LOCK(world->chunks[0]->lock);

    min_x = world->chunks[0]->x;
    min_z = world->chunks[0]->z;

    THREAD_LOCK_UNLOCK(world->chunks[0]->lock);

    if(ix >= min_x && ix < min_x+(long)world->width*CHUNK_WIDTH &&
       iz >= min_z && iz < min_z+(long)world->height*CHUNK_DEPTH){
        Chunk *c;

        long cx = ix;
        long cz = iz;

        ix -= min_x;
        iz -= min_z;

        c = world->chunks[iz/CHUNK_DEPTH*world->width+ix/CHUNK_WIDTH];

        THREAD_LOCK_LOCK(c->lock);

        c->chunk_data[cx-c->x][iy][cz-c->z] = tile;

        THREAD_LOCK_UNLOCK(c->lock);

        world_update_chunk_fast(world, c, CU_MESH);
    }
}

Tile world_get_tile(World *world, float x, float y, float z) {
    long min_x, min_z;
    long ix = floor(x+0.5);
    long iy = floor(y+0.5);
    long iz = floor(z+0.5);

    if(iy < 0 || iy >= CHUNK_HEIGHT) return T_VOID;

    /* TODO: Support multiple players */
    THREAD_LOCK_LOCK(world->chunks[0]->lock);

    min_x = world->chunks[0]->x;
    min_z = world->chunks[0]->z;

    THREAD_LOCK_UNLOCK(world->chunks[0]->lock);

    if(ix >= min_x && ix < min_x+(long)world->width*CHUNK_WIDTH &&
       iz >= min_z && iz < min_z+(long)world->height*CHUNK_DEPTH){
        Chunk *c;

        long cx = ix;
        long cz = iz;

        Tile t;

        ix -= min_x;
        iz -= min_z;

        c = world->chunks[iz/CHUNK_DEPTH*world->width+ix/CHUNK_WIDTH];

        THREAD_LOCK_LOCK(c->lock);

        if(!c->initialized) t = T_VOID;
        else t = c->chunk_data[cx-c->x][iy][cz-c->z];

        THREAD_LOCK_UNLOCK(c->lock);

        return t;
    }

    return T_VOID;
}

int world_change_size(World *world, int width, int height) {
    /* FIXME */

    return 0;
}

void world_free(World *world) {
    size_t i;

    world->stop = 1;
    for(i=0;i<world->queue_num;i++){
        if(world->thread_data[i].w != NULL){
            printf("join: %lu\n", world->threads[i]);
            THREAD_JOIN(world->threads[i]);
        }
    }
    world->stop = 0;

    for(i=0;i<world->queue_num;i++){
        chunk_queue_free(world->queues+i);
    }

    for(i=0;i<world->width*world->height*world->player_num;i++){
        chunk_free(world->chunk_data+i);
    }

    free(world->chunk_data);
    world->chunk_data = NULL;

    free(world->chunks);
    world->chunks = NULL;

    free(world->queues);
    world->queues = NULL;

    free(world->threads);
    world->threads = NULL;

    free(world->thread_data);
    world->thread_data = NULL;

    free(world->players);
    world->players = NULL;

    free(world->empty);
    world->empty = NULL;

    world->queue_num = 0;
}

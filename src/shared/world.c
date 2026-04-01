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

/* TODO: Do not remesh chunks that frequently. */


int world_init(World *world, size_t width, size_t height, size_t player_num,
               size_t queue_num, int seed, unsigned int texture) {
    size_t i;

    world->width = width;
    world->height = height;
    world->players = malloc(sizeof(Player)*player_num);
    world->player_num = player_num;

    world->stop = 0;

    if(THREAD_LOCK_INIT(world->last_queue_lock)){
        fputs("Failed to init last_queue_lock", stderr);

        return 1;
    }

    if(THREAD_RW_INIT(&world->chunks_lock)){
        fputs("Failed to init chunks_lock", stderr);

        THREAD_LOCK_FREE(world->last_queue_lock);

        return 1;
    }

    if(THREAD_LOCK_INIT(world->stop_lock)){
        fputs("Failed to init stop_lock", stderr);

        THREAD_LOCK_FREE(world->last_queue_lock);
        THREAD_RW_FREE(&world->chunks_lock);

        return 1;
    }

    if(world->players == NULL){
        fputs("Player init failed!\n", stderr);

        THREAD_LOCK_FREE(world->last_queue_lock);
        THREAD_RW_FREE(&world->chunks_lock);
        THREAD_LOCK_FREE(world->stop_lock);

        return 2;
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

        THREAD_LOCK_FREE(world->last_queue_lock);
        THREAD_RW_FREE(&world->chunks_lock);
        THREAD_LOCK_FREE(world->stop_lock);

        return 3;
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

            THREAD_LOCK_FREE(world->last_queue_lock);
            THREAD_RW_FREE(&world->chunks_lock);
            THREAD_LOCK_FREE(world->stop_lock);

            return 4;
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

                THREAD_LOCK_FREE(world->last_queue_lock);
                THREAD_RW_FREE(&world->chunks_lock);
                THREAD_LOCK_FREE(world->stop_lock);

                return 5;
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

    if(flags&CU_MESH){
        THREAD_LOCK_LOCK(chunk->flags_lock);
        chunk->flags |= CF_WAITMESH;
        THREAD_LOCK_UNLOCK(chunk->flags_lock);
    }

    THREAD_LOCK_LOCK(world->last_queue_lock);
    chunk_queue_push(world->queues+world->last_queue, update);

    world->last_queue = (world->last_queue+1)%world->queue_num;
    THREAD_LOCK_UNLOCK(world->last_queue_lock);
}

void world_update_chunk_fast(World *world, Chunk *chunk, unsigned char flags) {
    ChunkUpdate update;

    update.chunk = chunk;
    update.flags = flags;

    if(flags&CU_MESH){
        THREAD_LOCK_LOCK(chunk->flags_lock);
        chunk->flags |= CF_WAITMESH;
        THREAD_LOCK_UNLOCK(chunk->flags_lock);
    }

    THREAD_LOCK_LOCK(world->last_queue_lock);
    chunk_queue_bypass(world->queues+world->last_queue, update);

    world->last_queue = (world->last_queue+1)%world->queue_num;
    THREAD_LOCK_UNLOCK(world->last_queue_lock);
}

void world_update_all(World *world, size_t player, unsigned char flags) {
    size_t b = player*world->width*world->height;

    if(world->width == world->height && (world->width&1)){
        size_t s = world->width/2;
        size_t i;

        Chunk *c;

        c = world->chunks[b+s*world->width+s];

        world_update_chunk(world, c, flags);

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

    Chunk *c0;

    if(iy < 0 || iy >= CHUNK_HEIGHT) return T_VOID;

    /* TODO: Support multiple players */
    THREAD_RW_LOCK_READ(&world->chunks_lock);
    c0 = world->chunks[0];
    THREAD_RW_UNLOCK_READ(&world->chunks_lock);

    if(ch != c0) THREAD_RW_LOCK_READ(&c0->data_lock);

    min_x = c0->x;
    min_z = c0->z;

    if(ch != c0) THREAD_RW_UNLOCK_READ(&c0->data_lock);

    if(ix >= min_x && ix < min_x+(long)world->width*CHUNK_WIDTH &&
       iz >= min_z && iz < min_z+(long)world->height*CHUNK_DEPTH){
        Chunk *c;

        long cx = ix;
        long cz = iz;

        Tile t;

        ix -= min_x;
        iz -= min_z;

        THREAD_RW_LOCK_READ(&world->chunks_lock);
        c = world->chunks[iz/CHUNK_DEPTH*world->width+ix/CHUNK_WIDTH];
        THREAD_RW_UNLOCK_READ(&world->chunks_lock);

        if(ch != c) THREAD_RW_LOCK_READ(&c->data_lock);

        THREAD_LOCK_LOCK(c->flags_lock);
        if(!(c->flags&CF_INIT)) t = T_VOID;
        else t = c->chunk_data[cx-c->x][iy][cz-c->z];
        THREAD_LOCK_UNLOCK(c->flags_lock);

        if(ch != c) THREAD_RW_UNLOCK_READ(&c->data_lock);

        return t;
    }

    return T_VOID;
}

static void remesh_chunk(World *world, Chunk *chunk, long x, long z) {
    long min_x, min_z;

    Chunk *c0;

    /* TODO: Support multiple players */
    THREAD_RW_LOCK_READ(&world->chunks_lock);
    c0 = world->chunks[0];
    THREAD_RW_UNLOCK_READ(&world->chunks_lock);

    if(c0 != chunk) THREAD_RW_LOCK_READ(&c0->data_lock);

    min_x = c0->x;
    min_z = c0->z;

    if(c0 != chunk) THREAD_RW_UNLOCK_READ(&c0->data_lock);

    if(x >= min_x && x < min_x+(long)world->width*CHUNK_WIDTH &&
       z >= min_z && z < min_z+(long)world->height*CHUNK_DEPTH){
        Chunk *c;
        unsigned char flags;

        x -= min_x;
        z -= min_z;

        THREAD_RW_LOCK_READ(&world->chunks_lock);
        c = world->chunks[z/CHUNK_DEPTH*world->width+x/CHUNK_WIDTH];
        THREAD_RW_UNLOCK_READ(&world->chunks_lock);

        THREAD_LOCK_LOCK(c->flags_lock);
        flags = c->flags;
        THREAD_LOCK_UNLOCK(c->flags_lock);
        if((flags&(CF_INIT|CF_WAITMESH)) == CF_INIT){
            world_update_chunk(world, c, CU_MESH);
        }
    }
}

static THREAD_CALL(update_thread, vupdate_data) {
    UpdateData *d = vupdate_data;
    ChunkUpdate update;

    /*puts("thread");*/

    while((update = chunk_queue_pop(d->queue)).chunk != NULL){
        unsigned char stop;

#if !UNSAFE_THREADING
        THREAD_LOCK_LOCK(d->w->stop_lock);
#endif
        stop = d->w->stop;
#if !UNSAFE_THREADING
        THREAD_LOCK_UNLOCK(d->w->stop_lock);
#endif

        if(stop) break;
        THREAD_RW_LOCK_READ(&update.chunk->data_lock);

        if(update.flags&CU_DATA){
            long x = update.chunk->x;
            long z = update.chunk->z;

            THREAD_RW_UNLOCK_READ(&update.chunk->data_lock);
            THREAD_RW_LOCK_WRITE(&update.chunk->data_lock);

            chunk_generate_data(update.chunk, update.chunk->x,
                                update.chunk->z, d->w->seed);

            /* Update neighboring chunk meshes */
            remesh_chunk(d->w, update.chunk, x-1, z);
            remesh_chunk(d->w, update.chunk, x+CHUNK_WIDTH, z);
            remesh_chunk(d->w, update.chunk, x, z-1);
            remesh_chunk(d->w, update.chunk, x, z+CHUNK_DEPTH);

            THREAD_RW_UNLOCK_WRITE(&update.chunk->data_lock);
            THREAD_RW_LOCK_READ(&update.chunk->data_lock);
        }
        if(update.flags&CU_MESH){
            THREAD_RW_LOCK_WRITE(&update.chunk->mesh_lock);
            chunk_generate_model(update.chunk, d->w->texture, get_tile, d->w);
            THREAD_RW_UNLOCK_WRITE(&update.chunk->mesh_lock);
        }
        THREAD_RW_UNLOCK_READ(&update.chunk->data_lock);
    }

    THREAD_EXIT();
}

void world_update(World *world) {
    size_t i;

    for(i=0;i<world->queue_num;i++){
        if(!chunk_queue_empty(world->queues+i)){
            UpdateData *d = world->thread_data+i;

            if(world->thread_data[i].w != NULL){
                printf("join: %lu\n", i);
                THREAD_JOIN(world->threads[i]);
            }

            d->w = world;
            d->queue = world->queues+i;

            if(THREAD_CREATE(world->threads+i, update_thread, d)){
                world->thread_data[i].w = NULL;
                fprintf(stderr, "Thread %lu creation failed\n", i);
            }else{
                printf("create: %lu\n", i);
            }
        }
    }
}

#define UPDATE_NEIGHBOR(ix, iz) \
    { \
        if((ix) >= min_x && (ix) < min_x+(long)world->width*CHUNK_WIDTH && \
           (iz) >= min_z && (iz) < min_z+(long)world->height*CHUNK_DEPTH){ \
            Chunk *c; \
 \
            THREAD_RW_LOCK_READ(&world->chunks_lock); \
            c = world->chunks[((iz)-min_z)/CHUNK_DEPTH*world->width+ \
                              ((ix)-min_x)/CHUNK_WIDTH]; \
            THREAD_RW_UNLOCK_READ(&world->chunks_lock); \
            printf("n: %p -- %d, %d\n", (void*)c, c->x, c->z); \
 \
            world_update_chunk_fast(world, c, CU_MESH); \
        } \
    }

void world_set_tile(World *world, Tile tile, int x, int y, int z) {
    long min_x, min_z;
    long ix, iy, iz;

    Chunk *c0;

    ix = floor(x+0.5);
    iy = floor(y+0.5);
    iz = floor(z+0.5);

    if(iy < 0 || iy >= CHUNK_HEIGHT) return;

    /* TODO: Support multiple players */
    THREAD_RW_LOCK_READ(&world->chunks_lock);
    c0 = world->chunks[0];
    THREAD_RW_UNLOCK_READ(&world->chunks_lock);

    THREAD_RW_LOCK_READ(&c0->data_lock);

    min_x = c0->x;
    min_z = c0->z;

    THREAD_RW_UNLOCK_READ(&c0->data_lock);

    if(ix >= min_x && ix < min_x+(long)world->width*CHUNK_WIDTH &&
       iz >= min_z && iz < min_z+(long)world->height*CHUNK_DEPTH){
        Chunk *c;

        long cx = ix-min_x;
        long cz = iz-min_z;

        long tx, tz;

        THREAD_RW_LOCK_READ(&world->chunks_lock);
        c = world->chunks[cz/CHUNK_DEPTH*world->width+cx/CHUNK_WIDTH];
        THREAD_RW_UNLOCK_READ(&world->chunks_lock);

        printf("c: %p -- %d, %d\n", (void*)c, c->x, c->z);

        THREAD_RW_LOCK_READ(&c->data_lock);

        tx = ix-c->x;
        tz = iz-c->z;

        c->chunk_data[tx][iy][tz] = tile;

        THREAD_RW_UNLOCK_READ(&c->data_lock);

        world_update_chunk_fast(world, c, CU_MESH);

        if(!tx){
            UPDATE_NEIGHBOR(ix-1, iz);
        }else if(tx == CHUNK_WIDTH-1){
            UPDATE_NEIGHBOR(ix+1, iz);
        }

        if(!tz){
            UPDATE_NEIGHBOR(ix, iz-1);
        }else if(tz == CHUNK_DEPTH-1){
            UPDATE_NEIGHBOR(ix, iz+1);
        }
    }
}

Tile world_get_tile(World *world, float x, float y, float z) {
    long min_x, min_z;

    long ix, iy, iz;

    Chunk *c0;

    ix = floor(x+0.5);
    iy = floor(y+0.5);
    iz = floor(z+0.5);

    if(iy < 0 || iy >= CHUNK_HEIGHT) return T_VOID;

    THREAD_RW_LOCK_READ(&world->chunks_lock);
    c0 = world->chunks[0];
    THREAD_RW_UNLOCK_READ(&world->chunks_lock);

    /* TODO: Support multiple players */
    THREAD_RW_LOCK_READ(&c0->data_lock);

    min_x = c0->x;
    min_z = c0->z;

    THREAD_RW_UNLOCK_READ(&c0->data_lock);

    if(ix >= min_x && ix < min_x+(long)world->width*CHUNK_WIDTH &&
       iz >= min_z && iz < min_z+(long)world->height*CHUNK_DEPTH){
        Chunk *c;

        long cx = ix;
        long cz = iz;

        Tile t;

        ix -= min_x;
        iz -= min_z;

        THREAD_RW_LOCK_READ(&world->chunks_lock);
        c = world->chunks[iz/CHUNK_DEPTH*world->width+ix/CHUNK_WIDTH];
        THREAD_RW_UNLOCK_READ(&world->chunks_lock);

        THREAD_RW_LOCK_READ(&c->data_lock);

        THREAD_LOCK_LOCK(c->flags_lock);
        if(!(c->flags&CF_INIT)) t = T_VOID;
        else t = c->chunk_data[cx-c->x][iy][cz-c->z];
        THREAD_LOCK_UNLOCK(c->flags_lock);

        THREAD_RW_UNLOCK_READ(&c->data_lock);

        return t;
    }

    return T_VOID;
}

static void stop_threads(World *world) {
    size_t i;

#if !UNSAFE_THREADING
    THREAD_LOCK_LOCK(world->stop_lock);
#endif
    world->stop = 1;
#if !UNSAFE_THREADING
    THREAD_LOCK_UNLOCK(world->stop_lock);
#endif
    for(i=0;i<world->queue_num;i++){
        if(world->thread_data[i].w != NULL){
            printf("join: %lu\n", i);
            THREAD_JOIN(world->threads[i]);
            world->thread_data[i].w = NULL;
        }
    }
#if !UNSAFE_THREADING
    THREAD_LOCK_LOCK(world->stop_lock);
#endif
    world->stop = 0;
#if !UNSAFE_THREADING
    THREAD_LOCK_UNLOCK(world->stop_lock);
#endif
}

int world_change_size(World *world, size_t width, size_t height) {
    void *ptr;
    size_t i;

    /* TODO: Only reload the chunks that need to be reloaded. */

    stop_threads(world);

    for(i=0;i<world->queue_num;i++){
        chunk_queue_clear(world->queues+i);
    }

    for(i=0;i<world->width*world->height*world->player_num;i++){
        chunk_free(world->chunk_data+i);
    }

    ptr = realloc(world->chunk_data, width*height*world->player_num*
                                     sizeof(Chunk));
    if(ptr == NULL){
        return 1;
    }
    world->chunk_data = ptr;

    ptr = realloc(world->chunks,
                  width*height*world->player_num*sizeof(Chunk*));
    if(ptr == NULL){
        ptr = realloc(world->chunk_data, world->width*world->height*
                                         world->player_num*sizeof(Chunk));
        if(ptr != NULL) world->chunk_data = ptr;

        return 2;
    }
    world->chunks = ptr;

    ptr = realloc(world->empty, width*height*world->player_num*sizeof(Chunk*));
    if(ptr == NULL){
        ptr = realloc(world->chunk_data, world->width*world->height*
                                         world->player_num*sizeof(Chunk));
        if(ptr != NULL) world->chunk_data = ptr;

        ptr = realloc(world->chunks, world->width*world->height*
                                     world->player_num*sizeof(Chunk*));
        if(ptr != NULL) world->chunks = ptr;

        return 3;
    }
    world->empty = ptr;

    for(i=0;i<world->queue_num;i++){
        if(chunk_queue_resize(world->queues+i,
                              width*height*world->player_num)){
            size_t n;

            ptr = realloc(world->chunk_data, world->width*world->height*
                                             world->player_num*sizeof(Chunk));
            if(ptr != NULL) world->chunk_data = ptr;

            ptr = realloc(world->chunks, world->width*world->height*
                                         world->player_num*sizeof(Chunk*));
            if(ptr != NULL) world->chunks = ptr;

            ptr = realloc(world->empty, world->width*world->height*
                                        world->player_num*sizeof(Chunk*));
            if(ptr != NULL) world->empty = ptr;

            for(n=i;n--;){
                chunk_queue_resize(world->queues+n, world->width*world->height*
                                   world->player_num);
            }
            return 4;
        }
    }

    for(i=0;i<world->player_num*width*height;i++){
        world->chunks[i] = world->chunk_data+i;
        if(chunk_init(world->chunk_data+i)){
            size_t n;

            for(n=i;n--;){
                chunk_free(world->chunk_data+n);
            }

            ptr = realloc(world->chunk_data, world->width*world->height*
                                             world->player_num*
                                             sizeof(Chunk));
            if(ptr != NULL) world->chunk_data = ptr;

            ptr = realloc(world->chunks, world->width*world->height*
                                         world->player_num*sizeof(Chunk*));
            if(ptr != NULL) world->chunks = ptr;

            ptr = realloc(world->empty, world->width*world->height*
                                        world->player_num*sizeof(Chunk*));
            if(ptr != NULL) world->empty = ptr;

            for(n=0;n<world->queue_num;n++){
                chunk_queue_resize(world->queues+n,
                                   world->width*world->height*
                                   world->player_num);
            }

            return 4;
        }
    }

    printf("%ld, %ld -- %ld, %ld\n", world->width, world->height,
                                     width, height);

    world->width = width;
    world->height = height;

    /* TODO: Only update chunks that need to be updated. */
    world_init_data(world);

    puts("done");

    return 0;
}

void world_free(World *world) {
    size_t i;

    stop_threads(world);

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

    THREAD_LOCK_FREE(world->last_queue_lock);
    THREAD_RW_FREE(&world->chunks_lock);
    THREAD_LOCK_FREE(world->stop_lock);
}

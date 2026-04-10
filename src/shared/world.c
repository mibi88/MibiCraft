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

/* TODO:  Do not remesh chunks that frequently. */

/* XXX:   Is there a risk that the queues are full because of my new -- since I
 *        concieved this system with queues -- way of handling remeshing? */
/* FIXME: Chunks that should've been remeshed didn't got remeshed when
 *        neighbor generated when the world scrolls. */
/* FIXME: AddressSanitizer reporting an out of bounds write in world_scroll.
 *        2026-04-07: Fixed a memory error when scrolling towards +X.
 */
/* FIXME: SEGV because getting an empty chunk failed in world_scroll (when
 *        using a single thread and sleeping during 100 ms on every chunk
 *        update to slow it down). */
/* FIXME: Some chunks are sometimes missing. */

#define ABS(x) ((x) < 0 ? -(x) : (x))

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

#if UNSAFE_SCROLLING
    if(THREAD_RW_INIT(&world->chunks_lock)){
        fputs("Failed to init chunks_lock", stderr);

        THREAD_LOCK_FREE(world->last_queue_lock);

        return 1;
    }
#endif

    if(THREAD_LOCK_INIT(world->stop_lock)){
        fputs("Failed to init stop_lock", stderr);

        THREAD_LOCK_FREE(world->last_queue_lock);
#if UNSAFE_SCROLLING
        THREAD_RW_FREE(&world->chunks_lock);
#endif

        return 1;
    }

    if(world->players == NULL){
        fputs("Player init failed!\n", stderr);

        THREAD_LOCK_FREE(world->last_queue_lock);
#if UNSAFE_SCROLLING
        THREAD_RW_FREE(&world->chunks_lock);
#endif
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
#if UNSAFE_SCROLLING
        THREAD_RW_FREE(&world->chunks_lock);
#endif
        THREAD_LOCK_FREE(world->stop_lock);

        return 3;
    }

    world->queue_num = queue_num;
    world->last_queue = 0;

    for(i=0;i<queue_num;i++){
        world->thread_data[i].w = NULL;
        world->thread_data[i].finished = 1;
        /* TODO: Remove this lock when UNSAFE_THREADING is non-zero. */
        if(THREAD_LOCK_INIT(world->thread_data[i].finished_lock)){
            size_t n;

            fputs("thread_data finished_lock init failed!\n", stderr);

            for(n=0;n<i;n++){
                THREAD_LOCK_FREE(world->thread_data[n].finished_lock);
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
#if UNSAFE_SCROLLING
            THREAD_RW_FREE(&world->chunks_lock);
#endif
            THREAD_LOCK_FREE(world->stop_lock);

            return 4;
        }

        if(chunk_queue_init(world->queues+i, width*height*player_num)){
            size_t n;

            fputs("Queue init failed!\n", stderr);

            THREAD_LOCK_FREE(world->thread_data[i].finished_lock);

            for(n=0;n<i;n++){
                THREAD_LOCK_FREE(world->thread_data[n].finished_lock);
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
#if UNSAFE_SCROLLING
            THREAD_RW_FREE(&world->chunks_lock);
#endif
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
#if UNSAFE_SCROLLING
                THREAD_RW_FREE(&world->chunks_lock);
#endif
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

    unsigned char old_flags;
    unsigned char flags_update = 0;

    update.chunk = chunk;
    update.flags = flags;

    if(flags&CU_MESH){
        THREAD_LOCK_LOCK(chunk->flags_lock);
        old_flags = chunk->flags;
        chunk->flags |= CF_WAITMESH;
        flags_update = 1;
        THREAD_LOCK_UNLOCK(chunk->flags_lock);
    }

    THREAD_LOCK_LOCK(world->last_queue_lock);
    if(chunk_queue_push(world->queues+world->last_queue, update)){
        THREAD_LOCK_LOCK(chunk->flags_lock);
        if(flags_update) chunk->flags = old_flags;
        THREAD_LOCK_UNLOCK(chunk->flags_lock);
    }

    world->last_queue = (world->last_queue+1)%world->queue_num;
    THREAD_LOCK_UNLOCK(world->last_queue_lock);
}

void world_update_chunk_fast(World *world, Chunk *chunk, unsigned char flags) {
    ChunkUpdate update;

    unsigned char old_flags;
    unsigned char flags_update = 0;

    update.chunk = chunk;
    update.flags = flags;

    if(flags&CU_MESH){
        THREAD_LOCK_LOCK(chunk->flags_lock);
        old_flags = chunk->flags;
        chunk->flags |= CF_WAITMESH;
        flags_update = 1;
        THREAD_LOCK_UNLOCK(chunk->flags_lock);
    }

    THREAD_LOCK_LOCK(world->last_queue_lock);
    if(chunk_queue_bypass(world->queues+world->last_queue, update)){
        THREAD_LOCK_LOCK(chunk->flags_lock);
        if(flags_update) chunk->flags = old_flags;
        THREAD_LOCK_UNLOCK(chunk->flags_lock);
    }

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

static long world_get_x(World *world, size_t player) {
    /* TODO: Make something cleaner */

    long px = floor(world->players[player].entity.x);
    px -= CHUNK_WIDTH/2;
    px = (px-px%CHUNK_WIDTH);
    if(world->players[player].entity.x > 0) px += CHUNK_WIDTH;
    px -= world->width*CHUNK_WIDTH/2;

    return px;
}

static long world_get_z(World *world, size_t player) {
    /* TODO: Make something cleaner */

    long pz = floor(world->players[player].entity.z);
    pz -= CHUNK_DEPTH/2;
    pz = (pz-pz%CHUNK_DEPTH);
    if(world->players[player].entity.z > 0) pz += CHUNK_DEPTH;
    pz -= world->height*CHUNK_DEPTH/2;

    return pz;
}

static void world_set_chunk_positions(World *world, size_t player) {
    size_t i = 0;
    size_t x, z;

    long cx;

    long px, pz;

    px = world_get_x(world, player);
    pz = world_get_z(world, player);

    printf("player: %lu -- x: %f, z: %f -- px: %ld, pz: %ld\n", player,
           world->players[player].entity.x, world->players[player].entity.z,
           px, pz);

    for(z=0;z<world->height;z++,pz+=CHUNK_DEPTH){
        for(x=0,cx=px;x<world->width;x++,cx+=CHUNK_WIDTH,i++){
            world->chunks[i]->x = cx;
            world->chunks[i]->z = pz;
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

    THREAD_RW_UNLOCK_READ(&ch->data_lock);
    THREAD_RW_UNLOCK_WRITE(&ch->mesh_lock);

    /* TODO: Support multiple players */
#if UNSAFE_SCROLLING
    THREAD_RW_LOCK_READ(&world->chunks_lock);
#endif
    c0 = world->chunks[0];
#if UNSAFE_SCROLLING
    THREAD_RW_UNLOCK_READ(&world->chunks_lock);
#endif

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

#if UNSAFE_SCROLLING
        THREAD_RW_LOCK_READ(&world->chunks_lock);
#endif
        c = world->chunks[iz/CHUNK_DEPTH*world->width+ix/CHUNK_WIDTH];
#if UNSAFE_SCROLLING
        THREAD_RW_UNLOCK_READ(&world->chunks_lock);
#endif

        THREAD_RW_LOCK_READ(&c->data_lock);

        THREAD_LOCK_LOCK(c->flags_lock);
        if(!(c->flags&CF_INIT)) t = T_VOID;
        else t = c->chunk_data[cx-c->x][iy][cz-c->z];
        THREAD_LOCK_UNLOCK(c->flags_lock);

        THREAD_RW_UNLOCK_READ(&c->data_lock);

        THREAD_RW_LOCK_READ(&ch->data_lock);
        THREAD_RW_LOCK_WRITE(&ch->mesh_lock);
        return t;
    }

    THREAD_RW_LOCK_READ(&ch->data_lock);
    THREAD_RW_LOCK_WRITE(&ch->mesh_lock);
    return T_VOID;
}

static void remesh_chunk(World *world, Chunk *chunk, long x, long z) {
    long min_x, min_z;

    Chunk *c0;

    THREAD_RW_UNLOCK_WRITE(&chunk->data_lock);

    /* TODO: Support multiple players */
#if UNSAFE_SCROLLING
    THREAD_RW_LOCK_READ(&world->chunks_lock);
#endif
    c0 = world->chunks[0];
#if UNSAFE_SCROLLING
    THREAD_RW_UNLOCK_READ(&world->chunks_lock);
#endif

    THREAD_RW_LOCK_READ(&c0->data_lock);

    min_x = c0->x;
    min_z = c0->z;

    THREAD_RW_UNLOCK_READ(&c0->data_lock);

    if(x >= min_x && x < min_x+(long)world->width*CHUNK_WIDTH &&
       z >= min_z && z < min_z+(long)world->height*CHUNK_DEPTH){
        Chunk *c;
        unsigned char flags;

        unsigned char n = 0;
        unsigned char cn;

        x -= min_x;
        z -= min_z;

#if UNSAFE_SCROLLING
        THREAD_RW_LOCK_READ(&world->chunks_lock);
#endif
        c = world->chunks[z/CHUNK_DEPTH*world->width+x/CHUNK_WIDTH];
#if UNSAFE_SCROLLING
        THREAD_RW_UNLOCK_READ(&world->chunks_lock);
#endif

        THREAD_LOCK_LOCK(c->flags_lock);
        flags = c->flags;
        THREAD_LOCK_UNLOCK(c->flags_lock);

        THREAD_RW_LOCK_READ(&c->data_lock);

        /* Calculate the number of neighbors */
        if(c->x > min_x) n++;
        if(c->x < min_x+(long)world->width*CHUNK_WIDTH-CHUNK_WIDTH) n++;
        if(c->z > min_z) n++;
        if(c->z < min_z+(long)world->height*CHUNK_DEPTH-CHUNK_DEPTH) n++;

        THREAD_RW_UNLOCK_READ(&c->data_lock);

        THREAD_LOCK_LOCK(c->flags_lock);
        cn = (++c->generated_neighbors);
        flags = c->flags;
        THREAD_LOCK_UNLOCK(c->flags_lock);
        if(cn >= n && (flags&(CF_INIT|CF_WAITMESH)) == CF_INIT){
            world_update_chunk(world, c, CU_MESH);
        }
    }
    THREAD_RW_LOCK_WRITE(&chunk->data_lock);
}

static THREAD_CALL(update_thread, vupdate_data) {
    UpdateData *d = vupdate_data;
    ChunkUpdate update;

    unsigned char flags;

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

#if DEBUG_REMESHING
            {
                size_t x;
                size_t z;

                size_t i;

                World *world = d->w;

                puts("-UPDATE-");

                for(i=0,z=0;z<world->height;z++){
                    for(x=0;x<world->width;x++,i++){
                        Chunk *c;

                        c = world->chunks[i];

                        THREAD_LOCK_LOCK(c->flags_lock);
                        printf(c->flags&CF_INIT ? c == update.chunk ? "[%d]" : " %d " : " - ",
                               c->generated_neighbors);

                        THREAD_LOCK_UNLOCK(c->flags_lock);
                    }
                    fputc('\n', stdout);
                }
                puts("--------");
            }
#endif

            THREAD_RW_UNLOCK_WRITE(&update.chunk->data_lock);
            THREAD_RW_LOCK_READ(&update.chunk->data_lock);
        }

        THREAD_LOCK_LOCK(update.chunk->flags_lock);
        flags = update.chunk->flags;
        THREAD_LOCK_UNLOCK(update.chunk->flags_lock);

        if((update.flags&CU_MESH) && (flags&CF_INIT)){
            THREAD_RW_LOCK_WRITE(&update.chunk->mesh_lock);
            chunk_generate_model(update.chunk, d->w->texture, get_tile, d->w);
            THREAD_RW_UNLOCK_WRITE(&update.chunk->mesh_lock);
        }
        THREAD_RW_UNLOCK_READ(&update.chunk->data_lock);
    }

    THREAD_LOCK_LOCK(d->finished_lock);
    d->finished = 1;
    THREAD_LOCK_UNLOCK(d->finished_lock);

    THREAD_EXIT();
}

static int mark_as_empty(World *world, Chunk *c) {
    THREAD_LOCK_LOCK(world->empty_lock);

    if(world->empty_chunks >= world->width*world->height*world->player_num){
        THREAD_LOCK_UNLOCK(world->empty_lock);

        return 1;
    }

    world->empty[world->empty_chunks++] = c;

    THREAD_LOCK_UNLOCK(world->empty_lock);

    return 0;
}

static Chunk *get_empty_chunk(World *world) {
    Chunk *c = NULL;

    THREAD_LOCK_LOCK(world->empty_lock);

    if(world->empty_chunks) c = world->empty[--world->empty_chunks];

    THREAD_LOCK_UNLOCK(world->empty_lock);

    return c;
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
#if DEBUG_THREADING
            printf("join: %lu\n", i);
#endif
            THREAD_JOIN(world->threads[i]);
            world->thread_data[i].w = NULL;
        }
        world->thread_data[i].finished = 1;
    }
#if !UNSAFE_THREADING
    THREAD_LOCK_LOCK(world->stop_lock);
#endif
    world->stop = 0;
#if !UNSAFE_THREADING
    THREAD_LOCK_UNLOCK(world->stop_lock);
#endif
}

static void world_scroll(World *world, size_t player) {
    long x, z;
    long nx, nz;

    Chunk *c0;

    /* TODO: Support multiple players */

#if UNSAFE_SCROLLING
    THREAD_RW_LOCK_READ(&world->chunks_lock);
#endif
    c0 = world->chunks[0];
#if UNSAFE_SCROLLING
    THREAD_RW_UNLOCK_READ(&world->chunks_lock);
#endif

    THREAD_RW_LOCK_READ(&c0->data_lock);
    x = c0->x;
    z = c0->z;
    THREAD_RW_UNLOCK_READ(&c0->data_lock);

    nx = world_get_x(world, player);
    nz = world_get_z(world, player);

    if(nx != x || nz != z){
        long dx, dz;

#if !UNSAFE_SCROLLING
        /* XXX: Is there a more elegant solution than stopping all the
         *      threads?
         *
         * When I was not stopping the threads, I had the following issues:
         *   -  A SEGV in chunk_generate_model when getting an invalid tile
         *      value with get_tile (probably because of an out of bounds read)
         *      while scrolling the world. It was probably because I was
         *      unlocking world->chunks_lock while performing the world
         *      scrolling to avoid a deadlock. Thus when get_tile was called,
         *      not all chunks were adjacent to one another.
         *  -   There was thus a risk that some chunk meshes were incorrect
         *      because I wasn't locking world->chunks_lock during the whole
         *      world scrolling operation in world_scroll, as well as in
         *      get_tile and remesh_chunk to avoid deadlocks.
         */
        stop_threads(world);
#endif

#if UNSAFE_SCROLLING
        THREAD_RW_LOCK_WRITE(&world->chunks_lock);
#endif

#if DEBUG_WORLD_SCROLL
        printf("x: %ld, z: %ld -- nx: %ld, nz: %ld\n", x, z, nx, nz);
#endif

        dx = (nx-x)/CHUNK_WIDTH;
        dz = (nz-z)/CHUNK_DEPTH;

        if((size_t)ABS(dx) >= world->width ||
           (size_t)ABS(dz) >= world->height){

            goto RESET;
        }

#if DEBUG_WORLD_SCROLL
        printf("dx: %ld, dz: %ld\n", dx, dz);
#endif

        /* TODO: Handle having both dx != 0 and dy != 0 better: currently
         *       we are moving chunks in world->chunks uselessly in this case.
         */

        /* Move the world on the Z axis */
        if(dz < 0){
            long nz = z+dz*CHUNK_DEPTH;
            size_t i;
            long lz;

#if DEBUG_WORLD_SCROLL
            puts("Moving the world towards -Z");
#endif

            for(i=(world->height+dz)*world->width;
                i<world->width*world->height;
                i++){
#if DEBUG_WORLD_SCROLL
                printf("%lu -- Remove chunk at %d, %d\n", i,
                       world->chunks[i]->x, world->chunks[i]->z);
#endif

                if(mark_as_empty(world, world->chunks[i])){
                    fputs("World movement error!\n", stderr);

#if UNSAFE_SCROLLING
                    THREAD_RW_UNLOCK_WRITE(&world->chunks_lock);
#endif

                    goto RESET;
                }
            }

#if DEBUG_WORLD_SCROLL
            printf("memmove from 0 to %lu of %lu chunks\n", -dz*world->width,
                   (world->height+dz)*world->width);
#endif

            /* FIXME: An overflow happens here sometimes.
             * It happened to me with a NPROC of 4 and a render distance of 4,
             * with
             * x: -20, y: -20 and nx: -20, ny: -28
             * or with
             * x: 20, z: -4 and nx: 20, nz: -12.
             */
            memmove(world->chunks+-dz*world->width, world->chunks,
                    (world->height+dz)*world->width*sizeof(Chunk*));

            i = 0;

            for(lz=0;lz<-dz;lz++){
                size_t px = x;
                size_t x;

                for(x=0;x<world->width;x++,px+=CHUNK_WIDTH,i++){
                    Chunk *c;

                    if((c = get_empty_chunk(world)) != NULL){
                        world->chunks[i] = c;

                        THREAD_LOCK_LOCK(c->flags_lock);

                        c->flags &= ~CF_INIT;

                        /* TODO: Do something cleaner. */
                        c->generated_neighbors = 4;

                        THREAD_LOCK_UNLOCK(c->flags_lock);

#if UNSAFE_SCROLLING
                        THREAD_RW_UNLOCK_WRITE(&world->chunks_lock);
#endif
                        THREAD_RW_LOCK_WRITE(&c->data_lock);
                        c->x = px;
                        c->z = nz;
                        c->chunk_model.triangles = 0;
#if DEBUG_WORLD_SCROLL
                        printf("%lu -- New chunk at: %d, %d\n", i,
                               c->x, c->z);
#endif

                        THREAD_RW_UNLOCK_WRITE(&c->data_lock);
#if UNSAFE_SCROLLING
                        THREAD_RW_LOCK_WRITE(&world->chunks_lock);
#endif

                        world_update_chunk(world, c, CU_MESH|CU_DATA);
                    }else{
                        fputs("Got no next chunk\n", stderr);

                        goto RESET;
                    }

                }
            }
        }else if(dz > 0){
            long nz = nz;
            size_t i;

            size_t lz;

#if DEBUG_WORLD_SCROLL
            puts("Moving the world towards +Z");
#endif

            for(i=0;i<world->width*(size_t)dz;i++){
#if DEBUG_WORLD_SCROLL
                printf("%lu -- Remove chunk at %d, %d\n", i,
                       world->chunks[i]->x, world->chunks[i]->z);
#endif

                if(mark_as_empty(world, world->chunks[i])){
                    fputs("World movement error!\n", stderr);

#if UNSAFE_SCROLLING
                    THREAD_RW_UNLOCK_WRITE(&world->chunks_lock);
#endif

                    goto RESET;
                }
            }

            i = (world->height-dz)*world->width;

#if DEBUG_WORLD_SCROLL
            printf("memmove from %lu to 0 of %lu chunks\n", dz*world->width,
                   i);
#endif

            memmove(world->chunks, world->chunks+dz*world->width,
                    i*sizeof(Chunk*));

            nz = z+world->height*CHUNK_DEPTH;
            for(lz=world->height-dz;lz < world->height;lz++,nz+=CHUNK_DEPTH){
                size_t px = x;
                size_t x;

                for(x=0;x<world->width;x++,px+=CHUNK_WIDTH,i++){
                    Chunk *c;

                    if((c = get_empty_chunk(world)) != NULL){
                        world->chunks[i] = c;

                        THREAD_LOCK_LOCK(c->flags_lock);

                        c->flags &= ~CF_INIT;

                        /* TODO: Do something cleaner. */
                        c->generated_neighbors = 4;

                        THREAD_LOCK_UNLOCK(c->flags_lock);

#if UNSAFE_SCROLLING
                        THREAD_RW_UNLOCK_WRITE(&world->chunks_lock);
#endif
                        THREAD_RW_LOCK_WRITE(&c->data_lock);
                        c->x = px;
                        c->z = nz;
                        c->chunk_model.triangles = 0;
#if DEBUG_WORLD_SCROLL
                        printf("%lu -- New chunk at: %d, %d\n", i,
                               c->x, c->z);
#endif

                        THREAD_RW_UNLOCK_WRITE(&c->data_lock);
#if UNSAFE_SCROLLING
                        THREAD_RW_LOCK_WRITE(&world->chunks_lock);
#endif

                        world_update_chunk(world, c, CU_MESH|CU_DATA);
                    }else{
                        fputs("Got no next chunk\n", stderr);

                        goto RESET;
                    }
                }
            }
        }

        /* Move the world on the X axis */
        if(dx < 0){
            size_t lz;
            size_t i = 0;

#if DEBUG_WORLD_SCROLL
            puts("Moving the world towards -X");
#endif

            for(lz=0;lz<world->height;lz++,nz+=CHUNK_DEPTH){
                long n;

                size_t px = nx;

                for(n=world->width+dx;(size_t)n<world->width;n++){
#if DEBUG_WORLD_SCROLL
                    printf("%lu -- Remove chunk at %d, %d\n", i+n,
                           world->chunks[i+n]->x, world->chunks[i+n]->z);
#endif

                    if(mark_as_empty(world, world->chunks[i+n])){
                        fputs("World movement error!\n", stderr);

#if UNSAFE_SCROLLING
                        THREAD_RW_UNLOCK_WRITE(&world->chunks_lock);
#endif

                        goto RESET;
                    }
                }

#if DEBUG_WORLD_SCROLL
                printf("memmove from %lu to %lu of %lu chunks\n", i, i-dx,
                       world->width+dx);
#endif
                memmove(world->chunks+i-dx, world->chunks+i,
                        (world->width+dx)*sizeof(Chunk*));

                for(n=0;n<-dx;n++,i++,px+=CHUNK_WIDTH){
                    Chunk *c;

                    if((c = get_empty_chunk(world)) != NULL){
                        world->chunks[i] = c;

                        THREAD_LOCK_LOCK(c->flags_lock);

                        c->flags &= ~CF_INIT;

                        /* TODO: Do something cleaner. */
                        c->generated_neighbors = 4;

                        THREAD_LOCK_UNLOCK(c->flags_lock);

#if UNSAFE_SCROLLING
                        THREAD_RW_UNLOCK_WRITE(&world->chunks_lock);
#endif
                        THREAD_RW_LOCK_WRITE(&c->data_lock);
                        c->x = px;
                        c->z = nz;
                        c->chunk_model.triangles = 0;
#if DEBUG_WORLD_SCROLL
                        printf("%lu -- New chunk at: %d, %d\n", i,
                               c->x, c->z);
#endif
                        THREAD_RW_UNLOCK_WRITE(&c->data_lock);
#if UNSAFE_SCROLLING
                        THREAD_RW_LOCK_WRITE(&world->chunks_lock);
#endif
                        world_update_chunk(world, c, CU_MESH|CU_DATA);
                    }else{
                        fputs("Got no next chunk\n", stderr);

                        goto RESET;
                    }
                }

                i += world->width+dx;
            }
        }else if(dx > 0){
            size_t lz;
            size_t i = 0;

#if DEBUG_WORLD_SCROLL
            puts("Moving the world towards +X");
#endif

            for(lz=0;lz<world->height;lz++,nz+=CHUNK_DEPTH){
                long n;

                size_t px;

                for(n=0;n<dx;n++,i++){
#if DEBUG_WORLD_SCROLL
                    printf("%lu -- Remove chunk at %d, %d\n", i,
                           world->chunks[i]->x, world->chunks[i]->z);
#endif
                    if(mark_as_empty(world, world->chunks[i])){
                        fputs("World movement error!\n", stderr);

#if UNSAFE_SCROLLING
                        THREAD_RW_UNLOCK_WRITE(&world->chunks_lock);
#endif

                        goto RESET;
                    }
                }

#if DEBUG_WORLD_SCROLL
                printf("memmove from %lu to %lu of %lu chunks\n", i, i-dx,
                       world->width-dx);
#endif
                memmove(world->chunks+i-dx, world->chunks+i,
                        (world->width-dx)*sizeof(Chunk*));

                i += world->width-dx-dx;

                px = x+world->width*CHUNK_WIDTH;

                for(n=world->width-dx;
                    (size_t)n<world->width;
                    n++,i++,px+=CHUNK_WIDTH){
                    Chunk *c;

                    if((c = get_empty_chunk(world)) != NULL){
                        world->chunks[i] = c;

                        THREAD_LOCK_LOCK(c->flags_lock);

                        c->flags &= ~CF_INIT;

                        /* TODO: Do something cleaner. */
                        c->generated_neighbors = 4;

                        THREAD_LOCK_UNLOCK(c->flags_lock);

#if UNSAFE_SCROLLING
                        THREAD_RW_UNLOCK_WRITE(&world->chunks_lock);
#endif
                        THREAD_RW_LOCK_WRITE(&c->data_lock);
                        c->x = px;
                        c->z = nz;
                        c->chunk_model.triangles = 0;
#if DEBUG_WORLD_SCROLL
                        printf("%lu -- New chunk at: %d, %d\n", i,
                               c->x, c->z);
#endif
                        THREAD_RW_UNLOCK_WRITE(&c->data_lock);
#if UNSAFE_SCROLLING
                        THREAD_RW_LOCK_WRITE(&world->chunks_lock);
#endif

                        world_update_chunk(world, c, CU_MESH|CU_DATA);
                    }else{
                        fputs("Got no next chunk\n", stderr);

                        goto RESET;
                    }
                }
            }
        }

#if PRINT_CHUNK_POS
        {
            size_t i;

            puts("Chunk coordinates:");
            for(i=0;i<world->width*world->height;i++){
                printf("%d, %d\n", world->chunks[i]->x, world->chunks[i]->z);
            }
            puts("------------------");
        }
#endif

#if UNSAFE_SCROLLING
        THREAD_RW_UNLOCK_WRITE(&world->chunks_lock);
#endif
    }

    return;

RESET:

    nx = world_get_x(world, player);
    nz = world_get_z(world, player);

    {
        size_t x, z;
        size_t i = 0;

        long px;

        for(z=0;z<world->height;z++,nz+=CHUNK_DEPTH){
            for(x=0,px=nx;x<world->width;x++,i++,px+=CHUNK_WIDTH){
                Chunk *c;

                c = world->chunk_data+i;

#if UNSAFE_SCROLLING
                THREAD_RW_UNLOCK_WRITE(&world->chunks_lock);
#endif
                THREAD_RW_LOCK_WRITE(&c->data_lock);
                c->x = px;
                c->z = nz;
                THREAD_RW_UNLOCK_WRITE(&c->data_lock);
#if UNSAFE_SCROLLING
                THREAD_RW_LOCK_WRITE(&world->chunks_lock);
#endif
                world->chunks[i] = c;

                world_update_chunk(world, world->chunks[i], CU_DATA|CU_MESH);
            }
        }

#if UNSAFE_SCROLLING
        THREAD_RW_UNLOCK_WRITE(&world->chunks_lock);
#endif
    }
}

void world_update(World *world) {
    size_t i;

    for(i=0;i<world->queue_num;i++){
        if(!chunk_queue_empty(world->queues+i)){
            UpdateData *d = world->thread_data+i;

            unsigned char finished;

            THREAD_LOCK_LOCK(d->finished_lock);
            finished = d->finished;
            THREAD_LOCK_UNLOCK(d->finished_lock);

            if(!finished) continue;

            if(world->thread_data[i].w != NULL){
#if DEBUG_THREADING
                printf("join: %lu\n", i);
#endif
                THREAD_JOIN(world->threads[i]);
            }

            d->w = world;
            d->queue = world->queues+i;

            d->finished = 0;

            if(THREAD_CREATE(world->threads+i, update_thread, d)){
                world->thread_data[i].w = NULL;
                world->thread_data[i].finished = 1;
                fprintf(stderr, "Thread %lu creation failed\n", i);
            }else{
#if DEBUG_THREADING
                printf("create: %lu\n", i);
#endif
            }
        }
    }

    for(i=0;i<world->player_num;i++){
        world_scroll(world, i);
    }
}

#if UNSAFE_SCROLLING
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
#else
#define UPDATE_NEIGHBOR(ix, iz) \
    { \
        if((ix) >= min_x && (ix) < min_x+(long)world->width*CHUNK_WIDTH && \
           (iz) >= min_z && (iz) < min_z+(long)world->height*CHUNK_DEPTH){ \
            Chunk *c; \
 \
            c = world->chunks[((iz)-min_z)/CHUNK_DEPTH*world->width+ \
                              ((ix)-min_x)/CHUNK_WIDTH]; \
            printf("n: %p -- %d, %d\n", (void*)c, c->x, c->z); \
 \
            world_update_chunk_fast(world, c, CU_MESH); \
        } \
    }
#endif

void world_set_tile(World *world, Tile tile, int x, int y, int z) {
    long min_x, min_z;
    long ix, iy, iz;

    Chunk *c0;

    ix = floor(x+0.5);
    iy = floor(y+0.5);
    iz = floor(z+0.5);

    if(iy < 0 || iy >= CHUNK_HEIGHT) return;

    /* TODO: Support multiple players */
#if UNSAFE_SCROLLING
    THREAD_RW_LOCK_READ(&world->chunks_lock);
#endif
    c0 = world->chunks[0];
#if UNSAFE_SCROLLING
    THREAD_RW_UNLOCK_READ(&world->chunks_lock);
#endif

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

#if UNSAFE_SCROLLING
        THREAD_RW_LOCK_READ(&world->chunks_lock);
#endif
        c = world->chunks[cz/CHUNK_DEPTH*world->width+cx/CHUNK_WIDTH];
#if UNSAFE_SCROLLING
        THREAD_RW_UNLOCK_READ(&world->chunks_lock);
#endif

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

#if UNSAFE_SCROLLING
    THREAD_RW_LOCK_READ(&world->chunks_lock);
#endif
    c0 = world->chunks[0];
#if UNSAFE_SCROLLING
    THREAD_RW_UNLOCK_READ(&world->chunks_lock);
#endif

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

#if UNSAFE_SCROLLING
        THREAD_RW_LOCK_READ(&world->chunks_lock);
#endif
        c = world->chunks[iz/CHUNK_DEPTH*world->width+ix/CHUNK_WIDTH];
#if UNSAFE_SCROLLING
        THREAD_RW_UNLOCK_READ(&world->chunks_lock);
#endif

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
#if UNSAFE_SCROLLING
    THREAD_RW_FREE(&world->chunks_lock);
#endif
    THREAD_LOCK_FREE(world->stop_lock);
}

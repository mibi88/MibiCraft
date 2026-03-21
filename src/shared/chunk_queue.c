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

#include <shared/chunk_queue.h>

#include <string.h>

int chunk_queue_init(ChunkQueue *queue, size_t size) {
    queue->data = malloc(size*sizeof(ChunkUpdate));
    if(queue->data == NULL){
        return 1;
    }

    queue->size = size;
    queue->start = size/2;
    queue->end = size/2;

#if DEBUG_CHUNKQUEUE
    printf("%p: init -- size: %lu, start: %lu, end: %lu\n", (void*)queue,
           queue->size, queue->start, queue->end);
#endif

    if(THREAD_LOCK_INIT(queue->lock)){
        free(queue->data);
        queue->data = NULL;

        return 2;
    }

    return 0;
}

int chunk_queue_push(ChunkQueue *queue, ChunkUpdate update) {
    THREAD_LOCK_LOCK(queue->lock);

    if(queue->end+1 >= queue->size){
        size_t len;
        size_t new_start;
        size_t new_end;

        if(!queue->start){
            THREAD_LOCK_UNLOCK(queue->lock);
            return 1;
        }

        len = queue->end-queue->start;
        new_start = (queue->size-len)/2;
        new_end = new_start+len;

#if DEBUG_CHUNKQUEUE
        printf("%p: memmove (push) -- len: %lu, new_start: %lu, new_end: %lu\n",
               queue, len, new_start, new_end);
#endif

        memmove(queue->data+new_start, queue->data+queue->start,
                len*sizeof(ChunkUpdate));
        queue->start = new_start;
        queue->end = new_end;
    }

    queue->data[queue->end++] = update;

#if DEBUG_CHUNKQUEUE
    printf("%p: push -- start: %lu, end: %lu, chunk: %p\n", queue, queue->start,
           queue->end, update.chunk);
#endif

    THREAD_LOCK_UNLOCK(queue->lock);

    return 0;
}

int chunk_queue_bypass(ChunkQueue *queue, ChunkUpdate update) {
    THREAD_LOCK_LOCK(queue->lock);

    if(!queue->start){
        size_t len;
        size_t new_start;
        size_t new_end;

        if(queue->end+1 >= queue->size){
            THREAD_LOCK_UNLOCK(queue->lock);
            return 1;
        }

        len = queue->end-queue->start;
        new_start = queue->size-len <= 2 ? 1 : (queue->size-len)/2;
        new_end = new_start+len;

#if DEBUG_CHUNKQUEUE
        printf("%p: memmove (bypass) -- len: %lu, new_start: %lu, "
               "new_end: %lu\n", queue, len, new_start, new_end);
#endif

        memmove(queue->data+new_start, queue->data+queue->start,
                len*sizeof(ChunkUpdate));
        queue->start = new_start;
        queue->end = new_end;
    }

    queue->data[--queue->start] = update;

#if DEBUG_CHUNKQUEUE
    printf("%p: bypass -- start: %lu, end: %lu\n", queue, queue->start,
           queue->end);
#endif

    THREAD_LOCK_UNLOCK(queue->lock);

    return 0;
}

ChunkUpdate chunk_queue_pop(ChunkQueue *queue) {
    ChunkUpdate r = {NULL, 0};

    THREAD_LOCK_LOCK(queue->lock);

#if DEBUG_CHUNKQUEUE
    printf("%p: pop -- start: %lu, end: %lu, chunk: %p\n", queue, queue->start,
           queue->end, queue->data[queue->start].chunk);
#endif

    if(queue->start < queue->end) r = queue->data[queue->start++];

    THREAD_LOCK_UNLOCK(queue->lock);

    return r;
}

int chunk_queue_empty(ChunkQueue *queue) {
    int r;

    THREAD_LOCK_LOCK(queue->lock);

    r = queue->start == queue->end;

    THREAD_LOCK_UNLOCK(queue->lock);

    return r;
}

void chunk_queue_free(ChunkQueue *queue) {
#if DEBUG_CHUNKQUEUE
    printf("%p: free\n", queue);
#endif
    free(queue->data);
    queue->data = NULL;
    queue->start = 0;
    queue->end = 0;
    queue->size = 0;
}

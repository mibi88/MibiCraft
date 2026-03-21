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
    queue->data = malloc(size*sizeof(Chunk*));
    if(queue->data == NULL){
        return 1;
    }

    queue->size = size;
    queue->start = size/2;
    queue->end = size/2;

    if(THREAD_LOCK_INIT(queue->lock)){
        free(queue->data);
        queue->data = NULL;

        return 2;
    }

    return 0;
}

int chunk_queue_push(ChunkQueue *queue, Chunk *chunk) {
    THREAD_LOCK_LOCK(queue->lock);

    if(queue->end+1 >= queue->size){
        size_t len;
        size_t new_start;
        size_t new_end;

        if(!queue->start) return 1;

        len = queue->end-queue->start;
        new_start = (queue->size-len)/2;
        new_end = new_start+len;

        memmove(queue->data+new_start, queue->data+queue->start, len);
        queue->start = new_start;
        queue->end = new_end;
    }

    queue->data[queue->end++] = chunk;

    THREAD_LOCK_UNLOCK(queue->lock);

    return 0;
}

int chunk_queue_bypass(ChunkQueue *queue, Chunk *chunk) {
    THREAD_LOCK_LOCK(queue->lock);

    if(!queue->start){
        size_t len;
        size_t new_start;
        size_t new_end;

        if(queue->end+1 >= queue->size) return 1;

        len = queue->end-queue->start;
        new_start = queue->size-len <= 2 ? 1 : (queue->size-len)/2;
        new_end = new_start+len;

        memmove(queue->data+new_start, queue->data+queue->start, len);
        queue->start = new_start;
        queue->end = new_end;
    }

    queue->data[queue->start--] = chunk;

    THREAD_LOCK_UNLOCK(queue->lock);

    return 0;
}

Chunk *chunk_queue_pop(ChunkQueue *queue) {
    THREAD_LOCK_LOCK(queue->lock);

    if(queue->start < queue->end) return queue->data[queue->end--];

    THREAD_LOCK_UNLOCK(queue->lock);

    return NULL;
}

void chunk_queue_free(ChunkQueue *queue) {
    free(queue->data);
    queue->data = NULL;
    queue->start = 0;
    queue->end = 0;
    queue->size = 0;
}

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

#include <arena.h>

#include <limits.h>

#define SIZE_MAX (~(size_t)0)

void arena_init(Arena *arena, size_t block_size) {
    arena->blocks = NULL;
    arena->block = SIZE_MAX;
    arena->block_size = block_size;
    arena->block_usage = block_size;
}

void *arena_alloc(Arena *arena, size_t size, size_t count) {
    size_t align;
    size_t total;
    size_t offset;

    void *new;

    align = arena->block_usage%size ? size-arena->block_usage%size : 0;
    total = size*count;
    offset = arena->block_usage+align;

    if(offset+total > arena->block_size){
        /* Allocate a new block */
        size_t new_size = total > arena->block_size ?
                          arena->block_size : total;

        arena->block++;

        new = realloc(arena->blocks, (arena->block+1)*sizeof(void*));
        if(new == NULL){
            arena->block--;
            return NULL;
        }

        arena->blocks = new;

        arena->blocks[arena->block] = malloc(new_size);
        if(arena->blocks[arena->block] == NULL){
            arena->block--;
            return NULL;
        }
        arena->block_usage = 0;

        align = arena->block_usage%size ? size-arena->block_usage%size : 0;
        total = size*count;
        offset = arena->block_usage+align;
    }

    new = (char*)arena->blocks[arena->block]+offset;

    arena->block_usage += align+total;

    return new;
}

void arena_free(Arena *arena) {
    size_t i;

    for(i=0;i<=arena->block;i++){
        free(arena->blocks[i]);
    }

    free(arena->blocks);
    arena->blocks = NULL;

    arena_init(arena, arena->block_size);
}

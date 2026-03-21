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

#ifndef SHARED_CHUNK_QUEUE_H
#define SHARED_CHUNK_QUEUE_H

#include <shared/structs.h>

#define CHUNK_QUEUE_EMPTY(q) = (q)->start == (q)->end;

int chunk_queue_init(ChunkQueue *queue, size_t size);
int chunk_queue_push(ChunkQueue *queue, ChunkUpdate update);
int chunk_queue_bypass(ChunkQueue *queue, ChunkUpdate update);
ChunkUpdate chunk_queue_pop(ChunkQueue *queue);
void chunk_queue_free(ChunkQueue *queue);

#endif

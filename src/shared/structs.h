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

#ifndef SHARED_STRUCTS_H
#define SHARED_STRUCTS_H

#include <shared/config.h>

#include <shared/threading.h>

#include <client/gfx.h>

#include <stddef.h>

#define CHUNK_WIDTH           8
#define CHUNK_HEIGHT          128
#define CHUNK_DEPTH           8

#define BLOCK_VERTEX_MAX      (6*4*3/2)
#define BLOCK_INDEX_MAX       (6*6/2)
#define BLOCK_TEXTURE_MAX     (6*4*2/2)

#define HITBOX_SZ 2

typedef struct {
    float x, y, z;
    float rx, ry, rz;
    float x_velocity, y_velocity, z_velocity;
    float gravity;
    float hitbox[HITBOX_SZ*3];
} Entity;

typedef struct {
    Entity entity;
    float swimming_hitbox[HITBOX_SZ*3];
    float speed;
    float friction;
    float deceleration;
    float water_force;
    float velocity;
    float acceleration;
    float jump_force;
    float gravity;
    float swimming_down_speed;
    char moved;
    char sneak;
    unsigned char mode;
    int current_block;
    float adherence;
} Player;

/* It's unfortunate that OpenGL doesn't support storing vertices in chars */
typedef short int vertex_t;
#define TYPE_VERTEX TYPE_SHORT
typedef unsigned short int index_t;
#define TYPE_INDEX TYPE_USHORT
typedef short int texture_t;
#define TYPE_TEXTURE TYPE_SHORT

enum {
    CF_INIT = 1,
    CF_UPDATEMESH = 2,
    CF_UPDATEDATA = 4,
    CF_IN_QUEUE = 8
};

enum {
    CN_LEFT = 1,
    CN_RIGHT = 2,
    CN_ABOVE = 4,
    CN_BELOW = 8
};

typedef struct Chunk Chunk;
struct Chunk {
    /* TODO: Allocate all this stuff on the heap, and only allocate
     *       chunk_vertices, chunk_indices and chunk_texture_coords for the
     *       chunks that surround the player that's playing on this instance of
     *       the game (once I'm adding multiplayer capabilities to the game).
     */
    unsigned char chunk_data[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_DEPTH];

    vertex_t chunk_vertices[BLOCK_VERTEX_MAX*CHUNK_WIDTH*CHUNK_HEIGHT*
                            CHUNK_DEPTH];
    index_t chunk_indices[BLOCK_INDEX_MAX*CHUNK_WIDTH*CHUNK_HEIGHT*CHUNK_DEPTH];
    texture_t chunk_texture_coords[BLOCK_TEXTURE_MAX*CHUNK_WIDTH*CHUNK_HEIGHT*
                                   CHUNK_DEPTH];

    GFXModel chunk_model;

    thread_rwlock_t data_lock;
    thread_rwlock_t mesh_lock;
    thread_lock_t flags_lock;

    int x, z;

    unsigned char generated_neighbors;

    unsigned char flags;
};

typedef struct {
    Chunk *chunk;
} ChunkUpdate;

typedef struct {
    ChunkUpdate *data;
    size_t size;

    size_t start;
    size_t end;

    thread_lock_t lock;
} ChunkQueue;

typedef struct world World;

typedef struct {
    World *w;
    ChunkQueue *queue;
    thread_lock_t finished_lock;
    unsigned char finished;
} UpdateData;

struct world {
    Player *players;
    size_t player_num;

    Chunk *chunk_data;
    Chunk **chunks;

#if UNSAFE_SCROLLING
    thread_rwlock_t chunks_lock;
#endif

    ChunkQueue *queues;
    size_t queue_num;
    size_t last_queue;
    thread_lock_t last_queue_lock;
    thread_t *threads;
    UpdateData *thread_data;

    Chunk **empty;
    size_t empty_chunks;
    thread_lock_t empty_lock;

    unsigned char stop;
    thread_lock_t stop_lock;

    size_t width, height;

    int seed;

    unsigned int texture;
};

#endif

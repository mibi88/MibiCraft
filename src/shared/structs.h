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

#ifndef STRUCTS_H
#define STRUCTS_H

#include <shared/config.h>

#include <shared/threading.h>

#include <stddef.h>

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

    int x, z;
};

typedef struct {
    Chunk **data;
    size_t size;

    size_t start;
    size_t end;
} ChunkQueue;

typedef struct {
    Player *players;
    size_t player_num;

    Chunk *chunk_data;
    Chunk **chunks;

    ChunkQueue *queues;
    size_t queue_num;

    Chunk **empty;
    size_t empty_chunks;
    thread_lock_t empty_lock;

    thread_lock_t *chunk_locks;

    size_t width, height;

    int seed;

    unsigned int texture;
} World;

#endif

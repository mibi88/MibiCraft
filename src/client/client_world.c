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
#include <client/client_world.h>

void world_render(World *world, size_t player) {
    size_t i;
    size_t b = player*world->width*world->height;

    gfx_reset_texture_transforms();
    gfx_set_texture_scale(TILE_WIDTH/(float)TEX_WIDTH,
                          TILE_HEIGHT/(float)TEX_WIDTH);

    for(i=0;i<world->width*world->height;i++) {
        Chunk *c;

        THREAD_RW_LOCK_READ(&world->chunks_lock);
        c = world->chunks[b+i];
        THREAD_RW_UNLOCK_READ(&world->chunks_lock);

        if(THREAD_LOCK_TRYLOCK(c->lock)) continue;
        gfx_draw_model(&c->chunk_model, c->x-0.5, -(CHUNK_HEIGHT/2)-0.5,
                       c->z-0.5, 0, 0, 0);
        THREAD_LOCK_UNLOCK(c->lock);
    }

    gfx_reset_texture_transforms();
}

void world_update_chunk_model_at(World *world, float sx, float sz) {
    /* FIXME */
}

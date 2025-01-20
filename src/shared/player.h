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

#ifndef PLAYER_H
#define PLAYER_H

#include <entity.h>

#include <world.h>

#include <chunk.h>

#define PLAYER_WALKING_SPEED 6
#define PLAYER_DEFAULT_FRICTION 0.5
#define PLAYER_DEFAULT_DECELERATION 1
#define PLAYER_JUMP_FORCE 6
#define PLAYER_WATER_FORCE 4
#define PLAYER_GRAVITY 12
#define PLAYER_SWIMMING_DOWN_SPEED (PLAYER_WATER_FORCE*4)

/* Game mode */
enum {
    M_SPECTATOR,
    M_CREATIVE,
    M_SURVIVAL
};

void player_init(Player *player);
void player_move(Player *player, float delta, float direction);
void player_jump(Player *player, World *world);
void player_sneak(Player *player);
void player_break_block(Player *player, World *world);
void player_place_block(Player *player, World *world);
void player_update(Player *player, World *world, float delta);
void player_respawn(Player *player, World *world);

#endif

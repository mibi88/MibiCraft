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

#ifndef GAME_H
#define GAME_H

#include <world.h>
#include <raycast.h>

#include <blocks.h>
#include <crosshair.h>
#include <font.h>

#include <math.h>

#include <entity.h>

#include <config.h>

#include <player.h>

#include <button.h>

#define COPYRIGHT "(C) 2023 Mibi88"

/* Screen */
enum {
    D_TITLE,
    D_INGAME,
    D_PAUSE,
    D_AMOUNT
};

/* Input type */
enum {
    I_KEYPRESS,
    I_KEYRELEASE,
    I_MOUSE,
    I_LEFTCLICK,
    I_RIGHTCLICK
};

typedef struct {
    Player player;
    int mx, my;
    float gui_scale;
    unsigned int texture, crosshair, font;
    float rot_speed;
    int seed;
    int focus;
    World world;
    float mov_speed;
    int fog_enabled;
    char fps_str[20];
    char pos_str[100];
    unsigned char screen;
    float delta;
    Button button_singleplayer;
    Button button_resume;
    Button button_title;
    int close_asked;
} Game;

void game_init(Game *game, int seed);
void game_input(Game *game, int v1, int v2, int type);
void game_respawn(Game *game);
void game_logic(Game *game, float delta);
void game_draw(Game *game, float delta);
void game_free(Game *game);

#endif

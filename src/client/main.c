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

#include <stdio.h>
#include <stdlib.h>

#include <game.h>

Game game;

void draw(int delta) {
    game_logic(&game, delta/(float)1000);
    game_draw(&game, delta/(float)1000);
}

void keypress(int key) {
    game_input(&game, key, 0, I_KEYPRESS);
}

void keyrelease(int key) {
    game_input(&game, key, 0, I_KEYRELEASE);
}

void left_click(void) {
    game_input(&game, 0, 0, I_LEFTCLICK);
}

void right_click(void) {
    game_input(&game, 0, 0, I_RIGHTCLICK);
}

void mouse(int x, int y) {
    game_input(&game, x, y, I_MOUSE);
}

int main(int argc, char **argv) {
    puts("MibiCraft launched!");
    gfx_init(&argc, argv, "MibiCraft", 1, 0);
    game_init(&game, 1024);
    gfx_run(draw, keypress, keyrelease, mouse, left_click, right_click);
    return EXIT_SUCCESS;
}


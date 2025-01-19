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

#ifndef BUTTON_H
#define BUTTON_H

#define DEFAULT_R 0.5
#define DEFAULT_G 0.5
#define DEFAULT_B 0.5

#define DEFAULT_HOVER_R 0.7
#define DEFAULT_HOVER_G 0.7
#define DEFAULT_HOVER_B 0.7

typedef struct {
    int x, y;
    int w, h;
    float r, g, b;
    float hover_r, hover_g, hover_b;
    char *text;
} Button;

void button_init(Button *button, int x, int y, int w, int h, char *text);
void button_draw(Button *button, int mx, int my, float scale, int font);
int button_mouse_inside(Button *button, int mx, int my);

#endif

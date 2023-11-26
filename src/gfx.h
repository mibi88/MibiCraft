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

#ifndef GFX_H
#define GFX_H

#include <stdlib.h>
#include <stdio.h>

#define PI 3.14159

#define LINE_MAX_SIZE 64

typedef struct {
    float *vertices;
    int *indices;
    float *uv_coords;
    
    unsigned int texture;
    
    int has_indices, has_texture;
    int triangles;
} GFXModel;

int gfx_get_time(void);

void gfx_set_pointer_pos(int x, int y);

int gfx_get_width(void);

int gfx_get_height(void);

void gfx_enable_fog(float r, float g, float b, float density, float start,
                    float end);

void gfx_set_clear_color(float r, float g, float b);

void gfx_set_color(float r, float g, float b);

void gfx_set_camera(float x, float y, float z, float rx, float ry, float rz);

void gfx_draw_point(float x, float y, float z, int size);

void gfx_draw_model(GFXModel *model, float x, float y, float z, float rx,
                    float ry, float rz);

void gfx_free(GFXModel *model);

void gfx_init(int *argc, char **argv, char *title);

void gfx_run(void draw(int), void keypress(int), void mouse(int, int));

unsigned int gfx_load_texture(int width, int height,
                              unsigned char *texture_data);

#endif /* GFX_H */

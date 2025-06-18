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
    void *vertices;
    void *indices;
    void *uv_coords;

    unsigned int texture;

    int has_indices, has_texture;
    int triangles;

    unsigned char vertex_type;
} GFXModel;

enum {
    TYPE_INT,
    TYPE_UINT,
    TYPE_FLOAT,
    TYPE_SHORT,
    TYPE_USHORT,
    TYPE_CHAR,
    TYPE_UCHAR,
    TYPE_AMOUNT
};

int gfx_get_time(void);

void gfx_set_pointer_pos(int x, int y);

void gfx_cursor_hide(void);

void gfx_cursor_show(void);

int gfx_get_width(void);

int gfx_get_height(void);

int gfx_get_motion_blur(void);

void gfx_enable_fog(float r, float g, float b, float density, float start,
                    float end);

void gfx_disable_fog(void);

void gfx_set_clear_color(float r, float g, float b);

void gfx_set_color(float r, float g, float b, float a);

void gfx_enable_scissor_test(int x, int y, int width, int height);

void gfx_disable_scissor_test(void);

void gfx_set_motion_blur(int v);

void gfx_set_camera(float x, float y, float z, float rx, float ry, float rz);

void gfx_draw_point(float x, float y, float z, int size);

void gfx_draw_rect(float x1, float y1, float x2, float y2);

void gfx_init_model(GFXModel *model, void *vertices, void *indices,
                    void *uv_coords, int texture, int has_indices,
                    int has_texture, int triangles, int vertex_type);

void gfx_draw_model(GFXModel *model, float x, float y, float z, float rx,
                    float ry, float rz);

void gfx_render_wire_cube(float x, float y, float z, float s);

void gfx_start_2d(void);

void gfx_end_2d(void);

void gfx_draw_image(int sx, int sy, unsigned int texture, int width,
                    int height, float scale);

void gfx_disable_closing(void);

void gfx_enable_closing(void);

void gfx_draw_image_and_modulate(int sx, int sy, unsigned int texture,
                                 int width, int height, float scale);

void gfx_draw_image_from_atlas(int sx, int sy, unsigned int texture, int width,
                               int height, float scale, int cell_w, int cell_h,
                               int atlas_w, int atlas_h, int num);

void gfx_draw_string(float sx, float sy, char *string, unsigned int font,
                     int char_w, int char_h, float scale);

void gfx_free(GFXModel *model);

void gfx_init(char *title, int use_arrays, int motion_blur);

void gfx_run(void draw(int), void keypress(int), void keyrelease(int),
             void mouse(int, int), void left_click(int),
             void right_click(int), void middle_click(int),
             void mouse_wheel(int), void on_exit(void),
             void on_mainloop(void));

unsigned int gfx_load_texture(int width, int height,
                              unsigned char *texture_data);

#endif /* GFX_H */

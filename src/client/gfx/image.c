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

#include <gfx.h>

#include <string.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include "data.h"

void _3d_projection_matrix(float w, float h);

void gfx_start_2d(void) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, gfx_get_width(), gfx_get_height(), 0);
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_DEPTH_TEST);
}

void gfx_end_2d(void) {
    glEnable(GL_DEPTH_TEST);

    _3d_projection_matrix(gfx_get_width(), gfx_get_height());
}

void gfx_draw_image(int sx, int sy, unsigned int texture, int width,
                    int height, float scale) {
    float x2 = (float)width*scale, y2 = (float)height*scale;

    glBindTexture(GL_TEXTURE_2D, texture);

    glLoadIdentity();
    glTranslatef(sx, sy, 0);

    glBegin(GL_QUADS);

    glTexCoord2i(0, 0);
    glVertex2i(0, 0);
    glTexCoord2i(1, 0);
    glVertex2f(x2, 0);
    glTexCoord2i(1, 1);
    glVertex2f(x2, y2);
    glTexCoord2i(0, 1);
    glVertex2f(0, y2);

    glEnd();
}

void gfx_draw_image_and_modulate(int sx, int sy, unsigned int texture,
                                 int width, int height, float scale) {
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    gfx_draw_image(sx, sy, texture, width, height, scale);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

void gfx_draw_image_from_atlas(int sx, int sy, unsigned int texture, int width,
                               int height, float scale, int cell_w, int cell_h,
                               int atlas_w, int atlas_h, int num) {
    float x2 = (float)width*scale, y2 = (float)height*scale;
    int tex_x = num%atlas_w, tex_y = num/atlas_w;
    int total_w = atlas_w*cell_w, total_h = atlas_h*cell_h;

    float step_x = (float)cell_w/(float)total_w;
    float step_y = (float)cell_h/(float)total_h;

    float u1 = (float)tex_x*step_x;
    float v1 = (float)tex_y*step_y;
    float u2 = u1+step_x;
    float v2 = v1+step_y;

    glBindTexture(GL_TEXTURE_2D, texture);

    glLoadIdentity();
    glTranslatef(sx, sy, 0);

    glBegin(GL_QUADS);

    glTexCoord2f(u1, v1);
    glVertex2i(0, 0);
    glTexCoord2f(u2, v1);
    glVertex2f(x2, 0);
    glTexCoord2f(u2, v2);
    glVertex2f(x2, y2);
    glTexCoord2f(u1, v2);
    glVertex2f(0, y2);

    glEnd();
}

void gfx_draw_string(float sx, float sy, char *string, unsigned int font,
                     int char_w, int char_h, float scale) {
    int i;
    int org_sx = sx;
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    for(i=0;i<(int)strlen(string);i++){
        if(string[i] == '\n'){
            sy += char_h*scale;
            sx = org_sx;
            continue;
        }
        gfx_draw_image_from_atlas(sx, sy, font, char_w, char_h, scale, char_w,
                                  char_h, 16, 16, string[i]-0x20);
        sx += char_w*scale;
    }
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

unsigned int gfx_load_texture(int width, int height,
                              unsigned char *texture_data) {
    unsigned int id;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                   GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                   GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, texture_data);

    return id;
}

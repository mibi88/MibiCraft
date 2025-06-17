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

void gfx_draw_point(float x, float y, float z, int size) {
    glLoadIdentity();
    glPointSize(size);
    glBegin(GL_POINTS);
    glVertex3f(x, y, z);
    glEnd();
}

void gfx_draw_rect(float x1, float y1, float x2, float y2) {
    glDisable(GL_TEXTURE_2D);

    glLoadIdentity();

    glBegin(GL_QUADS);

    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);

    glEnd();

    glEnable(GL_TEXTURE_2D);
}

void gfx_create_modelview_matrix(float x, float y, float z, float rx, float ry,
                                 float rz);

void gfx_render_wire_cube(float x, float y, float z, float s) {
    gfx_create_modelview_matrix(x, y, z, 0, 0, 0);
    glDisable(GL_TEXTURE_2D);

    glLineWidth(5.0);

    glBegin(GL_LINES);

    glVertex3f(-s/2, -s/2, -s/2);
    glVertex3f(-s/2, -s/2, s/2);

    glVertex3f(s/2, -s/2, -s/2);
    glVertex3f(s/2, -s/2, s/2);

    glVertex3f(-s/2, s/2, -s/2);
    glVertex3f(-s/2, s/2, s/2);

    glVertex3f(s/2, s/2, -s/2);
    glVertex3f(s/2, s/2, s/2);


    glVertex3f(-s/2, -s/2, -s/2);
    glVertex3f(-s/2, s/2, -s/2);

    glVertex3f(s/2, -s/2, -s/2);
    glVertex3f(s/2, s/2, -s/2);

    glVertex3f(-s/2, s/2, s/2);
    glVertex3f(-s/2, -s/2, s/2);

    glVertex3f(s/2, s/2, s/2);
    glVertex3f(s/2, -s/2, s/2);


    glVertex3f(-s/2, -s/2, -s/2);
    glVertex3f(s/2, -s/2, -s/2);

    glVertex3f(-s/2, s/2, -s/2);
    glVertex3f(s/2, s/2, -s/2);

    glVertex3f(-s/2, -s/2, s/2);
    glVertex3f(s/2, -s/2, s/2);

    glVertex3f(-s/2, s/2, s/2);
    glVertex3f(s/2, s/2, s/2);

    glEnd();

    glEnable(GL_TEXTURE_2D);
}

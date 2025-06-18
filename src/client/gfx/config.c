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

#include "config.h"

#include "window.h"

extern int _can_close;
extern MWWindow _mwwindow;

void (*_draw)(int);
void (*_keypress)(int);
void (*_keyrelease)(int);

void (*_right_click)(int);
void (*_left_click)(int);
void (*_middle_click)(int);

void (*_mouse_wheel)(int);

int _w = 0, _h = 0;

int _old_time = 0;

float _cam_x, _cam_y, _cam_z, _cam_rx, _cam_ry, _cam_rz;

int _keys[256];

int _motion_blur_step = 0;

_Config _config;

int gfx_get_time(void) {
    return mw_get_time();
}

void gfx_set_pointer_pos(int x, int y) {
    mw_move_pointer(&_mwwindow, x, y);
}

void gfx_cursor_hide(void) {
    mw_hide_pointer(&_mwwindow);
}

void gfx_cursor_show(void) {
    mw_show_pointer(&_mwwindow);
}

int gfx_get_width(void) {
    return _w;
}

int gfx_get_height(void) {
    return _h;
}

int gfx_get_motion_blur(void) {
    return _config.motion_blur;
}

void gfx_enable_fog(float r, float g, float b, float density, float start,
                    float end) {
    GLfloat color[4] = {0.0, 0.0, 0.0, 1.0};
    color[0] = r;
    color[1] = g;
    color[2] = b;
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogfv(GL_FOG_COLOR, color);
    glFogf(GL_FOG_DENSITY, density);
    glHint(GL_FOG_HINT, GL_DONT_CARE);
    glFogf(GL_FOG_START, start);
    glFogf(GL_FOG_END, end);
}

void gfx_disable_fog(void) {
    glDisable(GL_FOG);
}

void gfx_set_clear_color(float r, float g, float b) {
    glClearColor(r, g, b, 1.0f);
}

void gfx_set_color(float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
}

void gfx_enable_scissor_test(int x, int y, int width, int height) {
    glLoadIdentity();
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, gfx_get_height()-(y+height), width, height);
}

void gfx_disable_scissor_test(void) {
    glDisable(GL_SCISSOR_TEST);
}

void gfx_set_motion_blur(int v) {
    _config.motion_blur = v;
}

void gfx_set_texture_scale(float x, float y) {
    glMatrixMode(GL_TEXTURE);
    glScalef(x, y, 1);
    glMatrixMode(GL_MODELVIEW);
}

void gfx_reset_texture_transforms(void) {
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
}

void gfx_set_camera(float x, float y, float z, float rx, float ry, float rz) {
    _cam_x = x;
    _cam_y = y;
    _cam_z = z;

    _cam_rx = rx;
    _cam_ry = ry;
    _cam_rz = rz;
}

void gfx_disable_closing(void) {
    _can_close = 0;
}

void gfx_enable_closing(void) {
    _can_close = 1;
}

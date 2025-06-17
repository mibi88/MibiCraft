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

#include "config.h"

extern void (*_draw)(int);
extern void (*_keypress)(int);
extern void (*_keyrelease)(int);

extern void (*_right_click)(int);
extern void (*_left_click)(int);
extern void (*_middle_click)(int);

extern void (*_mouse_wheel)(int);

extern int _w, _h;

extern int _old_time;

extern float _cam_x, _cam_y, _cam_z, _cam_rx, _cam_ry, _cam_rz;

extern int _keys[256];

extern int _motion_blur_step;

extern _Config _config;

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

#ifndef MW_WINDOW_H
#define MW_WINDOW_H

#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/glu.h>

typedef struct {
    Display *display;
    Window root;
    XVisualInfo *visual_info;
    Colormap color_map;
    XSetWindowAttributes set_window_attributes;
    Window window;
    GLXContext glx_context;
    XWindowAttributes window_attributes;
    XEvent event;
    Atom wm_delete;
    XIM xim;
    XIC xic;
    long mask;
    int pointer_shown;
} MWWindow;

int mw_init(MWWindow *window, int width, int height, char *title);

int mw_get_next_event(MWWindow *window);
void mw_get_size(MWWindow *window, int *width, int *height);
void mw_get_pointer_pos(MWWindow *window, int *x, int *y);
int mw_get_pointer_button(MWWindow *window);
unsigned long mw_get_time(void);

void mw_move_pointer(MWWindow *window, int x, int y);
void mw_hide_pointer(MWWindow *window);
void mw_show_pointer(MWWindow *window);

void mw_start_drawing(void);
void mw_end_drawing(void);
void mw_swap_buffers(MWWindow *window);

int mw_should_close(MWWindow *window);
int mw_window_resized(MWWindow *window);
int mw_pointer_moved(MWWindow *window);
int mw_button_pressed(MWWindow *window);
int mw_button_released(MWWindow *window);
int mw_key_pressed(MWWindow *window);
int mw_key_released(MWWindow *window);

char mw_get_key_char(MWWindow *window);
unsigned int mw_get_key_code(MWWindow *window);

void mw_close(MWWindow *window);

#endif

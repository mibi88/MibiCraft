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
#include "window.h"

MWWindow _mwwindow;
int _can_close;

void gfx_init(char *title, int use_arrays, int motion_blur) {
    _can_close = 1;

    _config.use_arrays = use_arrays;
    _config.motion_blur = motion_blur;
    if(_config.use_arrays){
        glEnable(GL_VERTEX_ARRAY);
    }
    mw_init(&_mwwindow, 500, 500, title);
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5);
}

void _3d_projection_matrix(float w, float h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(72, (float)w/(float)h, 0.1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void _display(void) {
    int i, frame_time;

    frame_time = gfx_get_time()-_old_time;
    _old_time = gfx_get_time();

    for(i=0;i<256;i++){
        if(_keys[i]){
            _keypress(i);
        }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    _draw(frame_time);

    if(_config.motion_blur){
        if(_motion_blur_step == 0){
            glAccum(GL_LOAD, 1.0/_config.motion_blur);
        }else{
            glAccum(GL_ACCUM, 1.0/_config.motion_blur);
        }
        _motion_blur_step++;
        if(_motion_blur_step >= _config.motion_blur){
            _motion_blur_step = 0;
            glAccum(GL_RETURN, 1.0);
            glFlush();
            glDisable(GL_TEXTURE_2D);
            mw_swap_buffers(&_mwwindow);
        }
    }else{
        glFlush();
        glDisable(GL_TEXTURE_2D);
        mw_swap_buffers(&_mwwindow);
    }
}

void _reshape(int w, int h) {
    _w = w;
    _h = h;
   glViewport(0, 0, (GLsizei)w, (GLsizei)h);
   _3d_projection_matrix(w, h);
}

void _keyboard(unsigned char key, int x, int y) {
    (void)x;
    (void)y;
    _keys[key] = 1;
}

void _release(unsigned char key, int x, int y) {
    (void)x;
    (void)y;
    _keys[key] = 0;
    _keyrelease(key);
}

void _click(int button, int state) {
    switch(button){
        case 1:
            _left_click(state);
            break;
        case 2:
            _middle_click(state);
            break;
        case 3:
            _right_click(state);
            break;
        case 4:
            /* FALLTHRU */
        case 5:
            _mouse_wheel(button-4);
    }
}

void gfx_run(void draw(int), void keypress(int), void keyrelease(int),
             void mouse(int, int), void left_click(int),
             void right_click(int), void middle_click(int),
             void mouse_wheel(int), void on_exit(void),
             void on_mainloop(void)) {
    int w, h;
    int x, y;
    int button = 0;
    char c;
    _motion_blur_step = 0;
    _left_click = left_click;
    _right_click = right_click;
    _middle_click = middle_click;
    _mouse_wheel = mouse_wheel;
    _old_time = gfx_get_time();
    _draw = draw;
    _keypress = keypress;
    _keyrelease = keyrelease;


    while(1){
        if(mw_get_next_event(&_mwwindow)){
            if(mw_pointer_moved(&_mwwindow)){
                mw_get_pointer_pos(&_mwwindow, &x, &y);
                mouse(x, y);
            }
            if(mw_window_resized(&_mwwindow)){
                mw_get_size(&_mwwindow, &w, &h);
                _reshape(w, h);
            }
            if(mw_button_pressed(&_mwwindow)){
                button = mw_get_pointer_button(&_mwwindow);
                _click(button, 1);
            }
            if(mw_button_released(&_mwwindow)){
                _click(button, 0);
            }
            if(mw_key_pressed(&_mwwindow)){
                c = mw_get_key_char(&_mwwindow);
                _keyboard(c, 0, 0);
            }
            if(mw_key_released(&_mwwindow)){
                c = mw_get_key_char(&_mwwindow);
                _release(c, 0, 0);
            }
            if(mw_should_close(&_mwwindow)){
                if(_can_close) break;
            }
        }else{
            mw_start_drawing();
            _display();
            mw_end_drawing();
            on_mainloop();
        }
    }
    on_exit();
    mw_close(&_mwwindow);
}

void gfx_create_modelview_matrix(float x, float y, float z, float rx, float ry,
                                 float rz) {
    glLoadIdentity();
    glRotatef(_cam_rx, 1, 0, 0);
    glRotatef(_cam_ry, 0, 1, 0);
    glRotatef(_cam_rz, 0, 0, 1);
    glTranslatef(-_cam_x, -_cam_y, -_cam_z);
    glTranslatef(x, y, z);
    glRotatef(rx, 1, 0, 0);
    glRotatef(ry, 0, 1, 0);
    glRotatef(rz, 0, 0, 1);
}

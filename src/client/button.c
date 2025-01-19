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

#include <button.h>

#include <gfx.h>

#include <string.h>

void button_init(Button *button, int x, int y, int w, int h, char *text) {
    button->x = x;
    button->y = y;
    button->w = w;
    button->h = h;
    button->r = DEFAULT_R;
    button->g = DEFAULT_G;
    button->b = DEFAULT_B;
    button->hover_r = DEFAULT_HOVER_R;
    button->hover_g = DEFAULT_HOVER_G;
    button->hover_b = DEFAULT_HOVER_B;
    button->text = text;
}

void button_draw(Button *button, int mx, int my, float scale, int font) {
    int text_x, text_y;
    if(mx >= button->x && my >= button->y && mx-button->x <= button->w &&
       my-button->y <= button->h){
        gfx_draw_quad(button->x, button->y, button->w, button->h,
                      button->hover_r, button->hover_g, button->hover_b);
    }else{
        gfx_draw_quad(button->x, button->y, button->w, button->h,
                      button->r, button->g, button->b);
    }
    text_x = (button->w-strlen(button->text)*8*scale)/2+button->x;
    text_y = (button->h-8*scale)/2+button->y;
    gfx_draw_string(text_x, text_y, button->text, font, 8, 8, scale);
}

int button_mouse_inside(Button *button, int mx, int my) {
    if(mx >= button->x && my >= button->y && mx-button->x <= button->w &&
       my-button->y <= button->h){
        return 1;
    }
    return 0;
}

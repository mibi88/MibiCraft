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

#include <world.h>

#include <blocks.h>
#include <math.h>

#include <entity.h>

#define SZ_PLAYER_HITBOX 5

#define RENDER_DISTANCE  2

Entity player = {CHUNK_WIDTH, CHUNK_HEIGHT/2, CHUNK_DEPTH, 0, 0, 0, 0, 0, 0};
int mx, my;

float step = 0.01;

unsigned int texture = 0;

float rot_speed = 4;

int seed = 1024;

int focus = 1;

float max_speed = 0.25, x_v_change = 0;
float acceleration = 0.1;
float gravity = 0.025;
float jump_force = 0.25;
int jump;

World world;

float mov_speed;

float player_hitbox[SZ_PLAYER_HITBOX*2] = {
    -0.8, -0.8,
    -0.8, 0.8,
    0.8, -0.8,
    0.8, 0.8,
    0, 0
};

void respawn(void) {
    player.x = CHUNK_WIDTH;
    player.y = CHUNK_HEIGHT/2;
    player.z = CHUNK_DEPTH;
    world.x = player.x;
    world.y = player.z;
    player.ry = 0;
    player.rx = 0;
    player.x_velocity = 0;
    player.y_velocity = 0;
}

void get_rotation_from_mouse(void) {
    int cx = gfx_get_width()/2;
    int cy = gfx_get_height()/2;
    int mov_x, mov_y;
    if(!mov_speed) return;
    if(focus){
        mov_x = mx-cx;
        mov_y = my-cy;
        player.ry += (float)mov_x/rot_speed*mov_speed;
        player.rx += (float)mov_y/rot_speed*mov_speed;
        if(player.rx > 90) player.rx = 90;
        else if(player.rx < -90) player.rx = -90;
        gfx_set_pointer_pos(cx, cy);
        mx = cx;
        my = cy;
    }
}

void draw(int delta) {
    int fps = 0;
    mov_speed = (float)delta/16;
    if(delta != 0) fps = 1000/delta;
    printf("FPS: %d\r", fps);
    world_render(&world);
    gfx_set_camera(player.x, player.y, player.z, player.rx, player.ry, 0);

    get_rotation_from_mouse();

    world_update(&world, player.x, player.z);
}

void keypress(int key) {
    float sin_rx = sin((player.rx+90)/180*PI);
    switch(key) {
        case 'z':
            player.x += cos((player.ry-90)/180*PI)*0.25*sin_rx*mov_speed;
            player.y -= sin(player.rx/180*PI)*0.25*mov_speed;
            player.z += sin((player.ry-90)/180*PI)*0.25*sin_rx*mov_speed;
            break;
        case 's':
            player.x -= cos((player.ry-90)/180*PI)*0.25*sin_rx*mov_speed;
            player.y += sin(player.rx/180*PI)*0.25*mov_speed;
            player.z -= sin((player.ry-90)/180*PI)*0.25*sin_rx*mov_speed;
            break;
        case 'p':
            focus = !focus;
            break;
    }
}

void mouse(int x, int y) {
    mx = x;
    my = y;
}

int main(int argc, char **argv) {
    gfx_init(&argc, argv, "MibiCraft", 1);
    gfx_set_clear_color(0.7, 0.9, 1.0);
    gfx_enable_fog(0.7, 0.9, 1.0, 0.01, CHUNK_DEPTH-1,
                   CHUNK_DEPTH);
    texture = gfx_load_texture(blocks_width, blocks_height,
                               (unsigned char*)blocks_data);
    world_init(&world, RENDER_DISTANCE*2+1, RENDER_DISTANCE*2+1, seed, texture);
    gfx_run(draw, keypress, mouse);
    world_free(&world);
    return EXIT_SUCCESS;
}


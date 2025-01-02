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
#include <raycast.h>

#include <blocks.h>
#include <crosshair.h>
#include <font.h>

#include <math.h>

#include <entity.h>

#include <stdio.h>
#include <stdlib.h>

#define SZ_PLAYER_HITBOX 5

#define RENDER_DISTANCE  4

#define RAYCAST_DISTANCE 20

Entity player = {CHUNK_WIDTH, CHUNK_HEIGHT/2, CHUNK_DEPTH, 0, 0, 0, 0, 0, 0};
int mx, my;

float step = 0.01;

float gui_scale = 1;

unsigned int texture = 0, crosshair = 0, font = 0;

float rot_speed = 4;

int seed = 1024;

int focus = 1;

float max_speed = 0.0625, x_v_change = 0;
float acceleration = 0.1;
float gravity = 0.025;
float jump_force = 0.25;
int jump;

World world;

float mov_speed;

int fog_enabled = 1;

char fps_str[20];
char pos_str[100];

float player_hitbox[SZ_PLAYER_HITBOX*2] = {
    -0.8, -0.8,
    -0.8, 0.8,
    0.8, -0.8,
    0.8, 0.8,
    0, 0
};

void respawn(void) {
    player.x = 0;
    player.y = 0;
    player.z = 0;
    world.x = -(RENDER_DISTANCE*CHUNK_WIDTH+CHUNK_WIDTH/2);
    world.y = -(RENDER_DISTANCE*CHUNK_DEPTH+CHUNK_DEPTH/2);
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
        player.ry += (float)mov_x/rot_speed;
        player.rx += (float)mov_y/rot_speed;
        if(player.rx > 90) player.rx = 90;
        else if(player.rx < -90) player.rx = -90;
        gfx_set_pointer_pos(cx, cy);
        mx = cx;
        my = cy;
    }
}

int selection_draw(int x, int y, int z, void *data);

void draw(int delta) {
    int crosshair_x = (int)((float)gfx_get_width()/2-
                            (crosshair_width*gui_scale)/2);
    int crosshair_y = (int)((float)gfx_get_height()/2-
                            (crosshair_height*gui_scale)/2);
    int fps = 0;
    mov_speed = (float)delta/16*0.25;
    if(delta != 0) fps = 1000/delta;

    gfx_set_camera(player.x, player.y, player.z, player.rx, player.ry, 0);

    get_rotation_from_mouse();

    world_update(&world, player.x, player.z);

    sprintf(fps_str, "FPS: %d", fps);
    sprintf(pos_str, "X: %.02f Y: %.02f Z: %.02f", player.x, player.y,
            player.z);
    world_render(&world);
    /* Display the selected block */
    raycast(&world, &player, RAYCAST_DISTANCE, selection_draw);
    /* gfx_render_wire_cube((int)player.x, (int)player.y, (int)player.z, 1); */
    /* gfx_render_wire_cube(player.x, player.y, player.z, 1); */

    gfx_start_2d();
    gfx_draw_image(crosshair_x, crosshair_y, crosshair, crosshair_width,
                   crosshair_height, gui_scale);
    gfx_draw_string(0, 0, fps_str, font, 8, 8, gui_scale+1);
    gfx_draw_string(0, 8+8*gui_scale, pos_str, font, 8, 8, gui_scale+1);
    gfx_end_2d();
}

int selection_draw(int x, int y, int z, void *data) {
    if(world_get_tile(data, x, y, z)){
        gfx_render_wire_cube(x, y-CHUNK_HEIGHT/2, z, 1);
        return 1;
    }
    /* gfx_render_wire_cube(x, y-CHUNK_HEIGHT/2, z, 1); */
    return 0;
}

void keypress(int key) {
    float cos_rx = cos((player.rx)/180*PI);
    switch(key) {
        case 'z':
            player.x += cos((player.ry-90)/180*PI)*cos_rx*mov_speed;
            player.y += -sin(player.rx/180*PI)*mov_speed;
            player.z += sin((player.ry-90)/180*PI)*cos_rx*mov_speed;
            break;
        case 's':
            player.x -= cos((player.ry-90)/180*PI)*cos_rx*mov_speed;
            player.y -= -sin(player.rx/180*PI)*mov_speed;
            player.z -= sin((player.ry-90)/180*PI)*cos_rx*mov_speed;
            break;
    }
}

int break_block(int x, int y, int z, void *vworld) {
    World *world = (World*)vworld;
    if(world_get_tile(world, x, y, z)){
        printf("Break block at %d, %d, %d\n", x, y, z);
        world_set_tile(world, T_VOID, x, y, z);
        return 1;
    }
    return 0;
}

int old_x = 0;
int old_y = 0;
int old_z = 0;
int raycasting_step = 0;

int place_block(int x, int y, int z, void *vworld) {
    World *world = (World*)vworld;
    if(world_get_tile(world, x, y, z) && step != 0){
        world_set_tile(world, T_GLASS, old_x, old_y, old_z);
        return 1;
    }
    printf("Place block at %d, %d, %d\n", x, y, z);
    old_x = x;
    old_y = y;
    old_z = z;
    raycasting_step++;
    return 0;
}

void keyrelease(int key) {
    switch(key) {
        case 'p':
            focus = !focus;
            if(focus){
                gfx_cursor_hide();
            }else{
                gfx_cursor_show();
            }
            break;
        case 'f':
            if(fog_enabled){
                gfx_disable_fog();
            }else{
                gfx_enable_fog(0.7, 0.9, 1.0, 0.01, CHUNK_DEPTH-1,
                               CHUNK_DEPTH);
            }
            fog_enabled = !fog_enabled;
            break;
        case 'w':
            gui_scale += 0.5;
            break;
        case 'x':
            if(gui_scale > 0.5){
                gui_scale -= 0.5;
            }
            break;
        case 'm':
            if(gfx_get_motion_blur()){
                gfx_set_motion_blur(0);
            }else{
                gfx_set_motion_blur(128);
            }
    }
}

void left_click(void) {
    raycast(&world, &player, RAYCAST_DISTANCE, break_block);
}

void right_click(void) {
    raycasting_step = 0;
    raycast(&world, &player, RAYCAST_DISTANCE, place_block);
}

void mouse(int x, int y) {
    mx = x;
    my = y;
}

int main(int argc, char **argv) {
    puts("MibiCraft launched!");
    gfx_init(&argc, argv, "MibiCraft", 1, 0);
    gfx_set_clear_color(0.7, 0.9, 1.0);
    gfx_enable_fog(0.7, 0.9, 1.0, 0.01, CHUNK_DEPTH-1,
                   CHUNK_DEPTH);
    texture = gfx_load_texture(blocks_width, blocks_height,
                               (unsigned char*)blocks_data);
    crosshair = gfx_load_texture(crosshair_width, crosshair_height,
                                 (unsigned char*)crosshair_data);
    font = gfx_load_texture(font_width, font_height,
                            (unsigned char*)font_data);
    respawn();
    world_init(&world, RENDER_DISTANCE*2+1, RENDER_DISTANCE*2+1, seed, texture);
    if(focus){
        gfx_cursor_hide();
    }else{
        gfx_cursor_show();
    }
    gfx_run(draw, keypress, keyrelease, mouse, left_click, right_click);
    world_free(&world);
    return EXIT_SUCCESS;
}


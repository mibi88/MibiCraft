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

#include <game.h>

extern const Block_property blocks[T_AMOUNT];

void game_init(Game *game, int seed) {
    Entity player = {
            CHUNK_WIDTH, CHUNK_HEIGHT/2, CHUNK_DEPTH,
            0, 0, 0,
            0, 0,
            0.1, 0.05, 0.3,
            {
                -0.4, -1.7, -0.4,
                0.4, 0.2, 0.4
            }
    };
    game->player = player;
    game->gui_scale = 1;

    game->texture = 0;
    game->crosshair = 0;
    game->font = 0;

    game->rot_speed = 4;

    game->seed = seed;

    game->focus = 1;

    game->max_speed = 0.0625;
    game->x_v_change = 0;
    game->gravity = 0.025;
    game->jump_force = 0.25;

    game->fog_enabled = 1;

    game->screen = D_INGAME;

    game->texture = gfx_load_texture(blocks_width, blocks_height,
                               (unsigned char*)blocks_data);
    game->crosshair = gfx_load_texture(crosshair_width, crosshair_height,
                                 (unsigned char*)crosshair_data);
    game->font = gfx_load_texture(font_width, font_height,
                            (unsigned char*)font_data);
    game->mode = M_CREATIVE;
    game->current_block = T_SPRUCE_PLANKS;
    game_respawn(game);
    world_init(&game->world, RENDER_DISTANCE*2+1, RENDER_DISTANCE*2+1,
               game->seed, game->texture);
    gfx_set_clear_color(0.7, 0.9, 1.0);
    gfx_enable_fog(0.7, 0.9, 1.0, 0.01, CHUNK_DEPTH-1,
                   CHUNK_DEPTH);
    if(game->focus){
        gfx_cursor_hide();
    }else{
        gfx_cursor_show();
    }
}

int _game_break_block(int x, int y, int z, void *vgame) {
    Game *game = vgame;
    if(!blocks[world_get_tile(&game->world, x, y, z)].replaceable){
        printf("Break block at %d, %d, %d\n", x, y, z);
        world_set_tile(&game->world, T_VOID, x, y, z);
        return 1;
    }
    return 0;
}

int _game_place_block(int x, int y, int z, void *vgame) {
    Game *game = vgame;
    if(!blocks[world_get_tile(&game->world, x, y, z)].replaceable){
        if(!entity_is_block_inside(&game->player, &game->world, game->old_x,
                                   game->old_y, game->old_z)){
            world_set_tile(&game->world, game->current_block, game->old_x,
                           game->old_y, game->old_z);
        }
        return 1;
    }
    printf("Place block at %d, %d, %d\n", x, y, z);
    game->old_x = x;
    game->old_y = y;
    game->old_z = z;
    return 0;
}

void game_input(Game *game, int v1, int v2, int type) {
    float cos_rx;
    switch(type){
        case I_KEYPRESS:
            cos_rx = cos((game->player.rx)/180*PI);
            switch(v1) {
                case 'z':
                    if(game->mode == M_SPECTATOR){
                        game->player.x += cos((game->player.ry-90)/180*PI)*
                                          cos_rx*game->mov_speed;
                        game->player.y += -sin(game->player.rx/180*PI)*
                                          game->mov_speed;
                        game->player.z += sin((game->player.ry-90)/180*PI)*
                                          cos_rx*game->mov_speed;
                    }else{
                        /* Survival or creative mode. */
                        game->player.velocity += game->player.acceleration;
                        game->moved = 1;
                    }
                    break;
                case 's':
                    if(game->mode == M_SPECTATOR){
                        game->player.x -= cos((game->player.ry-90)/180*PI)*
                                          cos_rx*game->mov_speed;
                        game->player.y -= -sin(game->player.rx/180*PI)*
                                          game->mov_speed;
                        game->player.z -= sin((game->player.ry-90)/180*PI)*
                                          cos_rx*game->mov_speed;
                    }else{
                        /* Survival or creative mode. */
                        game->player.velocity -= game->player.acceleration;
                        game->moved = 1;
                    }
                    break;
                case ' ':
                    if(entity_on_floor(&game->player, &game->world)){
                        game->player.y_velocity = 8;
                    }
                    break;
            }
            break;
        case I_KEYRELEASE:
            switch(v1) {
                case 'p':
                    game->focus = !game->focus;
                    if(game->focus){
                        gfx_cursor_hide();
                    }else{
                        gfx_cursor_show();
                    }
                    break;
                case 'f':
                    if(game->fog_enabled){
                        gfx_disable_fog();
                    }else{
                        gfx_enable_fog(0.7, 0.9, 1.0, 0.01, CHUNK_DEPTH-1,
                                       CHUNK_DEPTH);
                    }
                    game->fog_enabled = !game->fog_enabled;
                    break;
                case 'w':
                    game->gui_scale += 0.5;
                    break;
                case 'x':
                    if(game->gui_scale > 0.5){
                        game->gui_scale -= 0.5;
                    }
                    break;
                case 'm':
                    if(gfx_get_motion_blur()){
                        gfx_set_motion_blur(0);
                    }else{
                        gfx_set_motion_blur(128);
                    }
                    break;
                case 'i':
                    game->current_block--;
                    if(game->current_block <= T_VOID){
                        game->current_block = T_AMOUNT-1;
                    }
                    break;
                case 'o':
                    game->current_block++;
                    if(game->current_block >= T_AMOUNT){
                        game->current_block = T_VOID+1;
                    }
                    break;
            }
            break;
        case I_MOUSE:
            game->mx = v1;
            game->my = v2;
            break;
        case I_LEFTCLICK:
            raycast(&game->player, RAYCAST_DISTANCE, _game_break_block, game);
            break;
        case I_RIGHTCLICK:
            raycast(&game->player, RAYCAST_DISTANCE, _game_place_block, game);
            break;
        default:
            break;
    }
}

void game_respawn(Game *game) {
    /* TODO: Determine the spawn position correctly */
    game->player.x = 0;
    if(game->mode == M_SPECTATOR){
        game->player.y = 0;
    }else{
        game->player.y = CHUNK_HEIGHT/2;
    }
    game->player.z = 0;
    game->world.x = -(RENDER_DISTANCE*CHUNK_WIDTH+CHUNK_WIDTH/2);
    game->world.y = -(RENDER_DISTANCE*CHUNK_DEPTH+CHUNK_DEPTH/2);
    game->player.ry = 0;
    game->player.rx = 0;
    game->player.velocity = 0;
    game->player.y_velocity = 0;
    /* TODO: Refactor chunk loading to avoid having to reload the world to
     * avoid issues */
    world_generate_data(&game->world);
}

void game_logic(Game *game, float delta) {
    /* Variables to calculate the player rotation */
    int cx = gfx_get_width()/2;
    int cy = gfx_get_height()/2;
    int mov_x, mov_y;
    switch(game->screen) {
        case D_INGAME:
            game->mov_speed = 10*delta;
            /* Update the player rotation depending on the mouse position */
            if(!game->mov_speed) return;
            if(game->focus){
                mov_x = game->mx-cx;
                mov_y = game->my-cy;
                game->player.ry += (float)mov_x/game->rot_speed;
                game->player.rx += (float)mov_y/game->rot_speed;
                if(game->player.rx > 90) game->player.rx = 90;
                else if(game->player.rx < -90) game->player.rx = -90;
                gfx_set_pointer_pos(cx, cy);
                game->mx = cx;
                game->my = cy;
            }
            /* Update the player */
            if(game->moved) game->player.deceleration = 0.05;
            else game->player.deceleration = 0.2;
            if(game->player.velocity > 1) game->player.velocity = 1;
            if(game->player.velocity < -1) game->player.velocity = -1;
            game->moved = 0;
            entity_update(&game->player, &game->world, delta);
            /* Update the world */
            world_update(&game->world, game->player.x, game->player.z);
            if(game->mode != M_SPECTATOR && game->player.y < -CHUNK_HEIGHT){
                game_respawn(game);
            }
            break;
        default:
            game->screen = D_INGAME;
    }
}

int _game_selection_draw(int x, int y, int z, void *data) {
    if(!blocks[world_get_tile(data, x, y, z)].replaceable){
        gfx_render_wire_cube(x, y-CHUNK_HEIGHT/2, z, 1.01);
        return 1;
    }
    /* gfx_render_wire_cube(x, y-CHUNK_HEIGHT/2, z, 1); */
    return 0;
}

void game_draw(Game *game, float delta) {
    int crosshair_x, crosshair_y;
    int fps;
    switch(game->screen) {
        case D_INGAME:
            crosshair_x = (int)((float)gfx_get_width()/2-
                                    (crosshair_width*game->gui_scale)/2);
            crosshair_y = (int)((float)gfx_get_height()/2-
                                    (crosshair_height*game->gui_scale)/2);
            fps = 0;
            if(delta) fps = 1/delta;

            gfx_set_camera(game->player.x, game->player.y, game->player.z,
                           game->player.rx, game->player.ry, 0);

            sprintf(game->fps_str, "FPS: %d", fps);
            sprintf(game->pos_str, "X: %.02f Y: %.02f Z: %.02f", game->player.x,
                    game->player.y, game->player.z);
            world_render(&game->world);
            /* Display the selected block */
            raycast(&game->player, RAYCAST_DISTANCE, _game_selection_draw,
                    &game->world);
            /* gfx_render_wire_cube((int)player.x, (int)player.y, (int)player.z,
             *                      1); */
            /* gfx_render_wire_cube(player.x, player.y, player.z, 1); */

            gfx_start_2d();
            gfx_draw_image(crosshair_x, crosshair_y, game->crosshair,
                           crosshair_width, crosshair_height, game->gui_scale);
            gfx_draw_string(0, 0, game->fps_str, game->font, 8, 8,
                            game->gui_scale+1);
            gfx_draw_string(0, 8+8*game->gui_scale, game->pos_str, game->font,
                            8, 8, game->gui_scale+1);
            gfx_draw_image_from_atlas(gfx_get_width()-32*game->gui_scale, 0,
                                      game->texture, 32, 32, game->gui_scale,
                                      16, 16, 256/16, 256/16,
                                      game->current_block-1);
            gfx_end_2d();
            break;
        default:
            game->screen = D_INGAME;
    }
}
void game_free(Game *game) {
    world_free(&game->world);
}

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

#include <buildconfig.h>

extern const Block_property blocks[T_AMOUNT];

void _game_sky(Game *game){
    gfx_set_clear_color(0.7, 0.9, 1.0);
    if(game->fog_enabled){
        gfx_enable_fog(0.7, 0.9, 1.0, 0.01, CHUNK_DEPTH-1,
                       CHUNK_DEPTH);
    }else{
        gfx_disable_fog();
    }
}

void _game_water(Game *game){
    gfx_set_clear_color(0.0, 0.1, 1.0);
    if(game->fog_enabled){
        gfx_enable_fog(0.0, 0.1, 1.0, 0.1, CHUNK_DEPTH-1,
                       CHUNK_DEPTH);
    }else{
        gfx_disable_fog();
    }
}

void game_init(Game *game, int seed) {
    game->gui_scale = 2;
    game->close_asked = 0;
    game->render_distance = RENDER_DISTANCE;

    game->texture = 0;
    game->crosshair = 0;
    game->font = 0;

    game->rot_speed = 4;

    game->seed = seed;

    game->focus = 1;

    game->fog_enabled = 1;

    game->screen = D_TITLE;

    game->texture = gfx_load_texture(blocks_width, blocks_height,
                               (unsigned char*)blocks_data);
    game->crosshair = gfx_load_texture(crosshair_width, crosshair_height,
                                 (unsigned char*)crosshair_data);
    game->font = gfx_load_texture(font_width, font_height,
                            (unsigned char*)font_data);
    button_init(&game->button_singleplayer, 0, 0, 64*(game->gui_scale+1),
                32*(game->gui_scale+1), "Singleplayer");
    button_init(&game->button_resume, 0, 0, 64*(game->gui_scale+1),
                32*(game->gui_scale+1), "Resume");
    button_init(&game->button_title, 0, 0, 64*(game->gui_scale+1),
                32*(game->gui_scale+1), "Title screen");
    _game_sky(game);

    gfx_enable_closing();
}

void game_input(Game *game, int v1, int v2, int type) {
    switch(game->screen) {
        case D_TITLE:
            switch(type){
                case I_LEFTCLICK:
                    if(button_mouse_inside(&game->button_singleplayer, game->mx,
                                           game->my)){
                        if(!world_init(&game->world, game->render_distance*2+1,
                                       game->render_distance*2+1, game->seed,
                                       game->texture, 1)){
                            game->player = game->world.players;
                            game->screen = D_INGAME;
                            gfx_cursor_hide();
                            game_respawn(game);
                            _game_sky(game);
                            if(game->focus){
                                gfx_cursor_hide();
                            }else{
                                gfx_cursor_show();
                            }
                            gfx_disable_closing();
                        }
                    }
                    break;
                case I_RIGHTCLICK:
                    break;
                case I_MOUSE:
                    game->mx = v1;
                    game->my = v2;
                    break;
            }
            break;
        case D_PAUSE:
            switch(type){
                case I_LEFTCLICK:
                    if(button_mouse_inside(&game->button_resume, game->mx,
                                           game->my) && !game->close_asked){
                        game->screen = D_INGAME;
                        gfx_cursor_hide();
                    }
                    if(button_mouse_inside(&game->button_title, game->mx,
                                           game->my)){
                        game->close_asked = 1;
                    }
                    break;
                case I_RIGHTCLICK:
                    break;
                case I_MOUSE:
                    game->mx = v1;
                    game->my = v2;
                    break;
            }
            break;
        case D_INGAME:
            switch(type){
                case I_KEYPRESS:
                    switch(v1) {
                        case 'w':
                            /* FALLTHRU */
                        case 'z':
                            player_move(game->player, game->delta, 1);
                            break;
                        case 's':
                            player_move(game->player, game->delta, -1);
                            break;
                        case ' ':
                            player_jump(game->player, &game->world);
                            break;
                        case 'a':
                            player_sneak(game->player);
                    }
                    break;
                case I_KEYRELEASE:
                    switch(v1) {
                        case 'p':
                            game->screen = D_PAUSE;
                            gfx_cursor_show();
                            break;
                        case 'f':
                            game->fog_enabled = !game->fog_enabled;
                            if(entity_in_water(&game->player->entity,
                                               &game->world)){
                                _game_water(game);
                            }else{
                                _game_sky(game);
                            }
                            break;
                        case 'x':
                            game->gui_scale += 0.5;
                            break;
                        case 'c':
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
                            /* Add proper support for an inventory */
                            game->player->current_block--;
                            if(game->player->current_block <= T_VOID){
                                game->player->current_block = T_AMOUNT-1;
                            }
                            break;
                        case 'o':
                            /* Add proper support for an inventory */
                            game->player->current_block++;
                            if(game->player->current_block >= T_AMOUNT){
                                game->player->current_block = T_VOID+1;
                            }
                            break;
                        case 'y':
                            if(game->render_distance > 1){
                                game->render_distance--;
                                if(world_change_size(&game->world,
                                                 game->render_distance*2+1,
                                                 game->render_distance*2+1)){
                                    game->render_distance++;
                                }
                            }
                            break;
                        case 'u':
                            game->render_distance++;
                            if(world_change_size(&game->world,
                                                 game->render_distance*2+1,
                                                 game->render_distance*2+1)){
                                game->render_distance--;
                            }
                            break;
                        case 't':
                            game->player->mode++;
                            if(game->player->mode >= M_AMOUNT){
                                game->player->mode = 0;
                            }
                    }
                    break;
                case I_MOUSE:
                    game->mx = v1;
                    game->my = v2;
                    break;
                case I_LEFTCLICK:
                    player_break_block(game->player, &game->world);
                    break;
                case I_RIGHTCLICK:
                    player_place_block(game->player, &game->world);
                    break;
                default:
                    break;
            }
            break;
    }
}

void game_respawn(Game *game) {
    player_respawn(game->player, &game->world);
}

void game_logic(Game *game, float delta) {
    /* Variables to calculate the player rotation */
    int cx = gfx_get_width()/2;
    int cy = gfx_get_height()/2;
    int mov_x, mov_y;
    game->delta = delta;
    switch(game->screen) {
        case D_TITLE:
            game->button_singleplayer.x =
                    (gfx_get_width()-game->button_singleplayer.w)/2;
            game->button_singleplayer.y =
                    (gfx_get_height()-game->button_singleplayer.h)/2;
            break;
        case D_PAUSE:
            game->button_resume.x = (gfx_get_width()-game->button_resume.w)/2;
            game->button_resume.y = (gfx_get_height()-game->button_resume.h)/2;
            game->button_title.x = (gfx_get_width()-game->button_title.w)/2;
            game->button_title.y = (gfx_get_height()+game->button_resume.h)/2+8*
                    game->gui_scale;
            if(game->close_asked && game->world.finished){
                game->screen = D_TITLE;
                world_free(&game->world);
                _game_sky(game);
                gfx_enable_closing();
                game->close_asked = 0;
            }
            break;
        case D_INGAME:
            /* Update the player rotation depending on the mouse position */
            if(game->focus){
                mov_x = game->mx-cx;
                mov_y = game->my-cy;
                game->player->entity.ry += (float)mov_x/game->rot_speed;
                game->player->entity.rx += (float)mov_y/game->rot_speed;
                if(game->player->entity.rx > 90){
                    game->player->entity.rx = 90;
                }else if(game->player->entity.rx < -90){
                    game->player->entity.rx = -90;
                }
                gfx_set_pointer_pos(cx, cy);
                game->mx = cx;
                game->my = cy;
            }
            /* Update the player */
            player_update(game->player, &game->world, delta);
            if(entity_in_water(&game->player->entity, &game->world)){
                _game_water(game);
            }else{
                _game_sky(game);
            }
            if(game->player->mode != M_SPECTATOR){
                if(game->player->mode != M_SPECTATOR &&
                   game->player->entity.y < -CHUNK_HEIGHT){
                    game_respawn(game);
                }
            }
            /* Update the world */
            world_update(&game->world);
            break;
        default:
            game->screen = D_INGAME;
    }
}

int _game_selection_draw(int x, int y, int z, void *data) {
    if(!blocks[world_get_tile(data, x, y, z)].replaceable){
        gfx_set_color(0, 0, 0, 1);
        gfx_render_wire_cube(x, y-CHUNK_HEIGHT/2, z, 1.01);
        return 1;
    }
    /* gfx_render_wire_cube(x, y-CHUNK_HEIGHT/2, z, 1); */
    return 0;
}

void game_draw(Game *game, float delta) {
    int crosshair_x, crosshair_y;
    int fps;
    char *modes[M_AMOUNT] = {
        "Spectator",
        "Creative",
        "Survival"
    };
    switch(game->screen) {
        case D_TITLE:
            gfx_cursor_show();
            gfx_start_2d();

            gfx_set_color(1, 1, 1, 1);
            gfx_draw_string((gfx_get_width()-sizeof(NAME)*8*
                            game->gui_scale)/2, 32,
                            NAME, game->font, 8, 8, game->gui_scale);
            gfx_draw_string((gfx_get_width()-sizeof(COPYRIGHT)*8*
                            game->gui_scale)/2, 32+8*
                            game->gui_scale, COPYRIGHT, game->font, 8, 8,
                            game->gui_scale);
            gfx_draw_string(8, gfx_get_height()-16-8*game->gui_scale,
                            VERSION, game->font, 8, 8, game->gui_scale);
            button_draw(&game->button_singleplayer, game->mx, game->my,
                        game->gui_scale, game->font);
            gfx_end_2d();
            break;
        case D_PAUSE:
            world_render(&game->world);
            gfx_start_2d();
            button_draw(&game->button_resume, game->mx, game->my,
                        game->gui_scale, game->font);
            button_draw(&game->button_title, game->mx, game->my,
                        game->gui_scale, game->font);
            gfx_end_2d();
            break;
        case D_INGAME:
            crosshair_x = (int)((float)gfx_get_width()/2-
                                    (crosshair_width*game->gui_scale/2)/2);
            crosshair_y = (int)((float)gfx_get_height()/2-
                                    (crosshair_height*game->gui_scale/2)/2);
            fps = 0;
            if(delta) fps = 1/delta;

            gfx_set_camera(game->player->entity.x, game->player->entity.y,
                           game->player->entity.z, game->player->entity.rx,
                           game->player->entity.ry, 0);

            sprintf(game->fps_str, "FPS: %d", fps);
            sprintf(game->pos_str, "X: %.02f Y: %.02f Z: %.02f",
                    game->player->entity.x, game->player->entity.y,
                    game->player->entity.z);
            sprintf(game->render_distance_str, "Render distance: %d",
                    game->render_distance);
            world_render(&game->world);
            /* Display the selected block */
            raycast(&game->player->entity, RAYCAST_DISTANCE,
                    _game_selection_draw, &game->world);
            /* gfx_render_wire_cube((int)player.x, (int)player.y, (int)player.z,
             *                      1); */
            /* gfx_render_wire_cube(player.x, player.y, player.z, 1); */

            gfx_start_2d();
            gfx_draw_image(crosshair_x, crosshair_y, game->crosshair,
                           crosshair_width, crosshair_height,
                           game->gui_scale/2);
            gfx_set_color(1, 1, 1, 1);
            gfx_draw_string(0, 0, game->fps_str, game->font, 8, 8,
                            game->gui_scale);
            gfx_draw_string(0, 8+8*game->gui_scale, game->pos_str, game->font,
                            8, 8, game->gui_scale);
            gfx_draw_string(0, gfx_get_height()-8-8*game->gui_scale,
                            game->render_distance_str, game->font, 8, 8,
                            game->gui_scale);
            gfx_draw_string(gfx_get_width()-strlen(modes[game->player->mode])*8
                            *game->gui_scale, gfx_get_height()-8-8*
                            game->gui_scale, modes[game->player->mode],
                            game->font, 8, 8, game->gui_scale);

            gfx_draw_image_from_atlas(gfx_get_width()-32*game->gui_scale, 0,
                                      game->texture, 32, 32, game->gui_scale,
                                      16, 16, 256/16, 256/16,
                                      game->player->current_block-1);
            gfx_end_2d();
            break;
        default:
            game->screen = D_INGAME;
    }
}
void game_free(Game *game) {
    world_free(&game->world);
}

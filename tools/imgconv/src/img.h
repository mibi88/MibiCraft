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

#ifndef IMG_H
#define IMG_H

typedef struct {
    unsigned int width, height;
    int bit_depth;
    int color_type;
    unsigned char *data;
    unsigned char **rows;
    long row_bytes;
} GFXImage;

enum {
    OUT_SUCCESS,
    OUT_ERROR_NOT_PNG,
    OUT_ERROR_FILE,
    OUT_ERROR_OUT_OF_MEM,
    OUT_ERROR_UNKNOWN
};

int img_load(char *file, GFXImage *image);
void img_free(GFXImage *image);

#endif

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

#include <img.h>

#include <stdlib.h>

#include <png.h>

#define CHECK_NUM 8

int img_load(char *file, GFXImage *image) {
    png_byte header[CHECK_NUM];
    int is_png;

    png_structp png_ptr;
    png_infop info_ptr;

    png_uint_32 i;

    /* Check if the file is a png image. */
    FILE *fp = fopen(file, "rb");
    if(!fp){
        return OUT_ERROR_FILE;
    }
    fread(header, 1, CHECK_NUM, fp);
    is_png = !png_sig_cmp(header, 0, CHECK_NUM);
    if(!is_png){
        fclose(fp);
        return OUT_ERROR_NOT_PNG;
    }

    /* Allocate memory. */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                                                 NULL, NULL);
    if(!png_ptr){
        fclose(fp);
        return OUT_ERROR_OUT_OF_MEM;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr){
        fclose(fp);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return OUT_ERROR_OUT_OF_MEM;
    }

    /* Initialize everything */
    if(setjmp(png_jmpbuf(png_ptr))){
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return OUT_ERROR_UNKNOWN;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    /* Get some useful informations about the image */
    png_get_IHDR(png_ptr, info_ptr, &image->width, &image->height,
            &image->bit_depth, &image->color_type, NULL, NULL, NULL);

    /* Prepare for reading */
    if(image->color_type == PNG_COLOR_TYPE_PALETTE){
        png_set_expand(png_ptr);
    }
    if(image->color_type == PNG_COLOR_TYPE_GRAY && image->bit_depth < 8){
        png_set_expand(png_ptr);
    }
    if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)){
        png_set_expand(png_ptr);
    }

    png_read_update_info(png_ptr, info_ptr);

    /* Read! */
    image->rows = malloc(sizeof(png_bytep)*image->height);
    if(!image->rows){
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return OUT_ERROR_OUT_OF_MEM;
    }

    image->row_bytes = png_get_rowbytes(png_ptr, info_ptr);

    image->data = malloc(image->height*image->row_bytes);
    if(!image->data){
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        free(image->rows);
        return OUT_ERROR_OUT_OF_MEM;
    }

    for(i=0;i<image->height;i++){
        image->rows[i] = image->data+i*image->row_bytes;
    }

    png_read_image(png_ptr, image->rows);

    png_read_end(png_ptr, NULL);

    /* Free everything */
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    free(image->rows);

    fclose(fp);

    return OUT_SUCCESS;
}

void img_free(GFXImage *image) {
    free(image->data);
}

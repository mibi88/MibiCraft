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

#include <stdlib.h>
#include <stdio.h>

#include <parser.h>

int main(int argc, char **argv) {
    size_t size;
    char *data;
    FILE *fp;
    FILE *header_fp;
    FILE *source_fp;

    if(argc < 4){
        fputs("Need more args!\n", stderr);
        return EXIT_FAILURE;
    }

    fp = fopen(argv[1], "r");
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);

    data = malloc(size);
    if(!data){
        fputs("Out of memory!\n", stderr);
        fclose(fp);
        return EXIT_FAILURE;
    }

    fread(data, 1, size, fp);
    fclose(fp);

    /* Load the data and generate the C files */
    header_fp = fopen(argv[2], "w");
    source_fp = fopen(argv[3], "w");

    /* TODO: Generate the files */
    fclose(header_fp);
    fclose(source_fp);

    free(data);
    puts("Data generated!");
    return EXIT_SUCCESS;
}

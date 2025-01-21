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

#ifndef TREE_H
#define TREE_H

#include <stddef.h>

enum {
    N_FUNCTION,
    N_NAME,
    N_INT,
    N_BOOL,
    N_FLOAT,
    N_STR,
    N_AMOUNT
};

typedef struct {
    /* List of pointers to the child nodes */
    void **childs;
    size_t child_num;
    void *data;
    size_t size;
    unsigned char type;
} Node;

void node_init(Node *node, void *value, size_t size, int type);
void node_free(Node *node);

#endif

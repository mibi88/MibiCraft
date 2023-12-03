#ifndef RAYCAST_H
#define RAYCAST_H

#include <world.h>

void raycast(World *world, int *cx, int *cy, int *cz, float sx, float sy,
             float sz, float rx, float ry, float length);

#endif

#ifndef RAYCAST_H
#define RAYCAST_H

#include <world.h>
#include <entity.h>

void raycast(World *world, Entity *player, float len,
             int voxel(int x, int y, int z, void *data));

#endif

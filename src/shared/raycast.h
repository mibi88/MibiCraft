#ifndef RAYCAST_H
#define RAYCAST_H

#include <world.h>
#include <entity.h>

void raycast(Entity *player, float len,
             int voxel(int x, int y, int z, void *data), void *data);

#endif

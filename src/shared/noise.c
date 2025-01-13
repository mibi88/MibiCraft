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

#include <noise.h>

#include <math.h>

int _noise_xorshift(int seed) {
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return seed;
}

#define SMOOTHSTEP(x) ((x) > 0 ? ((x) < 1 ? 3*(x)*(x)-2*(x)*(x)*(x) : 1) : 0)

#define PRECISION 8
#define MASK ((1<<(PRECISION))-1)

float noise_2d(float sx, float sy, int seed) {
    /* 2D perlin noise */
    int x, y;
    float x1, x2, x3, x4;
    float y1, y2, y3, y4;
    float p1, p2, p3, p4;
    float v, v1, v2;
    int tmp;
    x = floor(sx);
    y = floor(sy);
    seed = (seed != 0 ? seed : 1);
    tmp = _noise_xorshift(seed*x*y);
    /* Calculate the gradient vectors */
    x1 = ((tmp&MASK)/(1<<PRECISION))*2-1;
    y1 = ((_noise_xorshift(tmp)&MASK)/(1<<PRECISION))*2-1;
    tmp = _noise_xorshift(seed*(x+1)*y);
    x2 = ((tmp&MASK)/(1<<PRECISION))*2-1;
    y2 = ((_noise_xorshift(tmp)&MASK)/(1<<PRECISION))*2-1;
    tmp = _noise_xorshift(seed*x*(y+1));
    x3 = ((tmp&MASK)/(1<<PRECISION))*2-1;
    y3 = ((_noise_xorshift(tmp)&MASK)/(1<<PRECISION))*2-1;
    tmp = _noise_xorshift(seed*(x+1)*(y+1));
    x4 = ((tmp&MASK)/(1<<PRECISION))*2-1;
    y4 = ((_noise_xorshift(tmp)&MASK)/(1<<PRECISION))*2-1;
    /* Calculate the dot product between the offset vectors and the gradient
     * vectors. */
    p1 = (sx-x)*2*x1+(sy-y)*2*y1;
    p2 = (sx-x-1)*2*x2+(sy-y)*2*y2;
    p3 = (sx-x)*2*x3+(sy-y-1)*2*y3;
    p4 = (sx-x-1)*2*x4+(sy-y-1)*2*y4;
    v1 = (p1+(p2-p1)*SMOOTHSTEP(sx-x));
    v2 = (p3+(p4-p3)*SMOOTHSTEP(sx-x));
    v = v1+(v2-v1)*SMOOTHSTEP(sy-y);
    return (v > -1 ? (v < 1 ? v : 1) : -1);
}

float noise_3d(float sx, float sy, float sz, int seed) {
    /* 3D perlin noise */
    int i;
    int x, y, z;
    float vx[8];
    float vy[8];
    float vz[8];
    float p[8];
    float v, v1, v2, v3, v4, v5, v6;
    int tmp;
    x = floor(sx);
    y = floor(sy);
    z = floor(sz);
    seed = (seed != 0 ? seed : 1);
    tmp = _noise_xorshift(seed*x*y);
    /* Calculate the gradient vectors */
    for(i=0;i<8;i++){
        tmp = _noise_xorshift(seed*(x+(i&1))*(y+((i>>1)&1))*
                              (z+((i>>2)&1)));
        vx[i] = ((tmp&MASK)/(float)(1<<PRECISION))*2-1;
        tmp = _noise_xorshift(tmp);
        vy[i] = ((tmp&MASK)/(float)(1<<PRECISION))*2-1;
        vz[i] = ((_noise_xorshift(tmp)&MASK)/(float)(1<<PRECISION))*2-1;
    }
    /* Calculate the dot product between the offset vectors and the gradient
     * vectors. */
    for(i=0;i<8;i++){
        p[i] = (sx-x-(i&1))*2*vx[i]+(sy-y-((i>>1)&1))*2*vy[i]+
                (sz-z-((i>>2)&1))*2*vz[i];
    }
    /* Using smoothstep for interpolation */
    v1 = (p[0]+(p[1]-p[0])*SMOOTHSTEP(sx-x));
    v2 = (p[2]+(p[3]-p[2])*SMOOTHSTEP(sx-x));
    v3 = v1+(v2-v1)*SMOOTHSTEP(sy-y);
    v4 = (p[4]+(p[5]-p[4])*SMOOTHSTEP(sx-x));
    v5 = (p[6]+(p[7]-p[6])*SMOOTHSTEP(sx-x));
    v6 = v4+(v5-v4)*SMOOTHSTEP(sy-y);
    v = v3+(v6-v3)*SMOOTHSTEP(sz-z);
    return (v > -1 ? (v < 1 ? v : 1) : -1);
}

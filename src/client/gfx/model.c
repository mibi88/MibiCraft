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

#include <gfx.h>

#include <string.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include <string.h>

#define STARTSWITH(string, start) !strncmp(string, start, (int)strlen(start))

#include "data.h"

void gfx_create_modelview_matrix(float x, float y, float z, float rx, float ry,
                                 float rz);

void gfx_init_model(GFXModel *model, void *vertices, void *indices,
                    void *uv_coords, int texture, int has_indices,
                    int has_texture, int triangles, int vertex_type,
                    int index_type, int texture_type) {
    model->vertices = vertices;
    model->indices = indices;
    model->uv_coords = uv_coords;

    model->texture = texture;

    model->has_indices = has_indices;
    model->has_texture = has_texture;

    model->triangles = triangles;

    model->vertex_type = vertex_type;
    model->index_type = index_type;
    model->texture_type = texture_type;
}

void gfx_draw_model(GFXModel *model, float x, float y, float z, float rx,
                    float ry, float rz) {
    int i;

    const int gl_types[TYPE_AMOUNT] = {
        GL_INT,
        GL_UNSIGNED_INT,
        GL_FLOAT,
        GL_SHORT,
        GL_UNSIGNED_SHORT,
        GL_BYTE,
        GL_UNSIGNED_BYTE
    };

    glPushMatrix();

    if(model->has_texture){
        glBindTexture(GL_TEXTURE_2D, model->texture);
    }

    gfx_create_modelview_matrix(x, y, z, rx, ry, rz);

    if(_config.use_arrays || 1){
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, gl_types[model->vertex_type], 0, model->vertices);
        glTexCoordPointer(2, gl_types[model->texture_type], 0,
                          model->uv_coords);

        glDrawElements(GL_TRIANGLES, model->triangles*3,
                       gl_types[model->index_type], model->indices);
    }else{
#if 0 /* TODO: Fix this */
        glBegin(GL_TRIANGLES);
        for(i=0;i<model->triangles;i++){
            float x1, y1, z1;
            float x2, y2, z2;
            float x3, y3, z3;

            float u1, v1;
            float u2, v2;
            float u3, v3;
            int indice1, indice2, indice3;
            if(model->has_indices){
                indice1 = model->indices[i*3];
                indice2 = model->indices[i*3+1];
                indice3 = model->indices[i*3+2];

                x1 = model->vertices[indice1*3];
                y1 = model->vertices[indice1*3+1];
                z1 = model->vertices[indice1*3+2];

                x2 = model->vertices[indice2*3];
                y2 = model->vertices[indice2*3+1];
                z2 = model->vertices[indice2*3+2];

                x3 = model->vertices[indice3*3];
                y3 = model->vertices[indice3*3+1];
                z3 = model->vertices[indice3*3+2];

                u1 = model->uv_coords[indice1*2];
                v1 = model->uv_coords[indice1*2+1];

                u2 = model->uv_coords[indice2*2];
                v2 = model->uv_coords[indice2*2+1];

                u3 = model->uv_coords[indice3*2];
                v3 = model->uv_coords[indice3*2+1];
            }else{
                x1 = model->vertices[i*9];
                y1 = model->vertices[i*9+1];
                z1 = model->vertices[i*9+2];

                x2 = model->vertices[i*9+3];
                y2 = model->vertices[i*9+4];
                z2 = model->vertices[i*9+5];

                x3 = model->vertices[i*9+6];
                y3 = model->vertices[i*9+7];
                z3 = model->vertices[i*9+8];

                u1 = model->uv_coords[i*6];
                v1 = model->uv_coords[i*6+1];

                u2 = model->uv_coords[i*6+2];
                v2 = model->uv_coords[i*6+3];

                u3 = model->uv_coords[i*6+4];
                v3 = model->uv_coords[i*6+5];
            }
            glTexCoord2f(u1, v1);
            glVertex3f(x1, y1, z1);
            glTexCoord2f(u2, v2);
            glVertex3f(x2, y2, z2);
            glTexCoord2f(u3, v3);
            glVertex3f(x3, y3, z3);
        }
        glEnd();
#endif
    }

    glPopMatrix();
}

void gfx_free(GFXModel *model) {
    glDeleteTextures(1, &model->texture);
}

void gfx_load_obj(FILE *fp) {
    char line[LINE_MAX_SIZE];
    float value = 0;
    while(fgets(line, LINE_MAX_SIZE, fp)){
        value = atof(line);
        if(STARTSWITH(line, "v")){
            /* Vertex */
        }else if(STARTSWITH(line, "vt")){
            /* Texture coordinates */
        }else if(STARTSWITH(line, "vn")){
            /* Normals */
        }else if(STARTSWITH(line, "f")){
            /* Triangles */
        }
    }
}

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
#include <GL/freeglut.h>

#define STARTSWITH(string, start) !strncmp(string, start, (int)strlen(start))

void (*_draw)(int);
void (*_keypress)(int);

int _w = 0, _h = 0;

int _old_time = 0;

float _cam_x, _cam_y, _cam_z, _cam_rx, _cam_ry, _cam_rz;

int _keys[256];

int gfx_get_time(void) {
    return glutGet(GLUT_ELAPSED_TIME);
}

void gfx_set_pointer_pos(int x, int y) {
    glutWarpPointer(x, y);
}

int gfx_get_width(void) {
    return _w;
}

int gfx_get_height(void) {
    return _h;
}

void gfx_enable_fog(float r, float g, float b, float density, float start,
                    float end) {
    GLfloat color[4] = {0.0, 0.0, 0.0, 1.0};
    color[0] = r;
    color[1] = g;
    color[2] = b;
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogfv(GL_FOG_COLOR, color);
    glFogf(GL_FOG_DENSITY, density);
    glHint(GL_FOG_HINT, GL_DONT_CARE);
    glFogf(GL_FOG_START, start);
    glFogf(GL_FOG_END, end);
}

void gfx_set_clear_color(float r, float g, float b) {
    glClearColor(r, g, b, 1.0f);
}

void gfx_set_color(float r, float g, float b) {
    glColor3f(r, g, b);
}

void gfx_set_camera(float x, float y, float z, float rx, float ry, float rz) {
    _cam_x = x;
    _cam_y = y;
    _cam_z = z;
    
    _cam_rx = rx;
    _cam_ry = ry;
    _cam_rz = rz;
}

void gfx_draw_point(float x, float y, float z, int size) {
    glPointSize(size);
    glBegin(GL_POINTS);
    glVertex3f(x, y, z);
    glEnd();
}

void gfx_draw_model(GFXModel *model, float x, float y, float z, float rx,
                    float ry, float rz) {
    int i;
    
    glPushMatrix();
    
    if(model->has_texture){
        glBindTexture(GL_TEXTURE_2D, model->texture);
    }
    
    glLoadIdentity();
    glRotatef(_cam_rx, 1, 0, 0);
    glRotatef(_cam_ry, 0, 1, 0);
    glRotatef(_cam_rz, 0, 0, 1);
    glTranslatef(-_cam_x, -_cam_y, -_cam_z);
    glTranslatef(x, y, z);
    glRotatef(rx, 1, 0, 0);
    glRotatef(ry, 0, 1, 0);
    glRotatef(rz, 0, 0, 1);
    
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
        
    glPopMatrix();
}

void gfx_init(int *argc, char **argv, char *title) {
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    glutSetWindowTitle(title);
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
}

void _display(void) {
    int i, frame_time;
    
    frame_time = gfx_get_time()-_old_time;
    _old_time = gfx_get_time();

    for(i=0;i<256;i++){
        if(_keys[i]){
            _keypress(i);
        }
    }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    _draw(frame_time);

    glFlush();
    glDisable(GL_TEXTURE_2D);
    glutSwapBuffers();
}

void _reshape(int w, int h) {
    _w = w;
    _h = h;
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(72, (float) w/(float) h, 0.1, 1000);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0.0, 0.0, -3.6);
}

void _keyboard(unsigned char key, int x, int y) {
    _keys[key] = 1;
}

void _release(unsigned char key, int x, int y) {
    _keys[key] = 0;
}

void _idle(void) {
    glutPostRedisplay();
}

void gfx_run(void draw(int), void keypress(int), void mouse(int, int)) {
    _old_time = gfx_get_time();
    _draw = draw;
    _keypress = keypress;
    glutDisplayFunc(_display);
    glutIdleFunc(_idle);
    glutReshapeFunc(_reshape);
    glutKeyboardFunc(_keyboard);
    glutKeyboardUpFunc(_release);
    glutPassiveMotionFunc(mouse);
    glutMotionFunc(mouse);
    glutMainLoop();
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

unsigned int gfx_load_texture(int width, int height,
                              unsigned char *texture_data) {
    unsigned int id;
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
                   GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                   GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, 
                height, 0, GL_RGB, GL_UNSIGNED_BYTE, 
                texture_data);
    
    return id;
}



#ifndef _GLFUNCS_H_
#define _GLFUNCS_H_

#include <GL/gl.h>
#include <SDL.h>
#include <SDL_image.h>

typedef struct {
    int width,height;
    int glwidth,glheight;
    GLfloat fx1,fy1,fx2,fy2;
    GLuint  image;
} noegnud_glfuncs_timageblock;

#define NOEGNUD_GLFUNCS_BLOCK_MAX 32

typedef struct {
    int width, height;
    int block_width;
    int block_height;
    noegnud_glfuncs_timageblock *block[NOEGNUD_GLFUNCS_BLOCK_MAX][NOEGNUD_GLFUNCS_BLOCK_MAX];
} noegnud_glfuncs_timage;

void noegnud_glfuncs_init ();
noegnud_glfuncs_timage *noegnud_glfuncs_loadimage (const char *filename, int split, int tileable, int forcemipmaps);
void noegnud_glfuncs_unloadimage(noegnud_glfuncs_timage *image);

#endif

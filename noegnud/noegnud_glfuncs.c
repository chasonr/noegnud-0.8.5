#include "noegnud_interface.h"
#include "noegnud_glfuncs.h"
#include "noegnud_extern.h"
#include "noegnud_common.h"
#include "noegnud_mem.h"
#include "noegnud_config.h"

#define VIEW_MIN 0.1
#define VIEW_MAX 20.0

int noegnud_glfuncs_maximum_texturesize;

static void noegnud_glfuncs_cheaplight_init() {
    GLfloat ambient[4];
    GLfloat diffuse[4];
    GLfloat specular[4];

    ambient[0]=ambient[1]=ambient[2]=0.2;
    ambient[3]=0.0;

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient);

    glEnable(GL_COLOR_MATERIAL);

    ambient[0]=ambient[1]=ambient[2]=0.1;
    ambient[3]=0.0;
    diffuse[0]=diffuse[1]=diffuse[2]=1.0;
    diffuse[3]=1.0;
    specular[0]=specular[1]=specular[2]=1.0;
    specular[3]=0.0;

    glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,specular);

    ambient[0]=ambient[1]=ambient[2]=0.0;
    ambient[3]=0.0;
    diffuse[0]=diffuse[1]=diffuse[2]=1.0;
    diffuse[3]=0.5;
    specular[0]=specular[1]=specular[2]=1.0;
    specular[3]=1.0;
    glLightfv(GL_LIGHT1,GL_AMBIENT,ambient);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,diffuse);
    glLightfv(GL_LIGHT1,GL_SPECULAR,specular);
}

static void noegnud_glfuncs_maximum_texturesize_init() {
    GLint texture;

    for (noegnud_glfuncs_maximum_texturesize=noegnud_options_opengl_texture_max->value;noegnud_glfuncs_maximum_texturesize>64;noegnud_glfuncs_maximum_texturesize/=2) {
	glGenTextures (1, &texture);
	glBindTexture (GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, 4, noegnud_glfuncs_maximum_texturesize, noegnud_glfuncs_maximum_texturesize, 0,GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	if (glGetError()==GL_NO_ERROR) {
	    glDeleteTextures(1,&texture);
	    break;
	}
	glDeleteTextures(1,&texture);
    }

    printf("* GL Maximum Texture 2n Dimension: %d\n",noegnud_glfuncs_maximum_texturesize);
}

void noegnud_glfuncs_init () {
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    glPointSize (1.0);
    glClearColor(0.0,0.0,0.0,1.0);
    glClearDepth (1000.0);
    glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc (GL_LEQUAL);
    glDisable (GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);

    noegnud_glfuncs_cheaplight_init();
    noegnud_glfuncs_maximum_texturesize_init();

}


noegnud_glfuncs_timage *noegnud_glfuncs_loadimage (const char *fname, int split, int tileable, int forcemipmaps) {
    int req_width, req_height;

    SDL_Surface *image;
    SDL_Surface *tmp;
    SDL_Rect rect;
    SDL_Rect tmprect;

    noegnud_glfuncs_timage *glimage;

    char *filename;
    char *szHomedir;

    int block_x, block_y;

    filename=noegnud_mem_malloc(strlen(fname)+128);

    szHomedir=noegnud_options_get_home();
    sprintf(filename,"%s/%s",szHomedir,fname);
    noegnud_mem_free(szHomedir);
    if (!noegnud_common_file_exists(filename)) sprintf(filename,"../" NOEGNUD_DATADIR "/%s",fname);

#ifdef NOEGNUDDEBUG
    printf("[GLFUNCS] loadimage: loading \"%s\" (%s)\n",fname,filename);
#endif
    image=noegnud_mem_img_load(filename);
    if (!image) {
	printf("[GLFUNCS] WARNING: Could not load image \"%s\"\n",filename);
	noegnud_mem_free(filename);
	return NULL;
    }
    noegnud_mem_free(filename);

    if (image->format->BytesPerPixel == 4) SDL_SetAlpha (image, 0, 0);

    glimage=noegnud_mem_malloc(sizeof(noegnud_glfuncs_timage));

    glimage->width=image->w;
    glimage->height=image->h;

    if ((tileable||(!split))||(image->w<=noegnud_glfuncs_maximum_texturesize&&image->h<=noegnud_glfuncs_maximum_texturesize)) {
	glimage->block_width=1;
	glimage->block_height=1;
    } else {
	glimage->block_width=(image->w+(noegnud_glfuncs_maximum_texturesize-1))/noegnud_glfuncs_maximum_texturesize;
	glimage->block_height=(image->h+(noegnud_glfuncs_maximum_texturesize-1))/noegnud_glfuncs_maximum_texturesize;
    }

    for (block_y=0; block_y<glimage->block_height;block_y++)
	for (block_x=0; block_x<glimage->block_width;block_x++) {
	    glimage->block[block_x][block_y]=noegnud_mem_malloc(sizeof(noegnud_glfuncs_timageblock));


	    if ((tileable||(!split))||(image->w<=noegnud_glfuncs_maximum_texturesize&&image->h<=noegnud_glfuncs_maximum_texturesize)) {
		glimage->block[block_x][block_y]->width=image->w;
		glimage->block[block_x][block_y]->height=image->h;
	    } else {
		glimage->block[block_x][block_y]->width=noegnud_glfuncs_maximum_texturesize;
		if ((image->w-block_x*noegnud_glfuncs_maximum_texturesize)<glimage->block[block_x][block_y]->width) glimage->block[block_x][block_y]->width=(image->w-block_x*noegnud_glfuncs_maximum_texturesize);
		glimage->block[block_x][block_y]->height=noegnud_glfuncs_maximum_texturesize;
		if ((image->h-block_y*noegnud_glfuncs_maximum_texturesize)<glimage->block[block_x][block_y]->height) glimage->block[block_x][block_y]->height=(image->h-block_y*noegnud_glfuncs_maximum_texturesize);
	    }

	    req_width = noegnud_common_power2 (glimage->block[block_x][block_y]->width);
	    req_height = noegnud_common_power2 (glimage->block[block_x][block_y]->height);


	    glimage->block[block_x][block_y]->glwidth=req_width;
	    glimage->block[block_x][block_y]->glheight=req_height;

	    glimage->block[block_x][block_y]->fx1=glimage->block[block_x][block_y]->fy1=0.0;
	    glimage->block[block_x][block_y]->fx2=(float)glimage->block[block_x][block_y]->width/(float)req_width;
	    glimage->block[block_x][block_y]->fy2=(float)glimage->block[block_x][block_y]->height/(float)req_height;

	    tmp = noegnud_mem_sdl_creatergbsurface(SDL_SWSURFACE, req_width, req_height,
						   image->format->BitsPerPixel,
						   image->format->Rmask, image->format->Gmask,
						   image->format->Bmask, image->format->Amask);
	    if (image->format->BytesPerPixel==4) SDL_SetAlpha (tmp, 0, 0);

	    tmprect.x = tmprect.y = 0;
	    tmprect.w = tmp->w;
	    tmprect.h = tmp->h;
	    SDL_FillRect (tmp, &tmprect, 0x00000000);


	    tmprect.x = tmprect.y = 0;
	    tmprect.w = glimage->block[block_x][block_y]->width;
	    tmprect.h = glimage->block[block_x][block_y]->height;
	    SDL_FillRect (tmp, &tmprect, 0xFF000000);

	    tmprect.x=tmprect.y=0;
	    rect.x=block_x*noegnud_glfuncs_maximum_texturesize;
	    rect.y=block_y*noegnud_glfuncs_maximum_texturesize;
	    tmprect.w=rect.w=glimage->block[block_x][block_y]->width;
	    tmprect.h=rect.h=glimage->block[block_x][block_y]->height;
	    SDL_BlitSurface (image, &rect, tmp, &tmprect);

	    glGenTextures (1, &glimage->block[block_x][block_y]->image);
	    glBindTexture (GL_TEXTURE_2D, glimage->block[block_x][block_y]->image);


	    if (!noegnud_options_render_opengl_mipmapping->value) {
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    }
	    else if (split) {
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    } else {
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	    }

	    if ((!tileable)||(split)) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	    } else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    }


	    SDL_LockSurface (tmp);
	    if (tmp->format->BytesPerPixel == 4) {
		if ((noegnud_options_render_opengl_mipmapping->value)&&(tileable||(!split)||forcemipmaps)) {
		    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, req_width, req_height, GL_RGBA, GL_UNSIGNED_BYTE, tmp->pixels);
		} else {
		    glTexImage2D(GL_TEXTURE_2D, 0, 4, req_width, req_height, 0,GL_RGBA, GL_UNSIGNED_BYTE, tmp->pixels);
		}
	    } else {
		if ((noegnud_options_render_opengl_mipmapping->value)&&(tileable||(!split)||forcemipmaps)) {
		    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, req_width, req_height, GL_RGB, GL_UNSIGNED_BYTE, tmp->pixels);
		} else {
		    glTexImage2D(GL_TEXTURE_2D, 0, 3, req_width, req_height, 0,GL_RGB, GL_UNSIGNED_BYTE, tmp->pixels);
		}
	    }

	    SDL_UnlockSurface (tmp);

	    noegnud_mem_sdl_freesurface(tmp);

	}

    noegnud_mem_sdl_freesurface(image);

    return glimage;
};

void noegnud_glfuncs_unloadimage(noegnud_glfuncs_timage *image) {
    int block_x, block_y;
    for (block_y=0; block_y<image->block_height;block_y++)
	for (block_x=0; block_x<image->block_width;block_x++)
	    noegnud_mem_free(image->block[block_x][block_y]);
    noegnud_mem_free(image);
}

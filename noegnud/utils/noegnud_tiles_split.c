#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL.h>
#include <SDL_image.h>
#include <png.h>
#include <sys/stat.h>

#include "../noegnud_gff_tga.h"
#include "../noegnud_collection.h"
#include "../noegnud_typeloader.h"

int desttilesetsize=0;

void add_to_enumeration(char *setting, int nparams, char *params, void *data) {
	noegnud_tcollection **enumerator=data;
	char *tmp;
	
	tmp=malloc(strlen(params)+1);
	strcpy(tmp,params);

//	printf("%s::%s\n",setting,params);
	
	if (*enumerator) {
		noegnud_collection_add(*enumerator,setting,tmp);
	} else {
		*enumerator=noegnud_collection_create(setting,tmp);
	}
	desttilesetsize++;
}

void gethelp(const char *progname) {
    printf("Usage: %s <referencefile> <tileset.png> <width> <height> [destination]\n",progname);
    exit(-1);
}

int main(int argc, char **argv) {
	noegnud_tcollection *tileset_enumerate=NULL;
	
	SDL_Surface *nethack_image_surface;
	void *slashem_image;

	int y,x,dy,dx,ny,nx;
	
	Uint8 r,g,b,a;
	
	char *pixels;
	char *rgba;
	
	char buffer[12];
	char *fullname;
	int  slashtilenum;
	
	int width,height;
	int set_width, set_height;
	
	char fullpath[2048];
	char destdir[2048];
	char *generatename;
	
	int row;

	FILE * volatile fp;
	png_bytep * volatile row_pointers = NULL;
	png_structp	png_ptr  = NULL;
	png_infop	info_ptr = NULL;


	if (argc<5) {
	    printf(" * not enough parameters\n");
	    gethelp(argv[0]);
	}
	
	width=atoi(argv[3]);
	height=atoi(argv[4]);
	
	if ((!width)||(!height)) {
	    printf("* invalid width or height\n");
	    gethelp(argv[0]);
	}
	
	SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);
	
	noegnud_typeloader_parsefile(add_to_enumeration,argv[1],&tileset_enumerate);
	
	if (!tileset_enumerate) {
	    printf("* invalid reference file\n");
	    gethelp(argv[0]);
	}
	
	
	printf("loading image [%s]\n",argv[2]);
	nethack_image_surface=IMG_Load(argv[2]);
	if (!nethack_image_surface) {
	    printf("* invalid image file, or error loading image\n");
	    gethelp(argv[0]);
	}
	
	SDL_SetAlpha (nethack_image_surface, 0, 0);
	
	set_width=nethack_image_surface->w/width;
	set_height=nethack_image_surface->h/height;
	
	slashem_image=calloc(nethack_image_surface->format->BytesPerPixel,width*height);

	SDL_LockSurface(nethack_image_surface);
	

	if (argc>5) {
	    sprintf(destdir,"%s",argv[5]);
	} else {
	    generatename=argv[2];
	    while (*generatename) generatename++;
	    while (generatename>=argv[2]&&*generatename!='/') generatename--;
	    generatename++;
	    sprintf(destdir,"noegnudtileset_%s",generatename);
	}
	printf("storing split tiles in [%s]\n",destdir);
	mkdir(destdir,0xFFFFFF);

	puts("converting...");
	
	row_pointers = (png_bytep *) malloc(height * sizeof(png_bytep));
	
	for (y=0;y<set_height;y++) {
		for (x=0;x<set_width;x++) {
			if ( y*set_width+x>=desttilesetsize) continue;

		    sprintf(buffer,"%d",y*set_width+x);
			
			fullname=noegnud_collection_data(tileset_enumerate,buffer);
			sprintf(fullpath,"%s/%s.png",destdir,fullname);
			
			if (!fullname) {
				printf("**ERROR** : no name found for slashem tile no [%d]\n",slashtilenum);
				continue;
			}

			
		ny=slashtilenum/set_width;
		nx=slashtilenum%set_width;
			
		fp=fopen(fullpath,"wb");
		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		info_ptr = png_create_info_struct(png_ptr);
		
		if (setjmp(png_ptr->jmpbuf)) {
			fprintf(stderr, "tile2png: Unknown problem while writing PNG.\n");
			fclose(fp);
			continue;
		}
		
		png_init_io(png_ptr, fp);
		png_set_IHDR(png_ptr, info_ptr, width, height, 8,
			nethack_image_surface->format->BytesPerPixel==4 ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		png_write_info(png_ptr, info_ptr);
				
		for (row=0; row < height; row++) {
			row_pointers[row] = slashem_image + row * width * nethack_image_surface->format->BytesPerPixel;
		}
			
		for (dy=0;dy<height;dy++) {
			pixels=nethack_image_surface->pixels + (((y*height)+dy)*nethack_image_surface->pitch) + x*width*nethack_image_surface->format->BytesPerPixel ;
			for (dx=0;dx<width;dx++) {
				SDL_GetRGBA(*((Uint32 *)pixels),nethack_image_surface->format,&r,&g,&b,&a);
				
				rgba=slashem_image+ ( (dy*width) + dx )*nethack_image_surface->format->BytesPerPixel;
				
				*rgba=r; rgba++;
				*rgba=g; rgba++;
				*rgba=b;
				if (nethack_image_surface->format->BytesPerPixel==4) {
					rgba++;
					*rgba=a; 
				}
				
				pixels+=nethack_image_surface->format->BytesPerPixel;
			}
		}
		
		png_write_image(png_ptr, row_pointers);
		png_write_end(png_ptr, info_ptr);
		
		png_destroy_write_struct(&png_ptr, &info_ptr);
		
		fclose(fp);
		

		}
	}
	
	free(row_pointers);
	return 0;
}





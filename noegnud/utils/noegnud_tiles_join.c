#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <png.h>

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
    printf("Usage: %s <referencefile> <tilesetdir> <columns> <joinedimagename.png> <tile_width> <tile_height> <mid_x> <mid_y> [imagemap.html]\n",progname);
    printf("    or %s <referencefile> test\n",progname);
    exit(-1);
}

int main(int argc, char **argv) {
	noegnud_tcollection *tileset_enumerate=NULL;

	SDL_Surface *nethack_image_surface;
	void *slashem_image;

	int y,x,dy,dx;

	Uint8 r,g,b,a;

	char *pixels;
	char *rgba;

	char buffer[2048];
	char *fullname;
	int  slashtilenum;

	int width,height;
	int set_width, set_height;

	char fullpath[2048];

	int row;

	int test_lowest;

	int output_bpp;

	char *file_filename;

	FILE * volatile fp;
	png_bytep * volatile row_pointers = NULL;
	png_structp	png_ptr  = NULL;
	png_infop	info_ptr = NULL;

	FILE *web;

	int largest_x;
	int largest_y;

	int mid_x;
	int mid_y;

	int mid_y_use;

	if (argc<9) {
		if (argc!=3||strcmp("test",argv[2])) {
			printf(" * not enough or incorrect parameters\n");
			gethelp(argv[0]);
		}
	}

	noegnud_typeloader_parsefile(add_to_enumeration,argv[1],&tileset_enumerate);

	if (!tileset_enumerate) {
	    printf("* invalid reference file\n");
	    gethelp(argv[0]);
	}


	if (argc==3&&(!strcmp("test",argv[2]))) {
		test_lowest=100000;

		for (set_width=sqrt(desttilesetsize);set_width>0;set_width--) {
			set_height=((desttilesetsize+(set_width-1))/set_width);
			printf("resizing %4d tiles into ( %4d x %4d ) blocks, %4d placements, %4d loss",desttilesetsize,set_width,set_height,set_width*set_height,set_width*set_height-desttilesetsize);
			if (test_lowest>set_width*set_height) {
				test_lowest=set_width*set_height;
				printf(" *");
			}
			printf("\n");
		}
		exit(0);
	}

	set_width=atoi(argv[3]);
	if ((!set_width)) {
	    printf("* invalid \"coloumns\" amount.\n");
	    gethelp(argv[0]);
	}

	set_height=((desttilesetsize+(set_width-1))/set_width);

	printf("resizing %4d tiles into ( %4d x %4d ) blocks, %4d placements, %4d loss\n",desttilesetsize,set_width,set_height,set_width*set_height,set_width*set_height-desttilesetsize);

	SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);

	/*	file_filename=(char *)noegnud_collection_data(tileset_enumerate,"0");

	sprintf(buffer,"%s/%s.png",argv[2],file_filename);
	printf("loading image [%s]\n",buffer);
	nethack_image_surface=IMG_Load(buffer);
	if (!nethack_image_surface) {
	    printf("* invalid image file, or error loading image\n");
	    gethelp(argv[0]);
	}
	SDL_SetAlpha (nethack_image_surface, 0, 0);

	width=nethack_image_surface->w;
	height=nethack_image_surface->h;
	output_bpp=nethack_image_surface->format->BytesPerPixel;

	SDL_FreeSurface(nethack_image_surface);
	*/

	width=atoi(argv[5]);
	if ((!width)) {
	    printf("* invalid \"width\".\n");
	    gethelp(argv[0]);
	}

	height=atoi(argv[6]);
	if ((!height)) {
	    printf("* invalid \"height\".\n");
	    gethelp(argv[0]);
	}
	output_bpp=4;

	mid_x=atoi(argv[7]);
	if ((!mid_x)) {
	    printf("* invalid \"mid_x\".\n");
	    gethelp(argv[0]);
	}

	mid_y=atoi(argv[8]);
	if ((!mid_y)) {
	    printf("* invalid \"mid_y\".\n");
	    gethelp(argv[0]);
	}


	printf("using images in format : %dx%dx%d\n",width,height,output_bpp);

	//exit(0);

	slashem_image=calloc(output_bpp,(width*height)*(set_width*set_height));

	if (argc>9) {
		web=fopen(argv[9],"w");
		fprintf(web,"<HTML>\n\t<BODY background=\"imagemap_back.png\">\n\t\t<CENTER>\n\t\t\t<MAP name=\"noegnud\">\n");
	}

	puts("injecting...");

	largest_x=largest_y=0;

	for (y=0;y<set_height;y++) {
		for (x=0;x<set_width;x++) {
			if ( y*set_width+x>=desttilesetsize) continue;

			sprintf(buffer,"%d",y*set_width+x);

			fullname=noegnud_collection_data(tileset_enumerate,buffer);

			if (argc>9) {
				fprintf(web,"\t\t\t\t<AREA\n\t\t\t\t\tshape=\"rect\" alt=\"%04d - %s\"\n\t\t\t\t\tcoords=\"%d,%d,%d,%d\"\n\t\t\t\t\tnohref\n\t\t\t\t\tonMouseOver=\"window.status='%d - %s';return true;\">\n",
					y*set_width+x,
					fullname,
					x*width, y*height,
					x*width+width-1, y*height+height-1,
					y*set_width+x,
					fullname);
			}

			sprintf(fullpath,"%s/%s.png",argv[2],fullname);

			if (!fullname) {
				printf("**ERROR** : no name found for tile no [%d]\n",slashtilenum);
				continue;
			}

			nethack_image_surface=IMG_Load(fullpath);
			if (!nethack_image_surface) {
				printf("* invalid image file, or error loading image (%s) for tile number %d.\n",fullpath,y*set_width+x);
				continue;
			}

			if (nethack_image_surface->w>largest_x) largest_x=nethack_image_surface->w;
			if (nethack_image_surface->h>largest_y) largest_y=nethack_image_surface->h;

			SDL_SetAlpha (nethack_image_surface, 0, 0);

			SDL_LockSurface(nethack_image_surface);

			if ( (strstr(fullname,"cmap.")==fullname) ||
			     (strstr(fullname,"explosion.")==fullname) ||
			     (strstr(fullname,"warning.")==fullname) ||
			     (strstr(fullname,"zap.")==fullname) ) {
			    mid_y_use=height-nethack_image_surface->h;
			} else {
 			    mid_y_use=mid_y-(nethack_image_surface->h );
			}


		for (dy=0;dy<nethack_image_surface->h;dy++) {
			pixels=nethack_image_surface->pixels + (dy*nethack_image_surface->pitch);

			for (dx=0;dx<nethack_image_surface->w;dx++) {
				SDL_GetRGBA(*((Uint32 *)pixels),nethack_image_surface->format,&r,&g,&b,&a);

				rgba=slashem_image+ ( ((y*height+(dy+mid_y_use))*(width*set_width)) + (x*width)+dx-(nethack_image_surface->w/2)+(mid_x) )*output_bpp;

 				*rgba=r; rgba++;
				*rgba=g; rgba++;
				*rgba=b;
				if (output_bpp==4) {
					rgba++;
					*rgba=a;
				}

				pixels+=nethack_image_surface->format->BytesPerPixel;
			}
		}

		SDL_FreeSurface(nethack_image_surface);

		}
	}

	if (argc>9) {
		fprintf(web,"\t\t\t</MAP>\n\t\t\t<IMG src=\"%s\" usemap=\"#noegnud\">\n\t\t</CENTER>\n\t</BODY>\n</HTML>\n",argv[4]);
		fclose(web);
	}

	row_pointers = (png_bytep *) malloc(height*set_height * sizeof(png_bytep));
	for (row=0; row < height*set_height; row++) {
		row_pointers[row] = slashem_image + row * (set_width*width*output_bpp);
	}

	printf("saving %s...\n",argv[4]);
	fp=fopen(argv[4],"wb");
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);

	if (setjmp(png_ptr->jmpbuf)) {
		fprintf(stderr, "tile2png: Unknown problem while writing PNG.\n");
		fclose(fp);
	} else {

		png_init_io(png_ptr, fp);
		png_set_IHDR(png_ptr, info_ptr, set_width*width, set_height*height, 8,
			output_bpp==4 ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		png_write_info(png_ptr, info_ptr);

		png_write_image(png_ptr, row_pointers);
		png_write_end(png_ptr, info_ptr);

		png_destroy_write_struct(&png_ptr, &info_ptr);

		fclose(fp);

	}

	free(row_pointers);


	printf("largest subimage was ( %dx%d )\n",largest_x,largest_y);

	return 0;
}





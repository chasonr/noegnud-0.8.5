#include <stdlib.h>
#include <SDL_image.h>

#include "noegnud_debug.h"
#include "noegnud_mem.h"
#include "noegnud_extern.h"

int noegnud_mem_lock=0;

#ifdef NOEGNUDDEBUG

void noegnud_mem_debug_printused(char *filename, int linenumber) {
	printf(" [%s] (%d) : %d\n",filename,linenumber,noegnud_debug_mem_memused);
}

tnoegnud_mem_block *noegnud_mem_memblock_create(const char *sourcefile, const int linenumber, void *pointer, int size) {
	tnoegnud_mem_block *memblock;

	memblock=noegnud_mem_malloc(sizeof(tnoegnud_mem_block));
	memblock->sourcefile=noegnud_mem_malloc(strlen(sourcefile)+1);
	strcpy(memblock->sourcefile,sourcefile);
	memblock->linenumber=linenumber;
	memblock->pointer=pointer;
	memblock->size=size;

	return memblock;
}

void *noegnud_mem_debug_malloc(const char *filename, const int linenumber,int size) {
	void *ptr=0;
	char buf[256];

	tnoegnud_mem_block *memblock;
	noegnud_tcollection *memstruct;

	noegnud_mem_lock=1;

	if (!size) {
		printf("[FATAL:%s:%d] malloc for 0 bytes of ram! aborting ...\n",filename,linenumber);
		noegnud_mem_lock=0;
		exit(-1);
	}

	ptr=malloc(size);

	//	printf("[DEBUG:%s:%d] noegnud_mem_debug_malloc(%d);\n",filename,linenumber,size);

	if (!ptr) {
		printf("[FATAL:%s:%d] malloc failed! aborting ...\n",filename,linenumber);
		noegnud_mem_lock=0;
		exit(-1);
	}

	noegnud_debug_mem_memused+=size;
	if (noegnud_debug_mem_memused>noegnud_debug_mem_mostmemused) noegnud_debug_mem_mostmemused=noegnud_debug_mem_memused;

	sprintf(buf,"malloc_0x%X",(int)ptr);

	memblock=noegnud_mem_memblock_create(filename,linenumber,ptr,size);
	memstruct=noegnud_collection_structure(noegnud_debug_mem_ptrsizemap,buf);
	if (memstruct) {
	    memstruct->data=memblock;
	} else {
	    noegnud_collection_add(noegnud_debug_mem_ptrsizemap,buf,memblock);
	}

	noegnud_mem_lock=0;
	return ptr;
}

void noegnud_mem_debug_free(const char *filename, const int linenumber,void *ptr) {
	noegnud_tcollection *ptrsize;
	noegnud_tcollection *ptrsize_find;
	char buf[256];
	tnoegnud_mem_block *memblock;

//	printf("[DEBUG:%s:%d] noegnud_mem_debug_free(0x%0X)\n",filename,linenumber,(int)ptr);

	if (!ptr) {
		printf("[FATAL:%s:%d] free for a NULL pointer! aborting ...\n",filename,linenumber);
		exit(-1);
	}

	sprintf(buf,"malloc_0x%X",(int)ptr);
	ptrsize=NULL;
	ptrsize_find=noegnud_debug_mem_ptrsizemap;
	while (ptrsize_find&&(!ptrsize)) {
	    memblock=ptrsize_find->data;
	    if ((!strcmp(ptrsize_find->name,buf))&&memblock&&(memblock->size)) ptrsize=ptrsize_find;
	    ptrsize_find=ptrsize_find->next;
	}
	//ptrsize=noegnud_collection_structure(noegnud_debug_mem_ptrsizemap,buf);
	if (!ptrsize) {
		printf("[DEBUG:%s:%d] *WARNING* failed to find debug reference for pointer (\"%s\") !\n",filename,linenumber,buf);
	}
	free(ptr);
	if (ptrsize) {
		memblock=ptrsize->data;
		noegnud_debug_mem_memused-=memblock->size;
		memblock->size=0;
	}
}

#define noegnud_mem_debug_getsize_sdlsurface(_surface) (sizeof(SDL_Surface)+_surface->w*_surface->h*_surface->format->BytesPerPixel)

SDL_Surface *noegnud_mem_debug_img_load(const char *filename, const int linenumber,const char *image_filename) {
	SDL_Surface *tmp;
	char buf[256];
	tnoegnud_mem_block *memblock;
	noegnud_tcollection *memstruct;

	noegnud_mem_lock=1;

	tmp=IMG_Load(image_filename);
	if (tmp) {
		sprintf(buf,"sdl_surface_0x%X",(int)tmp);
		memblock=noegnud_mem_memblock_create(filename,linenumber,tmp,noegnud_mem_debug_getsize_sdlsurface(tmp));
		noegnud_debug_mem_memused+=noegnud_mem_debug_getsize_sdlsurface(tmp);
		memstruct=noegnud_collection_structure(noegnud_debug_mem_ptrsizemap,buf);
		if (memstruct) {
		    memstruct->data=memblock;
		} else {
		    noegnud_collection_add(noegnud_debug_mem_ptrsizemap,buf,memblock);
		}
	}
	noegnud_mem_lock=0;
	return tmp;
}

SDL_Surface *noegnud_mem_debug_sdl_creatergbsurface(const char *filename, const int linenumber,Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask) {
	SDL_Surface *tmp;
	char buf[256];
	tnoegnud_mem_block *memblock;
	noegnud_tcollection *memstruct;

	noegnud_mem_lock=1;

	tmp=SDL_CreateRGBSurface(flags,width,height,depth,Rmask,Gmask,Bmask,Amask);
	if (tmp) {
		sprintf(buf,"sdl_surface_0x%X",(int)tmp);
		memblock=noegnud_mem_memblock_create(filename,linenumber,tmp,noegnud_mem_debug_getsize_sdlsurface(tmp));
		noegnud_debug_mem_memused+=noegnud_mem_debug_getsize_sdlsurface(tmp);
		memstruct=noegnud_collection_structure(noegnud_debug_mem_ptrsizemap,buf);
		if (memstruct) {
		    memstruct->data=memblock;
		} else {
		    noegnud_collection_add(noegnud_debug_mem_ptrsizemap,buf,memblock);
		}
	}
	noegnud_mem_lock=0;
	return tmp;
}

SDL_Surface *noegnud_mem_debug_sdl_creatergbsurfacefrom(const char *filename, const int linenumber, void *pixels, Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask) {
	SDL_Surface *tmp;
	char buf[256];
	tnoegnud_mem_block *memblock;
	noegnud_tcollection *memstruct;

	noegnud_mem_lock=1;

	tmp=SDL_CreateRGBSurfaceFrom(pixels,flags,width,height,depth,Rmask,Gmask,Bmask,Amask);
	if (tmp) {
		sprintf(buf,"sdl_surface_0x%X",(int)tmp);
		memblock=noegnud_mem_memblock_create(filename,linenumber,tmp,noegnud_mem_debug_getsize_sdlsurface(tmp));
		noegnud_debug_mem_memused+=noegnud_mem_debug_getsize_sdlsurface(tmp);
		memstruct=noegnud_collection_structure(noegnud_debug_mem_ptrsizemap,buf);
		if (memstruct) {
		    memstruct->data=memblock;
		} else {
		    noegnud_collection_add(noegnud_debug_mem_ptrsizemap,buf,memblock);
		}
	}
	noegnud_mem_lock=0;
	return tmp;
}

void noegnud_mem_debug_sdl_freesurface(const char *filename, const int linenumber,SDL_Surface *surface) {
	noegnud_tcollection *ptrsize;
	char buf[256];
	tnoegnud_mem_block *memblock;

	if (!surface) {
		printf("[FATAL:%s:%d] free for a NULL SDL_Surface! aborting ...\n",filename,linenumber);
		exit(-1);
	}
	sprintf(buf,"sdl_surface_0x%X",(int)surface);
	ptrsize=noegnud_collection_structure(noegnud_debug_mem_ptrsizemap,buf);
	if (!ptrsize) {
		printf("[DEBUG:%s:%d] *WARNING* failed to find debug reference for SDL_Surface (\"%s\") !\n",filename,linenumber,buf);
	}
	SDL_FreeSurface(surface);
	if (ptrsize) {
		memblock=ptrsize->data;
		noegnud_debug_mem_memused-=memblock->size;
		memblock->size=0;
	}
}

#endif


#ifndef _NOEGNUD_MEM_H_
#define _NOEGNUD_MEM_H_

#include "noegnud_debug.h"
#include "noegnud_extern.h"

#ifdef NOEGNUDDEBUG

typedef struct {
	void *pointer;
	int size;
	char *sourcefile;
	int linenumber;
} tnoegnud_mem_block;

# define noegnud_mem_malloc(_size) noegnud_mem_lock?malloc(_size):noegnud_mem_debug_malloc(__FILE__,__LINE__,_size)
# define noegnud_mem_free(_ptr) noegnud_mem_lock?free(_ptr):noegnud_mem_debug_free(__FILE__,__LINE__,_ptr)

# define noegnud_mem_sdl_creatergbsurface(_flags,_width,_height,_depth,_Rmask,_Gmask,_Bmask,_Amask) noegnud_mem_debug_sdl_creatergbsurface(__FILE__,__LINE__,_flags,_width,_height,_depth,_Rmask,_Gmask,_Bmask,_Amask)
# define noegnud_mem_sdl_creatergbsurfacefrom(_pixels,_flags,_width,_height,_depth,_Rmask,_Gmask,_Bmask,_Amask) noegnud_mem_debug_sdl_creatergbsurfacefrom(__FILE__,__LINE__,_pixels,_flags,_width,_height,_depth,_Rmask,_Gmask,_Bmask,_Amask)
# define noegnud_mem_img_load(_filename) noegnud_mem_debug_img_load(__FILE__,__LINE__,_filename);
# define noegnud_mem_sdl_freesurface(_surface) noegnud_mem_debug_sdl_freesurface(__FILE__,__LINE__,_surface)

# define noegnud_mem_realloc(_ptr,_size) realloc(_ptr,_size)

void noegnud_mem_debug_printused(char *filename, int linenumber);

tnoegnud_mem_block *noegnud_mem_memblock_create(const char *sourcefile, const int linenumber, void *pointer, int size);

void *noegnud_mem_debug_malloc(const char *filename, const int linenumber,int size);
void noegnud_mem_debug_free(const char *filename, const int linenumber,void *ptr);

SDL_Surface *noegnud_mem_debug_sdl_creatergbsurface(const char *filename, const int linenumber,Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
SDL_Surface *noegnud_mem_debug_sdl_creatergbsurfacefrom(const char *filename, const int linenumber, void *pixels,Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
SDL_Surface *noegnud_mem_debug_img_load(const char *filename, const int linenumber,const char *image_filename);
void noegnud_mem_debug_sdl_freesurface(const char *filename, const int linenumber,SDL_Surface *surface);

#else

# define noegnud_mem_malloc(_size) malloc(_size)
# define noegnud_mem_free(_ptr) free(_ptr)
# define noegnud_mem_realloc(_ptr,_size) realloc(_ptr,_size)

# define noegnud_mem_sdl_creatergbsurface(_flags,_width,_height,_depth,_Rmask,_Gmask,_Bmask,_Amask) SDL_CreateRGBSurface(_flags,_width,_height,_depth,_Rmask,_Gmask,_Bmask,_Amask)
# define noegnud_mem_sdl_creatergbsurfacefrom(_pixels,_flags,_width,_height,_depth,_Rmask,_Gmask,_Bmask,_Amask) SDL_CreateRGBSurfaceFrom(_pixels,_flags,_width,_height,_depth,_Rmask,_Gmask,_Bmask,_Amask)
# define noegnud_mem_img_load(_filename) IMG_Load(_filename);
# define noegnud_mem_sdl_freesurface(_surface) SDL_FreeSurface(_surface)

#endif

#endif // _NOEGNUD_MEM_H_

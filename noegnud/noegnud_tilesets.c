#include <stdlib.h>

#include "noegnud_interface.h"
#include "noegnud_tilesets.h"
#include "noegnud_typeloader.h"
#include "noegnud_gui.h"
#include "noegnud_extern.h"
#include "noegnud_common.h"
#include "noegnud_mem.h"
#include "noegnud_config.h"

ts_tileset *noegnud_activetileset;

extern noegnud_gui_twidget *noegnud_guiwidget_desktop;
extern noegnud_gui_twidget *noegnud_guiwidget_console;
extern void noegnud_render_loop();

static void noegnud_tilesets_init_addtileset_parser(char *setting, int nparams, char *params, void *data) {
	ts_tileset *tileset=data;

  if (!strcmp(setting,"location")) {
	  sprintf((char *)&tileset->location,"tilesets/%s",params);
  } else if (!strcmp(setting,"width")) {
	  tileset->width=atoi(params);
  } else if (!strcmp(setting,"height")) {
	  tileset->height=atoi(params);
  } else if (!strcmp(setting,"xdeltax")) {
	  tileset->xdeltax=atoi(params);
  } else if (!strcmp(setting,"xdeltay")) {
	  tileset->xdeltay=atoi(params);
  } else if (!strcmp(setting,"ydeltax")) {
	  tileset->ydeltax=atoi(params);
  } else if (!strcmp(setting,"ydeltay")) {
	  tileset->ydeltay=atoi(params);
  } else if (!strcmp(setting,"iso")) {
	  tileset->iso=atoi(params);
  }


}

void noegnud_tilesets_clearloaded(ts_tileset * tileset) {
	int c;

	if (!tileset) return;
	for (c=0;c<MAX_TILES;c++) if (tileset->loaded[c]) {
		if (tileset->tiles[c] != noegnud_image_tilenotfound->block[0][0]->image) glDeleteTextures (1, &tileset->tiles[c]);
		tileset->loaded[c]=FALSE;
	}
}

ts_tileset *noegnud_tilesets_loadtileset(const char *name) {
	char infname[1024];
	char *szHomedir;

	ts_tileset *loading;
	int c;


	loading = (ts_tileset *)noegnud_mem_malloc (sizeof (ts_tileset));

	loading->xdeltax=0;
	loading->xdeltay=0;
	loading->ydeltax=0;
	loading->ydeltay=0;
	loading->iso=0;


	szHomedir=noegnud_options_get_home();
	sprintf(infname,"%s/tilesets/%s.inf",szHomedir,name);
	noegnud_mem_free(szHomedir);
	if (!noegnud_common_file_exists(infname)) sprintf((char *)&infname,"../" NOEGNUD_DATADIR "/tilesets/%s.inf",name);
	printf("* Loading tileset : %s (%s)\n",name,infname);

	if (!noegnud_common_file_exists((const char *)infname)) {
	    printf("    error locating file [%s].\n",infname);
	    return NULL;
	}

	noegnud_typeloader_parsefile(noegnud_tilesets_init_addtileset_parser,(char *)&infname,loading);

/*
	loading->glwidth = ((float)loading->width/32)*noegnud_options_render_multiplier->value;
	loading->glheight = ((float)loading->height/32)*noegnud_options_render_multiplier->value;
*/
	loading->glwidth = ((((float)loading->width/(float)loading->height)*64.0)/32)*noegnud_options_render_multiplier->value;
	loading->glheight = (64.0/32)*noegnud_options_render_multiplier->value;

	if (!loading->xdeltax) loading->xdeltax=loading->width;
	if (!loading->ydeltay) loading->ydeltay=loading->height;
	loading->xdeltax=((loading->xdeltax/32)*noegnud_options_render_multiplier->value)*(64.0/(float)loading->height);
	loading->xdeltay=((loading->xdeltay/32)*noegnud_options_render_multiplier->value)*(64.0/(float)loading->height);
	loading->ydeltax=((loading->ydeltax/32)*noegnud_options_render_multiplier->value)*(64.0/(float)loading->height);
	loading->ydeltay=((loading->ydeltay/32)*noegnud_options_render_multiplier->value)*(64.0/(float)loading->height);

	if (noegnud_options_mode_tileset_halfwidth->value) {
	    loading->glwidth/=2.0;
	    loading->xdeltax/=2.0;
	    loading->ydeltax/=2.0;
	}

	loading->tileamount=noegnud_reference_tile_amount;
	loading->isalphatileset =1;


	for (c=0;c<MAX_TILES;c++) loading->loaded[c]=FALSE;
/*
	if ( !noegnud_tilesetload_images_from( loading, name, (char *)loading->location) ) {
		noegnud_mem_free(loading);
		loading=NULL;
	}
*/
	return loading;
}

void noegnud_tilesets_unloadtileset(ts_tileset *tileset) {
	noegnud_tilesets_clearloaded(tileset);
	noegnud_mem_free(tileset);
}

int noegnud_tilesetload_image_from(ts_tileset * tileset, int tilenum) {
	noegnud_tcollection *tile_reference;
	char sztilenum[16];
	char *filename;
	noegnud_glfuncs_timage *glimage;
	char *location;
	int loaderror;

	location=tileset->location;
	loaderror=FALSE;

	sprintf(sztilenum,"%d",tilenum);
	if (!(tile_reference=noegnud_collection_structure(noegnud_reference,sztilenum))) return 0;

	if (!tileset->loaded[tilenum]) {


		filename=noegnud_mem_malloc(1024);
		sprintf(filename,"%s/%s.png",location,(char *)tile_reference->data);

//		printf("* loading tile: %s\n",filename);

		glimage=noegnud_glfuncs_loadimage(filename,FALSE,FALSE,TRUE);
		if (!glimage) {
		    loaderror=TRUE;
		    glimage=noegnud_image_tilenotfound;
		}

		tileset->fx1=glimage->block[0][0]->fx1;
		tileset->fx2=glimage->block[0][0]->fx2;
		tileset->fy1=glimage->block[0][0]->fy1;
		tileset->fy2=glimage->block[0][0]->fy2;

		tileset->tiles[tilenum]=glimage->block[0][0]->image;
		tileset->loaded[tilenum]=TRUE;
		noegnud_mem_free(glimage->block[0][0]);
		if (!loaderror) noegnud_mem_free(glimage);
		noegnud_mem_free(filename);
	}
	return 1;
}

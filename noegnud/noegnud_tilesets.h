#ifndef _NOEGNUD_TILESETS_H_
#define _NOEGNUD_TILESETS_H_

#define MAX_TILES 2048
typedef struct {

	int tileamount;
	GLuint tiles[MAX_TILES];
	int  loaded[MAX_TILES];
	char location[1024];
	int width, height;
	float fx1,fy1;
	float fx2, fy2;
	float glwidth,glheight;
	float xdeltax, xdeltay;
	float ydeltax, ydeltay;
	int isalphatileset;
	int iso;
} ts_tileset;

void noegnud_tilesets_init ();
void noegnud_tilesets_next ();
void noegnud_tilesets_done ();

ts_tileset *noegnud_tilesets_loadtileset(const char *name);
void noegnud_tilesets_clearloaded(ts_tileset * tileset);
void noegnud_tilesets_unloadtileset(ts_tileset *tileset);

int noegnud_tilesetload_image_from(ts_tileset * tileset, int tilenum);

#endif //_NOEGNUD_TILESETS_H_

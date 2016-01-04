#include <math.h>

#include "noegnud_render.h"
#include "noegnud_render_tile.h"
#include "noegnud_extern.h"

void noegnud_render_map_single_tile (int glyph_tile,int normalface) {

    glBindTexture (GL_TEXTURE_2D, noegnud_activetileset->tiles[glyph_tile]);
    glBegin (GL_QUADS);

    glNormal3f(0,1*normalface,0);
    glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy2);
    glVertex3f (0, 0, noegnud_activetileset->glheight);

    glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy2);
    glVertex3f (noegnud_activetileset->glwidth, 0, noegnud_activetileset->glheight);

    glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy1);
    glVertex3f (noegnud_activetileset->glwidth, 0, 0);

    glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy1);
    glVertex3f (0, 0, 0);

    glEnd ();
}

void noegnud_render_map_single_selection_tile (int glyph_tile,int normalface) {

    glBegin (GL_QUADS);

    glVertex3f (0, 0, noegnud_activetileset->glheight);
    glVertex3f (noegnud_activetileset->glwidth, 0, noegnud_activetileset->glheight);
    glVertex3f (noegnud_activetileset->glwidth, 0, 0);
    glVertex3f (0, 0, 0);

    glEnd ();
}

void noegnud_render_map_single_tile_raised_walls_texture (GLint texture, float height, GLfloat alpha, int a, int b, int l, int r) {
    if (noegnud_options_render_opengl_lighting_method->value==NOEGNUD_RENDER_LIGHTING_METHOD_CHEAP) {
	glColor4f(1.0,1.0,1.0,alpha);
    } else {
	glColor4f(0.5,0.5,0.5,alpha);
    }

    glBindTexture (GL_TEXTURE_2D, texture);
    glBegin (GL_QUADS);

    if (a) {
	glNormal3f(0,0,-1);
	glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy1);
	glVertex3f (0, noegnud_activetileset->glheight*height, 0);

	glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy1);
	glVertex3f (noegnud_activetileset->glwidth, noegnud_activetileset->glheight*height, 0);

	glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy2);
	glVertex3f (noegnud_activetileset->glwidth, 0, 0);

	glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy2);
	glVertex3f (0, 0, 0);
    }

    if (b) {
	glNormal3f(0,0,1);

	glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy2);
	glVertex3f (0, 0, noegnud_activetileset->glheight);

	glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy2);
	glVertex3f (noegnud_activetileset->glwidth, 0, noegnud_activetileset->glheight);

	glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy1);
	glVertex3f (noegnud_activetileset->glwidth, noegnud_activetileset->glheight*height, noegnud_activetileset->glheight);

	glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy1);
	glVertex3f (0, noegnud_activetileset->glheight*height, noegnud_activetileset->glheight);
    }

    if (l) {
	glNormal3f(-1,0,0);
	glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy2);
	glVertex3f (0, 0, noegnud_activetileset->glheight);

	glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy1);
	glVertex3f (0, noegnud_activetileset->glheight*height, noegnud_activetileset->glheight);

	glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy1);
	glVertex3f (0, noegnud_activetileset->glheight*height, 0);

	glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy2);
	glVertex3f (0, 0, 0);
    }

    if (r) {
	glNormal3f(1,0,0);

	glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy2);
	glVertex3f (noegnud_activetileset->glwidth, 0, 0);

	glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy1);
	glVertex3f (noegnud_activetileset->glwidth, noegnud_activetileset->glheight*height, 0);

	glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy1);
	glVertex3f (noegnud_activetileset->glwidth, noegnud_activetileset->glheight*height, noegnud_activetileset->glheight);

	glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy2);
	glVertex3f (noegnud_activetileset->glwidth, 0, noegnud_activetileset->glheight);
    }

    glEnd ();
}

void noegnud_render_map_single_tile_raised_walls (int glyph_tile, float height, GLfloat alpha, int a, int b, int l, int r) {
    noegnud_render_map_single_tile_raised_walls_texture(
							noegnud_activetileset->tiles[glyph_tile],
							height,
							alpha,
							a,b,l,r);
}

void noegnud_render_map_single_tile_sunken_walls (int glyph_tile, float height, GLfloat alpha, int a, int b, int l, int r) {

    if (noegnud_options_render_opengl_lighting_method->value==NOEGNUD_RENDER_LIGHTING_METHOD_CHEAP) {
	glColor4f(1.0,1.0,1.0,alpha);
    } else {
	glColor4f(0.5,0.5,0.5,alpha);
    }

    glBindTexture (GL_TEXTURE_2D, noegnud_activetileset->tiles[glyph_tile]);
    glBegin (GL_QUADS);

    if (a) {
	glNormal3f(0,0,1);
	glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy1);
	glVertex3f (0, noegnud_activetileset->glheight*height, 0);

	glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy2);
	glVertex3f (0, 0, 0);

	glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy2);
	glVertex3f (noegnud_activetileset->glwidth, 0, 0);

	glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy1);
	glVertex3f (noegnud_activetileset->glwidth, noegnud_activetileset->glheight*height, 0);
    }

    if (b) {
	glNormal3f(0,0,-1);

	glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy2);
	glVertex3f (0, 0, noegnud_activetileset->glheight);

	glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy1);
	glVertex3f (0, noegnud_activetileset->glheight*height, noegnud_activetileset->glheight);

	glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy1);
	glVertex3f (noegnud_activetileset->glwidth, noegnud_activetileset->glheight*height, noegnud_activetileset->glheight);

	glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy2);
	glVertex3f (noegnud_activetileset->glwidth, 0, noegnud_activetileset->glheight);
    }

    if (l) {
	glNormal3f(1,0,0);
	glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy2);
	glVertex3f (0, 0, noegnud_activetileset->glheight);

	glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy2);
	glVertex3f (0, 0, 0);

	glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy1);
	glVertex3f (0, noegnud_activetileset->glheight*height, 0);

	glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy1);
	glVertex3f (0, noegnud_activetileset->glheight*height, noegnud_activetileset->glheight);
    }

    if (r) {
	glNormal3f(-1,0,0);

	glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy2);
	glVertex3f (noegnud_activetileset->glwidth, 0, 0);

	glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy2);
	glVertex3f (noegnud_activetileset->glwidth, 0, noegnud_activetileset->glheight);

	glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy1);
	glVertex3f (noegnud_activetileset->glwidth, noegnud_activetileset->glheight*height, noegnud_activetileset->glheight);

	glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy1);
	glVertex3f (noegnud_activetileset->glwidth, noegnud_activetileset->glheight*height, 0);
    }

    glEnd ();
}

void noegnud_render_map_tile_highlight_block () {

    glDisable(GL_TEXTURE_2D);

    glColor4f(1.0,1.0,1.0,noegnud_options_render_highlight_block_opacity->value);

    glBegin (GL_QUADS);

    glVertex3f (0, 0, noegnud_activetileset->glheight);
    glVertex3f (noegnud_activetileset->glwidth/10, 0, noegnud_activetileset->glheight);
    glVertex3f (noegnud_activetileset->glwidth/10, 0, 0);
    glVertex3f (0, 0, 0);

    glVertex3f (noegnud_activetileset->glwidth*9/10, 0, noegnud_activetileset->glheight);
    glVertex3f (noegnud_activetileset->glwidth, 0, noegnud_activetileset->glheight);
    glVertex3f (noegnud_activetileset->glwidth, 0, 0);
    glVertex3f (noegnud_activetileset->glwidth*9/10, 0, 0);

    glVertex3f (noegnud_activetileset->glwidth/10, 0, noegnud_activetileset->glheight/10);
    glVertex3f (noegnud_activetileset->glwidth*9/10, 0, noegnud_activetileset->glheight/10);
    glVertex3f (noegnud_activetileset->glwidth*9/10, 0, 0);
    glVertex3f (noegnud_activetileset->glwidth/10, 0, 0);

    glVertex3f (noegnud_activetileset->glwidth/10, 0, noegnud_activetileset->glheight);
    glVertex3f (noegnud_activetileset->glwidth*9/10, 0, noegnud_activetileset->glheight);
    glVertex3f (noegnud_activetileset->glwidth*9/10, 0, noegnud_activetileset->glheight*9/10);
    glVertex3f (noegnud_activetileset->glwidth/10, 0, noegnud_activetileset->glheight*9/10);

    glEnd ();

    glEnable(GL_TEXTURE_2D);

}

void noegnud_render_map_tile () {
    int x, y;
    int xs,xe,xd;
    int ys,ye,yd;
    GLfloat current_alpha;
    float nx, ny;
    float realangle;
    GLfloat light_position[4];

    ys=0;
    ye=MAX_MAP_Y;
    yd=1;

    xs=0;
    xe=MAX_MAP_X;
    xd=1;

    realangle=(noegnud_options_render_opengl_view_angle_x_current->value+(noegnud_activetileset->iso?0:noegnud_render_bullettime_angle));
    while (realangle>=360) realangle-=360;
    while (realangle<0) realangle+=360;

    if ((realangle>=90)&&(realangle<270)) {
	ys=MAX_MAP_Y-1;
	ye=-1;
	yd=-1;
    }

    if ((realangle<=180)) {
	xs=MAX_MAP_X-1;
	xe=-1;
	xd=-1;
    }

    if (noegnud_render_selection_busy) {
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
 	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
    }

    if (noegnud_activetileset->iso) {
	glEnable (GL_BLEND);
	glDisable (GL_DEPTH_TEST);
    } else {
	glDisable (GL_BLEND);
	if (noegnud_options_render_features_cardboardcutout->value&&noegnud_activetileset->isalphatileset) {
	    glEnable (GL_DEPTH_TEST);
	} else glDisable (GL_DEPTH_TEST);
    }


    if (noegnud_options_render_opengl_lighting_method->value==NOEGNUD_RENDER_LIGHTING_METHOD_CHEAP) {
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	if ((noegnud_options_input_mouse_3d_light->value)&&noegnud_render_mouse_map_x!=-1) {
	    glEnable(GL_LIGHT1);
	} else {
	    glDisable(GL_LIGHT1);
	}
    }


    x=noegnud_clip_x;
    y=noegnud_clip_y;
    ny = -(y - (noegnud_render_middle_y));
    nx = x - (noegnud_render_middle_x);

    light_position[0]=noegnud_activetileset->glwidth/2;
    light_position[1]=noegnud_activetileset->glheight*3/2;
    light_position[2]=noegnud_activetileset->glheight;
    light_position[3]=1;

    glTranslatef (
		  (noegnud_activetileset->xdeltax) * (nx) +
		  (noegnud_activetileset->ydeltax) * (ny) +
		  noegnud_render_offset_x, 0,
		  -((noegnud_activetileset->xdeltay) * (nx) +
		    (noegnud_activetileset->ydeltay) * (ny)) +
		  noegnud_render_offset_y);

    glLightfv(GL_LIGHT0,GL_POSITION,light_position);

    if (noegnud_options_input_mouse_3d_light->value) {
	ny = -(noegnud_render_mouse_map_y - (noegnud_render_middle_y));
	nx = (noegnud_render_mouse_map_x-1) - (noegnud_render_middle_x);
	light_position[0]+=(noegnud_activetileset->xdeltax) * (nx) +
			  (noegnud_activetileset->ydeltax) * (ny) +
	    noegnud_render_offset_x;
	light_position[2]+=-((noegnud_activetileset->xdeltay) * (nx) +
			    (noegnud_activetileset->ydeltay) * (ny)) +
	    noegnud_render_offset_y;
	glLightfv(GL_LIGHT1,GL_POSITION,light_position);
    }

    for (y = 0; y < MAX_MAP_Y; y++) {
	ny = -(y - (noegnud_render_middle_y));
	for (x = 0; x < MAX_MAP_X; x++) {
	    nx = x - (noegnud_render_middle_x);

	    glPopMatrix ();
	    glPushMatrix ();
	    glTranslatef (
			  (noegnud_activetileset->xdeltax) * (nx) +
			  (noegnud_activetileset->ydeltax) * (ny) +
			  noegnud_render_offset_x, 0,
			  -((noegnud_activetileset->xdeltay) * (nx) +
			    (noegnud_activetileset->ydeltay) * (ny)) +
			  noegnud_render_offset_y);

	    glLoadName(y*256+x);
	    glBegin(GL_LINES); glEnd();

	    /*
	    if (noegnud_options_input_mouse_3d_light->value&&((noegnud_render_mouse_map_x-1)==x)&&(noegnud_render_mouse_map_y==y)) {
		glLightfv(GL_LIGHT1,GL_POSITION,light_position);
	    }
	    */

	    if (((noegnud_render_mouse_map_x-1)==x)&&(noegnud_render_mouse_map_y==y)) {
		glColor4f(0.25,1.0,0.25,1.0);
	    }
	    else if ((x == noegnud_clip_x) && (y == noegnud_clip_y)) {

		glColor4f(noegnud_render_highlight_strobe/2+0.5,noegnud_render_highlight_strobe/2+0.5,noegnud_render_highlight_strobe/2+0.5,1.0);
	    } else if ((!noegnud_activetileset->isalphatileset)&&iflags.hilite_pet&&(noegnud_map[x][y].glyph_primary!=MAX_GLYPH)&&(noegnud_map[x][y].special_primary & MG_PET)) {
		glColor4f(noegnud_render_highlight_strobe/2+0.5,noegnud_render_highlight_strobe/2+0.5,noegnud_render_highlight_strobe/2+0.5,1.0);
	    } else	glColor4f(1.0,1.0,1.0,1.0);

	    if (noegnud_render_selection_busy) {
		noegnud_render_map_single_selection_tile (noegnud_map[x][y].tile_primary,1);
	    } else
		if (noegnud_options_mode_tileset_caverns->value&&(!noegnud_activetileset->iso)&&noegnud_image_dungeon&&(noegnud_map[x][y].glyph_primary==MAX_GLYPH||(glyph_to_cmap(noegnud_map[x][y].glyph_primary)==S_stone&&levl[x+1][y].lit))) {

		    if (levl[x+1][y].seenv) {
			if (noegnud_options_render_features_cardboardcutout->value &&
			    noegnud_options_mode_tileset_caverns_cutout->value &&
			    noegnud_options_render_features_cardboardcutout_raise_walls->value&&(!IS_ROOM(levl[x+1][y].typ))) {

			    glPushMatrix ();

			    noegnud_render_map_single_tile_raised_walls_texture(
										noegnud_image_dungeon->block[0][0]->image,
										noegnud_options_render_features_cardboardcutout_wall_height->value,
										current_alpha,
										!((y>0)&&noegnud_map[x][y-1].high),
										!((y<MAX_MAP_Y-1)&&noegnud_map[x][y+1].high),
										!((x>0)&&noegnud_map[x-1][y].high),
										!((x<MAX_MAP_X-1)&&noegnud_map[x+1][y].high));


			    glTranslatef(0,noegnud_activetileset->glheight*noegnud_options_render_features_cardboardcutout_wall_height->value,0);

			}
			    if (((noegnud_render_mouse_map_x-1)==x)&&(noegnud_render_mouse_map_y==y)) {
				glColor4f(0.25,1.0,0.25,1.0);
			    } else {
				glColor4f(1.0,1.0,1.0,current_alpha);
			    }
		    } else {
			if (noegnud_options_render_features_cardboardcutout->value &&
			    noegnud_options_mode_tileset_caverns_cutout->value &&
			    noegnud_options_render_features_cardboardcutout_raise_walls->value) {
			    glTranslatef(0,noegnud_activetileset->glheight*noegnud_options_render_features_cardboardcutout_wall_height->value,0);
			}
			if (((noegnud_render_mouse_map_x-1)==x)&&(noegnud_render_mouse_map_y==y)) {
			    glColor4f(0.125,0.5,0.125,1.0);
			} else
			    glColor4f(0.5,0.5,0.5,current_alpha);
		    }
		    if (levl[x+1][y].seenv) {
			if (IS_ROOM(levl[x+1][y].typ)) {
			} else {

			    glBindTexture (GL_TEXTURE_2D, noegnud_image_dungeon->block[0][0]->image);
			    glBegin (GL_QUADS);

			    glNormal3f(0,1,0);
			    glTexCoord2f (noegnud_image_dungeon->block[0][0]->fx1, noegnud_image_dungeon->block[0][0]->fy2);
			    glVertex3f (0, 0, noegnud_activetileset->glheight);

			    glTexCoord2f (noegnud_image_dungeon->block[0][0]->fx2, noegnud_image_dungeon->block[0][0]->fy2);
			    glVertex3f (noegnud_activetileset->glwidth, 0, noegnud_activetileset->glheight);

			    glTexCoord2f (noegnud_image_dungeon->block[0][0]->fx2, noegnud_image_dungeon->block[0][0]->fy1);
			    glVertex3f (noegnud_activetileset->glwidth, 0, 0);

			    glTexCoord2f (noegnud_image_dungeon->block[0][0]->fx1, noegnud_image_dungeon->block[0][0]->fy1);
			    glVertex3f (0, 0, 0);

			    glEnd ();
			}
		    } else {
			if (noegnud_options_mode_tileset_caverns_unexplored->value) {
			    glBindTexture (GL_TEXTURE_2D, noegnud_image_dungeon->block[0][0]->image);
			    glBegin (GL_QUADS);

			    glNormal3f(0,1,0);
			    glTexCoord2f (noegnud_image_dungeon->block[0][0]->fx1, noegnud_image_dungeon->block[0][0]->fy2);
			    glVertex3f (0, 0, noegnud_activetileset->glheight);

			    glTexCoord2f (noegnud_image_dungeon->block[0][0]->fx2, noegnud_image_dungeon->block[0][0]->fy2);
			    glVertex3f (noegnud_activetileset->glwidth, 0, noegnud_activetileset->glheight);

			    glTexCoord2f (noegnud_image_dungeon->block[0][0]->fx2, noegnud_image_dungeon->block[0][0]->fy1);
			    glVertex3f (noegnud_activetileset->glwidth, 0, 0);

			    glTexCoord2f (noegnud_image_dungeon->block[0][0]->fx1, noegnud_image_dungeon->block[0][0]->fy1);
			    glVertex3f (0, 0, 0);

			    glEnd ();
			}
		    }
		    if (levl[x+1][y].seenv) {

			if (noegnud_options_render_features_cardboardcutout->value &&
			    noegnud_options_mode_tileset_caverns_cutout->value &&
			    noegnud_options_render_features_cardboardcutout_raise_walls->value&&(!IS_ROOM(levl[x+1][y].typ))) {
			    glPopMatrix ();
			}
		    }

		}
		if ( ((!noegnud_activetileset->isalphatileset) && (noegnud_map[x][y].glyph_primary!=MAX_GLYPH))) {
		    noegnud_render_map_single_tile (noegnud_map[x][y].tile_primary,1);
		    if ((((x == noegnud_clip_x) && (y == noegnud_clip_y))||(((noegnud_render_mouse_map_x-1)==x)&&(noegnud_render_mouse_map_y==y))) ||
			//				if (((x == noegnud_clip_x) && (y == noegnud_clip_y)) ||
			(noegnud_map[x][y].glyph_primary!=MAX_GLYPH&&iflags.hilite_pet&&(noegnud_map[x][y].special_primary & MG_PET))) {

			glEnable (GL_BLEND);
			noegnud_render_map_tile_highlight_block ();
			glDisable (GL_BLEND);
		    }
		} else if (noegnud_activetileset->isalphatileset) {
		    if ( noegnud_options_render_features_cardboardcutout->value &&
			 noegnud_options_render_features_cardboardcutout_raise_walls->value &&
			 noegnud_map[x][y].low &&
			 (!noegnud_activetileset->iso)) {
			if ((noegnud_map[x][y].glyph_primary!=MAX_GLYPH)||(noegnud_map[x][y].glyph_primary==MAX_GLYPH&&noegnud_options_mode_tileset_caverns_cutout->value))
			glTranslatef(0,-noegnud_activetileset->glheight*noegnud_options_render_features_cardboardcutout_hole_depth->value,0);
		    }
		    if ( noegnud_map[x][y].glyph_back != MAX_GLYPH &&
			 (!noegnud_map[x][y].high)) noegnud_render_map_single_tile (noegnud_map[x][y].tile_back,1);
		    if ((!noegnud_render_selection_busy)&&noegnud_options_render_features_cardboardcutout->value&&noegnud_options_render_features_cardboardcutout_raise_walls->value&&(!noegnud_activetileset->iso)&&noegnud_map[x][y].low) {
			//glTranslatef(0,-noegnud_activetileset->glheight*noegnud_options_render_features_cardboardcutout_hole_depth->value,0);
			noegnud_render_map_single_tile_sunken_walls (
								     noegnud_map[x][y].tile_back,
								     noegnud_options_render_features_cardboardcutout_hole_depth->value,
								     current_alpha,
								     !((y>0)&&noegnud_map[x][y-1].low),
								     !((y<MAX_MAP_Y-1)&&noegnud_map[x][y+1].low),
								     !((x>0)&&noegnud_map[x-1][y].low),
								     !((x<MAX_MAP_X-1)&&noegnud_map[x][y].low));
		    }

		}

	}
    }

    if (noegnud_render_selection_busy) return;

    glEnable (GL_BLEND);
    if (noegnud_activetileset->isalphatileset)
	for (y = 0; y < MAX_MAP_Y; y++)
	    {
		ny = -(y - (noegnud_render_middle_y));
		for (x = 0; x < MAX_MAP_X; x++)
		    {
			nx = x - (noegnud_render_middle_x);

			if (	(	((noegnud_activetileset->iso)) /*||
									 (noegnud_options_render_features_cardboardcutout->value&&noegnud_activetileset->isalphatileset&&(!noegnud_activetileset->iso))
								       */			)&&
				(y >= noegnud_clip_y)&&
				(!((x == noegnud_clip_x) && (y == noegnud_clip_y)))&&
				(!glyph_is_object(noegnud_map[x][y].glyph_primary))&&
				(!glyph_is_monster(noegnud_map[x][y].glyph_primary))&&
				(!glyph_is_trap(noegnud_map[x][y].glyph_primary))&&
				(sqrt((noegnud_clip_y-y)*(noegnud_clip_y-y)+(noegnud_clip_x-x)*(noegnud_clip_x-x))<3)
				) {
			    current_alpha=noegnud_options_mode_tileset_iso_opacity->value;
			} else {
			    current_alpha=1.0;
			}



			glPopMatrix ();
			glPushMatrix ();
			glTranslatef ((noegnud_activetileset->xdeltax) * (nx) +
				      (noegnud_activetileset->ydeltax) * (ny) +
				      noegnud_render_offset_x, 0,
				      -((noegnud_activetileset->xdeltay) * (nx) +
					(noegnud_activetileset->ydeltay) * (ny)) +
				      noegnud_render_offset_y);

			glBegin(GL_LINES); glEnd();

			if (noegnud_image_dungeon&&(noegnud_map[x][y].glyph_primary==MAX_GLYPH||(glyph_to_cmap(noegnud_map[x][y].glyph_primary)==S_stone&&levl[x+1][y].lit))) {

			    if (levl[x+1][y].seenv) {
				if (noegnud_options_render_features_cardboardcutout->value &&
				    noegnud_options_mode_tileset_caverns_cutout->value &&
				    noegnud_options_render_features_cardboardcutout_raise_walls->value&&(!IS_ROOM(levl[x+1][y].typ))) {
				    glTranslatef(0,noegnud_activetileset->glheight*noegnud_options_render_features_cardboardcutout_wall_height->value,0);
				}
			    } else {
				if (noegnud_options_render_features_cardboardcutout->value &&
				    noegnud_options_mode_tileset_caverns_cutout->value &&
				    noegnud_options_render_features_cardboardcutout_raise_walls->value) {
				    glTranslatef(0,noegnud_activetileset->glheight*noegnud_options_render_features_cardboardcutout_wall_height->value,0);}
			    }
} else
				    if (noegnud_map[x][y].glyph_primary!=MAX_GLYPH&&noegnud_options_render_features_cardboardcutout->value&&noegnud_options_render_features_cardboardcutout_raise_walls->value&&noegnud_activetileset->isalphatileset&&(!noegnud_activetileset->iso)) {
					if (noegnud_map[x][y].high) {

				if (IS_DOOR(level.locations[x+1][y].typ)) {
				    noegnud_render_map_single_tile_raised_walls (
										 noegnud_map[x][y].tile_middle,
										 noegnud_options_render_features_cardboardcutout_wall_height->value,
										 current_alpha,
										 !((y>0)&&noegnud_map[x][y-1].high),
										 !((y<MAX_MAP_Y-1)&&noegnud_map[x][y+1].high),
										 !((x>0)&&noegnud_map[x-1][y].high),
										 !((x<MAX_MAP_X-1)&&noegnud_map[x+1][y].high));

				} else {
				    noegnud_render_map_single_tile_raised_walls (
										 noegnud_map[x][y].tile_back,
										 noegnud_options_render_features_cardboardcutout_wall_height->value,
										 current_alpha,
										 !((y>0)&&noegnud_map[x][y-1].high),
										 !((y<MAX_MAP_Y-1)&&noegnud_map[x][y+1].high),
										 !((x>0)&&noegnud_map[x-1][y].high),
										 !((x<MAX_MAP_X-1)&&noegnud_map[x+1][y].high));
				}
				glTranslatef(0,noegnud_activetileset->glheight*noegnud_options_render_features_cardboardcutout_wall_height->value,0);
			    } else


				if (noegnud_map[x][y].low) {
				    glTranslatef(0,-noegnud_activetileset->glheight*noegnud_options_render_features_cardboardcutout_hole_depth->value,0);

				    if (noegnud_map[x][y].translucent) {
					glTranslatef(0,noegnud_activetileset->glheight*noegnud_options_render_features_cardboardcutout_hole_depth->value*noegnud_options_render_features_cardboardcutout_liquid_depth->value,0);
					current_alpha=0.5;
				    }

				}
			}

			if (((noegnud_render_mouse_map_x-1)==x)&&(noegnud_render_mouse_map_y==y)) {
			    glColor4f(0.25,1.0,0.25,1.0);
			}
			else if ((x == noegnud_clip_x) && (y == noegnud_clip_y)) {
			    glColor4f(noegnud_render_highlight_strobe/2+0.5,noegnud_render_highlight_strobe/2+0.5,noegnud_render_highlight_strobe/2+0.5,current_alpha);
			} else if (iflags.hilite_pet&&(noegnud_map[x][y].glyph_primary!=MAX_GLYPH)&&(noegnud_map[x][y].special_primary & MG_PET)) {
			    glColor4f(noegnud_render_highlight_strobe/2+0.5,noegnud_render_highlight_strobe/2+0.5,noegnud_render_highlight_strobe/2+0.5,current_alpha);
			} else {
			    glColor4f(1.0,1.0,1.0,current_alpha);
			}

			if (noegnud_activetileset->isalphatileset) {
			    if (noegnud_map[x][y].glyph_middle != MAX_GLYPH) {
				noegnud_render_map_single_tile (noegnud_map[x][y].tile_middle,1);
			    }
			    if (noegnud_activetileset->iso&&noegnud_map[x][y].glyph_fore != MAX_GLYPH) {
				noegnud_render_map_single_tile (noegnud_map[x][y].tile_fore,1);

			    }
			    if (((x == noegnud_clip_x) && (y == noegnud_clip_y)) ||
				(((noegnud_render_mouse_map_x-1)==x)&&(noegnud_render_mouse_map_y==y)) ||
				(noegnud_map[x][y].glyph_primary!=MAX_GLYPH&&iflags.hilite_pet&&(noegnud_map[x][y].special_primary & MG_PET))) {
				if (noegnud_options_render_features_cardboardcutout->value) glTranslatef(0,0.02,0);
				noegnud_render_map_tile_highlight_block ();
			    }

			}


		    }

	    }


    if ((noegnud_activetileset->isalphatileset)&&(!noegnud_activetileset->iso)) {

	if (noegnud_options_render_features_cardboardcutout->value) glDisable(GL_CULL_FACE);

	if (noegnud_options_render_features_cardboardcutout->value&&noegnud_options_render_features_cardboardcutout_wall_translucency_current->value>0) {

	    glDisable(GL_DEPTH_TEST);

	    for (y = ys; y != ye; y+=yd)
		{
		    ny = -(y - (noegnud_render_middle_y));
		    for (x = xs; x != xe; x+=xd)
			{
			    nx = x - (noegnud_render_middle_x);

			    glPopMatrix ();
			    glPushMatrix ();

			    if (((noegnud_render_mouse_map_x-1)==x)&&(noegnud_render_mouse_map_y==y)) {
				glColor4f(0.25,1.0,0.25,noegnud_options_render_features_cardboardcutout_wall_translucency_current->value);
			    } else {
				glColor4f(1.0,1.0,1.0,noegnud_options_render_features_cardboardcutout_wall_translucency_current->value);
			    }


			    glTranslatef ((noegnud_activetileset->xdeltax) * (nx) +
					  (noegnud_activetileset->ydeltax) * (ny) +
					  noegnud_render_offset_x, 0,
					  -((noegnud_activetileset->xdeltay) * (nx) +
					    (noegnud_activetileset->ydeltay) * (ny)) +
					  noegnud_render_offset_y);

			    glBegin(GL_LINES); glEnd();

			    if (!(noegnud_map[x][y].high||noegnud_map[x][y].low)) {

				if (noegnud_map[x][y].glyph_back!=MAX_GLYPH) {
				    if (noegnud_options_render_opengl_lighting_method->value==NOEGNUD_RENDER_LIGHTING_METHOD_CHEAP) {
					glEnable(GL_LIGHTING);
				    }

				    noegnud_render_map_single_tile (noegnud_map[x][y].tile_back,1);

				    if (noegnud_options_render_opengl_lighting_method->value==NOEGNUD_RENDER_LIGHTING_METHOD_CHEAP) {
					glDisable(GL_LIGHTING);
				    }

				}

				if (noegnud_map[x][y].glyph_middle!=MAX_GLYPH) {
				    noegnud_render_map_single_tile (noegnud_map[x][y].tile_middle,1);
				}
			    }

			    if (noegnud_map[x][y].glyph_fore != MAX_GLYPH) {
				if ((x==(u.ux-1)&&y==u.uy)&&Levitation) {
				    if (noegnud_map[x][y].low) {
					glTranslatef(0,noegnud_activetileset->glheight*noegnud_options_render_features_cardboardcutout_hole_depth->value,0);
				    }
				    glTranslatef(0,noegnud_activetileset->glheight/2,0);
				}
				if (noegnud_map[x][y].low) glTranslatef(0,-noegnud_activetileset->glheight*noegnud_options_render_features_cardboardcutout_hole_depth->value,0);
				if (((glyph_is_monster(noegnud_map[x][y].glyph_fore)&&noegnud_options_render_features_cardboardcutout_raise_monsters->value)||(glyph_is_object(noegnud_map[x][y].glyph_fore)&&noegnud_options_render_features_cardboardcutout_raise_objects->value))) {
				    glTranslatef (0,noegnud_activetileset->glheight,noegnud_activetileset->glheight/2);
				    glRotatef (90, 1, 0, 0);
				    if (noegnud_options_render_features_autoface->value) {
					glTranslatef (
						      noegnud_activetileset->glwidth/2,
						      0,0);
					glRotatef ((noegnud_options_render_opengl_view_angle_x_current->value+noegnud_render_bullettime_angle), 0, 0, 1);
					glTranslatef (
						      -noegnud_activetileset->glwidth/2,
						      0,0);
				    }
				    glRotatef (-20, 1, 0, 0);
				}
				noegnud_render_map_single_tile (noegnud_map[x][y].tile_fore,1);
			    }

			}

		}
 	    glEnable(GL_DEPTH_TEST);
	}

    if (noegnud_options_render_opengl_lighting_method->value==NOEGNUD_RENDER_LIGHTING_METHOD_CHEAP) {
	glDisable(GL_LIGHTING);
    }

	for (y = ys; y != ye; y+=yd)
	    {
		ny = -(y - (noegnud_render_middle_y));
		for (x = xs; x != xe; x+=xd)
		    {
			nx = x - (noegnud_render_middle_x);



			if ((noegnud_activetileset->iso)&&(y >= noegnud_clip_y)&&(!((x == noegnud_clip_x) && (y == noegnud_clip_y)))&&(!glyph_is_object(noegnud_map[x][y].glyph_primary))&&(!glyph_is_monster(noegnud_map[x][y].glyph_primary))&&(sqrt((noegnud_clip_y-y)*(noegnud_clip_y-y)+(noegnud_clip_x-x)*(noegnud_clip_x-x))<3)) {
			    current_alpha=0.3;
			} else {
			    current_alpha=1.0;
			}



			glPopMatrix ();
			glPushMatrix ();

			glTranslatef ((noegnud_activetileset->xdeltax) * (nx) +
				      (noegnud_activetileset->ydeltax) * (ny) +
				      noegnud_render_offset_x, 0,
				      -((noegnud_activetileset->xdeltay) * (nx) +
					(noegnud_activetileset->ydeltay) * (ny)) +
				      noegnud_render_offset_y);

			glBegin(GL_LINES); glEnd();

			if (((noegnud_render_mouse_map_x-1)==x)&&(noegnud_render_mouse_map_y==y)) {
			    glColor4f(0.25,1.0,0.25,1.0);
			}
			else if ((x == noegnud_clip_x) && (y == noegnud_clip_y)) {
			    glColor4f(noegnud_render_highlight_strobe/2+0.5,noegnud_render_highlight_strobe/2+0.5,noegnud_render_highlight_strobe/2+0.5,current_alpha);

			} else if (iflags.hilite_pet&&(noegnud_map[x][y].glyph_primary!=MAX_GLYPH)&&(noegnud_map[x][y].special_primary & MG_PET)) {
			    glColor4f(noegnud_render_highlight_strobe/2+0.5,noegnud_render_highlight_strobe/2+0.5,noegnud_render_highlight_strobe/2+0.5,current_alpha);
			} else {
			    glColor4f(1.0,1.0,1.0,current_alpha);
			}


			if (noegnud_map[x][y].glyph_fore != MAX_GLYPH) {
			    if ((x==(u.ux-1)&&y==u.uy)&&(!noegnud_activetileset->iso)&&Levitation) {
				if (noegnud_map[x][y].low) {
				    glTranslatef(0,noegnud_activetileset->glheight*noegnud_options_render_features_cardboardcutout_hole_depth->value,0);
				}
				glTranslatef(0,noegnud_activetileset->glheight/2,0);
			    }
			    if (noegnud_map[x][y].low&&(!noegnud_activetileset->iso)) glTranslatef(0,-noegnud_activetileset->glheight*noegnud_options_render_features_cardboardcutout_hole_depth->value,0);
			    if (noegnud_options_render_features_cardboardcutout->value&&((glyph_is_monster(noegnud_map[x][y].glyph_fore)&&noegnud_options_render_features_cardboardcutout_raise_monsters->value)||(glyph_is_object(noegnud_map[x][y].glyph_fore)&&noegnud_options_render_features_cardboardcutout_raise_objects->value))) {
				glTranslatef (0,noegnud_activetileset->glheight,noegnud_activetileset->glheight/2);
				glRotatef (90, 1, 0, 0);
				if (noegnud_options_render_features_autoface->value) {
				    glTranslatef (
						  noegnud_activetileset->glwidth/2,
						  0,0);
				    glRotatef ((noegnud_options_render_opengl_view_angle_x_current->value+noegnud_render_bullettime_angle), 0, 0, 1);
				    glTranslatef (
						  -noegnud_activetileset->glwidth/2,
						  0,0);
				}
				glRotatef (-20, 1, 0, 0);
			    }
			    noegnud_render_map_single_tile (noegnud_map[x][y].tile_fore,1);
			}

		    }

	    }
	if (noegnud_options_render_features_cardboardcutout->value) glEnable(GL_CULL_FACE);
    } else {
	if (noegnud_options_render_opengl_lighting_method->value==NOEGNUD_RENDER_LIGHTING_METHOD_CHEAP) {
	    glDisable(GL_LIGHTING);
	}
    }
}




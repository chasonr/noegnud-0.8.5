#include "noegnud_render_char.h"
#include "noegnud_extern.h"

#define NOEGNUD_CHAR_MODIFIER 4.0

void noegnud_render_map_single_char (int x, int y, int glyph_char, int colour) {
	float highlight_multiplier;

	// I dont know yet why i need the following line :/ needs fixing @ some stage ....
	if (colour==CLR_BLACK) colour=CLR_BLUE;

	      if ((x == noegnud_clip_x) && (y == noegnud_clip_y)) {
		      highlight_multiplier=noegnud_render_highlight_strobe/2+0.5;
	      } else if (iflags.hilite_pet&&(noegnud_map[x][y].glyph_primary!=MAX_GLYPH)&&(noegnud_map[x][y].special_primary & MG_PET)) {
		      highlight_multiplier=noegnud_render_highlight_strobe/2+0.5;
	      } else {
		      highlight_multiplier=1.0;
	      }


	glBindTexture (GL_TEXTURE_2D, noegnud_font_map->ch[glyph_char]);

	glColor4f(
		noegnud_nethackcolours[colour][0]*highlight_multiplier,
		noegnud_nethackcolours[colour][1]*highlight_multiplier,
		noegnud_nethackcolours[colour][2]*highlight_multiplier,
		noegnud_nethackcolours[colour][3]
	);

	glLoadName(y*256+x);
	glBegin (GL_QUADS);

	glTexCoord2f (noegnud_font_map->fx1, noegnud_font_map->fy1);
	glVertex3f (0, 0, 0);

	glTexCoord2f (noegnud_font_map->fx1, noegnud_font_map->fy2);
	glVertex3f (0, 0, noegnud_font_map->glheight*NOEGNUD_CHAR_MODIFIER);

	glTexCoord2f (noegnud_font_map->fx2, noegnud_font_map->fy2);
	glVertex3f (noegnud_font_map->glwidth*NOEGNUD_CHAR_MODIFIER, 0, noegnud_font_map->glheight*NOEGNUD_CHAR_MODIFIER);

	glTexCoord2f (noegnud_font_map->fx2, noegnud_font_map->fy1);
	glVertex3f (noegnud_font_map->glwidth*NOEGNUD_CHAR_MODIFIER, 0, 0);

	glEnd ();
	glEnd ();

}

void noegnud_render_map_single_selection_char (int x, int y) {
    glLoadName(y*256+x);

    glBegin (GL_QUADS);
    glVertex3f (0, 0, 0);
    glVertex3f (0, 0, noegnud_font_map->glheight*NOEGNUD_CHAR_MODIFIER);
    glVertex3f (noegnud_font_map->glwidth*NOEGNUD_CHAR_MODIFIER, 0, noegnud_font_map->glheight*NOEGNUD_CHAR_MODIFIER);
    glVertex3f (noegnud_font_map->glwidth*NOEGNUD_CHAR_MODIFIER, 0, 0);
    glEnd ();

    glEnd ();

}

void noegnud_render_map_char_highlight_block () {
    float w,h;

	glDisable(GL_TEXTURE_2D);

	glColor4f(1.0,1.0,1.0,noegnud_options_render_highlight_block_opacity->value);

	glBegin (GL_QUADS);
/*
	glVertex3f (0, 0, noegnud_font_map->glheight);
	glVertex3f (noegnud_font_map->glwidth, 0, noegnud_font_map->glheight);
	glVertex3f (noegnud_font_map->glwidth, 0, 0);
	glVertex3f (0, 0, 0);
*/

	w=noegnud_font_map->glwidth*NOEGNUD_CHAR_MODIFIER;
	h=noegnud_font_map->glheight*NOEGNUD_CHAR_MODIFIER;


	glVertex3f (0, 0, h);
	glVertex3f (w/10, 0, h);
	glVertex3f (w/10, 0, 0);
	glVertex3f (0, 0, 0);

	glVertex3f (w*9/10, 0, h);
	glVertex3f (w, 0, h);
	glVertex3f (w, 0, 0);
	glVertex3f (w*9/10, 0, 0);

	glVertex3f (w/10, 0, h/10);
	glVertex3f (w*9/10, 0, h/10);
	glVertex3f (w*9/10, 0, 0);
	glVertex3f (w/10, 0, 0);

	glVertex3f (w/10, 0, h);
	glVertex3f (w*9/10, 0, h);
	glVertex3f (w*9/10, 0, h*9/10);
	glVertex3f (w/10, 0, h*9/10);



	glEnd ();

	glEnable(GL_TEXTURE_2D);
}


void noegnud_render_map_char() {

  int x, y;

  int ys,ye,yd;
  float realangle;


      ys=0;
      ye=MAX_MAP_Y-1;
      yd=1;

      realangle=(noegnud_options_render_opengl_view_angle_x_current->value+noegnud_render_bullettime_angle);
      while (realangle>=360) realangle-=360;

      if ((realangle>=90)&&(realangle<270)) {
	      ys=MAX_MAP_Y-1;
	      ye=0;
	      yd=-1;
      }


      glDisable (GL_BLEND);

	if (noegnud_render_selection_busy) {
	    glDisable(GL_TEXTURE_2D);
	    glDisable(GL_LIGHTING);
	}

      if ((noegnud_options_render_features_cardboardcutout->value)||(noegnud_render_selection_busy))
      for (y = 0; y < MAX_MAP_Y; y++)
	{
	  for (x = 0; x < MAX_MAP_X; x++)
	    {
	      float nx, ny;
	      nx = x - (noegnud_render_middle_x);
	      ny = -(y - (noegnud_render_middle_y));

	      glPopMatrix ();
	      glPushMatrix ();
	      glTranslatef ((noegnud_font_map->glwidth*NOEGNUD_CHAR_MODIFIER ) *
			    (nx) + noegnud_render_offset_x, 0,
			    -(noegnud_font_map->glheight*NOEGNUD_CHAR_MODIFIER ) *
			    (ny) + noegnud_render_offset_y);


	if (noegnud_render_selection_busy) {
	    noegnud_render_map_single_selection_char (x, y);
	} else
	if (noegnud_map[x][y].glyph_middle!=MAX_GLYPH) {
		if (iflags.wc_color) {
		noegnud_render_map_single_char (x, y, noegnud_map[x][y].ch_middle, noegnud_map[x][y].colour_middle);
		} else {
		noegnud_render_map_single_char (x, y, noegnud_map[x][y].ch_middle, CLR_WHITE);
		}
	} else if (noegnud_map[x][y].glyph_back!=MAX_GLYPH) {
		if (iflags.wc_color) {
		noegnud_render_map_single_char (x, y, noegnud_map[x][y].ch_back,CLR_GRAY);
		} else
		{
		noegnud_render_map_single_char (x, y, noegnud_map[x][y].ch_back,CLR_WHITE);
		}
	}

	if ((((x == noegnud_clip_x) && (y == noegnud_clip_y))||(((noegnud_render_mouse_map_x-1)==x)&&(noegnud_render_mouse_map_y==y))) ||
		(iflags.hilite_pet&&(noegnud_map[x][y].special_primary & MG_PET))) {
		glEnable (GL_BLEND);
		noegnud_render_map_char_highlight_block ();
		glDisable (GL_BLEND);
	}

	}}


	if (noegnud_render_selection_busy) return;

	if (noegnud_options_render_features_cardboardcutout->value) glEnable (GL_BLEND);
	glColor4f(1.0,1.0,1.0,1.0);

     for (y = ys; y != ye; y+=yd)
	{
	  for (x = 0; x < MAX_MAP_X; x++)
	    {
	      float nx, ny;

		    nx = x - (noegnud_render_middle_x);
	      ny = -(y - (noegnud_render_middle_y));

	      glPopMatrix ();
	      glPushMatrix ();
	      glTranslatef ((noegnud_font_map->glwidth*NOEGNUD_CHAR_MODIFIER ) *
			    (nx) + noegnud_render_offset_x, 0,
			    -(noegnud_font_map->glheight*NOEGNUD_CHAR_MODIFIER ) *
			    (ny) + noegnud_render_offset_y);
	if (noegnud_options_render_features_cardboardcutout->value) {
		if (noegnud_map[x][y].glyph_fore!=MAX_GLYPH) {
			glTranslatef (0,noegnud_font_map->glheight*NOEGNUD_CHAR_MODIFIER,noegnud_font_map->glheight*NOEGNUD_CHAR_MODIFIER/2);
			glRotatef (70, 1, 0, 0);
		if (iflags.wc_color) {
			noegnud_render_map_single_char (x, y, noegnud_map[x][y].ch_fore, noegnud_map[x][y].colour_fore);
		} else {
			noegnud_render_map_single_char (x, y, noegnud_map[x][y].ch_fore, CLR_WHITE);
		}
		}
	} else if (noegnud_map[x][y].glyph_primary!=MAX_GLYPH) {
		if (iflags.wc_color) {
	noegnud_render_map_single_char (x, y, noegnud_map[x][y].ch_primary, noegnud_map[x][y].colour_primary);
		} else {
	noegnud_render_map_single_char (x, y, noegnud_map[x][y].ch_primary, CLR_WHITE);
		}

	if ((((x == noegnud_clip_x) && (y == noegnud_clip_y))||(((noegnud_render_mouse_map_x-1)==x)&&(noegnud_render_mouse_map_y==y))) ||
		(iflags.hilite_pet&&(noegnud_map[x][y].special_primary & MG_PET))) {
		if (!noegnud_options_render_features_cardboardcutout->value) glEnable (GL_BLEND);
		noegnud_render_map_char_highlight_block ();
		if (!noegnud_options_render_features_cardboardcutout->value) glDisable (GL_BLEND);
	}

		}
	}}

}

#include "noegnud_rip.h"
#include "noegnud_collection.h"
#include "noegnud_extern.h"

#define STONE_LINE_CENT 28-(11)	/* char[] element of center of stone face */
#define STONE_LINE_LEN 16	/* # chars that fit on one line
				 * (note 1 ' ' border)
				 */
#define NAME_LINE 6		/* *char[] line # for player name */
#define GOLD_LINE 7		/* *char[] line # for amount of gold */
#define DEATH_LINE 8		/* *char[] line # for death description */
#define YEAR_LINE 12		/* *char[] line # for year */

extern const char * const killed_by_prefix[];	/* from topten.c */

void noegnud_rip_outrip_finalize(winid tmpwin) {
	char buf[BUFSZ];
	char *bufs;
	char *bufc;
	int  stringwidth;

	noegnud_gui_ttext *widget_text;

	noegnud_optiontype_string *rip_image;
	noegnud_optiontype_string *rip_font;
	noegnud_optiontype_int *rip_width;
	noegnud_optiontype_int *rip_name_x;
	noegnud_optiontype_int *rip_name_y;
	noegnud_optiontype_int *rip_year_x;
	noegnud_optiontype_int *rip_year_y;
	noegnud_optiontype_int *rip_gold_x;
	noegnud_optiontype_int *rip_gold_y;
	noegnud_optiontype_int *rip_how_x;
	noegnud_optiontype_int *rip_how_y;
	noegnud_optiontype_rgb *rip_tomb_colour;

	noegnud_fonts_tfont *rip_font_font;

	rip_image=noegnud_collection_data(noegnud_options,"gui.window.tombstone.image");
	rip_font=noegnud_collection_data(noegnud_options,"gui.window.tombstone.font");

	rip_width=noegnud_collection_data(noegnud_options,"gui.window.tombstone.width");

	rip_name_x=noegnud_collection_data(noegnud_options,"gui.window.tombstone.name.x");
	rip_name_y=noegnud_collection_data(noegnud_options,"gui.window.tombstone.name.y");
	rip_year_x=noegnud_collection_data(noegnud_options,"gui.window.tombstone.year.x");
	rip_year_y=noegnud_collection_data(noegnud_options,"gui.window.tombstone.year.y");
	rip_gold_x=noegnud_collection_data(noegnud_options,"gui.window.tombstone.gold.x");
	rip_gold_y=noegnud_collection_data(noegnud_options,"gui.window.tombstone.gold.y");
	rip_how_x=noegnud_collection_data(noegnud_options,"gui.window.tombstone.how.x");
	rip_how_y=noegnud_collection_data(noegnud_options,"gui.window.tombstone.how.y");

	rip_tomb_colour=noegnud_collection_data(noegnud_options,"gui.window.tombstone.tomb.colour");

	((noegnud_gui_twindow *)tmpwin)->image=noegnud_glfuncs_loadimage(rip_image->value,TRUE,FALSE,FALSE);
	((noegnud_gui_twindow *)tmpwin)->localimage=1;
	if (((noegnud_gui_twindow *)tmpwin)->image) {
		((noegnud_gui_twindow *)tmpwin)->widget.width=((noegnud_gui_twindow *)tmpwin)->image->width;
		((noegnud_gui_twindow *)tmpwin)->widget.height=((noegnud_gui_twindow *)tmpwin)->image->height;
		((noegnud_gui_twindow *)tmpwin)->widget.r=((noegnud_gui_twindow *)tmpwin)->widget.g=((noegnud_gui_twindow *)tmpwin)->widget.b=((noegnud_gui_twindow *)tmpwin)->widget.a=1.0;
		((noegnud_gui_twindow *)tmpwin)->image_tiled=0;
		((noegnud_gui_twindow *)tmpwin)->autoresize=0;
	}
	((noegnud_gui_twindow *)tmpwin)->widget.x=(noegnud_options_screenwidth->value-((noegnud_gui_twindow *)tmpwin)->widget.width)/2;
	((noegnud_gui_twindow *)tmpwin)->widget.y=(noegnud_options_screenheight->value-((noegnud_gui_twindow *)tmpwin)->widget.height)/2;

	rip_font_font=noegnud_fonts_loadfont(rip_font->value);

	//name
	stringwidth=noegnud_width_dynamic(rip_font_font,plname);
	widget_text=noegnud_gui_create_text(
		((noegnud_gui_twidget *)tmpwin)->child,
		rip_name_x->value-(((float)stringwidth)/2),
		rip_name_y->value,
//		strlen(plname)*rip_font_font->width,
		stringwidth,
		rip_font_font->height,
		rip_tomb_colour->r,
		rip_tomb_colour->g,
		rip_tomb_colour->b,
		plname);
	widget_text->font=rip_font_font;
	widget_text->dynamiccharwidth=TRUE;

	//year
	Sprintf(buf, "%4d", getyear());
	stringwidth=noegnud_width_dynamic(rip_font_font,buf);
	widget_text=noegnud_gui_create_text(
		((noegnud_gui_twidget *)tmpwin)->child,
		rip_year_x->value-(((float)stringwidth)/2),
		rip_year_y->value,
//		strlen(buf)*rip_font_font->width,
		stringwidth,
		rip_font_font->height,
		rip_tomb_colour->r,
		rip_tomb_colour->g,
		rip_tomb_colour->b,
		buf);
	widget_text->font=rip_font_font;
	widget_text->dynamiccharwidth=TRUE;


	//gold
#ifndef GOLDOBJ
	Sprintf(buf, "%ld Au", u.ugold);
#else
	Sprintf(buf, "%ld Au", done_money);
#endif
	stringwidth=noegnud_width_dynamic(rip_font_font,buf);
	widget_text=noegnud_gui_create_text(
		((noegnud_gui_twidget *)tmpwin)->child,
		rip_gold_x->value-(((float)stringwidth)/2),
		rip_gold_y->value,
//		strlen(buf)*rip_font_font->width,
		stringwidth,
		rip_font_font->height,
		rip_tomb_colour->r,
		rip_tomb_colour->g,
		rip_tomb_colour->b,
		buf);
	widget_text->font=rip_font_font;
	widget_text->dynamiccharwidth=TRUE;


	//how
	switch (killer_format) {
		default: impossible("bad killer format?");
		case KILLED_BY_AN:
			Strcpy(buf, killed_by_prefix[((noegnud_gui_twindow *)tmpwin)->rip_how]);
			Strcat(buf, an(killer));
			break;
		case KILLED_BY:
			Strcpy(buf, killed_by_prefix[((noegnud_gui_twindow *)tmpwin)->rip_how]);
			Strcat(buf, killer);
			break;
		case NO_KILLER_PREFIX:
			Strcpy(buf, killer);
			break;
	}

//	if (rip_width->value<23) rip_width->value=23;
	bufs=(char *)&buf;
	if (strlen(buf)>rip_width->value) {
		while (strlen(bufs)>rip_width->value) {
			bufc=bufs+rip_width->value;
			while ((bufc>bufs)&&(*bufc!=' ')) bufc--;
			if (bufc==bufs) break;
			*bufc=0;

	stringwidth=noegnud_width_dynamic(rip_font_font,bufs);
	widget_text=noegnud_gui_create_text(
		((noegnud_gui_twidget *)tmpwin)->child,
		rip_how_x->value-(((float)stringwidth)/2),
		rip_how_y->value,
//		strlen(buf)*rip_font_font->width,
		stringwidth,
		rip_font_font->height,
		rip_tomb_colour->r,
		rip_tomb_colour->g,
		rip_tomb_colour->b,
		bufs);
	widget_text->font=rip_font_font;
	widget_text->dynamiccharwidth=TRUE;
			rip_how_y->value+=rip_font_font->height;

			bufs=bufc+1;
		}
	}

	stringwidth=noegnud_width_dynamic(rip_font_font,bufs);
	widget_text=noegnud_gui_create_text(
		((noegnud_gui_twidget *)tmpwin)->child,
		rip_how_x->value-(((float)stringwidth)/2),
		rip_how_y->value,
		stringwidth,
		rip_font_font->height,
		rip_tomb_colour->r,
		rip_tomb_colour->g,
		rip_tomb_colour->b,
		bufs);
	widget_text->font=rip_font_font;
	widget_text->dynamiccharwidth=TRUE;

}

void noegnud_rip_outrip(winid tmpwin, int how) {
	noegnud_optiontype_int *rip_description_x;
	noegnud_optiontype_int *rip_description_y;

	rip_description_x=noegnud_collection_data(noegnud_options,"gui.window.tombstone.description.x");
	rip_description_y=noegnud_collection_data(noegnud_options,"gui.window.tombstone.description.y");

	((noegnud_gui_twindow *)tmpwin)->rip=1;
	((noegnud_gui_twindow *)tmpwin)->rip_how=how;

	noegnud_gui_create_widget(
		(noegnud_gui_twidget *)tmpwin,
		0,0,
		noegnud_options_screenwidth->value,
		noegnud_options_screenheight->value,
		0);

	noegnud_gui_create_widget((noegnud_gui_twidget *)tmpwin,rip_description_x->value,rip_description_y->value,80*noegnud_gui_get_fontwidth(),6*(noegnud_gui_get_fontheight()+8),0);

}

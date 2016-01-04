#ifndef _NOEGNUD_WIDGETTHEME_H_
#define _NOEGNUD_WIDGETTHEME_H


#include "noegnud_collection.h"

typedef struct {
    float x,y;
} noegnud_widgettheme_tsegmentlocation;

typedef struct {
    noegnud_widgettheme_tsegmentlocation parent_alignment;
    noegnud_widgettheme_tsegmentlocation segment_relative_offset;
    noegnud_widgettheme_tsegmentlocation pixel_offset;
} noegnud_widgettheme_tfulloffset;

typedef struct {
    noegnud_glfuncs_timage *image;
    noegnud_widgettheme_tfulloffset start;
    noegnud_widgettheme_tfulloffset end;

    int tiled_x;
    int tiled_y;
} noegnud_widgettheme_tsegment;

void noegnud_widgettheme_init();

int noegnud_widgettheme_startup();
void noegnud_widgettheme_done();

noegnud_tcollection *noegnud_widgettheme_load(const char *filename);

void noegnud_widgettheme_draw(noegnud_gui_twidget *widget, noegnud_tcollection *theme);

#endif


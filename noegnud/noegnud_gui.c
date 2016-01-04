#include <stdlib.h>

#include "noegnud_interface.h"
#include "noegnud_common.h"
#include "noegnud_gui.h"
#include "noegnud_fonts.h"
#include "noegnud_tilesets.h"
#include "noegnud_extern.h"
#include "noegnud_mem.h"
#include "noegnud_textcolour.h"
#include "noegnud_widgettheme.h"

#include "hack.h"

noegnud_gui_twidget *noegnud_gui_active = NULL;
noegnud_gui_twidget *noegnud_guiwidget_desktop = NULL;
noegnud_gui_twidget *noegnud_guiwidget_console = NULL;
noegnud_gui_twidget *noegnud_guiwidget_minimap = NULL;
noegnud_gui_twidget *noegnud_guiwidget_status = NULL;
noegnud_gui_twidget *noegnud_guiwidget_status_container = NULL;
noegnud_gui_twidget *noegnud_guiwidget_options = NULL;
#ifdef POSITIONBAR
noegnud_gui_twidget *noegnud_guiwidget_positionbar = NULL;
noegnud_gui_twidget *noegnud_guiwidget_positionbar_container = NULL;
#endif
noegnud_gui_twidget *noegnud_guiwidget_desktop_overlay_hallucinating = NULL;
noegnud_gui_twidget *noegnud_guiwidget_desktop_overlay_confused = NULL;
noegnud_gui_twidget *noegnud_guiwidget_desktop_overlay_stunned = NULL;
noegnud_gui_twidget *noegnud_guiwidget_desktop_overlay_poisoned = NULL;
noegnud_gui_twidget *noegnud_guiwidget_desktop_overlay_blind = NULL;
noegnud_gui_twidget *noegnud_guiwidget_desktop_overlay_slimed = NULL;

noegnud_fonts_tfont *noegnud_gui_font;

noegnud_gui_twidget *noegnud_gui_grabbed=NULL;

static noegnud_glfuncs_timage *noegnud_gui_windowimage;
noegnud_tcollection *noegnud_gui_default_widgettheme;
noegnud_tcollection *noegnud_gui_default_title_active_widgettheme;
noegnud_tcollection *noegnud_gui_default_title_widgettheme;
noegnud_tcollection *noegnud_gui_default_button_widgettheme;
noegnud_tcollection *noegnud_gui_default_buttonover_widgettheme;
noegnud_tcollection *noegnud_gui_default_minimap_widgettheme;
noegnud_tcollection *noegnud_gui_default_scrollbar_widgettheme;
noegnud_tcollection *noegnud_gui_default_scrollbararea_widgettheme;
noegnud_tcollection *noegnud_gui_default_item_widgettheme;
noegnud_tcollection *noegnud_gui_default_item_highlighted_widgettheme;
noegnud_tcollection *noegnud_gui_default_item_selected_widgettheme;

#define NOEGNUD_GUI_SCROLLER_WIDTH 16

static int noegnud_gui_grabbed_x;
static int noegnud_gui_grabbed_y;

int noegnud_gui_mouseoverwidget=0;
int noegnud_gui_mouseoverminimap=0;

int noegnud_gui_currentmenuselectionmethod;
int noegnud_gui_menuitem_throwreturn=0;

anything noegnud_gui_menuselect_identifier;

void noegnud_gui_init() {

    atexit(noegnud_gui_done);

    noegnud_gui_font=noegnud_fonts_loadfont(noegnud_options_gui_font->value);
    noegnud_gui_windowimage=noegnud_glfuncs_loadimage(noegnud_options_gui_texture->value,FALSE,TRUE,FALSE);

    noegnud_widgettheme_init();
    noegnud_gui_default_widgettheme=noegnud_widgettheme_load("default_window");
    noegnud_gui_default_title_active_widgettheme=noegnud_widgettheme_load("default_title_active");
    noegnud_gui_default_title_widgettheme=noegnud_widgettheme_load("default_title");
    noegnud_gui_default_button_widgettheme=noegnud_widgettheme_load("default_button");
    noegnud_gui_default_buttonover_widgettheme=noegnud_widgettheme_load("default_buttonover");
    noegnud_gui_default_minimap_widgettheme=noegnud_widgettheme_load("default_minimap");
    noegnud_gui_default_scrollbararea_widgettheme=noegnud_widgettheme_load("default_scrollbararea");
    noegnud_gui_default_scrollbar_widgettheme=noegnud_widgettheme_load("default_scrollbar");

    noegnud_gui_default_item_widgettheme=noegnud_widgettheme_load("default_item");
    noegnud_gui_default_item_highlighted_widgettheme=noegnud_widgettheme_load("default_item_highlight");
    noegnud_gui_default_item_selected_widgettheme=noegnud_widgettheme_load("default_item_selected");
}

void noegnud_gui_done() {
    noegnud_fonts_deletefont(&noegnud_gui_font);
    noegnud_glfuncs_unloadimage(noegnud_gui_windowimage);
}

int noegnud_gui_get_fontwidth(void) { return noegnud_gui_font->width; }
int noegnud_gui_get_fontheight(void) { return noegnud_gui_font->height; }

/*WIDGET*/

noegnud_gui_twidget *noegnud_gui_create_widget(noegnud_gui_twidget * parent, int x, int y, int width, int height, int datasize) {
    noegnud_gui_twidget *widget;

    widget=noegnud_mem_malloc(sizeof(noegnud_gui_twidget)+datasize);

    widget->type=NOEGNUD_GUI_WIDGET;

    widget->parent=parent;
    widget->child=NULL;
    widget->nextsibling=NULL;
    widget->prevsibling=NULL;

    widget->x=x;
    widget->y=y;
    widget->offset_x=0;
    widget->offset_y=0;
    widget->width=width;
    widget->height=height;

    widget->r=widget->g=widget->b=widget->a=1.0;

    widget->theme=NULL;

    widget->vscrollwidget=NULL;
    widget->hscrollwidget=NULL;

    widget->hscroll=0;
    widget->hscroll_pos=0;
    widget->vscroll=0;
    widget->vscroll_pos=0;

    widget->autocentre=TRUE;

    widget->mouseover_check=1;
    widget->mouseover=0;

    if (parent) noegnud_gui_widget_addchild(parent,widget);

    widget->draw=noegnud_gui_draw_widget;
    widget->kill=noegnud_gui_kill_widget;
    widget->autosize=noegnud_gui_autosize_widget;
    widget->event=noegnud_gui_event_widget;

    widget->clipto=widget->parent;

    return widget;
}

void noegnud_gui_kill_widget_relatives(noegnud_gui_twidget *widget) {
    noegnud_gui_twidget *sib;
    noegnud_gui_twidget *next;

    if (widget->child) {
	sib=widget->child;
	while (sib->prevsibling) sib=sib->prevsibling;
	while (sib) {
	    next=sib->nextsibling;
	    sib->kill(sib);
	    sib=next;
	}
	widget->child=NULL;
    }
}

static void noegnud_gui_draw_widget_relatives(noegnud_gui_twidget *widget) {
    if (widget->child) widget->child->draw(widget->child,1);
    if (widget->nextsibling) widget->nextsibling->draw(widget->nextsibling,1);
}

static void noegnud_gui_event_widget_relatives(noegnud_gui_twidget *widget, SDL_Event *event) {
    if (widget->nextsibling) widget->nextsibling->event(widget->nextsibling, event);
    if (widget->child) widget->child->event(widget->child, event);
}

int noegnud_gui_overselforchildrenandsibs(noegnud_gui_twidget *widget) {
    return (widget&&((widget->child&&noegnud_gui_overselforchildrenandsibs(widget->child))||(widget->nextsibling&&noegnud_gui_overselforchildrenandsibs(widget->nextsibling)))) || (noegnud_gui_mouseoverwidget==(int)widget);
}

int noegnud_gui_overselforchildren(noegnud_gui_twidget *widget) {
    return (widget&&((widget->child&&noegnud_gui_overselforchildrenandsibs(widget->child)))) || (noegnud_gui_mouseoverwidget==(int)widget);
}

void noegnud_gui_event_null(noegnud_gui_twidget *widget, SDL_Event *event) {
    if (widget->nextsibling) widget->nextsibling->event(widget->nextsibling, event);
}

void noegnud_gui_event_widget(noegnud_gui_twidget *widget, SDL_Event *event) {
    noegnud_gui_event_widget_relatives(widget,event);
    if (widget->mouseover_check) {
	if (event->type==SDL_MOUSEMOTION) widget->mouseover=noegnud_gui_widget_contained(widget,event->motion.x,event->motion.y);
	if (widget->mouseover) {
	    if (!noegnud_gui_mouseoverwidget) noegnud_gui_mouseoverwidget=(int)widget;
	}
    }
}

void noegnud_gui_kill_widget(noegnud_gui_twidget *widget) {

    noegnud_gui_kill_widget_relatives(widget);

    if ((widget->parent)&&(widget->parent->child==widget)) {
	if (widget->prevsibling) {
	    widget->parent->child=widget->prevsibling;
	} else
	    if (widget->nextsibling) {
		widget->parent->child=widget->nextsibling;
	    } else {
		widget->parent->child=NULL;
	    }
    }
    if (widget->prevsibling) widget->prevsibling->nextsibling=widget->nextsibling;
    if (widget->nextsibling) widget->nextsibling->prevsibling=widget->prevsibling;
    if (noegnud_gui_grabbed==widget) noegnud_gui_grabbed=NULL;
    noegnud_mem_free(widget);
}

void noegnud_gui_draw_null(noegnud_gui_twidget *widget, int drawchildren) {
    if (widget->nextsibling) widget->nextsibling->draw(widget->nextsibling,1);
}

void noegnud_gui_draw_widget(noegnud_gui_twidget *widget, int drawchildren) {
    if (drawchildren) noegnud_gui_draw_widget_relatives(widget);
}

void noegnud_gui_autosize_widget(noegnud_gui_twidget *widget) {
}

void noegnud_gui_resize_widget(noegnud_gui_twidget *widget, int width, int height) {
    int max_x, max_y;


    if (widget->type==NOEGNUD_GUI_WINDOW&&!((noegnud_gui_twindow *)widget)->autoresize) {
	max_x=widget->width;
	max_y=widget->height;
	if (width<max_x) width=max_x;
	if (height<max_y) height=max_y;
    } else {
	max_x=noegnud_options_screenwidth->value*3/4;
	max_y=noegnud_options_screenheight->value*3/4;
    }

    if (width<0) width=0;
    if (height<0) height=0;
    if (width>max_x) {
	widget->hscroll=width-max_x;
	width=max_x;
    }
    if (height>max_y) {
	widget->vscroll=height-(max_y);
	if (widget->vscrollwidget) {
	    widget->vscrollwidget->autosize(widget->vscrollwidget);
	} else {
	    if (widget->type==NOEGNUD_GUI_WINDOW&&(!widget->vscrollwidget)) {
		width+=NOEGNUD_GUI_SCROLLER_WIDTH;
		widget->vscrollwidget=(noegnud_gui_twidget *)noegnud_gui_create_vscroll(widget);
	    }
	}
	height= (max_y);
    }

    widget->width=width;
    widget->height=height;

    if (widget->vscrollwidget) {
    widget->vscrollwidget->x=widget->vscrollwidget->parent->width-NOEGNUD_GUI_SCROLLER_WIDTH;
    widget->vscrollwidget->height=widget->vscrollwidget->parent->height;
    }
}

void noegnud_gui_checkoffset_widget(noegnud_gui_twidget *widget) {
    if (widget->offset_x<0) widget->offset_x=0;
    if (widget->offset_y<0) widget->offset_y=0;

    if (widget->offset_x>widget->hscroll) widget->offset_x=widget->hscroll;
    if (widget->offset_y>widget->vscroll) widget->offset_y=widget->vscroll;
}

void noegnud_gui_offset_widget(noegnud_gui_twidget *widget, int offset_x, int offset_y) {
    widget->offset_x=offset_x;
    widget->offset_y=offset_y;

    noegnud_gui_checkoffset_widget(widget);
}

void noegnud_gui_widget_addchild(noegnud_gui_twidget *widget,noegnud_gui_twidget *child) {
    if (widget->child) {
	noegnud_gui_widget_addsibling(widget->child,child);
    } else {
	widget->child=child;
	child->parent=widget;
    }
}

void noegnud_gui_widget_addsibling(noegnud_gui_twidget *widget,noegnud_gui_twidget *sibling) {
    noegnud_gui_twidget *sib;

    sib=widget;
    while (sib->nextsibling) sib=sib->nextsibling;
    sib->nextsibling=sibling;
    sibling->prevsibling=sib;
}

int noegnud_gui_widget_getabsolute_x(noegnud_gui_twidget *widget) {
    int x;
    noegnud_gui_twidget *w;

    w=widget;
    x=w->x;
    while (w->parent) {
	x+=w->parent->x;
	w=w->parent;
    }
    return x;
}

int noegnud_gui_widget_getabsolute_y(noegnud_gui_twidget *widget) {
    int y;
    noegnud_gui_twidget *w;

    w=widget;
    y=w->y;
    while (w->parent) {
	y+=w->parent->y;
	if ( widget->type!=NOEGNUD_GUI_VSCROLL &&
	     widget->type!=NOEGNUD_GUI_TITLEBAR &&
	     widget->parent->type!=NOEGNUD_GUI_TITLEBAR) y-=w->parent->offset_y;
	w=w->parent;
    }
    return y;
}

void noegnud_gui_widget_cliptome(noegnud_gui_twidget *widget) {
    int ax, ay;

    if (!widget->clipto) {
	glScissor(0,0,noegnud_options_screenwidth->value,noegnud_options_screenheight->value);
	return;
    }

    ax=noegnud_gui_widget_getabsolute_x(widget->clipto);
    ay=noegnud_gui_widget_getabsolute_y(widget->clipto);

    if (noegnud_options_gui_widgetborder->value) {
	glScissor(
		  ax+1,
		  noegnud_options_screenheight->value-(ay+widget->clipto->height-1),
		  widget->clipto->width-1,
		  widget->clipto->height-1);
    } else {
	glScissor(
		  ax,
		  noegnud_options_screenheight->value-(ay+widget->clipto->height),
		  widget->clipto->width,
		  widget->clipto->height);
    }

}

/*WINDOW*/

noegnud_gui_twindow *
noegnud_gui_create_window(
			  noegnud_gui_twidget * parent,
			  int x, int y,
			  int width, int height,
			  GLfloat r, GLfloat g, GLfloat b, GLfloat a,
			  int datasize) {

    noegnud_gui_twindow *window;

    window=(noegnud_gui_twindow *)noegnud_gui_create_widget(parent,x,y,width,height,(sizeof(noegnud_gui_twindow)-sizeof(noegnud_gui_twidget))+datasize);

    window->widget.type=NOEGNUD_GUI_WINDOW;
    window->widget.draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_window;
    window->widget.event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_window;
    window->widget.kill=(noegnud_gui_kill_widget_proc *)noegnud_gui_kill_window;

    window->widget.r=r;
    window->widget.g=g;
    window->widget.b=b;
    window->widget.a=a;

    window->autoresize=1;

    window->rip=0;
    window->rip_how=0;

    //window->image=noegnud_gui_windowimage;
    window->image=NULL;
    window->image_tiled=1;
    window->localimage=0;

    window->movable=1;
    window->movement_lock_x=0;

    window->refreshtitlebar=TRUE;
    window->titlebar=NULL;

    window->tomove=(noegnud_gui_twidget *)window;

    return window;
}

void noegnud_gui_draw_window(noegnud_gui_twindow *window, int drawchildren) {
    noegnud_gui_twidget *parent;

    GLfloat image_delta_x, image_delta_y;

    int block_x, block_y;
    float start_x,start_y,width,height;

    if (!window->widget.width||!window->widget.height) {
	if (drawchildren) noegnud_gui_draw_widget_relatives((noegnud_gui_twidget *)window);
	return;
    }

    glDisable (GL_TEXTURE_2D);

    glLoadIdentity ();
    noegnud_gui_widget_cliptome((noegnud_gui_twidget *)window);

    if ((noegnud_gui_twidget *)window->widget.theme) {
	noegnud_widgettheme_draw((noegnud_gui_twidget *)window,window->widget.theme);
	if (!window->localimage) {
	    if (drawchildren) noegnud_gui_draw_widget_relatives((noegnud_gui_twidget *)window);
	    return;
	}
    }


    parent=window->widget.parent;
    while(parent) {
	glTranslated (parent->x,-parent->y, 0);
	noegnud_gui_checkoffset_widget(parent);
        if ( window->widget.type!=NOEGNUD_GUI_VSCROLL && window->widget.type!=NOEGNUD_GUI_TITLEBAR) glTranslated (parent->offset_x,parent->offset_y, 0);
	parent=parent->parent;
    }
    glTranslated (window->widget.x,noegnud_options_screenheight->value-window->widget.y, 0);

    glBegin (GL_QUADS);

    glColor4f(window->widget.r,window->widget.g,window->widget.b,window->widget.a);
    if (window->image) {
	glEnd ();
	glEnable (GL_TEXTURE_2D);
	if (window->image_tiled) {

	    image_delta_x=window->widget.offset_x/(float)window->image->block[0][0]->width;
	    image_delta_y=window->widget.offset_y/(float)window->image->block[0][0]->height;

	    glBindTexture(GL_TEXTURE_2D,window->image->block[0][0]->image);
	    glBegin (GL_QUADS);
	    glTexCoord2f (image_delta_x,image_delta_y);
	    glVertex2i (0, 0);
	    glTexCoord2f (image_delta_x,image_delta_y+(float)window->widget.height/(float)window->image->block[0][0]->height);
	    glVertex2i (0, -window->widget.height);
	    glTexCoord2f (image_delta_x+(float)window->widget.width/(float)window->image->block[0][0]->width,image_delta_y+(float)window->widget.height/(float)window->image->block[0][0]->height);
	    glVertex2i (window->widget.width, -window->widget.height);
	    glTexCoord2f (image_delta_x+(float)window->widget.width/(float)window->image->block[0][0]->width,image_delta_y);
	    glVertex2i (window->widget.width, 0);
	    glEnd ();
	} else {


	    block_x=0;
	    block_y=0;

	    for (block_y=0; block_y<window->image->block_height;block_y++)
		for (block_x=0; block_x<window->image->block_width;block_x++) {

		    start_x=((float)(block_x*noegnud_glfuncs_maximum_texturesize))*((float)(window->widget.width)/(float)(window->image->width));
		    start_y=((float)(block_y*noegnud_glfuncs_maximum_texturesize))*((float)(window->widget.height)/(float)(window->image->height));
		    width =((float)(window->image->block[block_x][block_y]->width ))*((float)(window->widget.width )/(float)(window->image->width));
		    height=((float)(window->image->block[block_x][block_y]->height))*((float)(window->widget.height)/(float)(window->image->height));

		    glBindTexture(GL_TEXTURE_2D,window->image->block[block_x][block_y]->image);
		    glBegin (GL_QUADS);
		    glTexCoord2f (window->image->block[block_x][block_y]->fx1,window->image->block[block_x][block_y]->fy1);
		    glVertex2f ( start_x, -start_y);
		    glTexCoord2f (window->image->block[block_x][block_y]->fx1,window->image->block[block_x][block_y]->fy2);
		    glVertex2f (start_x, -(start_y+height));
		    glTexCoord2f (window->image->block[block_x][block_y]->fx2,window->image->block[block_x][block_y]->fy2);
		    glVertex2f (start_x+width, -(start_y+height));
		    glTexCoord2f (window->image->block[block_x][block_y]->fx2,window->image->block[block_x][block_y]->fy1);
		    glVertex2f (start_x+width, -start_y);
		    glEnd ();

		}
	}
	glDisable (GL_TEXTURE_2D);
    } else {
	glVertex2i (0, 0);
	glVertex2i (0, -window->widget.height);
	glVertex2i (window->widget.width, -window->widget.height);
	glVertex2i (window->widget.width, 0);
	glEnd ();
    }

    if (noegnud_options_gui_widgetborder->value) {
	glColor4f(1.0,1.0,1.0,1.0);
	glBegin (GL_LINES);
	glVertex2i (0, 0);
	glVertex2i (window->widget.width, 0);
	glVertex2i (window->widget.width, 0);
	glVertex2i (window->widget.width, -window->widget.height);
	glVertex2i (window->widget.width, -window->widget.height);
	glVertex2i (0, -window->widget.height);
	glVertex2i (0, -window->widget.height);
	glVertex2i (0, 0);
	glEnd ();
    }

    if (drawchildren) noegnud_gui_draw_widget_relatives((noegnud_gui_twidget *)window);

}

void noegnud_gui_event_window(noegnud_gui_twindow *window, SDL_Event *event) {
    int px, py;
    noegnud_gui_twidget *tmp_widget;

    noegnud_gui_event_widget_relatives((noegnud_gui_twidget *)window,event);

    if (window->widget.type==NOEGNUD_GUI_TITLEBAR&&((noegnud_gui_twindow *)window->widget.parent)->refreshtitlebar) {
	if (window->widget.parent==noegnud_gui_active) {
	    window->widget.theme=noegnud_gui_default_title_active_widgettheme;
	} else {
	    window->widget.theme=noegnud_gui_default_title_widgettheme;
	}
    }


    if (window->widget.mouseover_check) {
	if (event->type==SDL_MOUSEMOTION) window->widget.mouseover=noegnud_gui_widget_contained((noegnud_gui_twidget *)window,event->motion.x,event->motion.y);
	if (window->widget.mouseover) {
	    if ( !noegnud_gui_mouseoverwidget) noegnud_gui_mouseoverwidget=(int)window;
	}
    }


    if ((!noegnud_gui_grabbed)||(noegnud_gui_grabbed==(noegnud_gui_twidget *)window)) {
	px=noegnud_gui_widget_getabsolute_x((noegnud_gui_twidget *)window);
	py=noegnud_gui_widget_getabsolute_y((noegnud_gui_twidget *)window);

	if (noegnud_gui_grabbed==(noegnud_gui_twidget *)window) {

	    if (event->type==SDL_MOUSEBUTTONUP) {
		noegnud_gui_grabbed=NULL;
	    }

	    if (event->type==SDL_MOUSEMOTION) {
		if (!window->movement_lock_x) window->tomove->x+=(event->motion.x-noegnud_gui_grabbed_x);
		window->tomove->y+=(event->motion.y-noegnud_gui_grabbed_y);
		noegnud_gui_grabbed_x=event->motion.x;
		noegnud_gui_grabbed_y=event->motion.y;
	    }

	} else
	    if (noegnud_gui_widget_contained((noegnud_gui_twidget *)window,event->motion.x,event->motion.y)) {

		if (event->type==SDL_MOUSEBUTTONDOWN&&((SDL_MouseButtonEvent *)event)->button==1&&noegnud_gui_overselforchildren((noegnud_gui_twidget *)window)) {
		    if (window->widget.type==NOEGNUD_GUI_TITLEBAR||(window->widget.parent==noegnud_guiwidget_desktop&&(noegnud_gui_twidget *)window!=noegnud_guiwidget_console)) {
			if (window->widget.type==NOEGNUD_GUI_TITLEBAR) {
			    tmp_widget=window->widget.parent;
			} else {
			    tmp_widget=(noegnud_gui_twidget *)window;
			}

			while (tmp_widget->nextsibling) {
			    if (tmp_widget->parent&&tmp_widget->parent->child==tmp_widget) {
				tmp_widget->parent->child=tmp_widget->nextsibling;
			    }
			    if (tmp_widget->prevsibling) {
				tmp_widget->prevsibling->nextsibling=tmp_widget->nextsibling;
				tmp_widget->nextsibling->prevsibling=tmp_widget->prevsibling;
			    } else {
				tmp_widget->nextsibling->prevsibling=NULL;
			    }
			    tmp_widget->prevsibling=tmp_widget->nextsibling;
			    tmp_widget->nextsibling=tmp_widget->prevsibling->nextsibling;
			    tmp_widget->prevsibling->nextsibling=tmp_widget;
			    if (tmp_widget->nextsibling) tmp_widget->nextsibling->prevsibling=tmp_widget;

			}
		    }
		    if (window->movable) {
			noegnud_gui_grabbed=(noegnud_gui_twidget *)window;
			noegnud_gui_grabbed_x=event->motion.x;
			noegnud_gui_grabbed_y=event->motion.y;
		    }
		}

		if (event->type==SDL_MOUSEBUTTONDOWN&&((SDL_MouseButtonEvent *)event)->button==4&&window->widget.vscroll) {
		    if (window->widget.offset_y>noegnud_options_input_mouse_gui_scrollspeed->value) {
			window->widget.offset_y-=noegnud_options_input_mouse_gui_scrollspeed->value;
		    } else {
			window->widget.offset_y=0;
		    }
		    if (window->widget.offset_y>window->widget.vscroll) window->widget.offset_y=window->widget.vscroll;
		};

		if (event->type==SDL_MOUSEBUTTONDOWN&&((SDL_MouseButtonEvent *)event)->button==5&&window->widget.vscroll) {
		    if (window->widget.offset_y<(window->widget.vscroll+noegnud_options_input_mouse_gui_scrollspeed->value)) {
			window->widget.offset_y+=noegnud_options_input_mouse_gui_scrollspeed->value;
		    } else {
			window->widget.offset_y=window->widget.vscroll;
		    }
		    if (window->widget.offset_y>window->widget.vscroll) window->widget.offset_y=window->widget.vscroll;
		};
	    }

    }
}

void noegnud_gui_kill_window(noegnud_gui_twindow *window) {
    if (window->localimage) noegnud_glfuncs_unloadimage(window->image);
    noegnud_gui_kill_widget((noegnud_gui_twidget *)window);
}

/*BUTTON*/

noegnud_gui_tbutton *noegnud_gui_create_button(
					       noegnud_gui_twidget * parent,
					       int x, int y,
					       int width, int height,
					       GLfloat r, GLfloat g, GLfloat b, GLfloat a,
					       GLfloat tr, GLfloat tg, GLfloat tb,
					       char *string,
					       int accelerator) {

    noegnud_gui_tbutton *button;
    noegnud_gui_ttext *text;

    //button=(noegnud_gui_tbutton *)noegnud_gui_create_window(parent,x,y,width,height,(sizeof(noegnud_gui_twindow)-sizeof(noegnud_gui_twidget))+datasize);

     button=(noegnud_gui_tbutton *)noegnud_gui_create_window(
                parent,
                x,y,
                width,height,
                r,g,b,a,
                sizeof(noegnud_gui_tbutton)-sizeof(noegnud_gui_twindow));
    text=noegnud_gui_create_text(
                 (noegnud_gui_twidget *)button,
				 (width-(strlen(string)*noegnud_gui_get_fontwidth()))/2,
				 (height-(noegnud_gui_get_fontheight()))/2,
				 noegnud_gui_font->width,
				 noegnud_gui_font->height,
				 tr,tg,tb,
				 string);

    button->window.widget.type=NOEGNUD_GUI_BUTTON;
    button->window.widget.draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_button;
    button->window.widget.event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_button;
    button->window.widget.theme=noegnud_gui_default_button_widgettheme;

    button->text=text;
    button->accelerator=accelerator;

    return button;
}

void noegnud_gui_draw_button(noegnud_gui_tbutton *button, int drawchildren) {
    noegnud_gui_twidget *parent;

    glDisable (GL_TEXTURE_2D);

    glLoadIdentity ();
    noegnud_gui_widget_cliptome((noegnud_gui_twidget *)button);


    if ((noegnud_gui_twidget *)button->window.widget.theme) {
	noegnud_widgettheme_draw((noegnud_gui_twidget *)button,button->window.widget.theme);
	if (drawchildren) noegnud_gui_draw_widget_relatives((noegnud_gui_twidget *)button);
	return;
    }

    parent=button->window.widget.parent;
    while(parent) {
	glTranslated (parent->x,-parent->y, 0);
	noegnud_gui_checkoffset_widget(parent);
        if ( button->window.widget.type!=NOEGNUD_GUI_VSCROLL) glTranslated (parent->offset_x,parent->offset_y, 0);
	parent=parent->parent;
    }
    glTranslated (button->window.widget.x,noegnud_options_screenheight->value-button->window.widget.y, 0);

    glBegin (GL_QUADS);

    if (button->window.widget.mouseover) {
	glColor4f(button->window.widget.r/2,button->window.widget.g/2,button->window.widget.b/2,button->window.widget.a);
    } else {
	glColor4f(button->window.widget.r,button->window.widget.g,button->window.widget.b,button->window.widget.a);
    }
    glVertex2i (0, 0);
    glVertex2i (0, -button->window.widget.height);
    glVertex2i (button->window.widget.width, -button->window.widget.height);
    glVertex2i (button->window.widget.width, 0);

    glEnd ();

    if (noegnud_options_gui_widgetborder->value) {
	glColor4f(1.0,1.0,1.0,1.0);
	glBegin (GL_LINES);
	glVertex2i (0, 0);
	glVertex2i (button->window.widget.width, 0);
	glVertex2i (button->window.widget.width, 0);
	glVertex2i (button->window.widget.width, -button->window.widget.height);
	glVertex2i (button->window.widget.width, -button->window.widget.height);
	glVertex2i (0, -button->window.widget.height);
	glVertex2i (0, -button->window.widget.height);
	glVertex2i (0, 0);
	glEnd ();

    }

    if (drawchildren) noegnud_gui_draw_widget_relatives((noegnud_gui_twidget *)button);

}

void noegnud_gui_event_button(noegnud_gui_tbutton *button, SDL_Event *event) {

    if ( button->window.widget.parent!=noegnud_gui_active &&
	 button->window.widget.parent->parent!=noegnud_gui_active) {
	noegnud_gui_event_null((noegnud_gui_twidget *)button,event);
	return;
    }

    noegnud_gui_event_widget_relatives((noegnud_gui_twidget *)button,event);

    if (button->window.widget.mouseover_check) {
	if (event->type==SDL_MOUSEMOTION) button->window.widget.mouseover=noegnud_gui_widget_contained((noegnud_gui_twidget *)button,event->motion.x,event->motion.y);
	if (button->window.widget.mouseover) {
	    button->window.widget.theme=noegnud_gui_default_buttonover_widgettheme;
	    if (!noegnud_gui_mouseoverwidget) noegnud_gui_mouseoverwidget=(int)button;
	} else {
	    button->window.widget.theme=noegnud_gui_default_button_widgettheme;
	}

    }


    if (event->type==SDL_MOUSEMOTION) button->window.widget.mouseover=noegnud_gui_widget_contained((noegnud_gui_twidget *)button,event->motion.x,event->motion.y);

    if (event->type==SDL_MOUSEBUTTONUP&&button->window.widget.mouseover&&((SDL_MouseButtonEvent *)event)->button==1) {
	noegnud_common_pushevent_keypress(button->accelerator);
    }
}

/*TEXTBUTTON*/
noegnud_gui_tbutton *noegnud_gui_create_textbutton(
						   noegnud_gui_twidget * parent,
						   int x, int y,
						   GLfloat r, GLfloat g, GLfloat b, GLfloat a,
						   GLfloat tr, GLfloat tg, GLfloat tb,
						   char *string,
						   char accelerator) {

    noegnud_gui_tbutton *button;

    button=noegnud_gui_create_button(
				     parent,
				     x,y,
				     NOEGNUD_GUI_BORDER_SIZE + noegnud_gui_font->width*(strlen(string)) + NOEGNUD_GUI_BORDER_SIZE,
				     NOEGNUD_GUI_BORDER_SIZE + noegnud_gui_font->height + NOEGNUD_GUI_BORDER_SIZE,
				     r,g,b,a,
				     tr,tg,tb,
				     string,
				     accelerator);

    return button;
}

/*PROGRESSBAR*/

noegnud_gui_tprogressbar *noegnud_gui_create_progressbar(
							 noegnud_gui_twidget * parent,
							 int x, int y,
							 int width, int height,
							 GLfloat r, GLfloat g, GLfloat b, GLfloat a,
							 int minimum, int maximum,
							 GLfloat br, GLfloat bg, GLfloat bb, GLfloat ba) {

    noegnud_gui_tprogressbar *progressbar;

    progressbar=(noegnud_gui_tprogressbar *)noegnud_gui_create_window(parent,x,y,width,height,r,g,b,a,sizeof(noegnud_gui_tprogressbar)-sizeof(noegnud_gui_twindow));

    progressbar->window.widget.type=NOEGNUD_GUI_PROGRESSBAR;
    progressbar->window.widget.draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_progressbar;

    progressbar->window.movable=0;

    progressbar->minimum=minimum;
    progressbar->maximum=maximum;
    progressbar->value=minimum;

    progressbar->br=br;
    progressbar->bg=bg;
    progressbar->bb=bb;
    progressbar->ba=ba;

    return progressbar;
}

void noegnud_gui_draw_progressbar(noegnud_gui_tprogressbar *progressbar, int drawchildren) {
    int w,v,t;

    noegnud_gui_draw_window((noegnud_gui_twindow *)progressbar,0);

    if (progressbar->value<progressbar->minimum) progressbar->value=progressbar->minimum;
    if (progressbar->value>progressbar->maximum) progressbar->value=progressbar->maximum;

    v=progressbar->value-progressbar->minimum;
    t=progressbar->maximum-progressbar->minimum;
    w=(progressbar->value*(progressbar->window.widget.width-1))/(t);

    glBegin (GL_QUADS);

    glColor4f(progressbar->br,progressbar->bg,progressbar->bb,progressbar->ba);
    glVertex2i (0, 0);
    glVertex2i (0, -progressbar->window.widget.height);
    glVertex2i (w+1, -progressbar->window.widget.height);
    glVertex2i (w+1, 0);

    glColor4f(1.0,1.0,1.0,0.5);
    glVertex2i (0, 0);
    glColor4f(1.0,1.0,1.0,0.0);
    glVertex2i (0, -progressbar->window.widget.height/2);
    glColor4f(1.0,1.0,1.0,0.0);
    glVertex2i (progressbar->window.widget.width, -progressbar->window.widget.height/2);
    glColor4f(1.0,1.0,1.0,0.5);
    glVertex2i (progressbar->window.widget.width, 0);


    glColor4f(0.0,0.0,0.0,0.0);
    glVertex2i (0, -progressbar->window.widget.height/2);
    glColor4f(0.0,0.0,0.0,0.5);
    glVertex2i (0, -progressbar->window.widget.height);
    glColor4f(0.0,0.0,0.0,0.5);
    glVertex2i (progressbar->window.widget.width, -progressbar->window.widget.height);
    glColor4f(0.0,0.0,0.0,0.0);
    glVertex2i (progressbar->window.widget.width, -progressbar->window.widget.height/2);

    glEnd ();

    if (noegnud_options_gui_widgetborder->value) {


	glColor4f(1.0,1.0,1.0,1.0);
	glBegin (GL_LINES);
	glVertex2i (0, 0);
	glVertex2i (progressbar->window.widget.width, 0);
	glVertex2i (progressbar->window.widget.width, 0);
	glVertex2i (progressbar->window.widget.width, -progressbar->window.widget.height);
	glVertex2i (progressbar->window.widget.width, -progressbar->window.widget.height);
	glVertex2i (0, -progressbar->window.widget.height);
	glVertex2i (0, -progressbar->window.widget.height);
	glVertex2i (0, 0);
	glEnd ();

    }

    if (drawchildren) noegnud_gui_draw_widget_relatives((noegnud_gui_twidget *)progressbar);

}

/*GLYPH*/

noegnud_gui_tglyph *noegnud_gui_create_glyph(
					     noegnud_gui_twidget * parent,
					     int x, int y,
					     int width, int height,
					     int glyph) {

    noegnud_gui_tglyph *wglyph;

    wglyph = (noegnud_gui_tglyph *)noegnud_gui_create_widget(parent,x,y,width,height,sizeof(noegnud_gui_twindow)-sizeof(noegnud_gui_twidget));

    wglyph->widget.type=NOEGNUD_GUI_GLYPH;
    wglyph->widget.draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_glyph;
    //	wglyph->widget.event=noegnud_gui_event_glyph;
    wglyph->widget.event=noegnud_gui_event_widget;

    wglyph->glyph = glyph;
    wglyph->activepopup=NULL;

    return wglyph;
}

void noegnud_gui_draw_glyph(noegnud_gui_tglyph *glyph, int drawchildren) {
    noegnud_gui_twidget *parent;

    glEnable (GL_TEXTURE_2D);

    glLoadIdentity ();

    noegnud_gui_widget_cliptome((noegnud_gui_twidget *)glyph);

    parent=glyph->widget.parent;
    while(parent) {
	glTranslated (parent->x,-parent->y, 0);
	noegnud_gui_checkoffset_widget(parent);
        glTranslated (parent->offset_x,parent->offset_y, 0);
	parent=parent->parent;
    }
    glTranslated (glyph->widget.x,noegnud_options_screenheight->value-glyph->widget.y, 0);

    glBindTexture (GL_TEXTURE_2D, noegnud_activetileset->tiles[glyph2tile[glyph->glyph]]);
    glBegin (GL_QUADS);

    glColor4f(1.0,1.0,1.0,1.0);
    glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy1);
    glVertex2i (0, 0);
    glTexCoord2f (noegnud_activetileset->fx1, noegnud_activetileset->fy2);
    glVertex2i (0, -glyph->widget.height);
    glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy2);
    glVertex2i (glyph->widget.width, -glyph->widget.height);
    glTexCoord2f (noegnud_activetileset->fx2, noegnud_activetileset->fy1);
    glVertex2i (glyph->widget.width, 0);

    glEnd ();

    if (drawchildren) noegnud_gui_draw_widget_relatives((noegnud_gui_twidget *)glyph);

}

void noegnud_gui_event_glyph(noegnud_gui_tglyph *glyph, SDL_Event *event) {

    noegnud_gui_event_widget_relatives((noegnud_gui_twidget *)glyph,event);

    if (glyph->widget.mouseover_check) {
	if (event->type==SDL_MOUSEMOTION) glyph->widget.mouseover=noegnud_gui_widget_contained((noegnud_gui_twidget *)glyph,event->motion.x,event->motion.y);
	if (glyph->widget.mouseover) {
	    if (!noegnud_gui_mouseoverwidget) noegnud_gui_mouseoverwidget=(int)glyph;
	}
    }

    if (glyph->activepopup) {
	if (event->type==SDL_MOUSEMOTION) {
	    if ( ( glyph->widget.mouseover ) &&
		 ( glyph->widget.parent->parent->mouseover ) &&
		 ( ( (noegnud_gui_mouseoverwidget==(int)glyph->activepopup) ||
		     (glyph->activepopup->child&&noegnud_gui_mouseoverwidget==(int)glyph->activepopup->child) ||
		     (glyph->activepopup->child&&glyph->activepopup->child->child&&noegnud_gui_mouseoverwidget==(int)glyph->activepopup->child->child)))) {
		glyph->activepopup->x=(event->motion.x-(glyph->widget.parent->parent->x-glyph->widget.parent->parent->offset_x))-(128);
		glyph->activepopup->y=(event->motion.y-(glyph->widget.parent->parent->y-glyph->widget.parent->parent->offset_y))-(128);
	    } else {
		glyph->activepopup->kill(glyph->activepopup);
		glyph->activepopup=NULL;
	    }
	}

    } else {
	if (noegnud_gui_mouseoverwidget==(int)glyph&&glyph->widget.parent->parent->mouseover) {

	    noegnud_gui_create_glyph(
	        (noegnud_gui_twidget *)noegnud_gui_create_window(
		    glyph->activepopup=(noegnud_gui_twidget *)noegnud_gui_create_widget(
		        (noegnud_gui_twidget *)glyph->widget.parent->parent,
			(event->motion.x-(glyph->widget.parent->parent->x-glyph->widget.parent->parent->offset_x))-(128),
			(event->motion.y-(glyph->widget.parent->parent->y-glyph->widget.parent->parent->offset_y))-(128),
			256,256,
			0),
		    64,64,
		    128,128,
		    0.0,0.0,0.0,0.4,
		    0),
		0,
		0,
		128,128,
		glyph->glyph);

	}
    }
}


/*TEXT*/

noegnud_gui_ttext *noegnud_gui_create_text(noegnud_gui_twidget * parent, int x, int y, int width, int height, GLfloat r, GLfloat g, GLfloat b, const char *text) {
    noegnud_gui_ttext *guitext;

    guitext=(noegnud_gui_ttext *)noegnud_gui_create_widget(parent,x,y,width,height,sizeof(noegnud_gui_ttext)-sizeof(noegnud_gui_twidget));

    guitext->widget.type=NOEGNUD_GUI_TEXT;
    guitext->widget.draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_text;
    guitext->widget.kill=(noegnud_gui_kill_widget_proc *)noegnud_gui_kill_text;

    guitext->r=r;
    guitext->g=g;
    guitext->b=b;

    guitext->font=noegnud_gui_font;

    guitext->text=noegnud_mem_malloc(strlen(text)+1);
    strcpy(guitext->text,text);

    guitext->dynamiccharwidth=FALSE;

    return guitext;

}
void noegnud_gui_kill_text(noegnud_gui_ttext *text) {
    char tmpstring[1024];
    char tmpvalue[1024];
    noegnud_gui_twindow *window;

    if (text->widget.parent&&text->widget.parent->type==NOEGNUD_GUI_TITLEBAR) {
	sprintf(tmpstring,"gui.window.implicit.%s.position.%d.%d.x",text->text,noegnud_options_screenwidth->value,noegnud_options_screenheight->value);
	sprintf(tmpvalue,"%d",text->widget.parent->parent->x);
	noegnud_collection_add(noegnud_options,tmpstring,noegnud_options_create_string(NOEGNUD_OPTIONPERM_CONFIGCHANGE,tmpvalue));
	sprintf(tmpstring,"gui.window.implicit.%s.position.%d.%d.y",text->text,noegnud_options_screenwidth->value,noegnud_options_screenheight->value);
	sprintf(tmpvalue,"%d",text->widget.parent->parent->y);
	noegnud_collection_add(noegnud_options,tmpstring,noegnud_options_create_string(NOEGNUD_OPTIONPERM_CONFIGCHANGE,tmpvalue));
	sprintf(tmpstring,"gui.window.implicit.%s.colour",text->text);
	window=(noegnud_gui_twindow *)text->widget.parent->parent;
	sprintf(tmpvalue,"%2.3f,%2.3f,%2.3f,%2.3f",window->widget.r,window->widget.g,window->widget.b,window->widget.a);
	noegnud_collection_add(noegnud_options,tmpstring,noegnud_options_create_string(NOEGNUD_OPTIONPERM_CONFIGCHANGE,tmpvalue));

    }

    noegnud_mem_free(text->text);
    noegnud_gui_kill_widget((noegnud_gui_twidget *)text);
}
void noegnud_gui_draw_text(noegnud_gui_ttext *text, int drawchildren) {
    noegnud_gui_twidget *parent;

    glLoadIdentity ();
    noegnud_gui_widget_cliptome((noegnud_gui_twidget *)text);

    parent=text->widget.parent;

    while(parent) {
	glTranslated (parent->x,-parent->y, 0);
	noegnud_gui_checkoffset_widget(parent);
	if ( text->widget.type!=NOEGNUD_GUI_VSCROLL && text->widget.parent->type!=NOEGNUD_GUI_TITLEBAR) glTranslated (parent->offset_x,parent->offset_y, 0);
	parent=parent->parent;
    }

    glTranslated (text->widget.x,noegnud_options_screenheight->value-(text->widget.y + noegnud_gui_font->height), 0);
    glColor3f(text->r,text->g,text->b);
    if (text->dynamiccharwidth) {
	noegnud_fonts_print_dynamic(text->font,0,0,text->text);
    } else {
	noegnud_fonts_print(text->font,0,0,text->text);
    }

    if (drawchildren) noegnud_gui_draw_widget_relatives((noegnud_gui_twidget *)text);
}

/*VSCROLL*/

noegnud_gui_tvscroll *noegnud_gui_create_vscroll(noegnud_gui_twidget * parent) {
    noegnud_gui_twindow *bar;
    noegnud_gui_twindow *container;


    container= noegnud_gui_create_window(
					 parent,
					 parent->width-NOEGNUD_GUI_SCROLLER_WIDTH,
					 0,
					 NOEGNUD_GUI_SCROLLER_WIDTH,
					 parent->height,
					 0.0,0.0,0.0,0.5,
					 0);

    container->movable=FALSE;

    container->widget.type=NOEGNUD_GUI_VSCROLL;
    container->widget.autosize=(noegnud_gui_autosize_widget_proc *)noegnud_gui_autosize_vscroll;
    container->widget.event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_vscroll;
    container->widget.theme=noegnud_gui_default_scrollbararea_widgettheme;

    bar=noegnud_gui_create_window(
				  (noegnud_gui_twidget *)container,
				  2,
				  2,
				  container->widget.width-4,
				  NOEGNUD_GUI_SCROLLER_WIDTH,
				  1.0,1.0,1.0,1.0,
				  0);

    bar->movable=FALSE;

    bar->widget.type=NOEGNUD_GUI_VSCROLL;
    bar->widget.theme=noegnud_gui_default_scrollbar_widgettheme;
    bar->widget.draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_vslider;

    noegnud_gui_autosize_vscroll((noegnud_gui_tvscroll *)container);

    return (noegnud_gui_tvscroll *)container;
}


int noegnud_gui_widget_contained(noegnud_gui_twidget* widget, int x, int y) {
    int px, py;
    px=noegnud_gui_widget_getabsolute_x(widget);
    py=noegnud_gui_widget_getabsolute_y(widget);
    return 	((x>=px)&&(y>=py)&&(x<px+widget->width)&&(y<py+widget->height));
}

void noegnud_gui_event_vscroll(noegnud_gui_tvscroll *vscroll, SDL_Event *event) {

    int px, py;

    noegnud_gui_event_widget_relatives((noegnud_gui_twidget *)vscroll,event);

    vscroll->widget.x=vscroll->widget.parent->width-NOEGNUD_GUI_SCROLLER_WIDTH;
    vscroll->widget.height=vscroll->widget.parent->height;

    if (vscroll->widget.mouseover_check) {
	if (event->type==SDL_MOUSEMOTION) vscroll->widget.mouseover=noegnud_gui_widget_contained((noegnud_gui_twidget *)vscroll,event->motion.x,event->motion.y);
	if (vscroll->widget.mouseover) {
	    if (!noegnud_gui_mouseoverwidget) noegnud_gui_mouseoverwidget=(int)vscroll;
	}
    }

    if ((!noegnud_gui_grabbed)||(noegnud_gui_grabbed==(noegnud_gui_twidget *)vscroll)) {

	px=noegnud_gui_widget_getabsolute_x((noegnud_gui_twidget *)vscroll);
	py=noegnud_gui_widget_getabsolute_y((noegnud_gui_twidget *)vscroll);

	if (noegnud_gui_grabbed==(noegnud_gui_twidget *)vscroll) {

	    if (event->type==SDL_MOUSEBUTTONUP) noegnud_gui_grabbed=NULL;

	    if (event->type==SDL_MOUSEMOTION) {
		//pos=(event->motion.y-((vscroll->widget.child->height/2)+2))-vscroll->widget.parent->y;


		vscroll->widget.vscroll_pos+=(event->motion.y-noegnud_gui_grabbed_y);
		noegnud_gui_grabbed_y=event->motion.y;
		if (vscroll->widget.vscroll_pos<0) vscroll->widget.vscroll_pos=0;
		if (vscroll->widget.vscroll_pos>vscroll->widget.height-(vscroll->widget.child->height+4)) vscroll->widget.vscroll_pos=vscroll->widget.height-(vscroll->widget.child->height+4);
		vscroll->widget.child->y=vscroll->widget.vscroll_pos+2;

		vscroll->widget.parent->offset_y=(vscroll->widget.vscroll_pos*vscroll->widget.parent->vscroll)/(vscroll->widget.height-(vscroll->widget.child->height+4));

	    }

	} else
	    if (noegnud_gui_widget_contained((noegnud_gui_twidget *)vscroll,event->motion.x,event->motion.y)) {
		if (event->type==SDL_MOUSEBUTTONDOWN) {
		    noegnud_gui_grabbed=(noegnud_gui_twidget *)vscroll;
		    noegnud_gui_grabbed_y=event->motion.y;
		}
	    }

    }
}

void noegnud_gui_autosize_vscroll(noegnud_gui_tvscroll *vscroll) {
    int height;

    if (vscroll->widget.parent->type!=NOEGNUD_GUI_VSCROLL) {
	vscroll->widget.x=vscroll->widget.parent->width-NOEGNUD_GUI_SCROLLER_WIDTH;
	noegnud_gui_resize_widget((noegnud_gui_twidget *)vscroll,NOEGNUD_GUI_SCROLLER_WIDTH,vscroll->widget.parent->height);
	noegnud_gui_autosize_vscroll((noegnud_gui_tvscroll *)vscroll->widget.child);
    } else {
	height=(vscroll->widget.parent->height-4)-vscroll->widget.parent->parent->vscroll;
	if (height < NOEGNUD_GUI_SCROLLER_WIDTH) height=NOEGNUD_GUI_SCROLLER_WIDTH;
	noegnud_gui_resize_widget((noegnud_gui_twidget *)vscroll,NOEGNUD_GUI_SCROLLER_WIDTH-4,height);
    }

}

void noegnud_gui_draw_vslider(noegnud_gui_tvslider *vslider, int drawchildren) {

    vslider->widget.y=((vslider->widget.parent->parent->offset_y*(vslider->widget.parent->height-(vslider->widget.height+4)))/vslider->widget.parent->parent->vscroll)+2;
    vslider->widget.parent->vscroll_pos=vslider->widget.y-1;

    noegnud_gui_draw_window((noegnud_gui_twindow *)vslider,0);

    glBegin (GL_QUADS);

    glColor4f(1.0,1.0,1.0,0.7);
    glVertex2i (0, 0);
    glVertex2i (0, -vslider->widget.height);
    glColor4f(1.0,1.0,1.0,0.0);
    glVertex2i (vslider->widget.width*2/3, -vslider->widget.height);
    glVertex2i (vslider->widget.width*2/3, 0);

    glColor4f(0.0,0.0,0.0,0.0);
    glVertex2i (vslider->widget.width*1/3, 0);
    glVertex2i (vslider->widget.width*1/3, -vslider->widget.height);
    glColor4f(0.0,0.0,0.0,0.7);
    glVertex2i (vslider->widget.width, -vslider->widget.height);
    glVertex2i (vslider->widget.width, 0);

    glEnd ();

    if (noegnud_options_gui_widgetborder->value) {


	glColor4f(1.0,1.0,1.0,1.0);
	glBegin (GL_LINES);
	glVertex2i (0, 0);
	glVertex2i (vslider->widget.width, 0);
	glVertex2i (vslider->widget.width, 0);
	glVertex2i (vslider->widget.width, -vslider->widget.height);
	glVertex2i (vslider->widget.width, -vslider->widget.height);
	glVertex2i (0, -vslider->widget.height);
	glVertex2i (0, -vslider->widget.height);
	glVertex2i (0, 0);
	glEnd ();

    }

    if (drawchildren) noegnud_gui_draw_widget_relatives((noegnud_gui_twidget *)vslider);

}


/*MENUITEM*/

noegnud_gui_tmenuitem *noegnud_gui_create_menuitem(
						   noegnud_gui_twidget * parent,
						   int x, int y,
						   int width, int height,
						   GLfloat r, GLfloat g, GLfloat b, GLfloat a,
						   anything identifier,
						   char ch, char gch,
						   int selected) {

    noegnud_gui_tmenuitem *menuitem;

    menuitem=(noegnud_gui_tmenuitem *)noegnud_gui_create_window(parent,x,y,width,height,1.0,1.0,1.0,1.0,sizeof(noegnud_gui_tmenuitem)-sizeof(noegnud_gui_twindow));
    //    menuitem=(noegnud_gui_tmenuitem *)noegnud_gui_create_widget(parent,x,y,width,height,sizeof(noegnud_gui_tmenuitem)-sizeof(noegnud_gui_twidget));
    menuitem->window.widget.type=NOEGNUD_GUI_MENUITEM;

    //menuitem->window.widget.draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_menuitem;
    menuitem->window.widget.draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_window;
    menuitem->window.widget.event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_menuitem;

    if (selected) {
	menuitem->window.widget.theme=noegnud_gui_default_item_selected_widgettheme;
    } else {
	menuitem->window.widget.theme=noegnud_gui_default_item_widgettheme;
    }
    // menuitem->window.widget.theme=noegnud_gui_default_widgettheme;

    menuitem->r=r;
    menuitem->g=g;
    menuitem->b=b;
    menuitem->a=a;

    menuitem->ch=ch;
    menuitem->gch=gch;
    menuitem->identifier=identifier;
    menuitem->selected=selected;
    menuitem->highlighted=FALSE;
    menuitem->text=NULL;

    return menuitem;
}

void noegnud_gui_draw_menuitem(noegnud_gui_tmenuitem *menuitem, int drawchildren) {
    noegnud_gui_twidget *parent;

    glDisable (GL_TEXTURE_2D);

    glLoadIdentity ();

    noegnud_gui_widget_cliptome((noegnud_gui_twidget *)menuitem);

    parent=menuitem->window.widget.parent;
    while(parent) {
	glTranslated (parent->x,-parent->y, 0);
	noegnud_gui_checkoffset_widget(parent);
        if (menuitem->window.widget.type!=NOEGNUD_GUI_VSCROLL) glTranslated (parent->offset_x,parent->offset_y, 0);
	parent=parent->parent;
    }
    glTranslated (menuitem->window.widget.x,noegnud_options_screenheight->value-menuitem->window.widget.y, 0);

    glBegin (GL_QUADS);


    if (menuitem->selected) {
	glColor4f(menuitem->r,menuitem->g,menuitem->b,menuitem->a);
    } else if (menuitem->highlighted) {
	glColor4f(menuitem->r/2,menuitem->g/2,menuitem->b/2,menuitem->a);
    } else {
	glColor4f(menuitem->r,menuitem->g,menuitem->b,0.0);
    }
    glVertex2i (0, 0);
    glVertex2i (0, -menuitem->window.widget.height);
    glVertex2i (menuitem->window.widget.width, -menuitem->window.widget.height);
    glVertex2i (menuitem->window.widget.width, 0);

    glEnd ();


    if (drawchildren) noegnud_gui_draw_widget_relatives((noegnud_gui_twidget *)menuitem);

}

void noegnud_gui_event_menuitem(noegnud_gui_tmenuitem *menuitem, SDL_Event *event) {

    if (menuitem->window.widget.parent!=noegnud_gui_active) {
	noegnud_gui_event_null((noegnud_gui_twidget *)menuitem,event);
	return;
    }

    noegnud_gui_event_widget_relatives((noegnud_gui_twidget *)menuitem,event);

    if (menuitem->window.widget.mouseover_check) {
	if (event->type==SDL_MOUSEMOTION) menuitem->window.widget.mouseover=noegnud_gui_widget_contained((noegnud_gui_twidget *)menuitem,event->motion.x,event->motion.y);
	if (menuitem->window.widget.mouseover) {
	    if (!noegnud_gui_mouseoverwidget) noegnud_gui_mouseoverwidget=(int)menuitem;
	}
    }

    //menuitem->highlighted=(menuitem->window.widget.mouseover);
    menuitem->highlighted=noegnud_gui_overselforchildren((noegnud_gui_twidget *)menuitem);

    if (menuitem->selected) {
	menuitem->window.widget.theme=noegnud_gui_default_item_selected_widgettheme;
    } else if (menuitem->highlighted) {
	menuitem->window.widget.theme=noegnud_gui_default_item_highlighted_widgettheme;
    } else {
	menuitem->window.widget.theme=noegnud_gui_default_item_widgettheme;
    }


    if (event->type==SDL_MOUSEBUTTONUP&&menuitem->highlighted&&menuitem->window.widget.mouseover&&((SDL_MouseButtonEvent *)event)->button==1) {
	if (menuitem->selected) {
	    menuitem->window.widget.theme=noegnud_gui_default_item_highlighted_widgettheme;
	    if (!noegnud_gui_menuitem_throwreturn) menuitem->selected=FALSE;
	} else {
	    menuitem->window.widget.theme=noegnud_gui_default_item_selected_widgettheme;
	    if (!noegnud_gui_menuitem_throwreturn) menuitem->selected=TRUE;
	}
	if (noegnud_gui_menuitem_throwreturn) noegnud_common_pushevent_keypress((SDLKey)menuitem->ch);
	noegnud_gui_menuselect_identifier=menuitem->identifier;
    }

}

/* HIGH LEVEL functions */

void noegnud_gui_nh_putstr(noegnud_gui_twindow *window, const char *text, GLfloat r, GLfloat g, GLfloat b) {
    noegnud_gui_twidget *child;
    int y;
    int w,h;

    if (window->widget.height<NOEGNUD_GUI_BORDER_SIZE*2) window->widget.height=NOEGNUD_GUI_BORDER_SIZE*2;

    y=NOEGNUD_GUI_BORDER_SIZE;
    child=window->widget.child;
    while ((child)&&(child->prevsibling)) child=child->prevsibling;
    while (child) {
	if (child->type!=NOEGNUD_GUI_VSCROLL) y+=child->height;
	child=child->nextsibling;
    }

    w=NOEGNUD_GUI_BORDER_SIZE + strlen(text)*noegnud_gui_font->width + NOEGNUD_GUI_BORDER_SIZE;
    h=noegnud_gui_font->height;

    noegnud_gui_create_text(
			    (noegnud_gui_twidget *)window,
			    NOEGNUD_GUI_BORDER_SIZE,
			    y,
			    w,
			    h,
			    r,g,b,
			    text );

    w=NOEGNUD_GUI_BORDER_SIZE + w + NOEGNUD_GUI_BORDER_SIZE;

    h+=y+NOEGNUD_GUI_BORDER_SIZE;
    if (window->widget.width>=w) w=window->widget.width;

    noegnud_gui_resize_widget((noegnud_gui_twidget *)window,w,h);


    if (window->widget.autocentre) {
	window->widget.x=(noegnud_options_screenwidth->value-window->widget.width)/2;
	window->widget.y=(noegnud_options_screenheight->value-window->widget.height)/2;
    }

}

void noegnud_gui_add_actionbutton(noegnud_gui_twindow *window, const char *text, char accelerator) {

    noegnud_gui_twidget *child;
    int y;
    int w,h;
    noegnud_gui_tbutton *button;

    //    if (window->widget.height<NOEGNUD_GUI_BORDER_SIZE*2) window->widget.height=NOEGNUD_GUI_BORDER_SIZE*2;

    y=NOEGNUD_GUI_BORDER_SIZE;
    child=window->widget.child;
    while ((child)&&(child->prevsibling)) child=child->prevsibling;

    w=0;
    while (child) {
	if (child->width>w) w=child->width;
	if (child->type!=NOEGNUD_GUI_VSCROLL)
	    if (child->y+child->height>y) y=child->y+child->height;
	child=child->nextsibling;
    }

    //y+=NOEGNUD_GUI_BORDER_SIZE;
    //w=window->widget.width-2*(NOEGNUD_GUI_BORDER_SIZE);
    h=noegnud_gui_font->height+NOEGNUD_GUI_BORDER_SIZE+2;

    button=noegnud_gui_create_button(
				     noegnud_gui_create_widget(
							       (noegnud_gui_twidget *)window,
							       NOEGNUD_GUI_BORDER_SIZE,
							       y,
							       w,
							       h,
							       0),
				     1,NOEGNUD_GUI_BORDER_SIZE+1,
				     w-2,h-(NOEGNUD_GUI_BORDER_SIZE+2),
				     1.0,1.0,1.0,1.0,
				     0.0,0.0,0.0,
				     (char *)text, accelerator);

    button->window.widget.clipto=(noegnud_gui_twidget *)window;
    button->window.widget.child->clipto=(noegnud_gui_twidget *)window;

    w=NOEGNUD_GUI_BORDER_SIZE + w + NOEGNUD_GUI_BORDER_SIZE;

    h+=y+NOEGNUD_GUI_BORDER_SIZE;
    if (window->widget.width>=w) w=window->widget.width;

    noegnud_gui_resize_widget((noegnud_gui_twidget *)window,w,h);

    if (window->widget.vscroll) button->window.widget.width-=NOEGNUD_GUI_SCROLLER_WIDTH;

    if (window->widget.autocentre) {
	window->widget.x=(noegnud_options_screenwidth->value-window->widget.width)/2;
	window->widget.y=(noegnud_options_screenheight->value-window->widget.height)/2;
    }

}

void noegnud_gui_nh_addmenu_menuitem_addaccelerator(noegnud_gui_twidget *menuitem, char ch) {
    char smallstring[2];

    int y2;

    noegnud_gui_twidget *item;

    smallstring[1]=0;

    y2=((menuitem->height)-noegnud_gui_font->height)/2;

    smallstring[0]=ch;
    item=(noegnud_gui_twidget *)noegnud_gui_create_text(
							menuitem,
							NOEGNUD_GUI_BORDER_SIZE + 0*noegnud_gui_font->width,
							y2,
							noegnud_gui_font->width,
							noegnud_gui_font->height,
							0.5,0.25,0.0,
							(char *)&smallstring );
    item->clipto=menuitem->parent;

    smallstring[0]='-';
    item=(noegnud_gui_twidget *)noegnud_gui_create_text(
							menuitem,
							NOEGNUD_GUI_BORDER_SIZE + 2*noegnud_gui_font->width,
							y2,
							noegnud_gui_font->width,
							noegnud_gui_font->height,
							7.0,7.0,7.0,
							(char *)&smallstring );
    item->clipto=menuitem->parent;
}

void noegnud_gui_nh_addmenu(noegnud_gui_twindow *window, anything identifier, int glyph, char ch, char gch, const char *text, int selected) {
    noegnud_gui_twidget *child;
    noegnud_gui_ttext   *wtext;
    noegnud_gui_tglyph   *wglyph;
    int y;
    int w,h;

    int y2;
    int x;

    char smallstring[2];

    smallstring[1]=0;

    if (window->widget.height<NOEGNUD_GUI_BORDER_SIZE*2) window->widget.height=NOEGNUD_GUI_BORDER_SIZE*2;

    y=NOEGNUD_GUI_BORDER_SIZE;
    child=window->widget.child;
    while ((child)&&(child->prevsibling)) child=child->prevsibling;
    while (child) {
	if (child->type!=NOEGNUD_GUI_VSCROLL) y+=child->height;
	child=child->nextsibling;
    }

    if (glyph!=NO_GLYPH) {
	h=32+2;
	y2=((32+2)-noegnud_gui_font->height)/2;
	w=NOEGNUD_GUI_BORDER_SIZE + (4+noegnud_textcolour_strlen(text))*noegnud_gui_font->width + NOEGNUD_GUI_BORDER_SIZE + 32+2 + NOEGNUD_GUI_BORDER_SIZE;
	x=NOEGNUD_GUI_BORDER_SIZE + 32+2 + NOEGNUD_GUI_BORDER_SIZE;
    } else {
	h=noegnud_gui_font->height;
	y2=0;
	w=NOEGNUD_GUI_BORDER_SIZE + (4+noegnud_textcolour_strlen(text))*noegnud_gui_font->width + NOEGNUD_GUI_BORDER_SIZE;
	x=NOEGNUD_GUI_BORDER_SIZE;
    }

    child=(noegnud_gui_twidget *)noegnud_gui_create_menuitem(
							     (noegnud_gui_twidget *)window,
							     NOEGNUD_GUI_BORDER_SIZE, y,
							     w, h,
							     0.5,0.0,0.0,0.5,
							     identifier,
							     ch,gch,
							     selected);

    if (ch) noegnud_gui_nh_addmenu_menuitem_addaccelerator(child,ch);

    if (glyph!=NO_GLYPH) {
	wglyph=noegnud_gui_create_glyph(
					child,
					NOEGNUD_GUI_BORDER_SIZE + (4)*noegnud_gui_font->width + 1,1,
					32,32,glyph);
	wglyph->widget.event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_glyph;
	wglyph->widget.clipto=(noegnud_gui_twidget *)window;

    }

    if (identifier.a_int) {
	wtext=noegnud_gui_create_text(
				      child,
				      x + 4*noegnud_gui_font->width,
				      y2,
				      w,
				      noegnud_gui_font->height,
				      noegnud_options_gui_window_console_text_colour_default->r,
				      noegnud_options_gui_window_console_text_colour_default->g,
				      noegnud_options_gui_window_console_text_colour_default->b,
				      text );
    } else {
	wtext=noegnud_gui_create_text(
				      child,
				      x + 4*noegnud_gui_font->width,
				      y2,
				      w,
				      noegnud_gui_font->height,
				      noegnud_options_gui_window_console_text_colour_default->r,
				      noegnud_options_gui_window_console_text_colour_default->g,
				      noegnud_options_gui_window_console_text_colour_default->b,
				      text );
    }
    wtext->widget.clipto=(noegnud_gui_twidget *)window;

    ((noegnud_gui_tmenuitem *)child)->text=wtext->text;




    w=NOEGNUD_GUI_BORDER_SIZE + w + NOEGNUD_GUI_BORDER_SIZE;

    h+=y+NOEGNUD_GUI_BORDER_SIZE;
    if (window->widget.width>=w) w=window->widget.width;

    noegnud_gui_resize_widget((noegnud_gui_twidget *)window,w,h);

    if (window->widget.autocentre) {
	window->widget.x=(noegnud_options_screenwidth->value-window->widget.width)/2;
	window->widget.y=(noegnud_options_screenheight->value-window->widget.height)/2;
    }

}

noegnud_gui_twidget *noegnud_gui_nh_yn_function(const char *question, const char *choices, char def) {
    noegnud_gui_twidget *widget;
    noegnud_gui_twidget *button;

    int width,height;
    char *ch;
    int c;
    char shortstring[2];

    GLfloat r,g,b;

    shortstring[1]=0;

    width=NOEGNUD_GUI_BORDER_SIZE;
    //width= NOEGNUD_GUI_BORDER_SIZE + strlen(question)*noegnud_gui_font->width + NOEGNUD_GUI_BORDER_SIZE;
    height=NOEGNUD_GUI_BORDER_SIZE;
    //    height= NOEGNUD_GUI_BORDER_SIZE + noegnud_gui_font->height + NOEGNUD_GUI_BORDER_SIZE;
    if (choices) height+=NOEGNUD_GUI_BORDER_SIZE + noegnud_gui_font->height + NOEGNUD_GUI_BORDER_SIZE + NOEGNUD_GUI_BORDER_SIZE;
    widget=(noegnud_gui_twidget *)noegnud_gui_create_window(
							    noegnud_guiwidget_desktop,
							    (noegnud_options_screenwidth->value-width)/2,
							    (noegnud_options_screenheight->value/2-height)/2,
							    width,height,
							    1.0,1.0,1.0,1.0,
							    0);
    widget->theme=noegnud_gui_default_widgettheme;
    noegnud_end_menu((winid)widget,question);
    width=widget->width;

    /*
    noegnud_gui_create_text( widget,
			     NOEGNUD_GUI_BORDER_SIZE,
			     NOEGNUD_GUI_BORDER_SIZE,
			     width-(2*NOEGNUD_GUI_BORDER_SIZE),
			     noegnud_gui_font->height,
			     noegnud_options_gui_window_console_text_colour_default->r,
			     noegnud_options_gui_window_console_text_colour_default->g,
			     noegnud_options_gui_window_console_text_colour_default->b,
			     question);
    */
    if (choices) {

	c=0;
	ch=(char *)choices;
	while (*ch) {
	    shortstring[0]=*ch;
	    r=g=b=0.0;
	    if (*ch==def) {
		r=0.75;
		g=0.5;
		b=0.0;
	    }
	    button=(noegnud_gui_twidget *)noegnud_gui_create_button(
								    widget,
								    NOEGNUD_GUI_BORDER_SIZE+c*(NOEGNUD_GUI_BORDER_SIZE + noegnud_gui_font->width + NOEGNUD_GUI_BORDER_SIZE + NOEGNUD_GUI_BORDER_SIZE),
								    NOEGNUD_GUI_BORDER_SIZE,
								    NOEGNUD_GUI_BORDER_SIZE + noegnud_gui_font->width + NOEGNUD_GUI_BORDER_SIZE,
								    NOEGNUD_GUI_BORDER_SIZE + noegnud_gui_font->height + NOEGNUD_GUI_BORDER_SIZE,
								    1.0,1.0,1.0,1.0,
								    r,g,b,
								    (char *)&shortstring,
								    (int)(*ch));

	    if (button->x+button->width+NOEGNUD_GUI_BORDER_SIZE>widget->width) widget->width=button->x+button->width+NOEGNUD_GUI_BORDER_SIZE;

	    ch++; c++;
	}
    } else {
	if (question[strlen(question)-1]==']') {
	    noegnud_gui_add_actionbutton((noegnud_gui_twindow *)widget,"[?] Obvious Choices",'?');
	    noegnud_gui_add_actionbutton((noegnud_gui_twindow *)widget,"[*] All",'*');
	}
	noegnud_gui_add_actionbutton((noegnud_gui_twindow *)widget,"Cancel",'\033');
    }

    widget->x=(noegnud_options_screenwidth->value-widget->width)/2;

    return widget;
}

noegnud_gui_twidget *noegnud_gui_nh_getlin(const char *ques, const char *input) {
    noegnud_gui_twidget *widget;
    noegnud_gui_twidget *widget_textbox;

    int width,height;

    width =
	NOEGNUD_GUI_BORDER_SIZE +
	strlen(ques)*noegnud_gui_font->width +
	NOEGNUD_GUI_BORDER_SIZE +
	NOEGNUD_GUI_BORDER_SIZE +
	(strlen(input)+1)*noegnud_gui_font->width +
	NOEGNUD_GUI_BORDER_SIZE +
	NOEGNUD_GUI_BORDER_SIZE;
    height =
	NOEGNUD_GUI_BORDER_SIZE +
	NOEGNUD_GUI_BORDER_SIZE +
	noegnud_gui_font->height +
	NOEGNUD_GUI_BORDER_SIZE +
	NOEGNUD_GUI_BORDER_SIZE;
    widget =
	(noegnud_gui_twidget *)noegnud_gui_create_window(
							 noegnud_guiwidget_desktop,
							 (noegnud_options_screenwidth->value-width)/2,
							 (noegnud_options_screenheight->value/2-height)/2,
							 width,height,
							 1.0,1.0,1.0,1.0,0);

    widget->theme=noegnud_gui_default_widgettheme;
    noegnud_gui_create_text(
			    widget,
			    NOEGNUD_GUI_BORDER_SIZE,
			    NOEGNUD_GUI_BORDER_SIZE*2,
			    strlen(ques)*noegnud_gui_font->width,
			    noegnud_gui_font->height,
			    noegnud_options_gui_window_console_text_colour_default->r,
			    noegnud_options_gui_window_console_text_colour_default->g,
			    noegnud_options_gui_window_console_text_colour_default->b,
			    ques);

    noegnud_gui_create_text(
			    widget_textbox=(noegnud_gui_twidget *)noegnud_gui_create_window(
											    widget,
											    NOEGNUD_GUI_BORDER_SIZE + strlen(ques)*noegnud_gui_font->width + NOEGNUD_GUI_BORDER_SIZE,
											    NOEGNUD_GUI_BORDER_SIZE,
											    NOEGNUD_GUI_BORDER_SIZE + (strlen(input)+1)*noegnud_gui_font->width + NOEGNUD_GUI_BORDER_SIZE,
											    NOEGNUD_GUI_BORDER_SIZE + noegnud_gui_font->height + NOEGNUD_GUI_BORDER_SIZE,
											    0.0,0.0,0.0,0.5,0),
			    NOEGNUD_GUI_BORDER_SIZE,
			    NOEGNUD_GUI_BORDER_SIZE,
			    strlen(input)*noegnud_gui_font->width,
			    noegnud_gui_font->height,
			    1.0,1.0,0.0,
			    input);
    widget_textbox->event=noegnud_gui_event_widget;

    noegnud_gui_create_text(
			    widget_textbox,
			    NOEGNUD_GUI_BORDER_SIZE + strlen(input)*noegnud_gui_font->width,
			    NOEGNUD_GUI_BORDER_SIZE,
			    strlen(input)*noegnud_gui_font->width,
			    noegnud_gui_font->height,
			    0.7,0.7,0.0,
			    "_");

    return widget;
}


noegnud_gui_tprogressbar *noegnud_gui_inconsole_progressbar(const char *text, int minimum, int maximum, GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    noegnud_gui_twidget *mainwidget;
    noegnud_gui_tprogressbar *required;

    int textwidth;

    textwidth=strlen(text)*noegnud_gui_font->width;

    mainwidget=noegnud_gui_create_widget(
					 noegnud_guiwidget_console->child,
					 10,0,
					 noegnud_guiwidget_console->child->width-20,noegnud_gui_font->height,0);

    noegnud_gui_create_text(
			    mainwidget,
			    0,0,
			    textwidth,noegnud_gui_font->height,
			    0.75,0.75,0.75,text);
    mainwidget->child->clipto=noegnud_guiwidget_console->child;

    required=noegnud_gui_create_progressbar(
					    mainwidget,
					    10+textwidth+10,1,
					    mainwidget->width-(30+textwidth),noegnud_gui_get_fontheight()-2,
					    0.0,0.0,0.0,0.0,
					    minimum,maximum,
					    r,g,b,a);
    required->window.widget.clipto=noegnud_guiwidget_console->child;
    noegnud_gui_reshuffle_console_children(noegnud_guiwidget_console->child);

    return required;

}

void noegnud_gui_inconsole_noegnudtext(noegnud_gui_twidget *widget, const char *text, int dynamic, GLfloat r, GLfloat g, GLfloat b) {
    int textwidth;
    noegnud_gui_ttext *widget_text;

    textwidth=strlen(text)*noegnud_gui_font->width;


    widget_text=noegnud_gui_create_text(
					widget,
					10,0,
					textwidth,noegnud_gui_font->height,
					r,g,b,text);
    widget_text->dynamiccharwidth=dynamic;

    noegnud_gui_reshuffle_console_children(widget);
}


#define NOEGNUD_GUI_CONSOLE_GAP 4
void noegnud_gui_reshuffle_console_children(noegnud_gui_twidget *widget) {
    int pos;
    noegnud_gui_twidget *child;

    if (!widget->child) return;
    child=widget->child;

    while (child->nextsibling) child=child->nextsibling;

    pos=widget->height;

    while (child) {
	pos-=(child->height+NOEGNUD_GUI_CONSOLE_GAP);
	child->y=pos;
	child=child->prevsibling;
    }

    while ((widget->child)&&(widget->child->y+widget->child->height<0)) widget->child->kill(widget->child);

}

#define MINIMAP_ALPHA 1.0
static GLfloat noegnud_nethackcolours_minimap[16][4] = {
    {0.0,0.0,0.0,MINIMAP_ALPHA},
    {0.5,0.0,0.0,MINIMAP_ALPHA},
    {0.0,0.5,0.0,MINIMAP_ALPHA},
    {0.625,0.5,0.25,MINIMAP_ALPHA},
    {0.0,0.0,0.5,MINIMAP_ALPHA},
    {0.5,0.0,0.5,MINIMAP_ALPHA},
    {0.0,0.5,0.5,MINIMAP_ALPHA},
    {0.75,0.75,0.75,MINIMAP_ALPHA},
    {0.5,0.5,0.5,MINIMAP_ALPHA},
    {1.0,0.625,0.0,MINIMAP_ALPHA},
    {0.0,1.0,0.0,MINIMAP_ALPHA},
    {1.0,1.0,0.0,MINIMAP_ALPHA},
    {0.0,0.0,1.0,MINIMAP_ALPHA},
    {1.0,0.0,1.0,MINIMAP_ALPHA},
    {0.0,1.0,1.0,MINIMAP_ALPHA},
    {1.0,1.0,1.0,MINIMAP_ALPHA} };

void noegnud_gui_draw_minimap(noegnud_gui_twindow *window, int drawchildren) {
    noegnud_gui_twidget *parent;
    int x,y;

    float highlight_multiplier;

    noegnud_fonts_tfont *font;

    font=noegnud_font_minimap;

	noegnud_gui_draw_window(window,0);

	glDisable(GL_BLEND);
	glLoadIdentity ();
	noegnud_gui_widget_cliptome((noegnud_gui_twidget *)window);

	parent=window->widget.parent;
	while(parent) {
	    glTranslated (parent->x,-parent->y, 0);
	    noegnud_gui_checkoffset_widget(parent);
	    if (window->widget.type!=NOEGNUD_GUI_VSCROLL) glTranslated (parent->offset_x,parent->offset_y, 0);
	    parent=parent->parent;
	}
	glTranslated (window->widget.x,noegnud_options_screenheight->value-window->widget.y, 0);

	glPushMatrix ();
	for (y = 0; y < MAX_MAP_Y; y++)
	    {
		for (x = 0; x < MAX_MAP_X; x++)
		    {

			glPopMatrix ();
			glPushMatrix ();
			glTranslated (x*8+5,-((y+1)*8+5),0);

			if (((x == noegnud_clip_x) && (y == noegnud_clip_y))||(((noegnud_render_mouse_map_x-1)==x)&&(noegnud_render_mouse_map_y==y))) {
			    highlight_multiplier=noegnud_render_highlight_strobe/2+0.5;
			    glColor4f(1.0,1.0,1.0,highlight_multiplier);
			    glDisable(GL_TEXTURE_2D);
			    glBegin(GL_LINES);
			    glVertex2i(0,0);
			    glVertex2i(8,0);

			    glVertex2i(8,0);
			    glVertex2i(8,8);

			    glVertex2i(8,8);
			    glVertex2i(0,8);

			    glVertex2i(0,8);
			    glVertex2i(0,0);
			    glEnd();
			} else if (iflags.hilite_pet&&(noegnud_map[x][y].glyph_primary!=MAX_GLYPH)&&(noegnud_map[x][y].special_primary & MG_PET)) {
			    highlight_multiplier=noegnud_render_highlight_strobe/2+0.5;
			} else {
			    highlight_multiplier=1.0;
			}


			if (noegnud_map[x][y].glyph_primary!=MAX_GLYPH) {


			    if (iflags.wc_color)
				{

				    glColor4f(
					      noegnud_nethackcolours_minimap[noegnud_map[x][y].colour_primary][0]*highlight_multiplier,
					      noegnud_nethackcolours_minimap[noegnud_map[x][y].colour_primary][1]*highlight_multiplier,
					      noegnud_nethackcolours_minimap[noegnud_map[x][y].colour_primary][2]*highlight_multiplier,
					      noegnud_nethackcolours_minimap[noegnud_map[x][y].colour_primary][3]
					      );
				} else {
				    glColor4f(
					      noegnud_nethackcolours_minimap[CLR_WHITE][0]*highlight_multiplier,
					      noegnud_nethackcolours_minimap[CLR_WHITE][1]*highlight_multiplier,
					      noegnud_nethackcolours_minimap[CLR_WHITE][2]*highlight_multiplier,
					      noegnud_nethackcolours_minimap[CLR_WHITE][3]
					      );
				}

			    noegnud_fonts_printchar_scaled (font, noegnud_map[x][y].ch_primary,8.0/(float)font->width,8.0/(float)font->height);
			}

		    }
	    }

	glPopMatrix ();


    if (drawchildren) noegnud_gui_draw_widget_relatives((noegnud_gui_twidget *)window);

}

void noegnud_gui_event_minimap(noegnud_gui_twidget *widget, SDL_Event *event) {
    int x,y;

    noegnud_gui_event_widget_relatives(widget,event);

    if ((!noegnud_options_minimap_visible->value)) {
	return;
    };

    if (event->type==SDL_MOUSEMOTION) widget->mouseover=noegnud_gui_widget_contained((noegnud_gui_twidget *)widget,event->motion.x,event->motion.y);
    if (widget->mouseover) {
	if (!noegnud_gui_mouseoverwidget) noegnud_gui_mouseoverwidget=(int)widget;
    }

    if ((int)widget==noegnud_gui_mouseoverwidget) {
	x=event->motion.x-(noegnud_gui_widget_getabsolute_x(widget)+5);
	y=event->motion.y-(noegnud_gui_widget_getabsolute_y(widget)+5);
	if ((x>=0)&&(y>=0)&&(x<MAX_MAP_X*8)&&(MAX_MAP_Y*8)) {
	    noegnud_gui_event_widget_relatives(widget,event);
	    noegnud_gui_mouseoverminimap=1;
	    noegnud_render_mouse_map_x=(x/8)+1;
	    noegnud_render_mouse_map_y=(y/8);
	    return;
	}

    }
}



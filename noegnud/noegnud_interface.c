#include <dirent.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_thread.h>
#include <ctype.h>		//toupper
#include <math.h>

#ifdef __WIN32__
# include <windows.h>
#endif

#include "noegnud_debug.h"
#include "noegnud_config.h"
#include "noegnud_common.h"
#include "noegnud_options.h"
#include "noegnud_interface.h"
#include "noegnud_tilesets.h"
#include "noegnud_glfuncs.h"
#include "noegnud_collection.h"
#include "noegnud_gui.h"
#include "noegnud_fonts.h"
#include "noegnud_typeloader.h"
#include "noegnud_render.h"
#include "noegnud_gff_tga.h"
#include "noegnud_extern.h"
#include "noegnud_rip.h"
#include "noegnud_lang.h"
#include "noegnud_textcolouring.h"
#include "noegnud_reference.h"
#include "noegnud_mem.h"
#include "noegnud_textcolour.h"
#include "noegnud_sound.h"
#include "noegnud_widgettheme.h"

#include "func_tab.h" // nethack's "extcmdlist" structure

extern void play_sound_for_message(const char *str);

#ifdef NOEGNUD_GRAPHICS

int noegnud_screenmode = SDL_OPENGL;

int noegnud_graphics_system_ready = 0;

int noegnud_startup_flag=0;

#define RECORD_GAME_OFF			0
#define RECORD_GAME_TXT_PLAIN		1
#define RECORD_GAME_TXT_HTML_BW		2
#define RECORD_GAME_TXT_HTML_COLOUR	3

int record_game = RECORD_GAME_OFF;

int noegnud_lastmove = -1;
int noegnud_moves = 0;

int noegnud_console_speed = 24;		/* speed "limit" */
int noegnud_console_delta = 0;		/* current speed */

int noegnud_toptenwindow_pending=0;

#ifdef __WIN32__
int win32_noegnud_tounicode_supported=0;
#endif

float noegnud_scrolling_delta_x=0.0;
float noegnud_scrolling_delta_y=0.0;
float noegnud_delta_scrolling_speed=0.2;

int noegnud_yn_function_busy=0;
int noegnud_inventory_open=0;

noegnud_glfuncs_timage *noegnud_image_dungeon;
noegnud_glfuncs_timage *noegnud_image_tilenotfound;

SDL_Surface *noegnud_primary_surface;

/* this must NOT __EVER__ be less than 5 !!! (screenshot) ... TODO add an element of dynamism here methinks */
#define NOEGNUD_CONSOLEMESSAGES_MAX 5
char *noegnud_consolemessages[NOEGNUD_CONSOLEMESSAGES_MAX];

noegnud_glfuncs_timage *noegnud_console_image_splash;
//noegnud_glfuncs_timage *noegnud_console_image_console;

noegnud_fonts_tfont *noegnud_font_map;
noegnud_fonts_tfont *noegnud_font_minimap;

GLfloat noegnud_nethackcolours[16][4] = {
    {0.0,0.0,0.0,1.0},
    {0.5,0.0,0.0,1.0},
    {0.0,0.5,0.0,1.0},
    {0.625,0.5,0.25,1.0},
    {0.0,0.0,0.5,1.0},
    {0.5,0.0,0.5,1.0},
    {0.0,0.5,0.5,1.0},
    {0.75,0.75,0.75,1.0},
    {0.5,0.5,0.5,1.0},
    {1.0,0.625,0.0,1.0},
    {0.0,1.0,0.0,1.0},
    {1.0,1.0,0.0,1.0},
    {0.0,0.0,1.0,1.0},
    {1.0,0.0,1.0,1.0},
    {0.0,1.0,1.0,1.0},
    {1.0,1.0,1.0,1.0} };


int noegnud_nethackcolours_int[16] = {
    0xFF000000,
    0xFF000080,
    0xFF00FF00,
    0xFF4080A0,
    0xFF800000,
    0xFF800080,
    0xFF808000,
    0xFF808080,
    0xFF404040,
    0xFF00A0FF,
    0xFF00FF00,
    0xFF00FFFF,
    0xFFFF0000,
    0xFFFF00FF,
    0xFFFFFF00,
    0xFFFFFFFF };

noegnud_tmap_cell noegnud_map[MAX_MAP_X][MAX_MAP_Y];

int noegnud_clip_x;
int noegnud_clip_y;

char szWindowCaption[] = NOEGNUD_FULLNAMEVERSION;

/* Interface definition, for windows.c */
struct window_procs noegnud_procs = {
    "noegnud",
    WC_COLOR|WC_HILITE_PET,
    0,
    noegnud_init_nhwindows,
    noegnud_player_selection,
    noegnud_askname,
    noegnud_get_nh_event,
    noegnud_exit_nhwindows,
    noegnud_suspend_nhwindows,
    noegnud_resume_nhwindows,
    noegnud_create_nhwindow,
    noegnud_clear_nhwindow,
    noegnud_display_nhwindow,
    noegnud_destroy_nhwindow,
    noegnud_curs,
    noegnud_putstr,
    noegnud_display_file,
    noegnud_start_menu,
    noegnud_add_menu,
    noegnud_end_menu,
    noegnud_select_menu,
    noegnud_message_menu,
    noegnud_update_inventory,
    noegnud_mark_synch,
    noegnud_wait_synch,
#ifdef CLIPPING
    noegnud_cliparound,
#endif
#ifdef POSITIONBAR
    noegnud_update_positionbar,
#endif
    noegnud_print_glyph,
    noegnud_raw_print,
    noegnud_raw_print_bold,
    noegnud_nhgetch,
    noegnud_nh_poskey,
    noegnud_nhbell,
    noegnud_doprev_message,
    noegnud_yn_function,
    noegnud_getlin,
    noegnud_get_ext_cmd,
    noegnud_number_pad,
    noegnud_delay_output,
#ifdef CHANGE_COLOR		/* the Mac uses a palette device */
    donull,
#ifdef MAC
    donull,
    donull,
#endif
    donull,
#endif
    noegnud_start_screen,
    noegnud_end_screen,
    noegnud_outrip,
    noegnud_preference_update
};


void abend(const char *message) {
    clearlocks();
    noegnud_exit_nhwindows(message);
    terminate(EXIT_SUCCESS);
}

void console_process () {
    if (noegnud_console_delta==0) {
	if (noegnud_guiwidget_console->y!=0) {
	    noegnud_options_console_gap->value=noegnud_guiwidget_console->y+noegnud_guiwidget_console->height;
	}
	return;
    }
    noegnud_guiwidget_console->y += noegnud_console_delta;
    if (-noegnud_guiwidget_console->y>noegnud_guiwidget_console->height-noegnud_options_console_gap->value) {
	noegnud_guiwidget_console->y=-(noegnud_guiwidget_console->height-noegnud_options_console_gap->value);
	noegnud_console_delta=0;
    }
    if (noegnud_guiwidget_console->y>0) {
	noegnud_guiwidget_console->y=0;
	noegnud_console_delta=0;
    }
}

void console_startmovement () {

    if (noegnud_console_delta != 0) {
	noegnud_console_delta = -noegnud_console_delta;
	return;
    }

    if (noegnud_guiwidget_console->y==0) {
	noegnud_console_delta = -noegnud_console_speed;
    } else {
	noegnud_console_delta = noegnud_console_speed;
    };
}

char *noegnud_create_rolename() {
    char *role;

    role=noegnud_mem_malloc(4*3 + 3 + 1);
    role[0]=0;

    if (flags.initrole==-1) {
	strcat(role,"???");
    } else {
	if (flags.initgend==1&&roles[flags.initrole].name.f) {
	    strncat(role,roles[flags.initrole].name.f,3);
	} else {
	strncat(role,roles[flags.initrole].name.m,3);
    }
    }
    strcat(role,"-");
    if (flags.initrace==-1) {
	strcat(role,"???");
    } else {
	strncat(role,races[flags.initrace].noun,3);
    }
    strcat(role,"-");
    if (flags.initgend==-1) {
	strcat(role,"???");
    } else {
	strncat(role,genders[flags.initgend].adj,3);
    }
    strcat(role,"-");
    if (flags.initalign==-1) {
	strcat(role,"???");
    } else {
	strncat(role,aligns[flags.initalign].adj,3);
    }
    return role;
}

void noegnud_create_screenshot_filename (char *dest, char *suffix, int num) {
    char *role;
    char *homedir;

    homedir=(char *)noegnud_options_get_home();
    sprintf (dest, "%s/noeGNUd_capture_%s_%d_%08d_%08d_",homedir,plname,(int)yyyymmdd(u.ubirthday),(int)moves,num);
    role=noegnud_create_rolename();
    strcat(dest,role);
    noegnud_mem_free(role);
    strcat(dest,".");
    strcat(dest,suffix);
    noegnud_mem_free(homedir);
}

void
noegnud_screenshot_txt ()
{
    int x, y;
    char filename[2048];
    FILE *screenshot_file;


    noegnud_create_screenshot_filename ((char *) &filename, "txt", noegnud_moves);
    screenshot_file = fopen (filename, "w");


    fprintf
	(screenshot_file,
	 "\n--- generated by noeGNUd [ http://noegnud.sourceforge.net ] ----------------\n");
    for (y = 0; y < MAX_MAP_Y; y++)
	{
	    for (x = 0; x < MAX_MAP_X; x++)
		{
		    if (noegnud_map[x][y].glyph_primary == MAX_GLYPH)
			{
			    fprintf (screenshot_file, " ");
			}
		    else
			{
			    fprintf (screenshot_file, "%c", noegnud_map[x][y].ch_primary);
			}
		}
	    fprintf (screenshot_file, "\n");
	}
    fprintf
	(screenshot_file,
	 "----------------------------------------------------------------------------\n");
    fprintf (screenshot_file, "%s\n", ((noegnud_gui_ttext *)noegnud_guiwidget_status_container->child)->text);
    fprintf (screenshot_file, "%s\n", ((noegnud_gui_ttext *)noegnud_guiwidget_status_container->child->nextsibling)->text);
    fprintf (screenshot_file,
	     "----------------------------------------------------------------------------\n\n");

    fclose (screenshot_file);
    noegnud_gui_inconsole_noegnudtext(noegnud_guiwidget_console->child,"screenshot taken.",TRUE,0.75,0.75,0.75);

};

void
noegnud_screenshot_html ()
{
    int x, y;
    int c;
    char filename[2048];
    char nextfilename[2048];
    FILE *screenshot_file;

    char colour[7];


    noegnud_create_screenshot_filename ((char *) &filename, "html", noegnud_moves);
    if (record_game) noegnud_create_screenshot_filename ((char *) &nextfilename, "html", noegnud_moves+1);
    screenshot_file = fopen (filename, "w");



    fprintf (screenshot_file, "\n<html>");
    if (record_game)
	fprintf (screenshot_file,
		 "<META http-equiv=\"refresh\" content=\"0;URL=%s\">",
		 nextfilename);

    fprintf (screenshot_file,
	     "<body bgcolor=#000000><font color=#808080><pre>--- generated by noeGNUd [ <a href=\"http://noegnud.sourceforge.net\">http://noegnud.sourceforge.net</a> ] ----------------\n");

    if (record_game) {
	for (c=0;c<NOEGNUD_CONSOLEMESSAGES_MAX;c++) {
	    fprintf(screenshot_file, "%s\n",noegnud_consolemessages[c]);
	}
	fprintf
	    (screenshot_file,
	     "----------------------------------------------------------------------------\n");
    }

    for (y = 0; y < MAX_MAP_Y; y++)
	{
	    for (x = 0; x < MAX_MAP_X; x++)
		{
		    if (noegnud_map[x][y].glyph_primary == MAX_GLYPH)
			{
			    fprintf (screenshot_file, " ");
			}
		    else
			{
			    if (noegnud_map[x][y].colour_primary != 7)
				{
				    fprintf (screenshot_file, "<font color=\"#");
				    switch (noegnud_map[x][y].colour_primary)
					{
					case CLR_BLACK:
					    strcpy ((char *) &colour, "000000");
					    fprintf (screenshot_file, "000000");
					    break;
					case CLR_RED:
					    fprintf (screenshot_file, "80000");
					    break;
					case CLR_GREEN:
					    fprintf (screenshot_file, "008000");
					    break;
					case CLR_BROWN:
					    fprintf (screenshot_file, "A08040");
					    break;
					case CLR_BLUE:
					    fprintf (screenshot_file, "000080");
					    break;
					case CLR_MAGENTA:
					    fprintf (screenshot_file, "800080");
					    break;
					case CLR_CYAN:
					    fprintf (screenshot_file, "008080");
					    break;
					case CLR_GRAY:
					    fprintf (screenshot_file, "404040");
					    break;
					case CLR_ORANGE:
					    fprintf (screenshot_file, "FFA000");
					    break;
					case CLR_BRIGHT_GREEN:
					    fprintf (screenshot_file, "00FF00");
					    break;
					case CLR_YELLOW:
					    fprintf (screenshot_file, "FFFF00");
					    break;
					case CLR_BRIGHT_BLUE:
					    fprintf (screenshot_file, "0000FF");
					    break;
					case CLR_BRIGHT_MAGENTA:
					    fprintf (screenshot_file, "FF00FF");
					    break;
					case CLR_BRIGHT_CYAN:
					    fprintf (screenshot_file, "00FFFF");
					    break;
					case CLR_WHITE:
					    fprintf (screenshot_file, "FFFFFF");
					    break;
					}
				    fprintf (screenshot_file, "\">");
				}
			    switch (noegnud_map[x][y].ch_primary)
				{
				case '<':
				    fprintf (screenshot_file, "&lt;");
				    break;
				case '>':
				    fprintf (screenshot_file, "&gt;");
				    break;
				default:
				    fprintf (screenshot_file, "%c", noegnud_map[x][y].ch_primary);
				}
			    if (noegnud_map[x][y].colour_primary != 7)
				fprintf (screenshot_file, "</font>");
			}
		}
	    fprintf (screenshot_file, "\n");
	}
    fprintf
	(screenshot_file,
	 "----------------------------------------------------------------------------\n");
    fprintf (screenshot_file, "%s\n",((noegnud_gui_ttext *)noegnud_guiwidget_status_container->child)->text);
    fprintf (screenshot_file, "%s\n",((noegnud_gui_ttext *)noegnud_guiwidget_status_container->child->nextsibling)->text);
    fprintf (screenshot_file,
	     "----------------------------------------------------------------------------</pre></font></body></html>\n");

    fclose (screenshot_file);

    noegnud_gui_inconsole_noegnudtext(noegnud_guiwidget_console->child,"screenshot taken.",TRUE,0.75,0.75,0.75);

};

void noegnud_screenshot_bmp() {
    char filename[2048];

    SDL_Surface *savesurface;

    void *pixeldata;
    void *pixeldata_fixed;
    void *pixel_to;
    void *pixel_from;

    int ycount;

    noegnud_create_screenshot_filename ((char *) &filename, "bmp", noegnud_moves);

    pixeldata=noegnud_mem_malloc(noegnud_options_screenwidth->value*noegnud_options_screenheight->value*3);
    glReadPixels(
		 0,0,
		 noegnud_options_screenwidth->value,
		 noegnud_options_screenheight->value,
		 GL_RGB,
		 GL_UNSIGNED_BYTE,
		 pixeldata);

    pixeldata_fixed=noegnud_mem_malloc(noegnud_options_screenwidth->value*noegnud_options_screenheight->value*3);
    pixel_to=pixeldata_fixed;
    pixel_from=pixeldata;
    pixel_from+=(noegnud_options_screenwidth->value*(noegnud_options_screenheight->value-1))*3;
    for (ycount=1;ycount<noegnud_options_screenheight->value;ycount++) {
	memcpy(pixel_to,pixel_from,noegnud_options_screenwidth->value*3);
	pixel_to+=noegnud_options_screenwidth->value*3;
	pixel_from-=noegnud_options_screenwidth->value*3;
    }
    noegnud_mem_free(pixeldata);

    if ((savesurface=noegnud_mem_sdl_creatergbsurfacefrom(
							  pixeldata_fixed,
							  noegnud_options_screenwidth->value,
							  noegnud_options_screenheight->value,
							  24, noegnud_options_screenwidth->value*3,
							  0x000000FF,
							  0x0000FF00,
							  0x00FF0000,
							  0x00000000))) {

	SDL_SaveBMP(savesurface, filename);
	noegnud_mem_sdl_freesurface(savesurface);
    }
    noegnud_mem_free(pixeldata_fixed);
    noegnud_gui_inconsole_noegnudtext(noegnud_guiwidget_console->child,"screenshot taken.",TRUE,0.75,0.75,0.75);
}

void noegnud_screenshot_tga() {
    char filename[2048];
    void *pixeldata;

    noegnud_create_screenshot_filename ((char *) &filename, "tga", noegnud_moves);

    pixeldata=noegnud_mem_malloc(noegnud_options_screenwidth->value*noegnud_options_screenheight->value*3);
    glReadPixels(
		 0,0,
		 noegnud_options_screenwidth->value,
		 noegnud_options_screenheight->value,
		 GL_RGB,
		 GL_UNSIGNED_BYTE,
		 pixeldata);

    noegnud_gff_tga_save(filename,noegnud_options_screenwidth->value,noegnud_options_screenheight->value,pixeldata);
    noegnud_mem_free(pixeldata);
    noegnud_gui_inconsole_noegnudtext(noegnud_guiwidget_console->child,"screenshot taken.",TRUE,0.75,0.75,0.75);
}

void noegnud_screenshot () {
    winid window;
    anything identifier;
    menu_item *menu_list;
    int chosen;

    menu_list=0;
    window=noegnud_create_nhwindow(NHW_MENU);
    noegnud_start_menu(window);
    identifier.a_int=0; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "text", 0);
    identifier.a_int=1; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "plain text", 0);
    identifier.a_int=2; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "html", 0);
    identifier.a_int=0; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "graphical", 0);
    identifier.a_int=11; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "bmp", 0);
    identifier.a_int=12; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "tga", 0);
    noegnud_end_menu(window,"Take Screenshot ?");
    chosen=0;
    if (noegnud_select_menu (window, PICK_ONE, &menu_list)==1) {
	chosen=menu_list->item.a_int;
	free(menu_list);
    }
    noegnud_destroy_nhwindow(window);

    /* yes twice, double buffering & shit */
    if (noegnud_guiwidget_desktop) noegnud_guiwidget_desktop->draw(noegnud_guiwidget_desktop,1);
    noegnud_render_loop();
    if (noegnud_guiwidget_desktop) noegnud_guiwidget_desktop->draw(noegnud_guiwidget_desktop,1);
    noegnud_render_loop();

    switch (chosen) {
    case 1: noegnud_screenshot_txt (); break;
    case 2: noegnud_screenshot_html (); break;

    case 11:noegnud_screenshot_bmp(); break;
    case 12:noegnud_screenshot_tga(); break;
    }
}

void
window_message_render ()
{

    glDisable (GL_TEXTURE_2D);
    //  glBindTexture (GL_TEXTURE_2D, gltexture_widget_window);
    glLoadIdentity ();
    glTranslated (0, (noegnud_options_screenheight->value - 512) / 2, 0);
    glBegin (GL_QUADS);
    glTexCoord2f (0, 1.0);
    glVertex2i (0, 0);
    glTexCoord2f (1.0, 1.0);
    glVertex2i (noegnud_options_screenwidth->value, 0);
    glTexCoord2f (1.0, 0);
    glVertex2i (noegnud_options_screenwidth->value, 512);
    glTexCoord2f (0, 0);
    glVertex2i (0, 512);
    glEnd ();
}

void
process_checkrecord ()
{
    if ((record_game) && (noegnud_lastmove != moves))
	{
	    if (!noegnud_guiwidget_status_container->child) return;
	    noegnud_lastmove = moves;
	    noegnud_moves++;
	    noegnud_screenshot_html ();
	}
}

void process () {

    process_checkrecord ();

    console_process ();
    noegnud_render_loop ();

    process_checkrecord ();

}


void clearmap () {
    int x, y;

    for (x = 0; x < MAX_MAP_X; x++) {
	for (y = 0; y < MAX_MAP_Y; y++) {
	    noegnud_map[x][y].glyph_primary = MAX_GLYPH;

	    noegnud_map[x][y].glyph_fore = noegnud_map[x][y].glyph_middle = noegnud_map[x][y].glyph_back = MAX_GLYPH;
	    noegnud_map[x][y].low=noegnud_map[x][y].high=FALSE;
	}
    }
    noegnud_tilesets_clearloaded(noegnud_activetileset);
    noegnud_render_mouse_map_x=-1;
}

GLuint noegnud_internal_textures_load_single (const char *fname) {
    SDL_Surface *tmp;
    GLuint texture;

    tmp=noegnud_mem_img_load(fname);

    if (!tmp)
	{
	    return 0;
	}
    glGenTextures (1, &texture);
    glBindTexture (GL_TEXTURE_2D, texture);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SDL_LockSurface (tmp);
    if (tmp->format->BytesPerPixel == 3)
	{
	    gluBuild2DMipmaps
		(GL_TEXTURE_2D,
		 tmp->format->BytesPerPixel,
		 tmp->w, tmp->h, GL_RGB, GL_UNSIGNED_BYTE, tmp->pixels);
	}
    else
	{
	    gluBuild2DMipmaps
		(GL_TEXTURE_2D,
		 tmp->format->BytesPerPixel,
		 tmp->w, tmp->h, GL_RGBA, GL_UNSIGNED_BYTE, tmp->pixels);
	}
    SDL_UnlockSurface (tmp);
    return texture;
}


void noegnud_noegnud_options_displaymethod_tile();
void noegnud_internal_init_graphics_system () {

    char buffer[512];
    int  viewport_coords[4]={0};

    clearmap ();
    SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE );

    // this is for nvidia cards in linux.
    //putenv ("__GL_SYNC_TO_VBLANK=1");

    SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
    if (noegnud_options_fullscreen->value) noegnud_screenmode = noegnud_screenmode | SDL_FULLSCREEN;

    noegnud_primary_surface=SDL_SetVideoMode (noegnud_options_screenwidth->value, noegnud_options_screenheight->value,
					      noegnud_options_screenbpp->value, noegnud_screenmode);

    if (!noegnud_primary_surface) {
	printf("[WARNING] failed to set video mode %dx%dx%d swiching to 640x480x16...\n",noegnud_options_screenwidth->value,noegnud_options_screenheight->value,noegnud_options_screenbpp->value);
	noegnud_options_screenwidth->value=640;
	noegnud_options_screenheight->value=480;
	noegnud_options_screenbpp->value=16;
	noegnud_primary_surface=SDL_SetVideoMode (noegnud_options_screenwidth->value, noegnud_options_screenheight->value,
						  noegnud_options_screenbpp->value, noegnud_screenmode);
	if (!noegnud_primary_surface) {
	    printf("\n\n[ERROR] Failed to create primary rendering surface.\n    You have a recent version of SDL ???\n    Your OpenGL is configured correctly ???\n    ..Aborting\n\n\n");
	    exit(-1);
	}
    }
    atexit (SDL_Quit);

    SDL_WM_GrabInput(SDL_GRAB_FULLSCREEN);
    SDL_WM_SetCaption (szWindowCaption, 0);
#ifndef __WIN32__
    SDL_EnableUNICODE (1);
#endif

    SDL_EnableKeyRepeat(noegnud_options_key_repeat_delay->value,noegnud_options_key_repeat_interval->value);

    noegnud_glfuncs_init ();
    noegnud_gui_init();
    noegnud_sound_init();
    noegnud_render_loop();

    glGetIntegerv(GL_VIEWPORT, viewport_coords);

    noegnud_options_screenwidth->value=viewport_coords[2];
    noegnud_options_screenheight->value=viewport_coords[3];

    noegnud_guiwidget_desktop=noegnud_gui_create_widget( NULL, 0, 0, noegnud_options_screenwidth->value, noegnud_options_screenheight->value, 0);
    noegnud_guiwidget_desktop->mouseover_check=0;

    noegnud_guiwidget_console=(noegnud_gui_twidget *)noegnud_gui_create_window(
									       //noegnud_gui_create_widget(
									       noegnud_guiwidget_desktop,
									       //0, 0, noegnud_options_screenwidth->value, noegnud_options_screenheight->value,0),
									       0, 0, noegnud_options_screenwidth->value, noegnud_options_screenheight->value,
									       1.0,1.0,1.0,1.0,
									       0);


    ((noegnud_gui_twindow *)noegnud_guiwidget_console)->movement_lock_x=TRUE;
    ((noegnud_gui_twindow *)noegnud_guiwidget_console)->movable=FALSE;
    noegnud_console_image_splash=noegnud_glfuncs_loadimage("gui/splashscreen.png",TRUE,FALSE,FALSE);
    ((noegnud_gui_twindow *)noegnud_guiwidget_console)->image=noegnud_console_image_splash;
    ((noegnud_gui_twindow *)noegnud_guiwidget_console)->image_tiled=0;
    ((noegnud_gui_twindow *)noegnud_guiwidget_console)->localimage=1;
    ((noegnud_gui_twindow *)noegnud_guiwidget_console)->refreshtitlebar=FALSE;

    noegnud_render_loop();


    /*
      noegnud_guiwidget_desktop_overlay_hallucinating=(noegnud_gui_twidget *)noegnud_gui_create_window(
      noegnud_guiwidget_desktop,
      0,0,
      noegnud_options_screenwidth->value, noegnud_options_screenheight->value,
      1.0,1.0,1.0,1.0,
      0,0);
      noegnud_guiwidget_desktop_overlay_hallucinating->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_widget;
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_hallucinating)->image=noegnud_glfuncs_loadimage("status/hallucinating.png",TRUE,FALSE);
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_hallucinating)->movable=0;
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_hallucinating)->image_tiled=0;

      noegnud_guiwidget_desktop_overlay_confused=(noegnud_gui_twidget *)noegnud_gui_create_window(
      noegnud_guiwidget_desktop,
      0,0,
      noegnud_options_screenwidth->value, noegnud_options_screenheight->value,
      1.0,1.0,1.0,1.0,
      0,0);
      noegnud_guiwidget_desktop_overlay_confused->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_widget;
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_confused)->image=noegnud_glfuncs_loadimage("status/confused.png",TRUE,FALSE);
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_confused)->movable=0;
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_confused)->image_tiled=0;

      noegnud_guiwidget_desktop_overlay_stunned=(noegnud_gui_twidget *)noegnud_gui_create_window(
      noegnud_guiwidget_desktop,
      0,0,
      noegnud_options_screenwidth->value, noegnud_options_screenheight->value,
      1.0,1.0,1.0,1.0,
      0,0);
      noegnud_guiwidget_desktop_overlay_stunned->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_widget;
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_stunned)->image=noegnud_glfuncs_loadimage("status/stunned.png",TRUE,FALSE);
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_stunned)->movable=0;
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_stunned)->image_tiled=0;

      noegnud_guiwidget_desktop_overlay_poisoned=(noegnud_gui_twidget *)noegnud_gui_create_window(
      noegnud_guiwidget_desktop,
      0,0,
      noegnud_options_screenwidth->value, noegnud_options_screenheight->value,
      1.0,1.0,1.0,1.0,
      0,0);
      noegnud_guiwidget_desktop_overlay_poisoned->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_widget;
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_poisoned)->image=noegnud_glfuncs_loadimage("status/poisoned.png",TRUE,FALSE);
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_poisoned)->movable=0;
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_poisoned)->image_tiled=0;

      noegnud_guiwidget_desktop_overlay_blind=(noegnud_gui_twidget *)noegnud_gui_create_window(
      noegnud_guiwidget_desktop,
      0,0,
      noegnud_options_screenwidth->value, noegnud_options_screenheight->value,
      1.0,1.0,1.0,1.0,
      0,0);
      noegnud_guiwidget_desktop_overlay_blind->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_widget;
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_blind)->image=noegnud_glfuncs_loadimage("status/blind.png",TRUE,FALSE);
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_blind)->movable=0;
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_blind)->image_tiled=0;

      noegnud_guiwidget_desktop_overlay_slimed=(noegnud_gui_twidget *)noegnud_gui_create_window(
      noegnud_guiwidget_desktop,
      0,0,
      noegnud_options_screenwidth->value, noegnud_options_screenheight->value,
      1.0,1.0,1.0,1.0,
      0,0);
      noegnud_guiwidget_desktop_overlay_slimed->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_widget;
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_slimed)->image=noegnud_glfuncs_loadimage("status/slimed.png",TRUE,FALSE);
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_slimed)->movable=0;
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_slimed)->image_tiled=0;

    */


    noegnud_gui_create_widget(
			      noegnud_guiwidget_console,
			      0,
			      0,
			      noegnud_guiwidget_console->width,
			      noegnud_guiwidget_console->height,
			      0);

    noegnud_render_loop();

    noegnud_gui_inconsole_noegnudtext( noegnud_guiwidget_console->child,NOEGNUD_FULLNAMEVERSION,TRUE,
				       noegnud_options_gui_window_console_text_colour_system->r,
				       noegnud_options_gui_window_console_text_colour_system->g,
				       noegnud_options_gui_window_console_text_colour_system->b);

    noegnud_gui_inconsole_noegnudtext( noegnud_guiwidget_console->child,"------------------------------",TRUE,
				       noegnud_options_gui_window_console_text_colour_system->r,
				       noegnud_options_gui_window_console_text_colour_system->g,
				       noegnud_options_gui_window_console_text_colour_system->b);

    noegnud_gui_inconsole_noegnudtext( noegnud_guiwidget_console->child,"loading language files",TRUE,
				       noegnud_options_gui_window_console_text_colour_system->r,
				       noegnud_options_gui_window_console_text_colour_system->g,
				       noegnud_options_gui_window_console_text_colour_system->b);
    noegnud_render_loop();
    noegnud_lang_init();


    noegnud_gui_inconsole_noegnudtext( noegnud_guiwidget_console->child,"loading colourisation files",TRUE,
				       noegnud_options_gui_window_console_text_colour_system->r,
				       noegnud_options_gui_window_console_text_colour_system->g,
				       noegnud_options_gui_window_console_text_colour_system->b);
    noegnud_render_loop();
    noegnud_textcolouring_init();


    noegnud_gui_inconsole_noegnudtext( noegnud_guiwidget_console->child,"loading variant reference",TRUE,
				       noegnud_options_gui_window_console_text_colour_system->r,
				       noegnud_options_gui_window_console_text_colour_system->g,
				       noegnud_options_gui_window_console_text_colour_system->b);

    noegnud_render_loop();
    noegnud_reference_init();

    noegnud_gui_inconsole_noegnudtext( noegnud_guiwidget_console->child,"loading console graphic",TRUE,
				       noegnud_options_gui_window_console_text_colour_system->r,
				       noegnud_options_gui_window_console_text_colour_system->g,
				       noegnud_options_gui_window_console_text_colour_system->b);

    noegnud_render_loop();
    //     noegnud_console_image_console=noegnud_glfuncs_loadimage("gui/console.png",TRUE,FALSE,FALSE);

    noegnud_gui_inconsole_noegnudtext( noegnud_guiwidget_console->child,"loading fonts",TRUE,
				       noegnud_options_gui_window_console_text_colour_system->r,
				       noegnud_options_gui_window_console_text_colour_system->g,
				       noegnud_options_gui_window_console_text_colour_system->b);

    noegnud_render_loop();
    noegnud_font_map=noegnud_fonts_loadfont(noegnud_options_mode_char_font->value);
    noegnud_font_minimap=noegnud_fonts_loadfont(noegnud_options_minimap_font->value);

    noegnud_graphics_system_ready = 1;

    noegnud_activetileset=noegnud_tilesets_loadtileset(noegnud_options_mode_tileset_tile->value);
    if (!noegnud_activetileset) {
      noegnud_noegnud_options_displaymethod_tile();
      if (!noegnud_activetileset) {
        printf("* [ERROR] Couldn't find tileset : \"%s\" trying nethack ...\n",noegnud_options_mode_tileset_tile->value);
        noegnud_activetileset=noegnud_tilesets_loadtileset("nethack");
        if (!noegnud_activetileset) {
          printf("* [ERROR] Couldn't find tileset : \"nethack\" either, aborting ...\n");
          exit(-1);
        }
      }
    }
#ifdef __WIN32__
    if ((win32_noegnud_tounicode_supported=GetProcAddress(LoadLibrary("user32.dll"),"ToUnicode"))) {
	noegnud_gui_inconsole_noegnudtext( noegnud_guiwidget_console->child,"unicode is supported on this win32 version",TRUE,
					   noegnud_options_gui_window_console_text_colour_system->r,
					   noegnud_options_gui_window_console_text_colour_system->g,
					   noegnud_options_gui_window_console_text_colour_system->b);
    } else {
	noegnud_gui_inconsole_noegnudtext( noegnud_guiwidget_console->child,"unicode is NOT supported on this win32 version",TRUE,
					   noegnud_options_gui_window_console_text_colour_system->r,
					   noegnud_options_gui_window_console_text_colour_system->g,
					   noegnud_options_gui_window_console_text_colour_system->b);
    }
#endif

    sprintf((char *)&buffer,"press ( %s ) to lower the console at any time.",SDL_GetKeyName(noegnud_options_keys_console->value));
    noegnud_gui_inconsole_noegnudtext( noegnud_guiwidget_console->child,buffer,TRUE,
				       noegnud_options_gui_window_console_text_colour_system->r,
				       noegnud_options_gui_window_console_text_colour_system->g,
				       noegnud_options_gui_window_console_text_colour_system->b);

    sprintf((char *)&buffer,"------------------------------------- happy hacking -----[ %s for Options ]--",SDL_GetKeyName(noegnud_options_keys_options->value));
    noegnud_gui_inconsole_noegnudtext( noegnud_guiwidget_console->child,buffer,TRUE,
				       noegnud_options_gui_window_console_text_colour_system->r,
				       noegnud_options_gui_window_console_text_colour_system->g,
				       noegnud_options_gui_window_console_text_colour_system->b);
    noegnud_render_loop();



    noegnud_guiwidget_status=(noegnud_gui_twidget *)noegnud_gui_create_window(
								 noegnud_guiwidget_desktop,
								 0,noegnud_options_screenheight->value-(noegnud_gui_get_fontheight()*2+4*3),
								 noegnud_gui_get_fontwidth()*80, noegnud_gui_get_fontheight()*2+4*3,
								 1.0,1.0,1.0,
								 1.0,
								 0);

    noegnud_guiwidget_status_container=noegnud_gui_create_widget(
								 noegnud_guiwidget_status,
								 0,0,
								 noegnud_gui_get_fontwidth()*80, noegnud_gui_get_fontheight()*2+4*3,
								 0);
    noegnud_end_menu((winid)noegnud_guiwidget_status,"Status Bar");
    noegnud_guiwidget_status->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_null;
    noegnud_guiwidget_status->theme=noegnud_gui_default_widgettheme;
    ((noegnud_gui_twindow *)noegnud_guiwidget_status)->refreshtitlebar=FALSE;
    //    ((noegnud_gui_twindow *)noegnud_guiwidget_status)->movable=0;

#ifdef POSITIONBAR
    noegnud_guiwidget_positionbar=
	(noegnud_gui_twidget *)noegnud_gui_create_window(
							 noegnud_guiwidget_desktop,
							 noegnud_options_screenwidth->value-(10+noegnud_gui_get_fontwidth()*MAX_MAP_X+10)+8,
							 (noegnud_options_screenheight->value-((noegnud_gui_get_fontheight()*1+4*2)+noegnud_guiwidget_status->height+16)),
							 10+noegnud_gui_get_fontwidth()*MAX_MAP_X+10, noegnud_gui_get_fontheight()*1+4*2,
							 1.0,1.0,1.0,
							 1.0,
							 0);
    noegnud_guiwidget_positionbar_container=
	(noegnud_gui_twidget *)noegnud_gui_create_widget(
							 noegnud_guiwidget_positionbar,
							 0,0,
							 10+noegnud_gui_get_fontwidth()*MAX_MAP_X+10, noegnud_gui_get_fontheight()*1+4*2,
							 0);
    noegnud_end_menu((winid)noegnud_guiwidget_positionbar,"Position Bar");
    noegnud_guiwidget_positionbar->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_null;
    noegnud_guiwidget_positionbar->event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_null;
    noegnud_guiwidget_positionbar->theme=noegnud_gui_default_widgettheme;
    ((noegnud_gui_twindow *)noegnud_guiwidget_positionbar)->refreshtitlebar=FALSE;
#endif

    noegnud_guiwidget_minimap=(noegnud_gui_twidget *)noegnud_gui_create_window(
									       noegnud_guiwidget_desktop,
									       noegnud_options_screenwidth->value-(MAX_MAP_X*8+10+8),
									       (noegnud_options_screenheight->value-(MAX_MAP_Y*8+10+16))-noegnud_guiwidget_status->height,
									       MAX_MAP_X*8+10, MAX_MAP_Y*8+10,
									       1.0,1.0,1.0,1.0,
									       0);
    noegnud_end_menu((winid)noegnud_guiwidget_minimap,"Mini Map");
    noegnud_guiwidget_minimap->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_null;
    noegnud_guiwidget_minimap->event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_null;
    noegnud_guiwidget_minimap->theme=noegnud_gui_default_minimap_widgettheme;
    ((noegnud_gui_twindow *)noegnud_guiwidget_minimap)->refreshtitlebar=FALSE;


    //((noegnud_gui_twindow *)noegnud_guiwidget_minimap)->image_tiled=0;
    //((noegnud_gui_twindow *)noegnud_guiwidget_minimap)->image=noegnud_glfuncs_loadimage("gui/minimap.png",TRUE,FALSE,FALSE);
    //((noegnud_gui_twindow *)noegnud_guiwidget_minimap)->localimage=TRUE;

    noegnud_guiwidget_options=(noegnud_gui_twidget *)noegnud_gui_create_button(
									       noegnud_guiwidget_desktop,
									       noegnud_options_screenwidth->value-(strlen(" Options ")*noegnud_gui_get_fontwidth() + 8 + 4),
									       4,
									       strlen(" Options ")*noegnud_gui_get_fontwidth() + 8,
									       noegnud_gui_get_fontheight() + 8,
									       1.0,1.0,1.0,1.0,
									       0.0,0.0,0.0,
									       " Options ",
									       noegnud_options_keys_options->value);

}


void noegnud_init_nhwidows_done() {
    puts("=========================  noeGNUd Actual Exit  ==========================");
    noegnud_fonts_cleanup();
}

void noegnud_init_nhwindows (int *argcp, char **argv) {

    int c;

    if (iflags.window_inited) return;
#ifdef NOEGNUDDEBUG
    if (argcp) noegnud_debug_init();
#endif

    if (argcp) atexit(noegnud_init_nhwidows_done);

    if (argcp) puts("=========================  noeGNUd Entry  ==========================");

    WIN_INVEN=0;

    for (c=0;c<NOEGNUD_CONSOLEMESSAGES_MAX;c++) {
	noegnud_consolemessages[c]=noegnud_mem_malloc(1);
	*noegnud_consolemessages[c]=0;
    }
    noegnud_options_init();

    if (!noegnud_graphics_system_ready)
	noegnud_internal_init_graphics_system ();

    noegnud_image_dungeon=noegnud_glfuncs_loadimage("images/dungeon.png",0,1,0);
    noegnud_image_tilenotfound=noegnud_glfuncs_loadimage("images/tilenotfound.png",0,1,0);


    iflags.window_inited=1;

    noegnud_common_flush_events();
}

int noegnud_player_selection_alignment_check(int anychange) {
    int amount, which, sel;
    amount=which=0;
    while (which<ROLE_ALIGNS) {
	if (ok_align(flags.initrole,flags.initrace,flags.initgend,which)) {
	    sel=which;
	    amount++;
	}
	which++;
    }
    if (amount==1&&flags.initalign!=sel) {
	flags.initalign=sel;
	return 1;
    }
    return (anychange&&amount==1);
}

int noegnud_player_selection_alignment() {
    winid window;
    anything identifier;
    menu_item *menu_list;
    int chosen;
    char *rolename;

    menu_list=0;
    window=noegnud_create_nhwindow(NHW_MENU);
    noegnud_start_menu(window);
    rolename=noegnud_create_rolename();
    identifier.a_int=0;
    noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, rolename, 0);
    noegnud_mem_free(rolename);
    chosen=0;
    while (chosen<ROLE_ALIGNS) {
	identifier.a_int=chosen+1;
	if (ok_align(flags.initrole,flags.initrace,flags.initgend,chosen)) {
	    noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, aligns[chosen].adj, chosen==flags.initalign);
	}
	chosen++;
    }
    identifier.a_int=-1; noegnud_add_menu ( window, NO_GLYPH, &identifier, '*', 0, 0, "Random", 0);
    noegnud_end_menu(window,"Alignment ?");
    if (noegnud_select_menu (window, PICK_ONE, &menu_list)==1) {
	chosen=menu_list->item.a_int;
	if (chosen==-1) {
	    chosen=pick_align(flags.initrole,flags.initrace,flags.initgend,PICK_RANDOM);
	} else {
	    chosen--;
	}
	flags.initalign=chosen;
	free(menu_list);
	noegnud_destroy_nhwindow(window);
	return 1;
    };
    flags.initalign=-1;
    noegnud_destroy_nhwindow(window);
    return 0;
}

int noegnud_player_selection_gender_check(int anychange) {
    int amount, which, sel;
    amount=which=0;
    while (which<ROLE_GENDERS) {
	if (ok_gend(flags.initrole,flags.initrole,which,flags.initalign)) {
	    sel=which;
	    amount++;
	}
	which++;
    }
    if (amount==1&&flags.initgend!=sel) {
	flags.initgend=sel;
	return 1;
    }
    return (anychange&&amount==1);
}

int noegnud_player_selection_gender() {
    winid window;
    anything identifier;
    menu_item *menu_list;
    int chosen;
    char *rolename;

    menu_list=0;
    window=noegnud_create_nhwindow(NHW_MENU);
    noegnud_start_menu(window);
    rolename=noegnud_create_rolename();
    identifier.a_int=0;
    noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, rolename, 0);
    noegnud_mem_free(rolename);
    chosen=0;
    while (chosen<ROLE_GENDERS) {
	identifier.a_int=chosen+1;
	if (ok_gend(flags.initrole,flags.initrole,chosen,flags.initalign))
	    noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, genders[chosen].adj, chosen==flags.initgend);
	chosen++;
    }
    identifier.a_int=-1; noegnud_add_menu ( window, NO_GLYPH, &identifier, '*', 0, 0, "Random", 0);
    noegnud_end_menu(window,"Gender ?");
    if (noegnud_select_menu (window, PICK_ONE, &menu_list)==1) {
	chosen=menu_list->item.a_int;
	if (chosen==-1) {
	    chosen=pick_gend(flags.initrole,flags.initrace,flags.initalign,PICK_RANDOM);
	} else {
	    chosen--;
	}
	flags.initgend=chosen;
	free(menu_list);
	noegnud_destroy_nhwindow(window);
	return 1;
    };
    flags.initgend=-1;
    noegnud_destroy_nhwindow(window);
    return 0;
}

int noegnud_player_selection_race_check(int anychange) {
    int amount, which, sel;
    amount=0;
    for (which=0;races[which].noun;which++) {
	if (ok_race(flags.initrole,which,flags.initgend,flags.initalign)) {
	    sel=which;
	    amount++;
	}
    }
    if (amount==1&&flags.initrace!=sel) {
	flags.initrace=sel;
	return 1;
    }
    return (anychange&&amount==1);
}

int noegnud_player_selection_race() {
    winid window;
    anything identifier;
    menu_item *menu_list;
    int chosen;
    char *rolename;

    menu_list=0;
    window=noegnud_create_nhwindow(NHW_MENU);
    noegnud_start_menu(window);
    rolename=noegnud_create_rolename();
    identifier.a_int=0;
    noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, rolename, 0);
    noegnud_mem_free(rolename);
    for (chosen=0;races[chosen].noun;chosen++) {
	identifier.a_int=chosen+1;
	if (ok_race(flags.initrole,chosen,flags.initgend,flags.initalign))
	    noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, races[chosen].noun, chosen==flags.initrace);
    }
    identifier.a_int=-1; noegnud_add_menu ( window, NO_GLYPH, &identifier, '*', 0, 0, "Random", 0);

    noegnud_end_menu(window,"Race ?");
    if (noegnud_select_menu (window, PICK_ONE, &menu_list)==1) {
	chosen=menu_list->item.a_int;
	if (chosen==-1) {
	    chosen=pick_race(flags.initrole,flags.initgend,flags.initalign,PICK_RANDOM);
	} else {
	    chosen--;
	}
	free(menu_list);
	flags.initrace=chosen;
	noegnud_destroy_nhwindow(window);
	return 1;
    };
    flags.initrace=-1;
    noegnud_destroy_nhwindow(window);
    return 0;
}

int noegnud_player_selection_role_check(int anychange) {
    int amount, which, sel;
    amount=0;
    for (which=0;roles[which].name.m||roles[which].name.f;which++) {
	if (ok_role(which,flags.initrace,flags.initgend,flags.initalign)) {
	    sel=which;
	    amount++;
	}
    }
    if (amount==1&&flags.initrole!=sel) {
	flags.initrole=sel;
	return 1;
    }
    return (anychange&&amount==1);
}

int noegnud_player_selection_role() {
    winid window;
    anything identifier;
    menu_item *menu_list;
    int chosen;
    char *rolename;

    menu_list=0;
    window=noegnud_create_nhwindow(NHW_MENU);
    noegnud_start_menu(window);
    rolename=noegnud_create_rolename();
    identifier.a_int=0;
    noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, rolename, 0);
    noegnud_mem_free(rolename);
    for (chosen=0;roles[chosen].name.m||roles[chosen].name.f;chosen++) {
	identifier.a_int=chosen+1;
	if (ok_role(chosen,flags.initrace,flags.initgend,flags.initalign)) {
	    if (flags.initgend==1&&roles[chosen].name.f) {
		noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, roles[chosen].name.f, chosen==flags.initrole);
	    } else {
	    noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, roles[chosen].name.m, chosen==flags.initrole);
    }
	}
    }
    identifier.a_int=-1; noegnud_add_menu ( window, NO_GLYPH, &identifier, '*', 0, 0, "Random", 0);
    noegnud_end_menu(window,"Role ?");
    if (noegnud_select_menu (window, PICK_ONE, &menu_list)==1) {
	chosen=menu_list->item.a_int;
	if (chosen==-1) {
	    chosen=pick_role(flags.initrace,flags.initgend,flags.initalign,PICK_RANDOM);
	} else {
	    chosen--;
	}
	free(menu_list);
	flags.initrole=chosen;
	noegnud_destroy_nhwindow(window);
	return 1;
    };
    flags.initrole=-1;
    noegnud_destroy_nhwindow(window);
    return 0;
}

int noegnud_player_selection_checks() {
    int changes;
    changes=noegnud_player_selection_role_check(0) ||
	    noegnud_player_selection_race_check(0) ||
	    noegnud_player_selection_gender_check(0) ||
	    noegnud_player_selection_alignment_check(0);
    if (changes) noegnud_player_selection_checks(0);
    return changes;
}

void noegnud_player_selection (void)
{
    int selected;
    char selection;

    int prev_break;

    winid window;
    anything identifier;
    menu_item *menu_list;
    int chosen;
    char *rolename;

    /* avoid asking if unnecessary */
    if ((flags.initrole ==ROLE_RANDOM)||(flags.randomall&&(flags.initrole ==ROLE_NONE))) flags.initrole =pick_role(flags.initrace,flags.initgend,flags.initalign,PICK_RANDOM);
    if ((flags.initrace ==ROLE_RANDOM)||(flags.randomall&&(flags.initrace ==ROLE_NONE))) flags.initrace =pick_race(flags.initrole,flags.initgend,flags.initalign,PICK_RANDOM);
    if ((flags.initgend ==ROLE_RANDOM)||(flags.randomall&&(flags.initgend ==ROLE_NONE))) flags.initgend =pick_gend(flags.initrole,flags.initrace,flags.initalign,PICK_RANDOM);
    if ((flags.initalign==ROLE_RANDOM)||(flags.randomall&&(flags.initalign==ROLE_NONE))) flags.initalign=pick_align(flags.initrole,flags.initrace,flags.initgend ,PICK_RANDOM);

    noegnud_player_selection_checks();

    selection=noegnud_yn_function ("Would you like me to auto-generate a character for you ? (Yes/No/Custom/Quit)", "yncq", 'n');
    if (selection=='q') abend("Game startup manually aborted.");

    if (selection=='n') {

	selected=0;
	while (1) {
	    noegnud_player_selection_checks();
	    prev_break=0;
	    if ((!prev_break&&noegnud_player_selection_role_check(1))||!(prev_break=(!noegnud_player_selection_role()))) {
		noegnud_player_selection_checks();
		prev_break=0;
		while ((!prev_break&&noegnud_player_selection_race_check(1))||!(prev_break=(!noegnud_player_selection_race()))) {
		    noegnud_player_selection_checks();
		    prev_break=0;
		    while ((!prev_break&&noegnud_player_selection_gender_check(1))||!(prev_break=(!noegnud_player_selection_gender()))) {
			noegnud_player_selection_checks();
			prev_break=0;
			if (noegnud_player_selection_alignment_check(1)||!(prev_break=(!noegnud_player_selection_alignment()))) {
			    selected=1;
			}
			if (selected) break;
		    }
		    if (selected) break;
		}
		if (selected) break;
	    } else {
		abend("Character selection aborted.");
	    }
	};

    } else if (selection=='c') {
	selected=1;
	while (selected) {
	    noegnud_player_selection_checks();
	    menu_list=0;
	    window=noegnud_create_nhwindow(NHW_MENU);
	    noegnud_start_menu(window);
	    rolename=noegnud_create_rolename();
	    identifier.a_int=0;
	    noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, rolename, 0);
	    noegnud_mem_free(rolename);
	    identifier.a_int='r';
	    if (!noegnud_player_selection_role_check(1)) noegnud_add_menu ( window, NO_GLYPH, &identifier, 'r', 0, 0, "role", 0);
	    identifier.a_int='c';
	    if (!noegnud_player_selection_race_check(1)) noegnud_add_menu ( window, NO_GLYPH, &identifier, 'c', 0, 0, "race", 0);
	    identifier.a_int='g';
	    if (!noegnud_player_selection_gender_check(1)) noegnud_add_menu ( window, NO_GLYPH, &identifier, 'g', 0, 0, "gender", 0);
	    identifier.a_int='a';
	    if (!noegnud_player_selection_alignment_check(1)) noegnud_add_menu ( window, NO_GLYPH, &identifier, 'a', 0, 0, "alignment", 0);

	    noegnud_end_menu(window,"Change which character attribute ?");

	    if ((selected=noegnud_select_menu (window, PICK_ONE, &menu_list))==1) {
		chosen=menu_list->item.a_int;
		switch (chosen) {
  		    case 'r': noegnud_player_selection_role(); break;
  		    case 'c': noegnud_player_selection_race(); break;
  		    case 'g': noegnud_player_selection_gender(); break;
  		    case 'a': noegnud_player_selection_alignment(); break;
		}
		free(menu_list);
	    };
	    noegnud_destroy_nhwindow(window);
	}
    }

    /* hmmm, did we forget anything ? if so, then randomize it */
    if (flags.initrole ==ROLE_NONE) flags.initrole =pick_role(flags.initrace,flags.initgend,flags.initalign,PICK_RANDOM);
    if (flags.initrace ==ROLE_NONE) flags.initrace =pick_race(flags.initrole,flags.initgend,flags.initalign,PICK_RANDOM);
    if (flags.initgend ==ROLE_NONE) flags.initgend =pick_align(flags.initrole,flags.initrace,flags.initalign,PICK_RANDOM);
    if (flags.initalign==ROLE_NONE) flags.initalign=pick_gend(flags.initrole,flags.initrace,flags.initgend ,PICK_RANDOM);

};

#ifdef __WIN32__
int noegnud_getlin_askname=0;
#endif

void noegnud_askname (void) {
#ifdef __WIN32__
    strcpy(plname,getenv("USERNAME"));
    noegnud_getlin_askname=1;
#endif
    noegnud_getlin("What is your name?",plname);
};

void noegnud_get_nh_event (void) {};

void noegnud_creditscreen();
void noegnud_exit_nhwindows (const char *str) {
    int c;

    if (str) noegnud_raw_print (str);

    for (c=0;c<NOEGNUD_CONSOLEMESSAGES_MAX;c++) {
	noegnud_mem_free(noegnud_consolemessages[c]);
    }

#ifndef NOEGNUDDEBUG
    if (noegnud_options_gui_window_credits_display->value) noegnud_creditscreen();
#endif

    noegnud_fonts_deletefont(&noegnud_font_map);
    noegnud_fonts_deletefont(&noegnud_font_minimap);

    noegnud_tilesets_unloadtileset(noegnud_activetileset);

    noegnud_guiwidget_desktop->kill(noegnud_guiwidget_desktop);

    noegnud_graphics_system_ready=0;

    SDL_Quit ();
    puts("=========================  noeGNUd Official Exit  ==========================");
}

void
noegnud_suspend_nhwindows (const char *str)
{
};
void
noegnud_resume_nhwindows (void)
{
};

winid noegnud_create_nhwindow (int type) {

    noegnud_gui_twindow* window;

    switch (type)
	{
	case NHW_MESSAGE:
	    return (winid)(noegnud_guiwidget_console);
	    break;
	case NHW_STATUS:
	    return (winid)noegnud_guiwidget_status;
	    break;
	case NHW_MAP:
	    return (winid)noegnud_guiwidget_minimap;
	    break;
	case NHW_MENU:
	    window=noegnud_gui_create_window(
						    //		    NULL,
						    noegnud_guiwidget_desktop,
						    100,100,
						    0,0,
						    //		    0.0,0.1,0.4,0.7,
						    1.0,1.0,1.0,1.0,0);
	    window->widget.theme=noegnud_gui_default_widgettheme;
	    ((noegnud_gui_twidget *)window)->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_widget;
	    return (winid)window;
	    break;
	case NHW_TEXT:
	    window= noegnud_gui_create_window(
					      //		    NULL,
					      noegnud_guiwidget_desktop,
					      100,100,
					      0,0,
					      1.0,1.0,1.0,1.0,
					      //		    0.0,0.1,0.4,0.7,
					      0);
	    window->widget.theme=noegnud_gui_default_widgettheme;

	    /*
	      if (noegnud_toptenwindow_pending) {
	      window->image_tiled=0;
	      window->image=noegnud_glfuncs_loadimage("gui/highscore.png");
	      }
	    */
	    return (winid)window;
	}

    return 0;
};

void noegnud_clear_nhwindow (winid window) {
    int saveperm;
    if (window == WIN_MESSAGE) {
    } else
	if (window == WIN_STATUS) {
	} else
	    if (window == WIN_MAP) {
		clearmap ();
    if (WIN_INVEN) {
        saveperm=flags.perm_invent;
	flags.perm_invent=TRUE;
	display_inventory(NULL,FALSE);
	flags.perm_invent=saveperm;
    }
	    } else
		if (window == WIN_INVEN) {
        saveperm=flags.perm_invent;
	flags.perm_invent=TRUE;
	display_inventory(NULL,FALSE);
	flags.perm_invent=saveperm;
		} else
		    {
		    }
};

void  noegnud_display_nhwindow (winid window, boolean blocking) {
    int busy;
    unsigned char ch;

    process_checkrecord ();


    if (((noegnud_gui_twidget *)window)->type==NOEGNUD_GUI_WINDOW&&window!=WIN_MAP&&window!=WIN_STATUS) {
	((noegnud_gui_twidget *)window)->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_window;
    }


    if (window == WIN_MESSAGE) {
    } else
	if (window == WIN_STATUS) {
	} else
	    if (window == WIN_MAP) {
		if (blocking) {
		    noegnud_nhgetch();
		}
	    } else
		if (window == WIN_INVEN) {
		} else
		    {
			if (((noegnud_gui_twindow *)window)->rip) {
			    noegnud_rip_outrip_finalize(window);
			    noegnud_toptenwindow_pending=TRUE;
			} else
			    if (noegnud_toptenwindow_pending) {

			    } else {
				noegnud_gui_add_actionbutton((noegnud_gui_twindow *)window,"Ok",'\n');
			    }
			noegnud_gui_active=(noegnud_gui_twidget *)window;
			busy=TRUE;
			while (busy) {
			    ch=noegnud_nhgetch();
			    if (ch==MENU_FIRST_PAGE) {
				((noegnud_gui_twindow *)window)->widget.offset_y=0;
			    } else if (ch==MENU_LAST_PAGE) {
				((noegnud_gui_twindow *)window)->widget.offset_y=((noegnud_gui_twindow *)window)->widget.vscroll;
			    } else if (ch==MENU_NEXT_PAGE) {
				((noegnud_gui_twindow *)window)->widget.offset_y+=((noegnud_gui_twindow *)window)->widget.height-20;
				if (((noegnud_gui_twindow *)window)->widget.offset_y > ((noegnud_gui_twindow *)window)->widget.vscroll) ((noegnud_gui_twindow *)window)->widget.offset_y=((noegnud_gui_twindow *)window)->widget.vscroll;
			    } else if (ch==MENU_PREVIOUS_PAGE) {
				((noegnud_gui_twindow *)window)->widget.offset_y-=((noegnud_gui_twindow *)window)->widget.height-20;
				if (((noegnud_gui_twindow *)window)->widget.offset_y<0) ((noegnud_gui_twindow *)window)->widget.offset_y=0;
			    } else busy=FALSE;
			}
			noegnud_gui_active=NULL;

		    }
};

void noegnud_destroy_nhwindow (winid window) {

    if ( window!= (winid)noegnud_guiwidget_console) {

	if ((winid)noegnud_gui_active==window) noegnud_gui_active=NULL;
	((noegnud_gui_twidget *)window)->kill((noegnud_gui_twidget *)window);
    }

};

void noegnud_curs (winid window, int x, int y) {};

void noegnud_putstr (winid window, int attr, const char *str) {
    noegnud_gui_twidget *wdgt_clipping;
    char *str_colour;
    noegnud_optiontype_rgb *rip_text_colour;

    if (window == WIN_MESSAGE) {
	noegnud_raw_print(str);
    } else
	if (window == WIN_STATUS) {
	    if (!noegnud_startup_flag) {
		noegnud_startup_flag=1;
		noegnud_console_delta = -noegnud_console_speed;
		//((noegnud_gui_twindow *)noegnud_guiwidget_console)->image=noegnud_console_image_console;
		((noegnud_gui_twindow *)noegnud_guiwidget_console)->localimage=FALSE;
		noegnud_guiwidget_console->theme=noegnud_gui_default_widgettheme;
		noegnud_glfuncs_unloadimage(noegnud_console_image_splash);
		((noegnud_gui_twindow *)noegnud_guiwidget_console)->movable=TRUE;
	    }
	    str_colour=noegnud_textcolouring_colourise(str);
	    noegnud_gui_inconsole_noegnudtext(noegnud_guiwidget_status_container,str_colour,TRUE,
			      noegnud_options_gui_window_console_text_colour_default->r,
			      noegnud_options_gui_window_console_text_colour_default->g,
			      noegnud_options_gui_window_console_text_colour_default->b);
	    noegnud_mem_free(str_colour);

	} else
	    if (window == WIN_MAP) {
	    } else
		if (window == WIN_INVEN) {
		} else
		    {
			if (((noegnud_gui_twindow *)window)->rip) {
			    rip_text_colour=noegnud_collection_data(noegnud_options,"gui.window.tombstone.text.colour");
			    noegnud_gui_inconsole_noegnudtext(
							      ((noegnud_gui_twidget *)window)->child->nextsibling,
							      str,TRUE,
							      rip_text_colour->r,
							      rip_text_colour->g,
							      rip_text_colour->b),
			    wdgt_clipping=((noegnud_gui_twidget *)window)->child->nextsibling->child; // WOW *YUCK* ;)
			    while (wdgt_clipping) {
				wdgt_clipping->clipto=((noegnud_gui_twidget *)window);
				wdgt_clipping=wdgt_clipping->nextsibling;
			    }
			} else
			    noegnud_gui_nh_putstr((noegnud_gui_twindow *)window,str,
			      noegnud_options_gui_window_console_text_colour_default->r,
			      noegnud_options_gui_window_console_text_colour_default->g,
			      noegnud_options_gui_window_console_text_colour_default->b);
		    }

};

void noegnud_display_file (const char *fname, boolean complain) {
    dlb *f;
    winid window;
    char buffer[BUFSZ];
    char *ch;

    if ((f=dlb_fopen(fname,"r"))) {
	window=noegnud_create_nhwindow(NHW_TEXT);
	while (dlb_fgets(buffer,BUFSZ,f)) {
	    if ((ch=index(buffer,'\n'))) *ch=0;
	    if ((ch=index(buffer,'\r'))) *ch=0;

	    noegnud_putstr(window,0,buffer);
	}

	((noegnud_gui_twindow *)window)->widget.height+=noegnud_gui_get_fontheight()+4;
	noegnud_display_nhwindow(window,0);
	noegnud_destroy_nhwindow(window);

	dlb_fclose(f);
    } else {
	printf("[WARNING] couldn't \"displayfile\" (dlb) : \"%s\"\n",fname);
    }
};

void noegnud_start_menu (winid window) {
    noegnud_gui_kill_widget_relatives((noegnud_gui_twidget *)window);
    ((noegnud_gui_twidget *)window)->width=0;
    ((noegnud_gui_twidget *)window)->height=0;
    ((noegnud_gui_twidget *)window)->vscrollwidget=NULL;
    ((noegnud_gui_twidget *)window)->hscrollwidget=NULL;
    if (((noegnud_gui_twidget *)window)->type==NOEGNUD_GUI_WINDOW) ((noegnud_gui_twindow *)window)->autoresize=TRUE;
    ((noegnud_gui_twidget *)window)->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_null;
    ((noegnud_gui_twidget *)window)->event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_null;
};

void noegnud_add_menu (winid
		       window,
		       int
		       glyph,
		       const anything* identifier,
		       CHAR_P ch, CHAR_P gch, int attr, const
		       char *str, boolean preselected)
{
    char *str_colour;

    str_colour=noegnud_textcolouring_colourise(str);

    if ((*identifier).a_int) {
	if(glyph!=NO_GLYPH) noegnud_tilesetload_image_from(noegnud_activetileset, glyph2tile[glyph] );
	noegnud_gui_nh_addmenu((noegnud_gui_twindow *)window, *identifier, glyph, (char)ch, (char)gch, str_colour, (int)preselected);
    } else {
	noegnud_gui_nh_putstr((noegnud_gui_twindow *)window, str_colour,
			      noegnud_options_gui_window_console_text_colour_default->r,
			      noegnud_options_gui_window_console_text_colour_default->g,
			      noegnud_options_gui_window_console_text_colour_default->b);
    }

    noegnud_mem_free(str_colour);

};


int noegnud_gui_menu_useschar(noegnud_gui_twindow *window, char ch) {
    noegnud_gui_tmenuitem *menuitem;

    menuitem=(noegnud_gui_tmenuitem *)((noegnud_gui_twidget *)window)->child;
    while (menuitem) {
	if ( (menuitem->window.widget.type==NOEGNUD_GUI_MENUITEM) &&
	     (menuitem->ch==ch) ) return 1;
	menuitem=(noegnud_gui_tmenuitem *)menuitem->window.widget.nextsibling;
    }
    return 0;
};

#define noegnud_gui_menu_assignchars_skiptext(skiptext) if (strstr(&menuitem->text[mti],skiptext)==&menuitem->text[mti]) mti+=(sizeof(skiptext)-1);

char noegnud_gui_menucharsavailable[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
void noegnud_gui_menu_assignchars(noegnud_gui_twindow *window) {
  int c;
  int mti;
  noegnud_gui_tmenuitem *menuitem;

  menuitem=(noegnud_gui_tmenuitem *)((noegnud_gui_twidget *)window)->child;
  while (menuitem) {

	if ( (menuitem->window.widget.type==NOEGNUD_GUI_MENUITEM) &&
	     (menuitem->identifier.a_int) &&
	     ((menuitem->ch==0)||(menuitem->ch==' ')) ) {

    if (noegnud_options_key_assignment_traditional->value) {
      c=0;
      while (c<sizeof(noegnud_gui_menucharsavailable)&&noegnud_gui_menu_useschar(window,noegnud_gui_menucharsavailable[c])) c++;
      if (c>=sizeof(noegnud_gui_menucharsavailable)) {
        menuitem->ch='#';
      } else {
        menuitem->ch=noegnud_gui_menucharsavailable[c];
      }
    } else {
      mti=0;
      if ((menuitem->text[mti]>='2')<=(menuitem->text[mti]<='9')) mti+=((index(&menuitem->text[mti],' ')-&menuitem->text[mti])+1);

      noegnud_gui_menu_assignchars_skiptext("a ")
      noegnud_gui_menu_assignchars_skiptext("an ")

      noegnud_gui_menu_assignchars_skiptext("blessed ")
      noegnud_gui_menu_assignchars_skiptext("cursed ")
      noegnud_gui_menu_assignchars_skiptext("uncursed ")

      if ((menuitem->text[mti]=='+')||(menuitem->text[mti]=='-')) mti+=((index(&menuitem->text[mti],' ')-&menuitem->text[mti])+1);

      noegnud_gui_menu_assignchars_skiptext("scroll of ")
      noegnud_gui_menu_assignchars_skiptext("spellbook of ")
      noegnud_gui_menu_assignchars_skiptext("potion of ")
      noegnud_gui_menu_assignchars_skiptext("ring of ")
      noegnud_gui_menu_assignchars_skiptext("wand of ")

      while (menuitem->text[mti]==' ') {mti++;}

      if ((menuitem->text[0])&&(!noegnud_gui_menu_useschar(window,tolower(menuitem->text[mti])))) {
        menuitem->ch=tolower(menuitem->text[mti]);
      } else
		    if ((menuitem->text[0])&&(!noegnud_gui_menu_useschar(window,toupper(menuitem->text[mti])))) {
          menuitem->ch=toupper(menuitem->text[mti]);
		    } else {
          c=0;
          while (c<sizeof(noegnud_gui_menucharsavailable)&&noegnud_gui_menu_useschar(window,noegnud_gui_menucharsavailable[c])) c++;
          if (c>=sizeof(noegnud_gui_menucharsavailable)) {
            menuitem->ch='#';
          } else {
            menuitem->ch=noegnud_gui_menucharsavailable[c];
          }
		    }
    }
    if (menuitem->ch) noegnud_gui_nh_addmenu_menuitem_addaccelerator((noegnud_gui_twidget *)menuitem,menuitem->ch);
  }
  menuitem=(noegnud_gui_tmenuitem *)menuitem->window.widget.nextsibling;
  }
}

void noegnud_end_menu (winid window, const char *prompt) {
    noegnud_gui_twindow *new_win;
    noegnud_gui_ttext   *new_text;
    char *title;
    int text_width,text_height;
    char tmpstring[1024];
    char *stringtoken;

    noegnud_optiontype_string *window_x;
    noegnud_optiontype_string *window_y;

    noegnud_optiontype_string *window_rgba;

    noegnud_gui_menu_assignchars((noegnud_gui_twindow *)window);

    if (prompt) {
	title=(char *)prompt;
    } else {
	title=noegnud_mem_malloc(1024);
	if (window==WIN_INVEN) {
	    strcpy(title,"Inventory");
	} else {
	    strcpy(title,"noeGNUd");
	}
    }


    new_text=noegnud_gui_create_text(
				     (noegnud_gui_twidget *)(new_win=noegnud_gui_create_window(
								       (noegnud_gui_twidget *)window,
								       0,-25,
								       ((noegnud_gui_twidget *)window)->width-20,
								       20,
								       1.0,1.0,1.0,1.0,
								       0)),
				     NOEGNUD_GUI_BORDER_SIZE,NOEGNUD_GUI_BORDER_SIZE/2,
				     ((noegnud_gui_twidget *)window)->width-20,
				     20,
				     1.0,1.0,1.0,
				     title);

    text_width=noegnud_width_dynamic(new_text->font,title);
    text_height=new_text->font->height;

    new_text->widget.width=text_width;
    new_text->widget.height=text_height;

    new_win->widget.x=0;
    new_win->widget.width=text_width+2*NOEGNUD_GUI_BORDER_SIZE;
    new_win->widget.height=text_height+NOEGNUD_GUI_BORDER_SIZE;
    new_win->widget.y=-new_win->widget.height;
    if (((noegnud_gui_twidget *)window)->width<new_win->widget.width+NOEGNUD_GUI_BORDER_SIZE) ((noegnud_gui_twidget *)window)->width=new_win->widget.width+NOEGNUD_GUI_BORDER_SIZE;
    new_text->dynamiccharwidth=text_width;


    new_win->widget.type=NOEGNUD_GUI_TITLEBAR;

    new_win->widget.clipto=new_win->widget.parent->clipto;
    new_win->movable=1;
    new_win->tomove=(noegnud_gui_twidget *)window;
    new_win->widget.theme=noegnud_gui_default_title_active_widgettheme;
    ((noegnud_gui_twindow *)window)->movable=0;
    ((noegnud_gui_twindow *)window)->titlebar=(noegnud_gui_twidget *)new_win;

    sprintf(tmpstring,"gui.window.implicit.%s.position.%d.%d.x",title,noegnud_options_screenwidth->value,noegnud_options_screenheight->value);
    window_x=noegnud_collection_data(noegnud_options,tmpstring);
    sprintf(tmpstring,"gui.window.implicit.%s.position.%d.%d.y",title,noegnud_options_screenwidth->value,noegnud_options_screenheight->value);
    if (window_x&&(window_y=noegnud_collection_data(noegnud_options,tmpstring))) {
	((noegnud_gui_twidget *)window)->x=atoi(window_x->value);
	((noegnud_gui_twidget *)window)->y=atoi(window_y->value);
	((noegnud_gui_twidget *)window)->autocentre=FALSE;
    }

    sprintf(tmpstring,"gui.window.implicit.%s.colour",title);
    window_rgba=noegnud_collection_data(noegnud_options,tmpstring);
    if (window_rgba) {
    strcpy(tmpstring,window_rgba->value);
    stringtoken=strtok(tmpstring,",");
    if (stringtoken) {
	((noegnud_gui_twidget *)window)->r=atof(stringtoken);
	stringtoken=strtok(NULL,",");
	if (stringtoken) {
	    ((noegnud_gui_twidget *)window)->g=atof(stringtoken);
	    stringtoken=strtok(NULL,",");
	    if (stringtoken) {
		((noegnud_gui_twidget *)window)->b=atof(stringtoken);
		stringtoken=strtok(NULL,",");
		if (stringtoken) {
		    ((noegnud_gui_twidget *)window)->a=atof(stringtoken);
		}
	    }
	}
    }
    }
    if (!prompt) noegnud_mem_free(title);
    /*
    if (window==WIN_INVEN) {
	noegnud_inventory_open=1;
	((noegnud_gui_twidget *)window)->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_window;

    }
    */
};

extern int noegnud_gui_currentmenuselectionmethod;
int noegnud_select_menu (winid window, int how, menu_item ** menu_list) {
    char ch;
    int busy;
    menu_item *mi;

    char stringbuffer[2048];

    noegnud_gui_tmenuitem *menuitem;

    int toreturn;

    noegnud_gui_twidget *old_noegnud_gui_active;

    toreturn=0;
    busy=1;


    noegnud_gui_menuitem_throwreturn=FALSE;

    ((noegnud_gui_twidget *)window)->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_window;
    ((noegnud_gui_twidget *)window)->event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_window;

    if (!(how==PICK_NONE&&flags.perm_invent&&window==WIN_INVEN))
	noegnud_gui_add_actionbutton((noegnud_gui_twindow *)window,"Ok",'\n');

    if (how==PICK_ANY||how==PICK_ONE) {
	noegnud_gui_add_actionbutton((noegnud_gui_twindow *)window,"Cancel",'\033');

    }

    if (((noegnud_gui_twindow *)window)->rip) {
	noegnud_rip_outrip_finalize(window);
    }

    old_noegnud_gui_active=noegnud_gui_active;
    noegnud_gui_active=(noegnud_gui_twidget *)window;
    noegnud_gui_currentmenuselectionmethod=how;

    if (how==PICK_NONE) {
	    while (busy) {
		if ((!flags.perm_invent)||(winid)window!=WIN_INVEN) {
		    ch=noegnud_nhgetch();
		}
		if (ch==MENU_FIRST_PAGE) {
		    ((noegnud_gui_twindow *)window)->widget.offset_y=0;
		} else if (ch==MENU_LAST_PAGE) {
		    ((noegnud_gui_twindow *)window)->widget.offset_y=((noegnud_gui_twindow *)window)->widget.vscroll;
		} else if (ch==MENU_NEXT_PAGE) {
		    ((noegnud_gui_twindow *)window)->widget.offset_y+=((noegnud_gui_twindow *)window)->widget.height-20;
		    if (((noegnud_gui_twindow *)window)->widget.offset_y > ((noegnud_gui_twindow *)window)->widget.vscroll) ((noegnud_gui_twindow *)window)->widget.offset_y=((noegnud_gui_twindow *)window)->widget.vscroll;
		} else if (ch==MENU_PREVIOUS_PAGE) {
		    ((noegnud_gui_twindow *)window)->widget.offset_y-=((noegnud_gui_twindow *)window)->widget.height-20;
		    if (((noegnud_gui_twindow *)window)->widget.offset_y<0) ((noegnud_gui_twindow *)window)->widget.offset_y=0;
		} else busy=FALSE;
	    }
    } else
	if (how==PICK_ONE) {
	    noegnud_gui_menuitem_throwreturn=TRUE;
	    while (busy) {
		ch=noegnud_nhgetch();
		if (ch=='\033') {
		    break;
		}
		if (ch==MENU_FIRST_PAGE) {
		    ((noegnud_gui_twindow *)window)->widget.offset_y=0;
		} else if (ch==MENU_LAST_PAGE) {
		    ((noegnud_gui_twindow *)window)->widget.offset_y=((noegnud_gui_twindow *)window)->widget.vscroll;
		} else if (ch==MENU_NEXT_PAGE) {
		    ((noegnud_gui_twindow *)window)->widget.offset_y+=((noegnud_gui_twindow *)window)->widget.height-20;
		    if (((noegnud_gui_twindow *)window)->widget.offset_y > ((noegnud_gui_twindow *)window)->widget.vscroll) ((noegnud_gui_twindow *)window)->widget.offset_y=((noegnud_gui_twindow *)window)->widget.vscroll;
		} else if (ch==MENU_PREVIOUS_PAGE) {
		    ((noegnud_gui_twindow *)window)->widget.offset_y-=((noegnud_gui_twindow *)window)->widget.height-20;
		    if (((noegnud_gui_twindow *)window)->widget.offset_y<0) ((noegnud_gui_twindow *)window)->widget.offset_y=0;
		} else if (ch=='\n'||ch==' ') {
		    menuitem=(noegnud_gui_tmenuitem *)((noegnud_gui_twidget *)window)->child;
		    while (menuitem) {
			if ( (menuitem->window.widget.type==NOEGNUD_GUI_MENUITEM) &&
			     (menuitem->selected) ) toreturn++;
			menuitem=(noegnud_gui_tmenuitem *)menuitem->window.widget.nextsibling;
		    }
		    if (toreturn) {
			*menu_list=noegnud_mem_malloc(toreturn*sizeof(menu_item));
			mi=*menu_list;
			menuitem=(noegnud_gui_tmenuitem *)((noegnud_gui_twidget *)window)->child;
			while (menuitem) {
			    if ( (menuitem->window.widget.type==NOEGNUD_GUI_MENUITEM) &&
				 (menuitem->selected) ) {
				mi->item=menuitem->identifier;
				mi->count=-1;
				mi++;
			    }
			    menuitem=(noegnud_gui_tmenuitem *)menuitem->window.widget.nextsibling;
			}
		    }
		    break;
		}
		menuitem=(noegnud_gui_tmenuitem *)((noegnud_gui_twidget *)window)->child;
		while (menuitem) {
		    if (menuitem->window.widget.type==NOEGNUD_GUI_MENUITEM) {
			if ( ( (ch!='#') &&
 			       (menuitem->ch==ch) ) ||
			     ( (ch=='#') &&
			       (menuitem->identifier.a_int==noegnud_gui_menuselect_identifier.a_int) ) ) {
			    /* malloc because the variant free's it sometimes */
			    *menu_list=malloc(sizeof(menu_item));
			    mi=*menu_list;
			    mi->item=menuitem->identifier;
			    mi->count=-1;
			    toreturn = 1;
			    break;
			};
		    }
		    menuitem=(noegnud_gui_tmenuitem *)menuitem->window.widget.nextsibling;
		}
		if (toreturn) break;
	    }
	} else
	    if (how==PICK_ANY) {
		while (busy) {
		    ch=map_menu_cmd(noegnud_nhgetch());

		    if (ch=='\033') break;
		    if ((((noegnud_gui_twindow *)window)->widget.vscroll)&&((ch==MENU_FIRST_PAGE)||(ch==MENU_LAST_PAGE)||(ch==MENU_NEXT_PAGE)||(ch==MENU_PREVIOUS_PAGE)||(ch==MENU_SELECT_PAGE)||(ch==MENU_UNSELECT_PAGE)||(ch==MENU_INVERT_PAGE))) {
			if (ch==MENU_FIRST_PAGE) {
			    ((noegnud_gui_twindow *)window)->widget.offset_y=0;
			} else if (ch==MENU_LAST_PAGE) {
			    ((noegnud_gui_twindow *)window)->widget.offset_y=((noegnud_gui_twindow *)window)->widget.vscroll;
			} else if (ch==MENU_NEXT_PAGE) {
			    ((noegnud_gui_twindow *)window)->widget.offset_y+=((noegnud_gui_twindow *)window)->widget.height-20;
			    if (((noegnud_gui_twindow *)window)->widget.offset_y > ((noegnud_gui_twindow *)window)->widget.vscroll) ((noegnud_gui_twindow *)window)->widget.offset_y=((noegnud_gui_twindow *)window)->widget.vscroll;
			} else if (ch==MENU_PREVIOUS_PAGE) {
			    ((noegnud_gui_twindow *)window)->widget.offset_y-=((noegnud_gui_twindow *)window)->widget.height-20;
			    if (((noegnud_gui_twindow *)window)->widget.offset_y<0) ((noegnud_gui_twindow *)window)->widget.offset_y=0;
			} else if (ch==MENU_SELECT_PAGE) {
			    menuitem=(noegnud_gui_tmenuitem *)((noegnud_gui_twidget *)window)->child;
			    while (menuitem) {
				if (menuitem->window.widget.type==NOEGNUD_GUI_MENUITEM&&menuitem->window.widget.y-((noegnud_gui_twindow *)window)->widget.offset_y>0&&(menuitem->window.widget.y+menuitem->window.widget.height)-((noegnud_gui_twindow *)window)->widget.offset_y<((noegnud_gui_twindow *)window)->widget.height) menuitem->selected=TRUE;
				menuitem=(noegnud_gui_tmenuitem *)menuitem->window.widget.nextsibling;
			    }
			} else if (ch==MENU_UNSELECT_PAGE) {
			    menuitem=(noegnud_gui_tmenuitem *)((noegnud_gui_twidget *)window)->child;
			    while (menuitem) {
				if (menuitem->window.widget.type==NOEGNUD_GUI_MENUITEM&&menuitem->window.widget.y-((noegnud_gui_twindow *)window)->widget.offset_y>0&&(menuitem->window.widget.y+menuitem->window.widget.height)-((noegnud_gui_twindow *)window)->widget.offset_y<((noegnud_gui_twindow *)window)->widget.height) menuitem->selected=FALSE;
				menuitem=(noegnud_gui_tmenuitem *)menuitem->window.widget.nextsibling;
			    }
			} else if (ch==MENU_INVERT_PAGE) {
			    menuitem=(noegnud_gui_tmenuitem *)((noegnud_gui_twidget *)window)->child;
			    while (menuitem) {
				if (menuitem->window.widget.type==NOEGNUD_GUI_MENUITEM&&menuitem->window.widget.y-((noegnud_gui_twindow *)window)->widget.offset_y>0&&(menuitem->window.widget.y+menuitem->window.widget.height)-((noegnud_gui_twindow *)window)->widget.offset_y<((noegnud_gui_twindow *)window)->widget.height) menuitem->selected=!menuitem->selected;
				menuitem=(noegnud_gui_tmenuitem *)menuitem->window.widget.nextsibling;
			    }
			}
		    } else if (ch==MENU_SELECT_ALL||ch==MENU_SELECT_PAGE) {
			menuitem=(noegnud_gui_tmenuitem *)((noegnud_gui_twidget *)window)->child;
			while (menuitem) {
			    if (menuitem->window.widget.type==NOEGNUD_GUI_MENUITEM) menuitem->selected=TRUE;
			    menuitem=(noegnud_gui_tmenuitem *)menuitem->window.widget.nextsibling;
			}
		    } else if (ch==MENU_UNSELECT_ALL||ch==MENU_UNSELECT_PAGE) {
			menuitem=(noegnud_gui_tmenuitem *)((noegnud_gui_twidget *)window)->child;
			while (menuitem) {
			    if (menuitem->window.widget.type==NOEGNUD_GUI_MENUITEM) menuitem->selected=FALSE;
			    menuitem=(noegnud_gui_tmenuitem *)menuitem->window.widget.nextsibling;
			}
		    } else if (ch==MENU_INVERT_ALL||ch==MENU_INVERT_PAGE) {
			menuitem=(noegnud_gui_tmenuitem *)((noegnud_gui_twidget *)window)->child;
			while (menuitem) {
			    if (menuitem->window.widget.type==NOEGNUD_GUI_MENUITEM) menuitem->selected=!menuitem->selected;
			    menuitem=(noegnud_gui_tmenuitem *)menuitem->window.widget.nextsibling;
			}
		    } else if (ch==MENU_SEARCH) {
			noegnud_getlin("Search for:",(char *)&stringbuffer);
			if (*stringbuffer) {
			    menuitem=(noegnud_gui_tmenuitem *)((noegnud_gui_twidget *)window)->child;
			    while (menuitem) {
				if (menuitem->window.widget.type==NOEGNUD_GUI_MENUITEM&&strstr(menuitem->text,(char *)&stringbuffer)) menuitem->selected=!menuitem->selected;
				menuitem=(noegnud_gui_tmenuitem *)menuitem->window.widget.nextsibling;
			    }
			}
		    } else if (ch=='\n'||ch==' ') {
			menuitem=(noegnud_gui_tmenuitem *)((noegnud_gui_twidget *)window)->child;
			while (menuitem) {
			    if ( (menuitem->window.widget.type==NOEGNUD_GUI_MENUITEM) &&
				 (menuitem->selected) ) toreturn++;
			    menuitem=(noegnud_gui_tmenuitem *)menuitem->window.widget.nextsibling;
			}

			if (toreturn) {
			    /* malloc because the variant free's it sometimes */
			    *menu_list=malloc(toreturn*sizeof(menu_item));
			    mi=*menu_list;
			    menuitem=(noegnud_gui_tmenuitem *)((noegnud_gui_twidget *)window)->child;
			    while (menuitem) {
				if ( (menuitem->window.widget.type==NOEGNUD_GUI_MENUITEM) &&
				     (menuitem->selected) ) {
				    mi->item=menuitem->identifier;
				    mi->count=-1;
				    mi++;
				}
				menuitem=(noegnud_gui_tmenuitem *)menuitem->window.widget.nextsibling;
			    }
			} else break;
		    }

		    if (toreturn) break;

		    menuitem=(noegnud_gui_tmenuitem *)((noegnud_gui_twidget *)window)->child;
		    while (menuitem) {
			if (menuitem->window.widget.type==NOEGNUD_GUI_MENUITEM) {
			if ( ( (ch!='#') &&
 			       ( (menuitem->ch==ch)||(menuitem->gch==ch) ) ) ||
			     ( (ch=='#') &&
			       (menuitem->identifier.a_int==noegnud_gui_menuselect_identifier.a_int) ) ) {
			    				menuitem->selected=!menuitem->selected;
			    };
			}
			menuitem=(noegnud_gui_tmenuitem *)menuitem->window.widget.nextsibling;
		    }
		}
	    }


    /*
    if (window==WIN_INVEN) {
	//	((noegnud_gui_twidget *)window)->kill((noegnud_gui_twidget *)window);
	//	WIN_INVEN=noegnud_create_nhwindow(NHW_MENU);
	noegnud_gui_kill_widget_relatives((noegnud_gui_twidget *)window);
	((noegnud_gui_twidget *)window)->vscrollwidget=NULL;
	((noegnud_gui_twidget *)window)->hscrollwidget=NULL;
	((noegnud_gui_twindow *)window)->autoresize=TRUE;
	((noegnud_gui_twidget *)window)->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_null;
	noegnud_inventory_open=0;
    }

    */

    //    if ((!flags.perm_invent)||(winid)window!=WIN_INVEN) {
    if ((flags.perm_invent)&&(winid)window==WIN_INVEN) {
	if (how!=PICK_NONE) display_inventory(NULL,FALSE);
    } else {
	((noegnud_gui_twidget *)window)->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_null;
	((noegnud_gui_twidget *)window)->event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_null;
	((noegnud_gui_twidget *)window)->offset_y=0;
	((noegnud_gui_twidget *)window)->offset_x=0;
	((noegnud_gui_twidget *)window)->vscroll=0;
	((noegnud_gui_twidget *)window)->hscroll=0;
    }


    noegnud_gui_active=old_noegnud_gui_active;
    return toreturn;
};

char noegnud_message_menu (CHAR_P let, int how, const char *mesg) {
    winid window;
    anything identifier;
    menu_item *menu_list;
    char *str;
    char retch;

    menu_list=0;
    window=noegnud_create_nhwindow(NHW_MENU);
    noegnud_start_menu(window);
    identifier.a_int=1;
    str=(char *)mesg;
    if ((str[0]==let)&&(str[2]=='-')) str+=4;
    noegnud_add_menu ( window, NO_GLYPH, &identifier, let, 0, 0, str, 0);
    noegnud_end_menu(window,"Pick one:");
    retch=0;
    if (noegnud_select_menu (window, PICK_ONE, &menu_list)==1) {
	free(menu_list);
	retch=let;
    }
    noegnud_destroy_nhwindow(window);
    return retch;
};

void noegnud_update_inventory (void) {
    if (flags.perm_invent) display_inventory(NULL,FALSE);
};

void noegnud_mark_synch (void) {
    //    printf("[*] mark_synch\n");
};

void noegnud_wait_synch (void) {
    //    printf("[*] wait_synch\n");
};

#ifndef CLIPPING
ABORT !!!!! -- noegnud _NEEDS_ clipping sorry
#endif

int noegnud_cliparound_firstcall=1;
void noegnud_cliparound (int x, int y) {
    noegnud_clip_x = x - 1;
    noegnud_clip_y = y;
    if (noegnud_options_render_features_autocentre->value) {
	if (!noegnud_options_render_features_autocentre_mouse->value) {
	    noegnud_render_middle_x = x - 0.5;
	    if (!noegnud_options_render_features_autocentre_xonly->value) {
		noegnud_render_middle_y = y + 0.5;
	    } else {
		noegnud_render_middle_y = (MAX_MAP_Y-1)/2 + 0.5;
	    }
	}
    } else {
	noegnud_render_middle_x = (MAX_MAP_X-1)/2 - 0.5;
	noegnud_render_middle_y = (MAX_MAP_Y-1)/2 + 0.5;
    }
    if (noegnud_cliparound_firstcall) {
	if (noegnud_options_fun_recording_ask->value&&noegnud_yn_function("Would you like to capture this game as an \"html movie\" ?","yn",'n')=='y') record_game = RECORD_GAME_TXT_HTML_COLOUR;
	if (noegnud_options_minimap_visible->value) {
	    noegnud_guiwidget_minimap->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_minimap;
	    noegnud_guiwidget_minimap->event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_minimap;
	}
	noegnud_guiwidget_status->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_window;
	if (noegnud_options_positionbar_visible->value) {
	    noegnud_guiwidget_positionbar->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_window;
	    noegnud_guiwidget_positionbar->event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_window;
	}

	noegnud_cliparound_firstcall=0;
    }
};

#ifdef POSITIONBAR

void noegnud_update_positionbar (char *features) {
    char thing[MAX_MAP_X+1];
    int c;
    char *ch;

    thing[0]=0;
    for (c=0;c<MAX_MAP_X;c++) strcat(thing," ");
    ch=features;
    while (*ch) {
	c=ch[1];
	thing[c-1]=*ch;
	ch+=2;
    }
    noegnud_gui_inconsole_noegnudtext(noegnud_guiwidget_positionbar_container,thing, FALSE,
				      noegnud_options_gui_window_console_text_colour_default->r,
				      noegnud_options_gui_window_console_text_colour_default->g,
				      noegnud_options_gui_window_console_text_colour_default->b);

};
#endif

void noegnud_print_glyph (winid window, int nhx, int nhy, int glyph) {
    int x, y;

    x = nhx - 1;
    y = nhy;

    /*
    if (glyph==cmap_to_glyph(S_stone))
	if (levl[nhx][nhy].seenv&&(levl[nhx][nhy].typ==STONE||levl[nhx][nhy].typ==SCORR||levl[nhx][nhy].typ==SDOOR)) {
	    glyph=cmap_to_glyph(S_stone);
	} else return;
    */

    if (glyph_to_cmap(glyph)==S_stone&&(levl[nhx][nhy].typ==STONE||(!level.locations[nhx][nhy].seenv))) {
	noegnud_map[x][y].glyph_primary = MAX_GLYPH;
	noegnud_map[x][y].glyph_fore = noegnud_map[x][y].glyph_middle = noegnud_map[x][y].glyph_back = MAX_GLYPH;
	noegnud_map[x][y].low=noegnud_map[x][y].high=FALSE;
	return;
    }

    noegnud_map[x][y].glyph_primary = glyph;
    noegnud_map[x][y].glyph_fore = glyph;
    noegnud_map[x][y].glyph_middle = back_to_glyph (nhx, nhy);


    noegnud_map[x][y].tile_primary = glyph2tile[noegnud_map[x][y].glyph_primary];
    mapglyph (glyph, &noegnud_map[x][y].ch_primary, &noegnud_map[x][y].colour_primary, &noegnud_map[x][y].special_primary, nhx, nhy);


    if (noegnud_map[x][y].glyph_fore == cmap_to_glyph(S_stone) || noegnud_map[x][y].glyph_fore == cmap_to_glyph(S_room) || noegnud_map[x][y].glyph_fore == cmap_to_glyph(S_corr) || noegnud_map[x][y].glyph_fore == cmap_to_glyph(S_litcorr)) {
	noegnud_map[x][y].glyph_back = noegnud_map[x][y].glyph_fore;
	noegnud_map[x][y].glyph_middle = MAX_GLYPH;
	noegnud_map[x][y].glyph_fore = MAX_GLYPH;
    } else
	if (noegnud_map[x][y].glyph_middle == cmap_to_glyph(S_stone) || noegnud_map[x][y].glyph_middle == cmap_to_glyph(S_room) || noegnud_map[x][y].glyph_middle == cmap_to_glyph(S_corr) || noegnud_map[x][y].glyph_middle == cmap_to_glyph(S_litcorr)) {
	    noegnud_map[x][y].glyph_back = noegnud_map[x][y].glyph_middle;
	    noegnud_map[x][y].glyph_middle = MAX_GLYPH;
	} else {
	    noegnud_map[x][y].glyph_back = cmap_to_glyph(S_room);
	}

    if (noegnud_map[x][y].glyph_fore == noegnud_map[x][y].glyph_middle) noegnud_map[x][y].glyph_fore = MAX_GLYPH;
    if (noegnud_map[x][y].glyph_fore == noegnud_map[x][y].glyph_back) noegnud_map[x][y].glyph_fore = MAX_GLYPH;
    if (noegnud_map[x][y].glyph_middle == noegnud_map[x][y].glyph_back) noegnud_map[x][y].glyph_middle = MAX_GLYPH;
    if (noegnud_map[x][y].glyph_fore!=MAX_GLYPH&&noegnud_map[x][y].glyph_middle==MAX_GLYPH&&(!(glyph_is_normal_object(noegnud_map[x][y].glyph_fore)||glyph_is_monster(noegnud_map[x][y].glyph_fore)))) {
	noegnud_map[x][y].glyph_middle=noegnud_map[x][y].glyph_fore;
	noegnud_map[x][y].glyph_fore=MAX_GLYPH;
    }
    noegnud_map[x][y].tile_back = glyph2tile[noegnud_map[x][y].glyph_back];

    /*  Alternate floors are non-vanilla and therefore removed

    # define NOEGNUD_TILES_BASE_FLOORS_START 1057
    if (noegnud_map[x][y].tile_back == glyph2tile[cmap_to_glyph(S_room)]) {
    #ifndef	In_caves
    # define In_caves(x) FALSE
    #endif
    #ifndef	Is_rogue_level
    # define Is_rogue_level(x) FALSE
    #endif
    noegnud_map[x][y].tile_back =
    In_mines(&u.uz) ? NOEGNUD_TILES_BASE_FLOORS_START + 0 :
    (In_sokoban(&u.uz)||Is_knox(&u.uz)) ? NOEGNUD_TILES_BASE_FLOORS_START + 1 :
    (In_V_tower(&u.uz) || In_W_tower(nhx,nhy,&u.uz)) ? NOEGNUD_TILES_BASE_FLOORS_START + 2 :
    In_hell(&u.uz) ? NOEGNUD_TILES_BASE_FLOORS_START + 3 :
    In_quest(&u.uz) ? NOEGNUD_TILES_BASE_FLOORS_START + 4 :
    In_endgame(&u.uz) ? NOEGNUD_TILES_BASE_FLOORS_START + 5 :
    //			(In_caves(&u.uz)||Is_rogue_level(&u.uz)) ? NOEGNUD_TILES_BASE_FLOORS_START + 6 :
    Is_medusa_level(&u.uz) ? NOEGNUD_TILES_BASE_FLOORS_START + 7 :
    glyph2tile[cmap_to_glyph(S_room)];
    }

    */

    mapglyph (noegnud_map[x][y].glyph_back, &noegnud_map[x][y].ch_back, &noegnud_map[x][y].colour_back, &noegnud_map[x][y].special_back, nhx, nhy);

    if (noegnud_map[x][y].glyph_middle != MAX_GLYPH) {
	noegnud_map[x][y].tile_middle = glyph2tile[noegnud_map[x][y].glyph_middle];
	mapglyph (noegnud_map[x][y].glyph_middle, &noegnud_map[x][y].ch_middle, &noegnud_map[x][y].colour_middle, &noegnud_map[x][y].special_middle, nhx, nhy);
	noegnud_tilesetload_image_from(noegnud_activetileset,  noegnud_map[x][y].tile_middle);
    }

    if (noegnud_map[x][y].glyph_fore != MAX_GLYPH) {
	noegnud_map[x][y].tile_fore = glyph2tile[noegnud_map[x][y].glyph_fore];
	mapglyph (noegnud_map[x][y].glyph_fore, &noegnud_map[x][y].ch_fore, &noegnud_map[x][y].colour_fore, &noegnud_map[x][y].special_fore, nhx, nhy);
	noegnud_tilesetload_image_from(noegnud_activetileset,  noegnud_map[x][y].tile_fore);
    }

    noegnud_tilesetload_image_from(noegnud_activetileset,  noegnud_map[x][y].tile_back);


    noegnud_map[x][y].high=( (noegnud_map[x][y].glyph_primary!=MAX_GLYPH) &&
			     ( ( (IS_WALL(level.locations[x+1][y].typ))||
				 (level.locations[x+1][y].typ==SDOOR)||
				 ( IS_DOOR(level.locations[x+1][y].typ)&&
				   ( level.locations[x+1][y].doormask&(D_CLOSED|D_LOCKED))))||
			       (glyph_to_cmap(noegnud_map[x][y].glyph_primary)>=S_vwall&&glyph_to_cmap(noegnud_map[x][y].glyph_primary)<=S_trwall )));

    {
	struct trap *ttrap;
	int cmap;

	if (glyph_is_cmap(noegnud_map[x][y].glyph_middle)) {
	    cmap=glyph_to_cmap(noegnud_map[x][y].glyph_middle);
	}
	noegnud_map[x][y].translucent=(noegnud_map[x][y].glyph_primary!=MAX_GLYPH)&&(glyph_is_cmap(noegnud_map[x][y].glyph_middle)&&((cmap==S_lava)||(cmap==S_pool)||(cmap==S_water)||(cmap==S_ice)));

	noegnud_map[x][y].low = level.locations[x+1][y].seenv &&
	    (noegnud_map[x][y].glyph_primary!=MAX_GLYPH) &&
	    ( ( (!Hallucination) &&
		( (ttrap=t_at(x+1,y)) &&
		  (ttrap->tseen) &&
		  ( ttrap->ttyp==PIT ||
		    ttrap->ttyp==SPIKED_PIT))) ||
	      (noegnud_map[x][y].translucent));


    }

};

void noegnud_raw_print (const char *instr) {
    int c;
    noegnud_gui_ttext *widget_text;
    char *str_tran;
    char *str_colour;
    char *str_time;
    char *str_tt;
    char *str;
    time_t time_;
    struct tm *time__;

    if (!noegnud_graphics_system_ready) {
	printf("[NOEGNUD TEXT] \"%s\"\n",instr);
	return;
    }

    play_sound_for_message(instr);

    str_tran=noegnud_lang_translate(instr);
    str_colour=noegnud_textcolouring_colourise(str_tran);

    noegnud_mem_free(str_tran);

    if (noegnud_options_gui_window_console_timestamp->value) {

	time(&time_);
	time__=localtime(&time_);

	str_time=noegnud_mem_malloc(1024);
	sprintf(str_time,"%02d:%02d:%02d",time__->tm_hour,time__->tm_min,time__->tm_sec);

	str_tt=noegnud_mem_malloc(1+strlen(str_time)+2+strlen(str_colour)+1);
	sprintf(str_tt,"[%s] %s",str_time,str_colour);

	str=noegnud_textcolour_rgb_text( str_tt,"^[[].*[]]",
					 noegnud_options_gui_window_console_text_colour_timestamp->r,
					 noegnud_options_gui_window_console_text_colour_timestamp->g,
					 noegnud_options_gui_window_console_text_colour_timestamp->b,
					 noegnud_options_gui_window_console_text_colour_default->r,
					 noegnud_options_gui_window_console_text_colour_default->g,
					 noegnud_options_gui_window_console_text_colour_default->b);

	if (str_tt!=str) noegnud_mem_free(str_tt);
	noegnud_mem_free(str_time);
	noegnud_mem_free(str_colour);

    } else {
	str=str_colour;
	str_colour=NULL;
    }


    noegnud_mem_free(noegnud_consolemessages[0]);
    for (c=0;c<NOEGNUD_CONSOLEMESSAGES_MAX-1;c++)
	noegnud_consolemessages[c]=noegnud_consolemessages[c+1];

    noegnud_consolemessages[NOEGNUD_CONSOLEMESSAGES_MAX-1]=noegnud_mem_malloc(strlen(str)+1);
    strcpy(noegnud_consolemessages[NOEGNUD_CONSOLEMESSAGES_MAX-1],str);

    widget_text=noegnud_gui_create_text( noegnud_guiwidget_console->child,
					 10, 0,
					 noegnud_guiwidget_console->width-20,
					 noegnud_gui_get_fontheight(),
					 noegnud_options_gui_window_console_text_colour_default->r,
					 noegnud_options_gui_window_console_text_colour_default->g,
					 noegnud_options_gui_window_console_text_colour_default->b,
					 str);
    widget_text->dynamiccharwidth=TRUE;
    noegnud_gui_reshuffle_console_children(noegnud_guiwidget_console->child);

    noegnud_mem_free(str);
}

void
noegnud_raw_print_bold (const char *str)
{
    noegnud_raw_print (str);
}

static int noegnud_internal_convert_sdlkey_nhkey_fastwalk=0;

int noegnud_internal_convert_sdlkey_nhkey (int sym, int unicode, int mod, int manualfix) {
    int shift = mod & KMOD_SHIFT;
    int ctrl = mod & KMOD_CTRL;
    int alt = mod & KMOD_ALT;

    int ch;

    ch=unicode;

    if (ch=='!') return 0; // damn shell thing :/

    if ((ctrl&&ch=='z')||(ch==('z'-('a'-1)))) return 0; // lets just ignore this nasty lil bugger...

    if (ctrl&&ch>='a'&&ch<'z') return ch-('a'-1);

    if (alt&&ch>='a'&&ch<='z') {
	noegnud_common_pushevent_keypress(ch);
	return '#';
    }

    if (ch>='a'&&ch<='z') {
	if (shift) ch+='A'-'a';
	return ch;
    }

    switch (sym) {
    case SDLK_BACKSPACE: return '\b';
    case SDLK_KP_ENTER:
    case SDLK_RETURN: return '\n';
    case SDLK_ESCAPE: return '\033';
    case SDLK_TAB: return '\t';
    case SDLK_KP8:
    case SDLK_UP: return iflags.num_pad ? '8' : (shift||noegnud_internal_convert_sdlkey_nhkey_fastwalk) ? 'K' : 'k';
    case SDLK_KP2:
    case SDLK_DOWN: return iflags.num_pad ? '2' : (shift||noegnud_internal_convert_sdlkey_nhkey_fastwalk) ? 'J' : 'j';
    case SDLK_KP4:
    case SDLK_LEFT: return iflags.num_pad ? '4' : (shift||noegnud_internal_convert_sdlkey_nhkey_fastwalk) ? 'H' : 'h';
    case SDLK_KP6:
    case SDLK_RIGHT: return iflags.num_pad ? '6' : (shift||noegnud_internal_convert_sdlkey_nhkey_fastwalk) ? 'L' : 'l';
    case SDLK_KP7: return iflags.num_pad ? '7' : (shift||noegnud_internal_convert_sdlkey_nhkey_fastwalk) ? 'Y' : 'y';
    case SDLK_KP9: return iflags.num_pad ? '9' : (shift||noegnud_internal_convert_sdlkey_nhkey_fastwalk) ? 'U' : 'u';
    case SDLK_KP1: return iflags.num_pad ? '1' : (shift||noegnud_internal_convert_sdlkey_nhkey_fastwalk) ? 'B' : 'b';
    case SDLK_KP3: return iflags.num_pad ? '3' : (shift||noegnud_internal_convert_sdlkey_nhkey_fastwalk) ? 'N' : 'n';

    case SDLK_PAGEUP: return MENU_PREVIOUS_PAGE;
    case SDLK_PAGEDOWN: return MENU_NEXT_PAGE;
    case SDLK_HOME: return MENU_FIRST_PAGE;
    case SDLK_END: return MENU_LAST_PAGE;
    }

    /* this should only kick in if unicode didn't find these */

    if (manualfix&&shift) {
	switch(sym) {
	case '0': return ')';
	case '1': return '!';
	case '2': return '@';
	case '3': return '#';
	case '4': return '$';
	case '5': return '%';
	case '6': return '^';
	case '7': return '&';
	case '8': return '*';
	case '9': return '(';
	case '-': return '_';
	case '=': return '+';
	case '[': return '{';
	case ']': return '}';
	case '\\': return '|';
	case ';': return ':';
	case '\'': return '"';
	case ',': return '<';
	case '.': return '>';
	case '/': return '?';
	}
    }


    if ((ch>0)&&(ch<0x7e)) return ch;

    return 0;

}

void noegnud_noegnud_options_displaymethod() {
    winid window;
    anything identifier;
    menu_item *menu_list;
    int chosen;

    menu_list=0;
    window=noegnud_create_nhwindow(NHW_MENU);
    noegnud_start_menu(window);
    identifier.a_int=NOEGNUD_RENDER_DRAWMODE_NONE; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "none", noegnud_options_mode_drawmode->value==NOEGNUD_RENDER_DRAWMODE_NONE);
    identifier.a_int=NOEGNUD_RENDER_DRAWMODE_CHAR; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "character", noegnud_options_mode_drawmode->value==NOEGNUD_RENDER_DRAWMODE_CHAR);
    identifier.a_int=NOEGNUD_RENDER_DRAWMODE_TILE; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "tiles", noegnud_options_mode_drawmode->value==NOEGNUD_RENDER_DRAWMODE_TILE);
    //	identifier.a_int=NOEGNUD_RENDER_DRAWMODE_3D; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "3D", 0);
    noegnud_end_menu(window,"Display Method");
    chosen=0;
    if (noegnud_select_menu (window, PICK_ONE, &menu_list)==1) {
	chosen=menu_list->item.a_int;
	free(menu_list);
    }
    noegnud_destroy_nhwindow(window);
    if (chosen) noegnud_options_mode_drawmode->value=chosen;
}

#define NOEGNUD_FONTS_PER_PAGE 30
void noegnud_noegnud_options_displaymethod_text(int offset) {
    winid window;
    anything identifier;
    menu_item *menu_list;
    int chosen;

    DIR *direcH;
    DIR *direcG;
    struct dirent *de;

    int skipoffset;

    noegnud_tcollection *filelist;
    noegnud_tcollection *filelist_run;

    noegnud_fonts_tfont *tmpoldfont;
    noegnud_fonts_tfont *tmpnewfont;

    char *fname;
    char *fname2;
    char *ffname;
    char *szHomedir;
    char direcname[2048];


    filelist=NULL;

    szHomedir=noegnud_options_get_home();
    sprintf(direcname,"%s/char",szHomedir);
    noegnud_mem_free(szHomedir);
    direcH=opendir(direcname);
    direcG=opendir("../" NOEGNUD_DATADIR "/char");
    if (!(direcH||direcG)) {
	printf("Error opening font directory :/\n");
	return;
    };

    while (direcH&&(de=readdir(direcH))) {
	if (de->d_name[0]!='.'&&(strstr(de->d_name,".png")==(&de->d_name[strlen(de->d_name)-4]))) {
	    fname=noegnud_mem_malloc(strlen(de->d_name)+1);
	    strncpy(fname,de->d_name,strlen(de->d_name)-4);
	    fname[strlen(de->d_name)-4]=0;
	    fname2=noegnud_mem_malloc(strlen(de->d_name)+1+4);
	    sprintf(fname2,"%s (~)",fname);

	    if (!filelist) {
		filelist=noegnud_collection_create(fname,fname2);
	    } else {
		noegnud_collection_add(filelist,fname,fname2);
	    }
	    noegnud_mem_free(fname);
	}
    }

    closedir(direcH);

    while (direcG&&(de=readdir(direcG))) {
	if (de->d_name[0]!='.'&&(strstr(de->d_name,".png")==(&de->d_name[strlen(de->d_name)-4]))) {
	    fname=noegnud_mem_malloc(strlen(de->d_name)+1);
	    strncpy(fname,de->d_name,strlen(de->d_name)-4);
	    fname[strlen(de->d_name)-4]=0;

	    if (!filelist) {
		filelist=noegnud_collection_create(fname,fname);
	    } else {
		if (!noegnud_collection_data(filelist,fname)) noegnud_collection_add(filelist,fname,fname);
	    }
	}
    }

    closedir(direcG);

    menu_list=0;
    window=noegnud_create_nhwindow(NHW_MENU);
    noegnud_start_menu(window);


    filelist_run=filelist;
    skipoffset=0;
    while(filelist_run&&skipoffset-offset<NOEGNUD_FONTS_PER_PAGE) {
	if (skipoffset>=offset) {
	    identifier.a_int=(int)filelist_run; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, (char *)filelist_run->data, 0);
	}
	filelist_run=filelist_run->next;
	skipoffset++;
    }
    if (filelist_run) {
	identifier.a_int=0; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "", 0);
	identifier.a_int=-1; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "more ..", 0);
    }

    noegnud_end_menu(window,"Select Font");
    chosen=0;
    if (noegnud_select_menu (window, PICK_ONE, &menu_list)==1) {
	chosen=menu_list->item.a_int;
	free(menu_list);
    }
    noegnud_destroy_nhwindow(window);

    if (chosen==-1) {
	noegnud_collection_destroy(&filelist);
	noegnud_noegnud_options_displaymethod_text(skipoffset);
	return;
    }

    if (chosen) {
	filelist_run=(noegnud_tcollection *)chosen;

	ffname=noegnud_mem_malloc(strlen((char *)filelist_run->name)+50);
	sprintf(ffname,"char/%s",(char *)filelist_run->name);

	strcpy(noegnud_options_mode_char_font->value,ffname);

	tmpnewfont=noegnud_fonts_loadfont(ffname);
	noegnud_mem_free(ffname);

	tmpoldfont=noegnud_font_map;
	noegnud_font_map=tmpnewfont;
	noegnud_fonts_deletefont(&tmpoldfont);

    }

    noegnud_collection_destroy(&filelist);
}

void noegnud_noegnud_options_displaymethod_tile() {
    winid window;
    anything identifier;
    menu_item *menu_list;
    int chosen;

    DIR *direcH;
    DIR *direcG;
    struct dirent *de;

    noegnud_tcollection *filelist;
    noegnud_tcollection *filelist_run;

    ts_tileset *tmpoldtile;
    ts_tileset *tmpnewtile;

    char *fname;
    char *fname2;

    int c;
    char *szHomedir;
    char direcname[2048];


    filelist=NULL;


    szHomedir=noegnud_options_get_home();
    sprintf(direcname,"%s/tilesets",szHomedir);
    direcH=opendir(direcname);
    direcG=opendir("../" NOEGNUD_DATADIR "/tilesets");
    if (!(direcH||direcG)) {
	printf("Error opening tile directory :/\n");
	return;
    };

    while (direcH&&(de=readdir(direcH))) {
	if (de->d_name[0]!='.'&&(strstr(de->d_name,".inf")==(&de->d_name[strlen(de->d_name)-4]))) {
	    fname=noegnud_mem_malloc(strlen(de->d_name)+1);
	    strncpy(fname,de->d_name,strlen(de->d_name)-4);
	    fname[strlen(de->d_name)-4]=0;
	    fname2=noegnud_mem_malloc(strlen(de->d_name)+1+4);
	    sprintf(fname2,"%s (~)",fname);

	    if (!filelist) {
		filelist=noegnud_collection_create(fname,fname2);
	    } else {
		noegnud_collection_add(filelist,fname,fname2);
	    }
	    noegnud_mem_free(fname);
	}
    }
    closedir(direcH);

    sprintf(direcname,"%s/tilesets/" VARIANT_TILETYPE,szHomedir);
    noegnud_mem_free(szHomedir);
    if ((direcH=opendir(direcname))) {
	while ((de=readdir(direcH))) {
	    if (de->d_name[0]!='.'&&(strstr(de->d_name,".inf")==(&de->d_name[strlen(de->d_name)-4]))) {
		fname=noegnud_mem_malloc(strlen(VARIANT_TILETYPE) + 1 + strlen(de->d_name)+1);
		snprintf(fname,strlen(VARIANT_TILETYPE) + 1 + strlen(de->d_name),"%s/%s",VARIANT_TILETYPE,de->d_name);
		fname[strlen(VARIANT_TILETYPE) + 1 + strlen(de->d_name)-4]=0;
		fname2=noegnud_mem_malloc(strlen(de->d_name)+2+strlen(VARIANT_TILETYPE)+1+4+1);
		sprintf(fname2,"%s",de->d_name);
		fname2[strlen(fname2)-4]=0;
		sprintf(fname2,"%s [%s] (~)",fname2,VARIANT_TILETYPE);

		if (!filelist) {
		    filelist=noegnud_collection_create(fname,fname2);
		} else {
		    noegnud_collection_add(filelist,fname,fname2);
		}
		noegnud_mem_free(fname);
	    }
	}
    };

    closedir(direcH);

    while (direcG&&(de=readdir(direcG))) {
	if (de->d_name[0]!='.'&&(strstr(de->d_name,".inf")==(&de->d_name[strlen(de->d_name)-4]))) {
	    fname=noegnud_mem_malloc(strlen(de->d_name)+1);
	    strncpy(fname,de->d_name,strlen(de->d_name)-4);
	    fname[strlen(de->d_name)-4]=0;

	    if (!filelist) {
		filelist=noegnud_collection_create(fname,fname);
	    } else {
		if (!noegnud_collection_data(filelist,fname)) noegnud_collection_add(filelist,fname,fname);
	    }
	}
    }
    closedir(direcG);

    if ((direcG=opendir("../" NOEGNUD_DATADIR "/tilesets/" VARIANT_TILETYPE))) {
	while ((de=readdir(direcG))) {
	    if (de->d_name[0]!='.'&&(strstr(de->d_name,".inf")==(&de->d_name[strlen(de->d_name)-4]))) {
		fname=noegnud_mem_malloc(strlen(VARIANT_TILETYPE) + 1 + strlen(de->d_name)+1);
		snprintf(fname,strlen(VARIANT_TILETYPE) + 1 + strlen(de->d_name),"%s/%s",VARIANT_TILETYPE,de->d_name);
		fname[strlen(VARIANT_TILETYPE) + 1 + strlen(de->d_name)-4]=0;
		fname2=noegnud_mem_malloc(strlen(de->d_name)+2+strlen(VARIANT_TILETYPE)+1+1);
		sprintf(fname2,"%s",de->d_name);
		fname2[strlen(fname2)-4]=0;
		sprintf(fname2,"%s [%s]",fname2,VARIANT_TILETYPE);

		if (!filelist) {
		    filelist=noegnud_collection_create(fname,fname2);
		} else {
		    if (!noegnud_collection_data(filelist,fname)) {
			noegnud_collection_add(filelist,fname,fname2);
		    } else noegnud_mem_free(fname2);
		}

		noegnud_mem_free(fname);
	    }
	}
    };
    closedir(direcG);

    menu_list=0;
    window=noegnud_create_nhwindow(NHW_MENU);
    noegnud_start_menu(window);


    filelist_run=filelist;
    while(filelist_run) {
	identifier.a_int=(int)filelist_run; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, filelist_run->data, 0);
	filelist_run=filelist_run->next;
    }

    noegnud_end_menu(window,"Select Tileset");
    chosen=0;
    if (noegnud_select_menu (window, PICK_ONE, &menu_list)==1) {
	chosen=menu_list->item.a_int;
	free(menu_list);
    }

    noegnud_destroy_nhwindow(window);
    if (chosen) {
	filelist_run=(noegnud_tcollection *)chosen;

	if ((tmpnewtile=noegnud_tilesets_loadtileset(filelist_run->name))) {

	    strcpy(noegnud_options_mode_tileset_tile->value,filelist_run->name);

	    tmpoldtile=noegnud_activetileset;

	    for (c=0;c<MAX_TILES;c++) if (tmpoldtile&&tmpoldtile->loaded[c]) noegnud_tilesetload_image_from(tmpnewtile,c);

	    noegnud_activetileset=tmpnewtile;
	    if (tmpoldtile) noegnud_tilesets_unloadtileset(tmpoldtile);
	}
    }

    filelist_run=filelist;
    while (filelist_run) {
	noegnud_mem_free(filelist_run->data);
	filelist_run=filelist_run->next;
    }
    noegnud_collection_destroy(&filelist);
}

void noegnud_noegnud_options_displaymethod_3d() {
}

int noegnud_noegnud_options_keys() {
    winid window;
    anything identifier;
    menu_item *menu_list;
    noegnud_optiontype_int *chosen;

    SDL_Event event;

    char buffer[512];

    menu_list=0;
    window=noegnud_create_nhwindow(NHW_MENU);
    noegnud_start_menu(window);

    sprintf((char *)&buffer,"Options - [ %s ]",SDL_GetKeyName(noegnud_options_keys_options->value));
    identifier.a_int=(int)noegnud_options_keys_options; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, (char *)&buffer, 0);
    sprintf((char *)&buffer,"Console - [ %s ]",SDL_GetKeyName(noegnud_options_keys_console->value));
    identifier.a_int=(int)noegnud_options_keys_console; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, (char *)&buffer, 0);
    sprintf((char *)&buffer,"MiniMap - [ %s ]",SDL_GetKeyName(noegnud_options_keys_minimap->value));
    identifier.a_int=(int)noegnud_options_keys_minimap; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, (char *)&buffer, 0);
    sprintf((char *)&buffer,"HideGUI - [ %s ]",SDL_GetKeyName(noegnud_options_keys_hidegui->value));
    identifier.a_int=(int)noegnud_options_keys_hidegui; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, (char *)&buffer, 0);

    noegnud_end_menu(window,"View/Change noeGNUd keys");
    chosen=0;
    if (noegnud_select_menu (window, PICK_ONE, &menu_list)==1) {
	chosen=(noegnud_optiontype_int *)menu_list->item.a_int;
	free(menu_list);
    }
    noegnud_destroy_nhwindow(window);
    if (chosen) {
	window=noegnud_create_nhwindow(NHW_TEXT);
	noegnud_putstr(window,0,"Press new key..");

	noegnud_gui_active=(noegnud_gui_twidget *)window;

	while (1) {
	    while (SDL_PollEvent (&event)) {
		if (event.type==SDL_KEYDOWN) {
		    chosen->value=event.key.keysym.sym;
		    noegnud_destroy_nhwindow((winid) window);
		    noegnud_gui_active=NULL;

		    return 1;
		}
	    }
	    noegnud_render_loop();
	}
    };
    return 0;
}


void noegnud_creditscreen() {
    noegnud_gui_twindow * window;
    noegnud_gui_twindow * container;
    noegnud_gui_twidget * button;
    char buffer[BUFSZ];
    dlb *f;
    char *ch;

    window=noegnud_gui_create_window(
				     noegnud_guiwidget_desktop,
				     0,0,
				     noegnud_options_screenwidth->value,
				     noegnud_options_screenheight->value,
				     1.0,1.0,1.0,1.0,
				     0);

    window->image_tiled=0;
    window->image=noegnud_glfuncs_loadimage("gui/credits.png",TRUE,FALSE,FALSE);
    window->localimage=TRUE;
    window->movable=0;

    button=(noegnud_gui_twidget *)noegnud_gui_create_textbutton(
								(noegnud_gui_twidget *)window,
								0,0,
								1.0,1.0,1.0,1.0,
								0.0,0.0,0.0,
								" Ok ",
								'\n');

    button->x=window->widget.width-(button->width + 8);
    button->y=window->widget.height-(button->height + 8);

    container=noegnud_gui_create_window(
				     (noegnud_gui_twidget *)window,
				     (320*noegnud_options_screenwidth->value)/window->image->width,
				     (290*noegnud_options_screenheight->value)/window->image->height,
				     (610*noegnud_options_screenwidth->value)/window->image->width,
				     (540*noegnud_options_screenheight->value)/window->image->height,
				     1.0,1.0,1.0,1.0,
				     0);
    container->widget.draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_widget;
    container->movable=FALSE;
    container->widget.autocentre=FALSE;
    container->autoresize=0;


    if ((f=dlb_fopen("../data/credits.txt","r"))) {
	while (dlb_fgets(buffer,BUFSZ,f)) {
	    if ((ch=index(buffer,'\n'))) *ch=0;
	    if ((ch=index(buffer,'\r'))) *ch=0;

	    noegnud_gui_nh_putstr(container,buffer,0.0,0.0,0.0);
	}
	dlb_fclose(f);
    } else {
	noegnud_gui_nh_putstr(container,"** Could not find \"data/credits.txt\" !",1.0,0.0,0.0);
	noegnud_gui_nh_putstr(container,"",1.0,0.0,0.0);
	noegnud_gui_nh_putstr(container,"see project team at http://noegnud.sourceforge.net/",0.0,0.0,0.0);
    }


    noegnud_gui_active=(noegnud_gui_twidget *)window;

    noegnud_nhgetch();

    glDeleteTextures(1,&window->image->block[0][0]->image);

    noegnud_destroy_nhwindow((winid)window);
}


#define NOEGNUD_OPTIONS_MENU_DISPLAY 0x00
#define NOEGNUD_OPTIONS_MENU_DISPLAY_METHOD NOEGNUD_OPTIONS_MENU_DISPLAY + 0x01
#define NOEGNUD_OPTIONS_MENU_DISPLAY_FULLSCREEN NOEGNUD_OPTIONS_MENU_DISPLAY + 0x02
#define NOEGNUD_OPTIONS_MENU_DISPLAY_FPS NOEGNUD_OPTIONS_MENU_DISPLAY + 0x03

#define NOEGNUD_OPTIONS_MENU_TEXT 0x10
#define NOEGNUD_OPTIONS_MENU_TEXT_FONT NOEGNUD_OPTIONS_MENU_TEXT + 0x01

#define NOEGNUD_OPTIONS_MENU_2D 0x20
#define NOEGNUD_OPTIONS_MENU_2D_TILESET NOEGNUD_OPTIONS_MENU_2D + 0x01
#define NOEGNUD_OPTIONS_MENU_2D_TOGGLE NOEGNUD_OPTIONS_MENU_2D + 0x02
#define NOEGNUD_OPTIONS_MENU_2D_WALLS NOEGNUD_OPTIONS_MENU_2D + 0x03
#define NOEGNUD_OPTIONS_MENU_2D_OBJECTS NOEGNUD_OPTIONS_MENU_2D + 0x04
#define NOEGNUD_OPTIONS_MENU_2D_MONSTERS NOEGNUD_OPTIONS_MENU_2D + 0x05
#define NOEGNUD_OPTIONS_MENU_2D_TRANSLUCENCY NOEGNUD_OPTIONS_MENU_2D + 0x06
#define NOEGNUD_OPTIONS_MENU_2D_HALFWIDTH NOEGNUD_OPTIONS_MENU_2D + 0x07
#define NOEGNUD_OPTIONS_MENU_2D_WALLHALFHEIGHT NOEGNUD_OPTIONS_MENU_2D + 0x08
#define NOEGNUD_OPTIONS_MENU_2D_CAVERNS NOEGNUD_OPTIONS_MENU_2D + 0x09
#define NOEGNUD_OPTIONS_MENU_2D_CAVERNS_UNEXPLORED NOEGNUD_OPTIONS_MENU_2D + 0x0A
#define NOEGNUD_OPTIONS_MENU_2D_CAVERNS_CUTOUT NOEGNUD_OPTIONS_MENU_2D + 0x0B

#define NOEGNUD_OPTIONS_MENU_3D 0x30
#define NOEGNUD_OPTIONS_MENU_3D_THEME NOEGNUD_OPTIONS_MENU_3D + 0x01

#define NOEGNUD_OPTIONS_INTERFACE 0x40
#define NOEGNUD_OPTIONS_INTERFACE_FUNCTIONMETHOD NOEGNUD_OPTIONS_INTERFACE + 0x01
#define NOEGNUD_OPTIONS_INTERFACE_CONSOLE_TIMESTAMP NOEGNUD_OPTIONS_INTERFACE + 0x02

#define NOEGNUD_OPTIONS_MENU_VIEW 0x50
#define NOEGNUD_OPTIONS_MENU_VIEW_RESET NOEGNUD_OPTIONS_MENU_VIEW + 0x01
#define NOEGNUD_OPTIONS_MENU_VIEW_FLATTEN NOEGNUD_OPTIONS_MENU_VIEW + 0x02
#define NOEGNUD_OPTIONS_MENU_VIEW_ROTATIONMETHOD NOEGNUD_OPTIONS_MENU_VIEW + 0X03
#define NOEGNUD_OPTIONS_MENU_VIEW_LIGHTING NOEGNUD_OPTIONS_MENU_VIEW + 0x04
#define NOEGNUD_OPTIONS_MENU_VIEW_MIPMAPPING NOEGNUD_OPTIONS_MENU_VIEW + 0x05
#define NOEGNUD_OPTIONS_MENU_VIEW_MINIMAP NOEGNUD_OPTIONS_MENU_VIEW + 0x06
#define NOEGNUD_OPTIONS_MENU_VIEW_POSITIONBAR NOEGNUD_OPTIONS_MENU_VIEW + 0x07
#define NOEGNUD_OPTIONS_MENU_VIEW_REFRESH NOEGNUD_OPTIONS_MENU_VIEW + 0x08
#define NOEGNUD_OPTIONS_MENU_VIEW_AUTOCENTRE NOEGNUD_OPTIONS_MENU_VIEW + 0x09
#define NOEGNUD_OPTIONS_MENU_VIEW_AUTOCENTRE_XONLY NOEGNUD_OPTIONS_MENU_VIEW + 0x0A
#define NOEGNUD_OPTIONS_MENU_VIEW_AUTOCENTRE_METHOD NOEGNUD_OPTIONS_MENU_VIEW + 0x0B
#define NOEGNUD_OPTIONS_MENU_VIEW_AUTOFACE NOEGNUD_OPTIONS_MENU_VIEW + 0x0C
#define NOEGNUD_OPTIONS_MENU_VIEW_SCREENSHOT NOEGNUD_OPTIONS_MENU_VIEW + 0x0D

#define NOEGNUD_OPTIONS_MENU_NOEGNUD 0x60
#define NOEGNUD_OPTIONS_MENU_NOEGNUD_CREDITS NOEGNUD_OPTIONS_MENU_NOEGNUD + 0x01
#define NOEGNUD_OPTIONS_MENU_NOEGNUD_HELP NOEGNUD_OPTIONS_MENU_NOEGNUD + 0x02
#define NOEGNUD_OPTIONS_MENU_NOEGNUD_KEYS NOEGNUD_OPTIONS_MENU_NOEGNUD + 0x03
#define NOEGNUD_OPTIONS_MENU_NOEGNUD_3DMOUSE NOEGNUD_OPTIONS_MENU_NOEGNUD + 0x04
#define NOEGNUD_OPTIONS_MENU_NOEGNUD_MOUSE_3D_LIGHT NOEGNUD_OPTIONS_MENU_NOEGNUD + 0x05
#define NOEGNUD_OPTIONS_MENU_NOEGNUD_SAVE NOEGNUD_OPTIONS_MENU_NOEGNUD + 0x06
#define NOEGNUD_OPTIONS_MENU_NOEGNUD_QUIT NOEGNUD_OPTIONS_MENU_NOEGNUD + 0x07

#define NOEGNUD_OPTIONS_MENU_VARIANT 0x70
#define NOEGNUD_OPTIONS_MENU_VARIANT_OPTIONS NOEGNUD_OPTIONS_MENU_VARIANT + 0x01
#define NOEGNUD_OPTIONS_MENU_VARIANT_HELP NOEGNUD_OPTIONS_MENU_VARIANT + 0x02

int noegnud_noegnud_options_busy=0;
void noegnud_noegnud_options() {
    winid window;
    anything identifier;
    menu_item *menu_list;
    int chosen;

    if (noegnud_noegnud_options_busy) return;
    noegnud_noegnud_options_busy=1;

    menu_list=0;
    window=noegnud_create_nhwindow(NHW_MENU);
    noegnud_start_menu(window);
    identifier.a_int=0; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "Display", 0);
    switch (noegnud_options_mode_drawmode->value) {
    case NOEGNUD_RENDER_DRAWMODE_NONE:
	identifier.a_int=NOEGNUD_OPTIONS_MENU_DISPLAY_METHOD; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "display method [none]", 0);
	break;
    case NOEGNUD_RENDER_DRAWMODE_CHAR:
	identifier.a_int=NOEGNUD_OPTIONS_MENU_DISPLAY_METHOD; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "display method [text]", 0);
	break;
    case NOEGNUD_RENDER_DRAWMODE_TILE:
	identifier.a_int=NOEGNUD_OPTIONS_MENU_DISPLAY_METHOD; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "display method [2D]", 0);
	break;
    case NOEGNUD_RENDER_DRAWMODE_3D:
	identifier.a_int=NOEGNUD_OPTIONS_MENU_DISPLAY_METHOD; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "display method [3D]", 0);
	break;
    }
#ifndef __WIN32__
    if (noegnud_options_fullscreen->value) {
	identifier.a_int=NOEGNUD_OPTIONS_MENU_DISPLAY_FULLSCREEN; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "fullscreen [on]", 0);
    } else {
	identifier.a_int=NOEGNUD_OPTIONS_MENU_DISPLAY_FULLSCREEN; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "fullscreen [off]", 0);
    }
#endif

    if (noegnud_options_render_engine_fps_show->value) {
	identifier.a_int=NOEGNUD_OPTIONS_MENU_DISPLAY_FPS; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "FPS display [on]", 0);
    } else {
	identifier.a_int=NOEGNUD_OPTIONS_MENU_DISPLAY_FPS; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "FPS display [off]", 0);
    }

    if (noegnud_options_mode_drawmode->value==NOEGNUD_RENDER_DRAWMODE_CHAR) {
	identifier.a_int=0; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "Text Mode", 0);
	identifier.a_int=NOEGNUD_OPTIONS_MENU_TEXT_FONT; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "select font", 0);
	if (noegnud_options_render_features_cardboardcutout->value) {
	    identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_TOGGLE; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "cutout mode [on]", 0);
	} else {
	    identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_TOGGLE; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "cutout mode [off]", 0);
	}
    }

    if (noegnud_options_mode_drawmode->value==NOEGNUD_RENDER_DRAWMODE_TILE) {
	identifier.a_int=0; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "2D Mode", 0);
	identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_TILESET; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "select tileset", 0);
	if (noegnud_options_render_features_cardboardcutout->value) {
	    identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_TOGGLE; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "cutout mode [on]", 0);
	    if (noegnud_options_render_features_cardboardcutout_raise_walls->value) {
		identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_WALLS; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "raise walls [on]", 0);
	    } else {
		identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_WALLS; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "raise walls [off]", 0);
	    }
	    if (noegnud_options_render_features_cardboardcutout_raise_objects->value) {
		identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_OBJECTS; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "raise objects [on]", 0);
	    } else {
		identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_OBJECTS; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "raise objects [off]", 0);
	    }
	    if (noegnud_options_render_features_cardboardcutout_raise_monsters->value) {
		identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_MONSTERS; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "raise monsters [on]", 0);
	    } else {
		identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_MONSTERS; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "raise monsters [off]", 0);
	    }
	    if (noegnud_options_render_features_cardboardcutout_wall_translucency_current->value) {
		identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_TRANSLUCENCY; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "wall translucency [on]", 0);
	    } else {
		identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_TRANSLUCENCY; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "wall translucency [off]", 0);
	    }
	    if (noegnud_options_render_features_cardboardcutout_wall_height->value==1.0) {
		identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_WALLHALFHEIGHT; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "wall half height [off]", 0);
	    } else {
		identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_WALLHALFHEIGHT; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "wall half height [on]", 0);
	    }
	} else {
	    identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_TOGGLE; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "cutout mode [off]", 0);
	}
	if (noegnud_activetileset) {
	    if (noegnud_options_mode_tileset_halfwidth->value) {
		identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_HALFWIDTH; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "half tilewidth [on]", 0);
	    } else {
		identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_HALFWIDTH; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "half tilewidth [off]", 0);
	    }
	    if (noegnud_options_mode_tileset_caverns->value) {
		identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_CAVERNS; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "caverns [on]", 0);
		if (noegnud_options_mode_tileset_caverns_unexplored->value) {
		    identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_CAVERNS_UNEXPLORED; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "unexplored caverns [on]", 0);
		} else {
		    identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_CAVERNS_UNEXPLORED; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "unexplored caverns [off]", 0);
		}
		if (noegnud_options_render_features_cardboardcutout->value) {
		    if (noegnud_options_mode_tileset_caverns_cutout->value) {
			identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_CAVERNS_CUTOUT; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "cutout caverns [on]", 0);
		    } else {
			identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_CAVERNS_CUTOUT; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "cutout caverns [off]", 0);
		    }
		}
	    } else {
		identifier.a_int=NOEGNUD_OPTIONS_MENU_2D_CAVERNS; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "caverns [off]", 0);
	    }
	}
    }


    if (noegnud_options_mode_drawmode->value==NOEGNUD_RENDER_DRAWMODE_3D) {
	identifier.a_int=0; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "3D Mode", 0);
	identifier.a_int=NOEGNUD_OPTIONS_MENU_3D_THEME; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "select theme", 0);
    }


    identifier.a_int=0; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "Interface", 0);
    if (!noegnud_yn_function_busy) {
	if (noegnud_options_interface_yn_function_windowed->value) {
	    identifier.a_int=NOEGNUD_OPTIONS_INTERFACE_FUNCTIONMETHOD; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "function query [windowed]", 0);
	} else {
	    identifier.a_int=NOEGNUD_OPTIONS_INTERFACE_FUNCTIONMETHOD; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "function query [inline]", 0);
	}
    }
    if (noegnud_options_gui_window_console_timestamp->value) {
	identifier.a_int=NOEGNUD_OPTIONS_INTERFACE_CONSOLE_TIMESTAMP; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "console timestamp [on]", 0);
    } else {
	identifier.a_int=NOEGNUD_OPTIONS_INTERFACE_CONSOLE_TIMESTAMP; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "console timestamp [off]", 0);
    }

    identifier.a_int=0; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "View", 0);
    identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_RESET; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "reset", 0);
    identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_FLATTEN; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "flatten", 0);
    if (noegnud_options_mode_all_rotationabsolute->value) {
	identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_ROTATIONMETHOD; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "rotation [absolute]", 0);
    } else {
	identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_ROTATIONMETHOD; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "rotation [relative]", 0);
    }
    switch (noegnud_options_render_opengl_lighting_method->value) {
    case NOEGNUD_RENDER_LIGHTING_METHOD_NONE:
	identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_LIGHTING; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "lighting [none]", 0);
	break;
    case NOEGNUD_RENDER_LIGHTING_METHOD_CHEAP:
	identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_LIGHTING; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "lighting [cheap]", 0);
	break;
    }
    if (!noegnud_inventory_open) {
	if (noegnud_options_render_opengl_mipmapping->value) {
	    identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_MIPMAPPING; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "mipmapping [on]", 0);
	} else {
	    identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_MIPMAPPING; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "mipmapping [off]", 0);
	}
    }
    if (noegnud_startup_flag) {
	if (noegnud_options_minimap_visible->value) {
	    identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_MINIMAP; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "minimap [on]", 0);
	} else {
	    identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_MINIMAP; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "minimap [off]", 0);
	}
	if (noegnud_options_positionbar_visible->value) {
	    identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_POSITIONBAR; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "positionbar [on]", 0);
	} else {
	    identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_POSITIONBAR; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "positionbar [off]", 0);
	}
    }
    if (noegnud_startup_flag) { identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_REFRESH; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "refresh map",0); };
    if (noegnud_options_render_features_autocentre->value) {
	identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_AUTOCENTRE; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "auto-centre [on]", 0);
	if (noegnud_options_render_features_autocentre_mouse->value) {
	    identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_AUTOCENTRE_METHOD; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "auto-centre method [on mouse]", 0);
	} else {
	    identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_AUTOCENTRE_METHOD; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "auto-centre method [on player]", 0);
	    if (noegnud_options_render_features_autocentre_xonly->value) {
		identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_AUTOCENTRE_XONLY; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "auto-centre on X axis only [on]", 0);
	    } else {
		identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_AUTOCENTRE_XONLY; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "auto-centre on X axis only [off]", 0);
	    }
	}
    } else {
	identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_AUTOCENTRE; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "auto-centre [off]", 0);
    }
    if (noegnud_options_render_features_autoface->value) {
	identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_AUTOFACE; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "monster/object auto-face [on]", 0);
    } else {
	identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_AUTOFACE; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "monster/object auto-face [off]", 0);
    }
    if (noegnud_startup_flag) {
	identifier.a_int=NOEGNUD_OPTIONS_MENU_VIEW_SCREENSHOT; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "screenshot", 0);
    }





    identifier.a_int=0; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, NOEGNUD_NAME, 0);
    identifier.a_int=NOEGNUD_OPTIONS_MENU_NOEGNUD_CREDITS; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "credits", 0);
    identifier.a_int=NOEGNUD_OPTIONS_MENU_NOEGNUD_HELP; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "help", 0);
    identifier.a_int=NOEGNUD_OPTIONS_MENU_NOEGNUD_KEYS; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "keyboard configure", 0);
    if (noegnud_options_input_3dmouse->value) {
	identifier.a_int=NOEGNUD_OPTIONS_MENU_NOEGNUD_3DMOUSE; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "3D mouse cursor [on]", 0);
	if (noegnud_options_input_mouse_3d_light->value) {
	    identifier.a_int=NOEGNUD_OPTIONS_MENU_NOEGNUD_MOUSE_3D_LIGHT; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "3D mouse light [on]", 0);
	} else {
	    identifier.a_int=NOEGNUD_OPTIONS_MENU_NOEGNUD_MOUSE_3D_LIGHT; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "3D mouse light [off]", 0);
	}
    } else {
	identifier.a_int=NOEGNUD_OPTIONS_MENU_NOEGNUD_3DMOUSE; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "3D mouse cursor [off]", 0);
    }
    if (noegnud_startup_flag) { identifier.a_int=NOEGNUD_OPTIONS_MENU_NOEGNUD_SAVE; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "save", 0); }
    if (noegnud_startup_flag) { identifier.a_int=NOEGNUD_OPTIONS_MENU_NOEGNUD_QUIT; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "quit", 0); }


    identifier.a_int=0; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, VARIANT_FULLNAME, 0);
    identifier.a_int=NOEGNUD_OPTIONS_MENU_VARIANT_OPTIONS; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "options", 0);
    identifier.a_int=NOEGNUD_OPTIONS_MENU_VARIANT_HELP; noegnud_add_menu ( window, NO_GLYPH, &identifier, 0, 0, 0, "help", 0);


    noegnud_end_menu(window,"noeGNUd Options");
    chosen=0;
    if (noegnud_select_menu (window, PICK_ONE, &menu_list)==1) {
	chosen=menu_list->item.a_int;
	free(menu_list);
    }
    noegnud_destroy_nhwindow(window);

    switch (chosen) {
    case NOEGNUD_OPTIONS_MENU_DISPLAY_METHOD : noegnud_noegnud_options_displaymethod(); break;
    case NOEGNUD_OPTIONS_MENU_DISPLAY_FULLSCREEN:
	SDL_WM_ToggleFullScreen(noegnud_primary_surface);
	noegnud_options_fullscreen->value=!noegnud_options_fullscreen->value;
	break;
    case NOEGNUD_OPTIONS_MENU_DISPLAY_FPS:
	noegnud_options_render_engine_fps_show->value=!noegnud_options_render_engine_fps_show->value;
	break;


    case NOEGNUD_OPTIONS_MENU_TEXT_FONT : noegnud_noegnud_options_displaymethod_text(0); break;


    case NOEGNUD_OPTIONS_MENU_2D_TILESET : noegnud_noegnud_options_displaymethod_tile(); break;
    case NOEGNUD_OPTIONS_MENU_2D_TOGGLE: noegnud_options_render_features_cardboardcutout->value=!noegnud_options_render_features_cardboardcutout->value; break;
    case NOEGNUD_OPTIONS_MENU_2D_WALLS: noegnud_options_render_features_cardboardcutout_raise_walls->value=!noegnud_options_render_features_cardboardcutout_raise_walls->value; break;
    case NOEGNUD_OPTIONS_MENU_2D_OBJECTS: noegnud_options_render_features_cardboardcutout_raise_objects->value=!noegnud_options_render_features_cardboardcutout_raise_objects->value; break;
    case NOEGNUD_OPTIONS_MENU_2D_MONSTERS: noegnud_options_render_features_cardboardcutout_raise_monsters->value=!noegnud_options_render_features_cardboardcutout_raise_monsters->value; break;
    case NOEGNUD_OPTIONS_MENU_2D_TRANSLUCENCY:
	if (noegnud_options_render_features_cardboardcutout_wall_translucency_current->value) {
	    noegnud_options_render_features_cardboardcutout_wall_translucency_current->value = 0.0;
	} else {
	    noegnud_options_render_features_cardboardcutout_wall_translucency_current->value =
		noegnud_options_render_features_cardboardcutout_wall_translucency_default->value;
	};
	break;
    case NOEGNUD_OPTIONS_MENU_2D_HALFWIDTH:
	if (noegnud_options_mode_tileset_halfwidth->value) {
	    noegnud_options_mode_tileset_halfwidth->value=!noegnud_options_mode_tileset_halfwidth->value;
	    noegnud_activetileset->glwidth*=2.0;
	    noegnud_activetileset->xdeltax*=2.0;
	    noegnud_activetileset->ydeltax*=2.0;
	} else {
	    noegnud_options_mode_tileset_halfwidth->value=!noegnud_options_mode_tileset_halfwidth->value;
	    noegnud_activetileset->glwidth/=2.0;
	    noegnud_activetileset->xdeltax/=2.0;
	    noegnud_activetileset->ydeltax/=2.0;
	};
	break;
    case NOEGNUD_OPTIONS_MENU_2D_WALLHALFHEIGHT:
	if (noegnud_options_render_features_cardboardcutout_wall_height->value==1) {
	    noegnud_options_render_features_cardboardcutout_wall_height->value=0.5;
	} else {
	    noegnud_options_render_features_cardboardcutout_wall_height->value=1.0;
	}
	break;

    case NOEGNUD_OPTIONS_MENU_2D_CAVERNS:
	noegnud_options_mode_tileset_caverns->value=!noegnud_options_mode_tileset_caverns->value;
        break;
    case NOEGNUD_OPTIONS_MENU_2D_CAVERNS_UNEXPLORED:
	noegnud_options_mode_tileset_caverns_unexplored->value=!noegnud_options_mode_tileset_caverns_unexplored->value;
        break;
    case NOEGNUD_OPTIONS_MENU_2D_CAVERNS_CUTOUT:
	noegnud_options_mode_tileset_caverns_cutout->value=!noegnud_options_mode_tileset_caverns_cutout->value;
        break;

    case NOEGNUD_OPTIONS_MENU_3D_THEME : noegnud_noegnud_options_displaymethod_3d(); break;


    case NOEGNUD_OPTIONS_INTERFACE_FUNCTIONMETHOD :
	noegnud_options_interface_yn_function_windowed->value=!noegnud_options_interface_yn_function_windowed->value;
	break;

    case NOEGNUD_OPTIONS_INTERFACE_CONSOLE_TIMESTAMP :
	noegnud_options_gui_window_console_timestamp->value=!noegnud_options_gui_window_console_timestamp->value;
	break;

    case NOEGNUD_OPTIONS_MENU_VIEW_RESET:
	noegnud_options_render_opengl_view_angle_y_current->value = noegnud_options_render_opengl_view_angle_y_default->value;
	noegnud_options_render_opengl_view_angle_x_current->value = noegnud_options_render_opengl_view_angle_x_default->value;
	noegnud_options_render_opengl_view_zoom_current->value = noegnud_options_render_opengl_view_zoom_default->value;
	noegnud_render_offset_x = noegnud_render_offset_y = 0.0;
	break;
    case NOEGNUD_OPTIONS_MENU_VIEW_FLATTEN:
	noegnud_options_render_opengl_view_angle_y_current->value = 90.0;
	noegnud_options_render_opengl_view_angle_x_current->value = 0.0;
	noegnud_options_render_opengl_view_zoom_current->value = 750.00;
	noegnud_render_offset_x = noegnud_render_offset_y = 0.0;
	break;
    case NOEGNUD_OPTIONS_MENU_VIEW_ROTATIONMETHOD:
	noegnud_options_mode_all_rotationabsolute->value=!noegnud_options_mode_all_rotationabsolute->value;
	break;
    case NOEGNUD_OPTIONS_MENU_VIEW_LIGHTING:
	switch (noegnud_options_render_opengl_lighting_method->value) {
	case NOEGNUD_RENDER_LIGHTING_METHOD_NONE:
	    noegnud_options_render_opengl_lighting_method->value=NOEGNUD_RENDER_LIGHTING_METHOD_CHEAP; break;
	case NOEGNUD_RENDER_LIGHTING_METHOD_CHEAP:
	    noegnud_options_render_opengl_lighting_method->value=NOEGNUD_RENDER_LIGHTING_METHOD_NONE; break;
	default: noegnud_options_render_opengl_lighting_method->value=NOEGNUD_RENDER_LIGHTING_METHOD_NONE; break;
	}
	break;
    case NOEGNUD_OPTIONS_MENU_VIEW_MIPMAPPING:
	noegnud_options_render_opengl_mipmapping->value=!noegnud_options_render_opengl_mipmapping->value;
	clearmap();
	noegnud_common_pushevent_keypress('r'-('a'-1));
	break;
    case NOEGNUD_OPTIONS_MENU_VIEW_MINIMAP:
	noegnud_options_minimap_visible->value=!noegnud_options_minimap_visible->value;
	if (noegnud_options_minimap_visible->value) {
	    noegnud_guiwidget_minimap->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_minimap;
	    noegnud_guiwidget_minimap->event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_minimap;
	} else {
	    noegnud_guiwidget_minimap->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_null;
	    noegnud_guiwidget_minimap->event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_null;
	}
	break;
    case NOEGNUD_OPTIONS_MENU_VIEW_POSITIONBAR:
	noegnud_options_positionbar_visible->value=!noegnud_options_positionbar_visible->value;
	if (noegnud_options_positionbar_visible->value) {
	    noegnud_guiwidget_positionbar->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_window;
	    noegnud_guiwidget_positionbar->event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_window;
	} else {
	    noegnud_guiwidget_positionbar->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_null;
	    noegnud_guiwidget_positionbar->event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_null;
	}
	break;
    case NOEGNUD_OPTIONS_MENU_VIEW_REFRESH:
	clearmap();
	noegnud_common_pushevent_keypress('r'-('a'-1));
	break;
    case NOEGNUD_OPTIONS_MENU_VIEW_AUTOCENTRE:
	noegnud_options_render_features_autocentre->value=!noegnud_options_render_features_autocentre->value;
	noegnud_cliparound(noegnud_clip_x+1,noegnud_clip_y);
	break;
    case NOEGNUD_OPTIONS_MENU_VIEW_AUTOCENTRE_XONLY:
	noegnud_options_render_features_autocentre_xonly->value=!noegnud_options_render_features_autocentre_xonly->value;
	noegnud_cliparound(noegnud_clip_x+1,noegnud_clip_y);
	break;
    case NOEGNUD_OPTIONS_MENU_VIEW_AUTOCENTRE_METHOD:
	noegnud_options_render_features_autocentre_mouse->value=!noegnud_options_render_features_autocentre_mouse->value;
	noegnud_cliparound(noegnud_clip_x+1,noegnud_clip_y);
	break;
    case NOEGNUD_OPTIONS_MENU_VIEW_AUTOFACE:
	noegnud_options_render_features_autoface->value=!noegnud_options_render_features_autoface->value;
	break;
    case NOEGNUD_OPTIONS_MENU_VIEW_SCREENSHOT: noegnud_screenshot(); break;



    case NOEGNUD_OPTIONS_MENU_NOEGNUD_CREDITS: noegnud_creditscreen(); break;
    case NOEGNUD_OPTIONS_MENU_NOEGNUD_HELP:
	noegnud_display_file("../" NOEGNUD_DATADIR "/news",0);
	break;
    case NOEGNUD_OPTIONS_MENU_NOEGNUD_KEYS: while (noegnud_noegnud_options_keys()) {}; break;
    case NOEGNUD_OPTIONS_MENU_NOEGNUD_3DMOUSE:
	noegnud_options_input_3dmouse->value=!noegnud_options_input_3dmouse->value;
	break;
    case NOEGNUD_OPTIONS_MENU_NOEGNUD_MOUSE_3D_LIGHT:
	noegnud_options_input_mouse_3d_light->value=!noegnud_options_input_mouse_3d_light->value;
	break;
    case NOEGNUD_OPTIONS_MENU_NOEGNUD_SAVE:
	noegnud_common_pushevent_keypress('S');
	break;
    case NOEGNUD_OPTIONS_MENU_NOEGNUD_QUIT:
	noegnud_common_pushevent_keypress('#');
	noegnud_common_pushevent_keypress('q');
	if (!iflags.extmenu) noegnud_common_pushevent_keypress('\n');
	break;


    case NOEGNUD_OPTIONS_MENU_VARIANT_OPTIONS:
	noegnud_common_pushevent_keypress('O');
	break;
    case NOEGNUD_OPTIONS_MENU_VARIANT_HELP:
	noegnud_common_pushevent_keypress('?');
	break;
    }

    noegnud_noegnud_options_busy=0;
}

int noegnud_nh_poskey_active=0;
int noegnud_nh_poskey_button;

int noegnud_nhgetch_relative_active=0;
int noegnud_nhgetch_relative_x;
int noegnud_nhgetch_relative_y;
float noegnud_nhgetch_relative_rotation_was_x;
float noegnud_nhgetch_relative_rotation_was_y;
float noegnud_nhgetch_relative_zoom_was;
float noegnud_nhgetch_relative_translation_was_x;
float noegnud_nhgetch_relative_translation_was_y;

int noegnud_nhgetch (void) {
    SDL_Event event;
    SDLMod modstate;
    int ch;
#ifdef __WIN32__
    int  win32_unicode_buffer[256];
    char win32_unicode_keystate[256];
#endif
    int render_selection, render_selection_x,render_selection_y,render_selection_button;
    //  float overlay_reduction;

    process_checkrecord ();

    //    if ((!noegnud_inventory_open)&&WIN_INVEN) ((noegnud_gui_twidget *)WIN_INVEN)->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_widget;

    if (!noegnud_graphics_system_ready) noegnud_internal_init_graphics_system ();

    /*
      overlay_reduction=0.0;

      if (Hallucination) {
      noegnud_guiwidget_desktop_overlay_hallucinating->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_window;
      overlay_reduction+=0.15;
      } else {
      noegnud_guiwidget_desktop_overlay_hallucinating->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_widget;
      }

      if (Confusion) {
      noegnud_guiwidget_desktop_overlay_confused->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_window;
      overlay_reduction+=0.15;
      } else {
      noegnud_guiwidget_desktop_overlay_confused->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_widget;
      }

      //  if (Stunned) {
      if (youmonst.data&&Stunned) {
      noegnud_guiwidget_desktop_overlay_stunned->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_window;
      overlay_reduction+=0.15;
      } else {
      noegnud_guiwidget_desktop_overlay_stunned->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_widget;
      }

      if (Sick) {
      noegnud_guiwidget_desktop_overlay_poisoned->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_window;
      overlay_reduction+=0.15;
      } else {
      noegnud_guiwidget_desktop_overlay_poisoned->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_widget;
      }

      if (youmonst.data&&Blind) {
      noegnud_guiwidget_desktop_overlay_blind->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_window;
      overlay_reduction+=0.15;
      } else {
      noegnud_guiwidget_desktop_overlay_blind->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_widget;
      }

      if (Slimed) {
      noegnud_guiwidget_desktop_overlay_slimed->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_window;
      overlay_reduction+=0.15;

      } else {
      noegnud_guiwidget_desktop_overlay_slimed->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_widget;
      }

      overlay_reduction-=0.15;
      if (overlay_reduction>=0) {
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_hallucinating)->a=1.0-overlay_reduction;
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_confused)->a=1.0-overlay_reduction;
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_stunned)->a=1.0-overlay_reduction;
      ((noegnud_gui_twindow *)noegnud_guiwidget_desktop_overlay_poisoned)->a=1.0-overlay_reduction;
      }
    */

    while (1)
	{

	    if (WIN_INVEN) {
		if (!flags.perm_invent&&(winid)noegnud_gui_active!=WIN_INVEN) {
		    ((noegnud_gui_twidget *)WIN_INVEN)->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_null;
		    ((noegnud_gui_twidget *)WIN_INVEN)->event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_null;
		    ((noegnud_gui_twidget *)WIN_INVEN)->offset_y=0;
		    ((noegnud_gui_twidget *)WIN_INVEN)->hscroll=0;
		    ((noegnud_gui_twidget *)WIN_INVEN)->vscroll=0;
		} else {
		    if (((noegnud_gui_twidget *)WIN_INVEN)->draw==(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_null) {
		    printf("*****************************\n");
		    //clive
			noegnud_clear_nhwindow(WIN_INVEN);
			}
		    ((noegnud_gui_twidget *)WIN_INVEN)->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_window;
		    ((noegnud_gui_twidget *)WIN_INVEN)->event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_window;
		}
	    }


	    noegnud_render_middle_x+=noegnud_scrolling_delta_x;
	    noegnud_render_middle_y+=noegnud_scrolling_delta_y;

	    render_selection=FALSE;
	    render_selection_button=0;
	    while (SDL_PollEvent (&event)) {

		noegnud_render_mouse_map_x=-1;

		noegnud_render_bullettime_lastevent=SDL_GetTicks();
		noegnud_render_bullettime_busy=0;
		noegnud_render_bullettime_angle=0.0;

		noegnud_guiwidget_desktop->event(noegnud_guiwidget_desktop,&event);

		if (event.type == SDL_QUIT) {
		    noegnud_common_pushevent_keypress('S');
		}
		if (!noegnud_gui_mouseoverwidget&&event.type == SDL_MOUSEBUTTONDOWN) {
		    if (noegnud_nh_poskey_active) {
			render_selection=TRUE;
			render_selection_x=event.motion.x;
			render_selection_y=event.motion.y;
			render_selection_button=event.button.button;
		    }
		    if (event.button.button == 4) {
			noegnud_options_render_opengl_view_zoom_current->value /= noegnud_options_render_opengl_view_zoom_delta->value;
			if (noegnud_options_render_opengl_view_zoom_current->value<noegnud_options_render_opengl_view_zoom_min->value)
			    noegnud_options_render_opengl_view_zoom_current->value=noegnud_options_render_opengl_view_zoom_min->value;
		    }
		    if (event.button.button == 5) {
			noegnud_options_render_opengl_view_zoom_current->value *= noegnud_options_render_opengl_view_zoom_delta->value;
			if (noegnud_options_render_opengl_view_zoom_current->value>noegnud_options_render_opengl_view_zoom_max->value)
			    noegnud_options_render_opengl_view_zoom_current->value=noegnud_options_render_opengl_view_zoom_max->value;
		    }
		}
		if (event.type == SDL_MOUSEMOTION)
		    {
			render_selection=TRUE;
			render_selection_x=event.motion.x;
			render_selection_y=event.motion.y;


			modstate = SDL_GetModState ();

			if (noegnud_options_mode_all_rotationabsolute->value&&(!noegnud_nhgetch_relative_active)) {
			    if ((modstate & KMOD_LSHIFT)||(modstate & KMOD_RSHIFT)||(noegnud_options_render_features_autocentre->value&&noegnud_options_render_features_autocentre_mouse->value)) {
				noegnud_render_offset_x =
				    (((float)event.motion.x)/((float)noegnud_options_screenwidth->value)-0.5)*(-noegnud_activetileset->glwidth);
				noegnud_render_offset_x*=((float)MAX_MAP_X);
				noegnud_render_offset_y =
				    (((float)event.motion.y)/((float)noegnud_options_screenheight->value)-0.5)*(-noegnud_activetileset->glheight);
				noegnud_render_offset_y*=((float)MAX_MAP_Y);
			    }
			    if ((modstate & KMOD_LCTRL)||(modstate & KMOD_RCTRL)) {
				noegnud_options_render_opengl_view_zoom_current->value =
				    ((((float) event.motion.y)/((float) noegnud_options_screenheight->value)))*
				    (noegnud_options_render_opengl_view_zoom_max->value-noegnud_options_render_opengl_view_zoom_min->value) +
				    noegnud_options_render_opengl_view_zoom_min->value;

			    }
			    if ((modstate & KMOD_LALT)||(modstate & KMOD_RALT)||event.motion.state&SDL_BUTTON(SDL_BUTTON_MIDDLE)) {
				noegnud_options_render_opengl_view_angle_x_current->value =
				    (((float) event.motion.x)/((float) noegnud_options_screenwidth->value)) *
				    (noegnud_options_render_opengl_view_angle_x_max->value-noegnud_options_render_opengl_view_angle_x_min->value) +
				    noegnud_options_render_opengl_view_angle_x_min->value;
				noegnud_options_render_opengl_view_angle_y_current->value =
				    (((float) event.motion.y)/((float) noegnud_options_screenheight->value)) *
				    (noegnud_options_render_opengl_view_angle_y_max->value-noegnud_options_render_opengl_view_angle_y_min->value) +
				    noegnud_options_render_opengl_view_angle_y_min->value;

			    }
			} else {


			    if ( (modstate & KMOD_LSHIFT) ||
				 (modstate & KMOD_RSHIFT) ||
				 (modstate & KMOD_LCTRL) ||
				 (modstate & KMOD_RCTRL) ||
				 (modstate & KMOD_LALT) ||
				 (modstate & KMOD_RALT)||
				 event.motion.state&SDL_BUTTON(SDL_BUTTON_MIDDLE) ||
				 ( noegnud_options_render_features_autocentre->value &&
				   noegnud_options_render_features_autocentre_mouse->value)) {

				if (noegnud_nhgetch_relative_active) {
				    if ((modstate & KMOD_LSHIFT)||(modstate & KMOD_RSHIFT)||(noegnud_options_render_features_autocentre->value&&noegnud_options_render_features_autocentre_mouse->value)) {
					if ((noegnud_options_render_features_autocentre->value&&noegnud_options_render_features_autocentre_mouse->value)) {
					    noegnud_render_offset_x =
						(((float)event.motion.x)/((float)noegnud_options_screenwidth->value)-0.5)*(-noegnud_activetileset->glwidth);
					    noegnud_render_offset_x*=((float)MAX_MAP_X);
					    noegnud_render_offset_y =
						(((float)event.motion.y)/((float)noegnud_options_screenheight->value)-0.5)*(-noegnud_activetileset->glheight);
					    noegnud_render_offset_y*=((float)MAX_MAP_Y);

					} else {
					    noegnud_render_offset_x =
						(((float)(event.motion.x))/((float)noegnud_options_screenwidth->value)-0.5)*(-noegnud_activetileset->glwidth)-
						(((float)(noegnud_nhgetch_relative_x))/((float)noegnud_options_screenwidth->value)-0.5)*(-noegnud_activetileset->glwidth);
					    noegnud_render_offset_x*=((float)MAX_MAP_X);
					    noegnud_render_offset_x+=noegnud_nhgetch_relative_translation_was_x;
					    noegnud_nhgetch_relative_translation_was_x=noegnud_render_offset_x;


					    noegnud_render_offset_y =
						(((float)event.motion.y)/((float)noegnud_options_screenheight->value)-0.5)*(-noegnud_activetileset->glheight)-
						(((float)noegnud_nhgetch_relative_y)/((float)noegnud_options_screenheight->value)-0.5)*(-noegnud_activetileset->glheight);
					    noegnud_render_offset_y*=((float)MAX_MAP_Y);
					    noegnud_render_offset_y+=noegnud_nhgetch_relative_translation_was_y;
					    noegnud_nhgetch_relative_translation_was_y=noegnud_render_offset_y;
					}
				    }

				    if ((modstate & KMOD_LCTRL)||(modstate & KMOD_RCTRL)) {
					noegnud_options_render_opengl_view_zoom_current->value +=
					    ((((float) (event.motion.y-noegnud_nhgetch_relative_y))/((float) noegnud_options_screenheight->value)))*
					    (noegnud_options_render_opengl_view_zoom_max->value-noegnud_options_render_opengl_view_zoom_min->value);

					if (noegnud_options_render_opengl_view_zoom_current->value<noegnud_options_render_opengl_view_zoom_min->value)
					    noegnud_options_render_opengl_view_zoom_current->value=noegnud_options_render_opengl_view_zoom_min->value;
					if (noegnud_options_render_opengl_view_zoom_current->value>noegnud_options_render_opengl_view_zoom_max->value)
					    noegnud_options_render_opengl_view_zoom_current->value=noegnud_options_render_opengl_view_zoom_max->value;

					noegnud_nhgetch_relative_zoom_was=noegnud_options_render_opengl_view_zoom_current->value;

				    }

				    if ((modstate & KMOD_LALT)||(modstate & KMOD_RALT)||event.motion.state&SDL_BUTTON(SDL_BUTTON_MIDDLE)) {

					noegnud_options_render_opengl_view_angle_x_current->value +=
					    (((float) (event.motion.x-noegnud_nhgetch_relative_x)*10.0) /
					      ( noegnud_options_render_opengl_view_angle_x_max->value -
						noegnud_options_render_opengl_view_angle_x_min->value));

					noegnud_options_render_opengl_view_angle_y_current->value +=
					    (((float) (event.motion.y-noegnud_nhgetch_relative_y)*10.0) /
					      ( noegnud_options_render_opengl_view_angle_y_max->value -
						noegnud_options_render_opengl_view_angle_y_min->value));

					if (noegnud_options_render_opengl_view_angle_x_current->value<noegnud_options_render_opengl_view_angle_x_min->value) noegnud_options_render_opengl_view_angle_x_current->value=noegnud_options_render_opengl_view_angle_x_min->value;
					if (noegnud_options_render_opengl_view_angle_x_current->value>noegnud_options_render_opengl_view_angle_x_max->value) noegnud_options_render_opengl_view_angle_x_current->value=noegnud_options_render_opengl_view_angle_x_max->value;
					if (noegnud_options_render_opengl_view_angle_y_current->value<noegnud_options_render_opengl_view_angle_y_min->value) noegnud_options_render_opengl_view_angle_y_current->value=noegnud_options_render_opengl_view_angle_y_min->value;
					if (noegnud_options_render_opengl_view_angle_y_current->value>noegnud_options_render_opengl_view_angle_y_max->value) noegnud_options_render_opengl_view_angle_y_current->value=noegnud_options_render_opengl_view_angle_y_max->value;

					noegnud_nhgetch_relative_rotation_was_x=noegnud_options_render_opengl_view_angle_x_current->value;
					noegnud_nhgetch_relative_rotation_was_y=noegnud_options_render_opengl_view_angle_y_current->value;

				    }

				} else {
				    noegnud_nhgetch_relative_active=1;
				    noegnud_nhgetch_relative_rotation_was_x=noegnud_options_render_opengl_view_angle_x_current->value;
				    noegnud_nhgetch_relative_rotation_was_y=noegnud_options_render_opengl_view_angle_y_current->value;
				    noegnud_nhgetch_relative_zoom_was=noegnud_options_render_opengl_view_zoom_current->value;
				    noegnud_nhgetch_relative_translation_was_x=noegnud_render_offset_x;
				    noegnud_nhgetch_relative_translation_was_y=noegnud_render_offset_y;
				}

				noegnud_nhgetch_relative_x=event.motion.x;
				noegnud_nhgetch_relative_y=event.motion.y;

			    } else {

				if (noegnud_nhgetch_relative_active) {
				    noegnud_nhgetch_relative_active=0;
				}

			    }
			}
		    }
		if (event.type == SDL_KEYDOWN)
		    {

			if (event.key.keysym.sym==noegnud_options_keys_options->value) {
			    noegnud_noegnud_options();
			} else
			if (event.key.keysym.sym==noegnud_options_keys_minimap->value) {
			    noegnud_options_minimap_visible->value=!noegnud_options_minimap_visible->value;
			    if (noegnud_options_minimap_visible->value) {
				noegnud_guiwidget_minimap->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_minimap;
				noegnud_guiwidget_minimap->event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_minimap;
			    } else {
				noegnud_guiwidget_minimap->draw=(noegnud_gui_draw_widget_proc *)noegnud_gui_draw_null;
				noegnud_guiwidget_minimap->event=(noegnud_gui_event_widget_proc *)noegnud_gui_event_null;
			    }
			} else
			if (event.key.keysym.sym==noegnud_options_keys_hidegui->value) {
			    noegnud_render_rendergui=!noegnud_render_rendergui;
			} else
			    if (event.key.keysym.sym==noegnud_options_keys_console->value) {
				if (noegnud_startup_flag) console_startmovement ();
			    } else
				{
#ifdef __WIN32__
				    if (win32_noegnud_tounicode_supported) {
					if (!event.key.keysym.scancode) {
					    ch = noegnud_internal_convert_sdlkey_nhkey
						(event.key.keysym.sym,event.key.keysym.unicode,event.key.keysym.mod,FALSE);
					} else {
					    GetKeyboardState(&win32_unicode_keystate);
					    ToUnicode(MapVirtualKey(event.key.keysym.scancode,1),
						      event.key.keysym.scancode,
						      &win32_unicode_keystate,
						      &win32_unicode_buffer,
						      128, 0 );
					    ch = noegnud_internal_convert_sdlkey_nhkey
						(event.key.keysym.sym,win32_unicode_buffer[0],event.key.keysym.mod,FALSE);
					}
				    } else {
					ch = noegnud_internal_convert_sdlkey_nhkey
					    (event.key.keysym.sym,event.key.keysym.sym,event.key.keysym.mod,TRUE);
				    }
#else
				    ch = noegnud_internal_convert_sdlkey_nhkey
					(event.key.keysym.sym,event.key.keysym.unicode,event.key.keysym.mod,FALSE);
#endif
				    noegnud_internal_convert_sdlkey_nhkey_fastwalk=(event.key.keysym.sym==SDLK_KP5);

				    if (ch) return ch;
				}
		    }
	    }
	    if (render_selection) {
		noegnud_render_selection(render_selection_x,render_selection_y);
		if (render_selection_button) {
		    if (  render_selection_button==SDL_BUTTON_LEFT ) {
			if (noegnud_render_mouse_map_x!=-1) {
			    noegnud_nh_poskey_button=CLICK_1;
			    return 0;
			}
		    }
		    if (  render_selection_button==SDL_BUTTON_RIGHT ) {
			if (noegnud_render_mouse_map_x!=-1) {
			    noegnud_nh_poskey_button=CLICK_2;
			    return 0;
			}
		    }
		}
	    }

	    process ();
	}

};

int
noegnud_nh_poskey (int *x, int *y, int *mod)
{
    int cha;

    noegnud_nh_poskey_active=1;
    noegnud_nh_poskey_button=0;
    cha=noegnud_nhgetch ();
    noegnud_nh_poskey_active=0;

    if (noegnud_nh_poskey_button) {
	*x=noegnud_render_mouse_map_x;
	*y=noegnud_render_mouse_map_y;
	*mod=noegnud_nh_poskey_button;
	return 0;
    }

    return cha;
};

void noegnud_nhbell (void) {
    printf("# noegnud_nhbell\n");
};

int noegnud_doprev_message (void) {
    printf("# noegnud_doprev_message\n");
    return 0;
};

char noegnud_yn_function (const char *ques, const char *choices, CHAR_P def) {
    char ch;
    char *ch2;
    char retch;
    char stringbuffer[2048];
    noegnud_gui_twidget *noegnud_gui_active_old;

    noegnud_yn_function_busy=1;

#ifdef NOEGNUDDEBUG
    printf("[YN_FUNCTION] %s : %s (%c)\n",ques,choices,def);
#endif

    /* this is wierd, why this should happen, fuck knows */
    if (!iflags.window_inited) noegnud_init_nhwindows(NULL,NULL);

    if (noegnud_options_interface_yn_function_windowed->value) {
	noegnud_gui_active_old=noegnud_gui_active;
	noegnud_gui_active = noegnud_gui_nh_yn_function(ques,choices,(char)def);
    } else {
	strcpy((char *)&stringbuffer,ques);
	if (choices) {
	    strcat((char *)&stringbuffer," [");
	    strcat((char *)&stringbuffer,choices);
	    strcat((char *)&stringbuffer,"]");
	}
	if (def) {
	    strcat((char *)&stringbuffer," (");
	    stringbuffer[strlen(stringbuffer)+1]=0;
	    stringbuffer[strlen(stringbuffer)]=def;
	    strcat((char *)&stringbuffer,")");
	}
	noegnud_raw_print((char *)&stringbuffer);
    }
    ch = noegnud_nhgetch ();
    if (noegnud_options_interface_yn_function_windowed->value) {
	noegnud_gui_active->kill(noegnud_gui_active);
	noegnud_gui_active=noegnud_gui_active_old;
    }

    if (choices)
	{
	    retch = def;
	    ch2 = (char *) choices;
	    while (*ch2)
		{
		    if (*ch2 == ch)
			retch = ch;
		    ch2++;
		}
	}
    else
	{
	    retch = ch;
	};
    noegnud_yn_function_busy=0;
    return retch;
};

void noegnud_getlin (const char *ques, char *input) {
    char ch;
    noegnud_gui_twidget *noegnud_gui_active_old;

#ifdef __WIN32__
    if (noegnud_getlin_askname) {
        noegnud_getlin_askname=0;
    } else {
#endif
	*input=0;
#ifdef __WIN32__
    }
#endif

    noegnud_gui_active_old=noegnud_gui_active;
    while ((ch!='\n')&&(ch!='\033')) {
	noegnud_gui_active=noegnud_gui_nh_getlin(ques,input);
	ch=noegnud_nhgetch ();
	noegnud_gui_active->kill(noegnud_gui_active);
	noegnud_gui_active=noegnud_gui_active_old;
	switch (ch) {
	case '\b':
	    if (strlen(input)>0) input[strlen(input)-1]=0;
	    break;
	case '\n':
	    break;
	case '\033':
	    *input=0;
	    break;
	default:
	    input[strlen(input)+1]=0;
	    input[strlen(input)]=ch;
	}
    }
};

int noegnud_get_ext_cmd (void) {
    /*
    iflags.extmenu=TRUE;
    return extcmd_via_menu();
    */
    char ch;
    noegnud_gui_twidget *noegnud_gui_active_old;
    char *input;
    char *input_check;
    int counter;
    int matches;
    int returnval;

    if (iflags.extmenu) return extcmd_via_menu();

    input_check=input=noegnud_mem_malloc(2048);
    strcpy(input,"#");
    input_check++;

    noegnud_gui_active_old=noegnud_gui_active;

    returnval=-1;
    while ((ch!='\n')&&(ch!='\033')) {
	noegnud_gui_active=noegnud_gui_nh_getlin("Extended Command: ",input);
	ch=noegnud_nhgetch ();
	noegnud_gui_active->kill(noegnud_gui_active);
	noegnud_gui_active=noegnud_gui_active_old;
	switch (ch) {
	case '\b':
	    if (strlen(input)>1) input[strlen(input)-1]=0;
	    break;
	case '\n':
		for (counter=0;extcmdlist[counter].ef_txt != (char *)0;counter++) {
		    if (!strcmp(input_check,extcmdlist[counter].ef_txt)) {
			returnval = counter;
			break;
		    }
		}
		if (returnval==-1) {
		    strcat(input,": unknown extended command.");
		    noegnud_raw_print(input_check);
		}
	    break;
	case '\033':
	    break;
	default:
	    input[strlen(input)+1]=0;
	    input[strlen(input)]=ch;
	    matches=1;
	    while (matches==1) {
		matches=0;
		for (counter=0;extcmdlist[counter].ef_txt != (char *)0;counter++) {
		    if (!strncmp(input_check,extcmdlist[counter].ef_txt,strlen(input_check))&&strlen(extcmdlist[counter].ef_txt)>strlen(input_check)) matches++;
		}
		if (matches==1) {
		    for (counter=0;extcmdlist[counter].ef_txt != (char *)0;counter++) {
			if (!strncmp(input_check,extcmdlist[counter].ef_txt,strlen(input_check))&&strlen(extcmdlist[counter].ef_txt)>strlen(input_check)) {
			    input[strlen(input)+1]=0;
			    input[strlen(input)]=extcmdlist[counter].ef_txt[strlen(input)-1];

			}
		    }
		}
	    }
	}
    }

    noegnud_mem_free(input);

    return returnval;

};

void noegnud_number_pad (int a) {};

void noegnud_delay_output (void) {
    int timer_start;
    timer_start=SDL_GetTicks();
    while((SDL_GetTicks()-noegnud_options_variant_delay->value)<timer_start) {
	noegnud_render_loop();
    }
};

#ifdef CHANGE_COLOR		/* the Mac uses a palette device */
void noegnud_change_color (void) {};

#ifdef MAC
void noegnud_change_background (void) {};

void set_noegnud_font_name (void) {};
#endif

void noegnud_get_color_string (void) {};
#endif

void noegnud_start_screen(void) { return; };

void noegnud_end_screen(void) { return; };

void noegnud_outrip(winid window, int how) {
    if (noegnud_options_interface_texttombstone->value) {
	genl_outrip(window,how);
    } else {
	noegnud_rip_outrip(window,how);
    }
}

void noegnud_preference_update(const char *pref) {
    genl_preference_update(pref);
}

void play_usersound(const char *filename, int volume) {
    noegnud_sound_play(filename,((float)volume)/100.0);
}

#endif /* noegnud_GRAPHICS */


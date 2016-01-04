#ifndef _NOEGNUD_INTERFACE_H_
#define _NOEGNUD_INTERFACE_H_

#include <SDL.h>
#include <SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "hack.h"
#include "dlb.h"

#include "noegnud_options.h"

#define boolean int

#define MAX_MAP_X (80-1)
#define MAX_MAP_Y (22-1)

typedef struct {
    int glyph_primary;
    int glyph_back, glyph_middle, glyph_fore;
    unsigned special_primary;
    unsigned special_fore, special_middle, special_back;
    int colour_primary;
    int colour_fore,colour_middle,colour_back;
    int ch_primary;
    int ch_back,ch_middle,ch_fore;
    int tile_primary;
    int tile_back, tile_middle, tile_fore;
    int low,high,translucent;
} noegnud_tmap_cell;

extern noegnud_optiontype_int *noegnud_screenwidth;
extern noegnud_optiontype_int *noegnud_screenheight;
extern noegnud_optiontype_int *noegnud_screenbpp;
extern noegnud_optiontype_int *noegnud_fullscreen;

void noegnud_init_nhwindows (int *argcp, char **argv);
void noegnud_player_selection (void);
void noegnud_askname (void);
void noegnud_get_nh_event (void);
void noegnud_exit_nhwindows (const char *str);
void noegnud_suspend_nhwindows (const char *str);
void noegnud_resume_nhwindows (void);
winid noegnud_create_nhwindow (int type);
void noegnud_clear_nhwindow (winid window);
void noegnud_display_nhwindow (winid window, boolean blocking);
void noegnud_destroy_nhwindow (winid window);
void noegnud_curs (winid window, int x, int y);
void noegnud_putstr (winid window, int attr, const char *str);
void noegnud_display_file (const char *fname, boolean complain);
void noegnud_start_menu (winid window);
void noegnud_add_menu (winid window,
		       int glyph,
		       const anything * identifier,
		       CHAR_P ch,
		       CHAR_P gch,
		       int attr, const char *str, boolean preselected);
void noegnud_end_menu (winid window, const char *prompt);
int noegnud_select_menu (winid window, int how, menu_item ** menu_list);
char noegnud_message_menu (CHAR_P let, int how, const char *mesg);
void noegnud_update_inventory (void);
void noegnud_mark_synch (void);
void noegnud_wait_synch (void);
#ifdef CLIPPING
void noegnud_cliparound (int x, int y);
#endif
#ifdef POSITIONBAR
void noegnud_update_positionbar (char *features);
#endif
void noegnud_print_glyph (winid window, int x, int y, int glyph);
void noegnud_raw_print (const char *str);
void noegnud_raw_print_bold (const char *str);
int noegnud_nhgetch (void);
int noegnud_nh_poskey (int *x, int *y, int *mod);
void noegnud_nhbell (void);
int noegnud_doprev_message (void);
char noegnud_yn_function (const char *ques, const char *choices, CHAR_P def);
void noegnud_getlin (const char *ques, char *input);
int noegnud_get_ext_cmd (void);
void noegnud_number_pad (int a);
void noegnud_delay_output (void);
#ifdef CHANGE_COLOR		/* the Mac uses a palette device */
void noegnud_change_color (void);
#ifdef MAC
void noegnud_change_background (void);
void set_noegnud_font_name (void);
#endif
void noegnud_get_color_string (void);
#endif
void noegnud_start_screen(void);
void noegnud_end_screen(void);
void noegnud_outrip(winid window, int how);
void noegnud_preference_update(const char *pref);

#endif /* _NOEGNUD_INTERFACE_H_ */


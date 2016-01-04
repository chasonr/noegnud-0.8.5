#include <GL/gl.h>
#include <math.h>

#include "noegnud_extern.h"
#include "noegnud_render.h"
#include "noegnud_render_char.h"
#include "noegnud_render_tile.h"

float noegnud_render_highlight_strobe;

int noegnud_render_bullettime_busy=0;
int noegnud_render_bullettime_lastevent=0;
float noegnud_render_bullettime_angle=0.0;

float noegnud_render_middle_x=MAX_MAP_X/2;
float noegnud_render_middle_y=MAX_MAP_Y/2;

float noegnud_render_offset_x = 0.0;
float noegnud_render_offset_y = 0.0;

int noegnud_render_mouse_map_x = -1;
int noegnud_render_mouse_map_y = -1;

int noegnud_render_frame_count=0;

static int noegnud_render_lastframe_time=0;

int noegnud_render_selection_busy=0;
int noegnud_render_selection_x=0;
int noegnud_render_selection_y=0;
unsigned int noegnud_render_selection_buffer[32]={0};

int noegnud_render_rendergui=1;

void noegnud_render_loop (void) {
    int fpslimiter_diff;
    int fpslimiter_min;
    int sdltimer;

    noegnud_render_frame_count++;

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (noegnud_graphics_system_ready) {

	if (noegnud_options_mode_drawmode->value!=NOEGNUD_RENDER_DRAWMODE_3D) {
	    glDisable (GL_DEPTH_TEST);
	} else {
	    glEnable (GL_DEPTH_TEST);
	}

	noegnud_render_map ();
    }

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glOrtho (0, noegnud_options_screenwidth->value, 0, noegnud_options_screenheight->value, -1,1);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

    glDisable (GL_DEPTH_TEST);
    glColor3f (1.0, 1.0, 1.0);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    noegnud_gui_mouseoverwidget=0;
    noegnud_gui_mouseoverminimap=0;

    if (noegnud_guiwidget_console&&noegnud_guiwidget_console->y>0) noegnud_guiwidget_console->y=0;

    glEnable(GL_SCISSOR_TEST);
    if (noegnud_guiwidget_desktop&&noegnud_render_rendergui) noegnud_guiwidget_desktop->draw(noegnud_guiwidget_desktop,1);
    glDisable(GL_SCISSOR_TEST);

    noegnud_gui_mouseoverwidget=noegnud_gui_mouseoverwidget||noegnud_gui_grabbed;
    if (noegnud_gui_mouseoverwidget&&(!noegnud_gui_mouseoverminimap)) noegnud_render_mouse_map_x=-1;

    sdltimer=SDL_GetTicks();

    if (sdltimer-noegnud_render_bullettime_lastevent>noegnud_options_render_bullettime_trigger->value) {
	noegnud_render_bullettime_angle=((float)(sdltimer-(noegnud_render_bullettime_lastevent+noegnud_options_render_bullettime_trigger->value)))/50;
    }

    noegnud_render_highlight_strobe=(sin(((((float)sdltimer)/5.0)*noegnud_options_render_highlight_strobe_speed->value)*3.1415926535897931/180.0)+1.0)/2.0;


    if ((fpslimiter_diff=(sdltimer-noegnud_render_lastframe_time))<(fpslimiter_min=(1000/noegnud_options_render_engine_fps_limit->value)))
	SDL_Delay(fpslimiter_min-fpslimiter_diff);

    if (noegnud_options_render_engine_fps_show->value&&noegnud_font_minimap) {
	static char fps_string[256];

	glLoadIdentity ();
	glColor4f(1.0,1.0,0.0,0.75);
	sprintf(fps_string,"%4.1f fps",1000.0/((float)((sdltimer-noegnud_render_lastframe_time))+0.001));
	noegnud_fonts_print(noegnud_gui_font,10,50,fps_string);
    }

    SDL_GL_SwapBuffers ();
    noegnud_render_lastframe_time=sdltimer;
}

void noegnud_render_map () {
    unsigned char ch2[2];
    int viewport_coords[4]={0};

    ch2[1] = 0;

    glEnable (GL_TEXTURE_2D);
    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();
    glLoadIdentity ();


    if (noegnud_render_selection_busy) {
	glSelectBuffer(32,noegnud_render_selection_buffer);
	glGetIntegerv(GL_VIEWPORT, viewport_coords);
	glRenderMode(GL_SELECT);
	gluPickMatrix(noegnud_render_selection_x, (viewport_coords[3] - noegnud_render_selection_y), 1, 1, viewport_coords);
    }



    gluPerspective (noegnud_options_render_opengl_view_fov->value,
		    (float) noegnud_options_screenwidth->value /
		    (float) noegnud_options_screenheight->value, noegnud_options_render_opengl_view_min->value, noegnud_options_render_opengl_view_max->value);

    glMatrixMode (GL_MODELVIEW);

    glPushMatrix ();
    glLoadIdentity ();

    glTranslatef ( 0, 0,
		   -noegnud_options_render_opengl_view_zoom_current->value);

    if ((noegnud_options_mode_drawmode->value==NOEGNUD_RENDER_DRAWMODE_TILE)&&(noegnud_activetileset&&noegnud_activetileset->iso)) {
	glRotatef (90, 1, 0, 0);
	glRotatef (noegnud_options_render_opengl_view_angle_x_current->value, 0, 1, 0);
    } else {
	glRotatef (noegnud_options_render_opengl_view_angle_y_current->value, 1, 0, 0);
	glRotatef (noegnud_options_render_opengl_view_angle_x_current->value+noegnud_render_bullettime_angle, 0, 1, 0);
    }


    glPushMatrix ();
    glDisable (GL_BLEND);

    glInitNames();
    glPushName(0);

    switch(noegnud_options_mode_drawmode->value) {
    case NOEGNUD_RENDER_DRAWMODE_CHAR:
	noegnud_render_map_char ();
	break;
    case NOEGNUD_RENDER_DRAWMODE_TILE:
	if (noegnud_activetileset) noegnud_render_map_tile ();
	break;
    }

    glPopMatrix ();
    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix ();
    glDisable (GL_TEXTURE_2D);
}


void noegnud_render_selection(int x, int y) {
    int tiles_selected=0;

    if ((!noegnud_options_input_3dmouse->value)||noegnud_gui_mouseoverwidget||noegnud_gui_mouseoverminimap) {
	if (!noegnud_gui_mouseoverminimap) noegnud_render_mouse_map_x=-1;
	return;
    }

    noegnud_render_selection_busy=1;
    noegnud_render_selection_x=x;
    noegnud_render_selection_y=y;

    noegnud_render_map();

    glMatrixMode(GL_PROJECTION);
    tiles_selected=glRenderMode(GL_RENDER);

    if (tiles_selected) {

	unsigned int lowest_depth = noegnud_render_selection_buffer[1];
	int selected_object = noegnud_render_selection_buffer[3];
	int i;

	for(i = 1; i < tiles_selected; i++) {
	    if(noegnud_render_selection_buffer[(i * 4) + 1] < lowest_depth) {
		lowest_depth = noegnud_render_selection_buffer[(i * 4) + 1];
		selected_object = noegnud_render_selection_buffer[(i * 4) + 3];
	    }
	}
	noegnud_render_selection_busy=0;
	noegnud_render_mouse_map_x=(selected_object%256)+1;
	noegnud_render_mouse_map_y=selected_object/256;
	return;
    };
    noegnud_render_selection_busy=0;
    noegnud_render_mouse_map_x=-1;
    noegnud_render_mouse_map_y=-1;
    return;

}

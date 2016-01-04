#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

#include "noegnud_config.h"
#include "noegnud_options.h"
#include "noegnud_typeloader.h"
#include "noegnud_collection.h"
#include "noegnud_interface.h"
#include "noegnud_render.h"
#include "noegnud_extern.h"
#include "noegnud_mem.h"
#include "noegnud_common.h"

noegnud_optiontype_int		*noegnud_options_screenwidth=NULL;
noegnud_optiontype_int		*noegnud_options_screenheight=NULL;
noegnud_optiontype_int		*noegnud_options_screenbpp=NULL;
noegnud_optiontype_int		*noegnud_options_fullscreen=NULL;

noegnud_optiontype_int		*noegnud_options_keys_options=NULL;
noegnud_optiontype_int		*noegnud_options_keys_console=NULL;
noegnud_optiontype_int		*noegnud_options_keys_minimap=NULL;
noegnud_optiontype_int		*noegnud_options_keys_hidegui=NULL;
noegnud_optiontype_int		*noegnud_options_key_repeat_delay=NULL;
noegnud_optiontype_int		*noegnud_options_key_repeat_interval=NULL;
noegnud_optiontype_int		*noegnud_options_key_assignment_traditional=NULL;

noegnud_optiontype_int		*noegnud_options_variant_delay=NULL;

noegnud_optiontype_int    *noegnud_options_input_3dmouse=NULL;
noegnud_optiontype_int    *noegnud_options_input_mouse_3d_light=NULL;

noegnud_optiontype_int    *noegnud_options_input_mouse_gui_scrollspeed=NULL;

noegnud_optiontype_int		*noegnud_options_interface_yn_function_windowed=NULL;
noegnud_optiontype_int		*noegnud_options_interface_texttombstone=NULL;

noegnud_optiontype_int    *noegnud_options_gui_window_credits_display=NULL;

noegnud_optiontype_int		*noegnud_options_console_gap=NULL;
noegnud_optiontype_float	*noegnud_options_console_speed=NULL;

noegnud_optiontype_int	  *noegnud_options_gui_window_console_timestamp=NULL;
noegnud_optiontype_rgb	  *noegnud_options_gui_window_console_text_colour_system=NULL;
noegnud_optiontype_rgb	  *noegnud_options_gui_window_console_text_colour_timestamp=NULL;
noegnud_optiontype_rgb	  *noegnud_options_gui_window_console_text_colour_default=NULL;

noegnud_optiontype_int		*noegnud_options_mode_drawmode=NULL;
noegnud_optiontype_int    *noegnud_options_mode_all_rotationabsolute=NULL;

noegnud_optiontype_string	*noegnud_options_mode_tileset_tile=NULL;
noegnud_optiontype_int		*noegnud_options_mode_tileset_forcealpha=NULL;
noegnud_optiontype_float	*noegnud_options_mode_tileset_iso_opacity=NULL;
noegnud_optiontype_int	  *noegnud_options_mode_tileset_halfwidth=NULL;

noegnud_optiontype_int	  *noegnud_options_mode_tileset_caverns=NULL;
noegnud_optiontype_int	  *noegnud_options_mode_tileset_caverns_unexplored=NULL;
noegnud_optiontype_int	  *noegnud_options_mode_tileset_caverns_cutout=NULL;

noegnud_optiontype_string	*noegnud_options_mode_char_font=NULL;

noegnud_optiontype_string	*noegnud_options_gui_font=NULL;
noegnud_optiontype_string	*noegnud_options_gui_texture=NULL;

noegnud_optiontype_string	*noegnud_options_gui_text_colouring=NULL;

noegnud_optiontype_int		*noegnud_options_gui_widgetborder=NULL;

noegnud_optiontype_string	*noegnud_options_minimap_font=NULL;
noegnud_optiontype_int		*noegnud_options_minimap_visible=NULL;

noegnud_optiontype_int		*noegnud_options_positionbar_visible=NULL;

noegnud_optiontype_float	*noegnud_options_render_multiplier=NULL;

noegnud_optiontype_int		*noegnud_options_render_engine_fps_limit=NULL;
noegnud_optiontype_int    *noegnud_options_render_engine_fps_show=NULL;

noegnud_optiontype_float	*noegnud_options_render_opengl_view_min=NULL;
noegnud_optiontype_float	*noegnud_options_render_opengl_view_max=NULL;

noegnud_optiontype_float	*noegnud_options_render_opengl_view_fov=NULL;

noegnud_optiontype_float	*noegnud_options_render_opengl_view_zoom_default=NULL;
noegnud_optiontype_float	*noegnud_options_render_opengl_view_zoom_current=NULL;
noegnud_optiontype_float	*noegnud_options_render_opengl_view_zoom_delta=NULL;
noegnud_optiontype_float	*noegnud_options_render_opengl_view_zoom_min=NULL;
noegnud_optiontype_float	*noegnud_options_render_opengl_view_zoom_max=NULL;

noegnud_optiontype_float 	*noegnud_options_render_opengl_view_angle_y_default=NULL;
noegnud_optiontype_float	*noegnud_options_render_opengl_view_angle_y_current=NULL;
noegnud_optiontype_float	*noegnud_options_render_opengl_view_angle_y_min=NULL;
noegnud_optiontype_float	*noegnud_options_render_opengl_view_angle_y_max=NULL;

noegnud_optiontype_float	*noegnud_options_render_opengl_view_angle_x_default=NULL;
noegnud_optiontype_float	*noegnud_options_render_opengl_view_angle_x_current=NULL;
noegnud_optiontype_float	*noegnud_options_render_opengl_view_angle_x_min=NULL;
noegnud_optiontype_float	*noegnud_options_render_opengl_view_angle_x_max=NULL;

noegnud_optiontype_int		*noegnud_options_render_opengl_lighting_method=NULL;

noegnud_optiontype_int		*noegnud_options_render_opengl_mipmapping=NULL;

noegnud_optiontype_int		*noegnud_options_fun_recording_ask=NULL;

noegnud_optiontype_int    *noegnud_options_opengl_texture_max=NULL;

noegnud_optiontype_int		*noegnud_options_render_features_autocentre=NULL;
noegnud_optiontype_int		*noegnud_options_render_features_autocentre_xonly=NULL;
noegnud_optiontype_int		*noegnud_options_render_features_autocentre_mouse=NULL;

noegnud_optiontype_int		*noegnud_options_render_features_autoface=NULL;

noegnud_optiontype_int		*noegnud_options_render_features_cardboardcutout=NULL;
noegnud_optiontype_int		*noegnud_options_render_features_cardboardcutout_raise_monsters=NULL;
noegnud_optiontype_int		*noegnud_options_render_features_cardboardcutout_raise_objects=NULL;
noegnud_optiontype_int		*noegnud_options_render_features_cardboardcutout_raise_walls=NULL;
noegnud_optiontype_float	*noegnud_options_render_features_cardboardcutout_wall_height=NULL;
noegnud_optiontype_float	*noegnud_options_render_features_cardboardcutout_hole_depth=NULL;
noegnud_optiontype_float	*noegnud_options_render_features_cardboardcutout_liquid_depth=NULL;
noegnud_optiontype_float	*noegnud_options_render_features_cardboardcutout_wall_translucency_current=NULL;
noegnud_optiontype_float	*noegnud_options_render_features_cardboardcutout_wall_translucency_default=NULL;

noegnud_optiontype_float	*noegnud_options_render_highlight_strobe_speed=NULL;
noegnud_optiontype_float	*noegnud_options_render_highlight_block_opacity=NULL;
noegnud_optiontype_int		*noegnud_options_render_bullettime_trigger=NULL;

noegnud_optiontype_string	*noegnud_options_language=NULL;

noegnud_tcollection *noegnud_options=NULL;

static void noegnud_options_defaults();

void noegnud_options_init() {


    noegnud_options_defaults();
    noegnud_options_load();

    atexit(noegnud_options_done);
}
void noegnud_options_done() {
    noegnud_tcollection *stepcoll;

    noegnud_options_save();
    stepcoll=noegnud_options;
    while (stepcoll) {
	noegnud_mem_free(stepcoll->data);
	stepcoll=stepcoll->next;
    }
    noegnud_collection_destroy(&noegnud_options);
}

int noegnud_options_build_incompatibility=0;
static void noegnud_options_loadfrom_parser(char *setting, int nparams, char *params, void *data) {
    noegnud_optiontype *option;
    int c;
    char tmpstring[2048];
    char *paramp;

    if (noegnud_options_build_incompatibility) return;

    if ((option=noegnud_collection_data(noegnud_options,setting))&&(option->perm)) {
	switch (option->type) {
	case NOEGNUD_OPTIONTYPE_INT:
	    ((noegnud_optiontype_int *)option)->value=atoi(params);
	    break;
	case NOEGNUD_OPTIONTYPE_FLOAT:
	    ((noegnud_optiontype_float *)option)->value=atof(params);
	    break;
	case NOEGNUD_OPTIONTYPE_STRING:
	    if ((!strcmp(setting,"noegnud.version"))&&(strcmp(params,NOEGNUD_VERSION))) {
		printf("[WARNING] Build incompatibility. current [%s], but in config [%s], reinjecting default config...\n",NOEGNUD_BUILD,params);
		noegnud_options_build_incompatibility=1;
	    } else
	    if ((!strcmp(setting,VARIANT_FULLNAME ".noegnud.build"))&&(strcmp(params,NOEGNUD_BUILD))) {
		printf("[WARNING] Build incompatibility. current [%s], but in config [%s], reinjecting default config...\n",NOEGNUD_BUILD,params);
		noegnud_options_build_incompatibility=1;
	    } else {
		strcpy((char *)(&((noegnud_optiontype_string *)option)->value),params);
	    }
	    break;
	case NOEGNUD_OPTIONTYPE_RGB:
	    if (nparams==3) {
		((noegnud_optiontype_rgb *)option)->r=atof(params);
		while (*params) params++; params++;
		((noegnud_optiontype_rgb *)option)->g=atof(params);
		while (*params) params++; params++;
		((noegnud_optiontype_rgb *)option)->b=atof(params);
		while (*params) params++; params++;
	    } else {
		printf("[WARNING] option \"%s\" is type RGB, but has incorrect parameter amount\n",setting);
		tmpstring[0]=0;
		paramp=params;
		for(c=0;c<nparams;c++) {
		    strcat(tmpstring,paramp);
		    if (c!=nparams-1) strcat(tmpstring,	",");
		    while (*paramp++) {};
		}
		noegnud_collection_add(noegnud_options,setting,noegnud_options_create_string(NOEGNUD_OPTIONPERM_CONFIGCHANGE,tmpstring));
	    }
	    break;
	}
    } else {
#ifdef NOEGNUDDEBUG
	printf("WARNING: unknown option [%s] encountered.\n",setting);
#endif
	tmpstring[0]=0;
	paramp=params;
	for(c=0;c<nparams;c++) {
	    strcat(tmpstring,paramp);
	    if (c!=nparams-1) strcat(tmpstring,	",");
	    while (*paramp++) {};
	}
	noegnud_collection_add(noegnud_options,setting,noegnud_options_create_string(NOEGNUD_OPTIONPERM_CONFIGCHANGE,tmpstring));
    }
}

char *noegnud_options_get_home() {
    char *directory;
    /* *unix */
    if (getenv("HOME")) {
	directory=noegnud_mem_malloc(strlen(getenv("HOME")) + 1 + 1 + sizeof(NOEGNUD_HOMECONFIG_DIR) + 1);
	sprintf(directory,"%s/%s/",getenv("HOME"),NOEGNUD_HOMECONFIG_DIR);
    }
    /* win2k */
    else if (getenv("USERPROFILE")) {

	directory=noegnud_mem_malloc(strlen(getenv("USERPROFILE")) + 1 + 1 + sizeof(NOEGNUD_HOMECONFIG_DIR) + 1);
	sprintf(directory,"%s/%s/",getenv("USERPROFILE"),NOEGNUD_HOMECONFIG_DIR);
    }
    /* fuck knows */
    else {
	directory=noegnud_mem_malloc(1 + 1 + sizeof(NOEGNUD_HOMECONFIG_DIR) + 1);
	sprintf(directory,"/%s/",NOEGNUD_HOMECONFIG_DIR);
    }
#ifdef __WIN32__
    mkdir(directory);
#else
    mkdir(directory,S_IRWXU); /* i wonder why this doesn't set the exec part ? */
    chmod(directory,S_IRWXU);
#endif
    return directory;
};


void noegnud_options_load() {
    char *homedir;
    char *homedir_configfile;
    noegnud_optiontype_string *tombstone_theme;
    char filename[2048];

    printf("loading config from: %s\n","../global.config");
    noegnud_typeloader_parsefile(noegnud_options_loadfrom_parser,"global.config",NULL);

    homedir= noegnud_options_get_home();
    homedir_configfile=noegnud_mem_malloc(strlen(homedir)+sizeof(NOEGNUD_HOMECONFIG_FILENAME)+1);
    sprintf(homedir_configfile,"%s%s",homedir,NOEGNUD_HOMECONFIG_FILENAME);
    printf("loading config from: %s\n",homedir_configfile);
    noegnud_typeloader_parsefile(noegnud_options_loadfrom_parser,homedir_configfile,NULL);
    noegnud_options_build_incompatibility=FALSE;
    tombstone_theme=noegnud_collection_data(noegnud_options,"gui.window.tombstone.theme");

    sprintf(filename,"%s/%s",homedir,tombstone_theme->value);
    if (!noegnud_common_file_exists(filename)) sprintf(filename,"../" NOEGNUD_DATADIR "/%s",tombstone_theme->value);
    printf("loading sub-config(tomb) from: %s\n",filename);
    noegnud_typeloader_parsefile(noegnud_options_loadfrom_parser,filename,NULL);

    noegnud_mem_free(homedir_configfile);
    noegnud_mem_free(homedir);
}

void noegnud_options_save() {
    noegnud_tcollection *save_options;
    char *homedir;
    char *homedir_configfile;
    FILE *savefile;
    noegnud_optiontype *option;

    homedir= noegnud_options_get_home();
    homedir_configfile=noegnud_mem_malloc(strlen(homedir)+sizeof(NOEGNUD_HOMECONFIG_FILENAME)+1);
    sprintf(homedir_configfile,"%s%s",homedir,NOEGNUD_HOMECONFIG_FILENAME);
    printf("saving config to: %s\n",homedir_configfile);

    save_options=noegnud_options;
    savefile=fopen(homedir_configfile,"w");
    while (save_options) {
	option=(noegnud_optiontype *)save_options->data;
	if (option->perm&&(!(option->perm&NOEGNUD_OPTIONPERM_DONTSAVETHIS))) {
	    fprintf(savefile,"%s=",save_options->name);
	    switch (option->type) {
	    case NOEGNUD_OPTIONTYPE_INT:
		fprintf(savefile,"%d",((noegnud_optiontype_int *)option)->value);
		break;
	    case NOEGNUD_OPTIONTYPE_FLOAT:
		fprintf(savefile,"%f",((noegnud_optiontype_float *)option)->value);
		break;
	    case NOEGNUD_OPTIONTYPE_STRING:
		fprintf(savefile,"%s",(char *)&((noegnud_optiontype_string *)option)->value);
		break;
	    case NOEGNUD_OPTIONTYPE_RGB:
		fprintf( savefile,
			 "%f,%f,%f",
			 ((noegnud_optiontype_rgb *)option)->r,
			 ((noegnud_optiontype_rgb *)option)->g,
			 ((noegnud_optiontype_rgb *)option)->b);
		break;
	    }
	    fprintf(savefile,"\n");
	}
	save_options=save_options->next;
    }
    fclose(savefile);

    noegnud_mem_free(homedir_configfile);
    noegnud_mem_free(homedir);
}


#define noegnud_options_defadd_int(_n,_p,_v,_v2) noegnud_collection_add(noegnud_options,_n,noegnud_options_create_int(_p,_v)); _v2=noegnud_collection_data(noegnud_options,_n)
#define noegnud_options_defadd_float(_n,_p,_v,_v2) noegnud_collection_add(noegnud_options,_n,noegnud_options_create_float(_p,_v)); _v2=noegnud_collection_data(noegnud_options,_n)
#define noegnud_options_defadd_string(_n,_p,_v,_v2) noegnud_collection_add(noegnud_options,_n,noegnud_options_create_string(_p,_v)); _v2=noegnud_collection_data(noegnud_options,_n)
#define noegnud_options_defadd_rgb(_n,_p,_vr,_vg,_vb,_v2) noegnud_collection_add(noegnud_options,_n,noegnud_options_create_rgb(_p,_vr,_vg,_vb)); _v2=noegnud_collection_data(noegnud_options,_n)

static void noegnud_options_defaults() {
    noegnud_options=noegnud_collection_create("noegnud.name",noegnud_options_create_string(NOEGNUD_OPTIONPERM_NONE,NOEGNUD_NAME));
    noegnud_collection_add(noegnud_options,"noegnud.version",noegnud_options_create_string(NOEGNUD_OPTIONPERM_NONE,NOEGNUD_VERSION));

    noegnud_collection_add(noegnud_options,VARIANT_FULLNAME ".noegnud.build",noegnud_options_create_string(NOEGNUD_OPTIONPERM_CONFIGCHANGE,NOEGNUD_BUILD));

    noegnud_options_defadd_int("screen.width",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1024,noegnud_options_screenwidth);
    noegnud_options_defadd_int("screen.height",NOEGNUD_OPTIONPERM_CONFIGCHANGE,768,noegnud_options_screenheight);
    noegnud_options_defadd_int("screen.bpp",NOEGNUD_OPTIONPERM_CONFIGCHANGE,32,noegnud_options_screenbpp);

#ifdef __WIN32__
    noegnud_options_defadd_int("screen.fullscreen",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1,noegnud_options_fullscreen);
#else
    noegnud_options_defadd_int("screen.fullscreen",NOEGNUD_OPTIONPERM_CONFIGCHANGE,0,noegnud_options_fullscreen);
#endif

    noegnud_options_defadd_int("input.keyboard.mapping.options",NOEGNUD_OPTIONPERM_CONFIGCHANGE,SDLK_F10,noegnud_options_keys_options);
    noegnud_options_defadd_int("input.keyboard.mapping.console",NOEGNUD_OPTIONPERM_CONFIGCHANGE,SDLK_BACKQUOTE,noegnud_options_keys_console);
    noegnud_options_defadd_int("input.keyboard.mapping.minimap",NOEGNUD_OPTIONPERM_CONFIGCHANGE,SDLK_TAB,noegnud_options_keys_minimap);
    noegnud_options_defadd_int("input.keyboard.mapping.hidegui",NOEGNUD_OPTIONPERM_CONFIGCHANGE,SDLK_F5,noegnud_options_keys_hidegui);

    noegnud_options_defadd_int("nethack.delay",NOEGNUD_OPTIONPERM_CONFIGCHANGE,50,noegnud_options_variant_delay);

    noegnud_options_defadd_int("input.keyboard.repeatrate.delay",NOEGNUD_OPTIONPERM_CONFIGCHANGE,500,noegnud_options_key_repeat_delay);
    noegnud_options_defadd_int("input.keyboard.repeatrate.interval",NOEGNUD_OPTIONPERM_CONFIGCHANGE,100,noegnud_options_key_repeat_interval);

    noegnud_options_defadd_int("input.keyboard.assignment.traditional",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1,noegnud_options_key_assignment_traditional);

    noegnud_options_defadd_int("input.mouse.3d.cursor",NOEGNUD_OPTIONPERM_CONFIGCHANGE,TRUE,noegnud_options_input_3dmouse);
    noegnud_options_defadd_int("input.mouse.3d.light",NOEGNUD_OPTIONPERM_CONFIGCHANGE,TRUE,noegnud_options_input_mouse_3d_light);

    noegnud_options_defadd_int("input.mouse.gui.scrollspeed",NOEGNUD_OPTIONPERM_CONFIGCHANGE,30,noegnud_options_input_mouse_gui_scrollspeed);

    noegnud_options_defadd_int("interface.yn_function.windowed",NOEGNUD_OPTIONPERM_CONFIGCHANGE,TRUE,noegnud_options_interface_yn_function_windowed);
    noegnud_options_defadd_int("interface.texttombstone",NOEGNUD_OPTIONPERM_CONFIGCHANGE,FALSE,noegnud_options_interface_texttombstone);
    noegnud_options_defadd_string("gui.default.font",NOEGNUD_OPTIONPERM_CONFIGCHANGE,"char/deco",noegnud_options_gui_font);
    noegnud_options_defadd_string("gui.default.texture",NOEGNUD_OPTIONPERM_CONFIGCHANGE,"gui/back.png",noegnud_options_gui_texture);

    noegnud_options_defadd_string("gui.text.colouring",NOEGNUD_OPTIONPERM_CONFIGCHANGE,"default",noegnud_options_gui_text_colouring);

    noegnud_options_defadd_int("gui.default.widgetborder",NOEGNUD_OPTIONPERM_CONFIGCHANGE,0,noegnud_options_gui_widgetborder);

    noegnud_options_defadd_int("gui.window.console.gap",NOEGNUD_OPTIONPERM_CONFIGCHANGE,96,noegnud_options_console_gap);
    noegnud_options_defadd_float("gui.window.console.speed",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1.0,noegnud_options_console_speed);
    noegnud_options_defadd_rgb( "gui.window.console.text.colour.system",
				NOEGNUD_OPTIONPERM_CONFIGCHANGE,
			        0.2,0.0,0.0,
				noegnud_options_gui_window_console_text_colour_system);
    noegnud_options_defadd_rgb( "gui.window.console.text.colour.timestamp",
				NOEGNUD_OPTIONPERM_CONFIGCHANGE,
				0.1, 0.3, 0.1,
				noegnud_options_gui_window_console_text_colour_timestamp);
    noegnud_options_defadd_rgb( "gui.window.console.text.colour.default",
				NOEGNUD_OPTIONPERM_CONFIGCHANGE,
				0.0, 0.0, 0.0,
				noegnud_options_gui_window_console_text_colour_default);
    noegnud_options_defadd_int("gui.window.console.timestamp",NOEGNUD_OPTIONPERM_CONFIGCHANGE,0,noegnud_options_gui_window_console_timestamp);

    noegnud_options_defadd_string("gui.window.minimap.font",NOEGNUD_OPTIONPERM_CONFIGCHANGE,"minimap-fonts/default",noegnud_options_minimap_font);
    noegnud_options_defadd_int("gui.window.minimap.visible",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1,noegnud_options_minimap_visible);

    noegnud_options_defadd_int("gui.window.positionbar.visible",NOEGNUD_OPTIONPERM_CONFIGCHANGE,0,noegnud_options_positionbar_visible);

    noegnud_options_defadd_int("gui.window.credits.display",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1,noegnud_options_gui_window_credits_display);

    noegnud_collection_add(noegnud_options,"gui.window.tombstone.theme",noegnud_options_create_string(NOEGNUD_OPTIONPERM_CONFIGCHANGE,"tomb/theme_traditional.inf"));

    noegnud_collection_add(noegnud_options,"gui.window.tombstone.image",noegnud_options_create_string(NOEGNUD_OPTIONPERM_CONFIGCHANGE|NOEGNUD_OPTIONPERM_DONTSAVETHIS,""));
    noegnud_collection_add(noegnud_options,"gui.window.tombstone.font",noegnud_options_create_string(NOEGNUD_OPTIONPERM_CONFIGCHANGE|NOEGNUD_OPTIONPERM_DONTSAVETHIS,""));
    noegnud_collection_add(noegnud_options,"gui.window.tombstone.width",noegnud_options_create_int(NOEGNUD_OPTIONPERM_CONFIGCHANGE|NOEGNUD_OPTIONPERM_DONTSAVETHIS,23));
    noegnud_collection_add(noegnud_options,"gui.window.tombstone.name.x",noegnud_options_create_int(NOEGNUD_OPTIONPERM_CONFIGCHANGE|NOEGNUD_OPTIONPERM_DONTSAVETHIS,0));
    noegnud_collection_add(noegnud_options,"gui.window.tombstone.name.y",noegnud_options_create_int(NOEGNUD_OPTIONPERM_CONFIGCHANGE|NOEGNUD_OPTIONPERM_DONTSAVETHIS,0));
    noegnud_collection_add(noegnud_options,"gui.window.tombstone.year.x",noegnud_options_create_int(NOEGNUD_OPTIONPERM_CONFIGCHANGE|NOEGNUD_OPTIONPERM_DONTSAVETHIS,0));
    noegnud_collection_add(noegnud_options,"gui.window.tombstone.year.y",noegnud_options_create_int(NOEGNUD_OPTIONPERM_CONFIGCHANGE|NOEGNUD_OPTIONPERM_DONTSAVETHIS,0));
    noegnud_collection_add(noegnud_options,"gui.window.tombstone.gold.x",noegnud_options_create_int(NOEGNUD_OPTIONPERM_CONFIGCHANGE|NOEGNUD_OPTIONPERM_DONTSAVETHIS,0));
    noegnud_collection_add(noegnud_options,"gui.window.tombstone.gold.y",noegnud_options_create_int(NOEGNUD_OPTIONPERM_CONFIGCHANGE|NOEGNUD_OPTIONPERM_DONTSAVETHIS,0));
    noegnud_collection_add(noegnud_options,"gui.window.tombstone.how.x",noegnud_options_create_int(NOEGNUD_OPTIONPERM_CONFIGCHANGE|NOEGNUD_OPTIONPERM_DONTSAVETHIS,0));
    noegnud_collection_add(noegnud_options,"gui.window.tombstone.how.y",noegnud_options_create_int(NOEGNUD_OPTIONPERM_CONFIGCHANGE|NOEGNUD_OPTIONPERM_DONTSAVETHIS,0));
    noegnud_collection_add(noegnud_options,"gui.window.tombstone.description.x",noegnud_options_create_int(NOEGNUD_OPTIONPERM_CONFIGCHANGE|NOEGNUD_OPTIONPERM_DONTSAVETHIS,0));
    noegnud_collection_add(noegnud_options,"gui.window.tombstone.description.y",noegnud_options_create_int(NOEGNUD_OPTIONPERM_CONFIGCHANGE|NOEGNUD_OPTIONPERM_DONTSAVETHIS,0));
    noegnud_collection_add(noegnud_options,"gui.window.tombstone.tomb.colour",noegnud_options_create_rgb(NOEGNUD_OPTIONPERM_CONFIGCHANGE|NOEGNUD_OPTIONPERM_DONTSAVETHIS,1.0,1.0,1.0));
    noegnud_collection_add(noegnud_options,"gui.window.tombstone.text.colour",noegnud_options_create_rgb(NOEGNUD_OPTIONPERM_CONFIGCHANGE|NOEGNUD_OPTIONPERM_DONTSAVETHIS,1.0,1.0,1.0));



    noegnud_options_defadd_int("drawmode",NOEGNUD_OPTIONPERM_CONFIGCHANGE,NOEGNUD_RENDER_DRAWMODE_TILE,noegnud_options_mode_drawmode);

    noegnud_options_defadd_int("drawmode.all.rotation.absolute",NOEGNUD_OPTIONPERM_CONFIGCHANGE,0,noegnud_options_mode_all_rotationabsolute);

    noegnud_options_defadd_string("drawmode.2d." VARIANT_FULLNAME ".tileset",NOEGNUD_OPTIONPERM_CONFIGCHANGE,"nethack",noegnud_options_mode_tileset_tile);

    noegnud_options_defadd_int("drawmode.2d.forcealpha",NOEGNUD_OPTIONPERM_CONFIGCHANGE,0,noegnud_options_mode_tileset_forcealpha);
    noegnud_options_defadd_float("drawmode.2d.isometric.opacity",NOEGNUD_OPTIONPERM_CONFIGCHANGE,0.5,noegnud_options_mode_tileset_iso_opacity);
    noegnud_options_defadd_int("drawmode.2d.halfwidth",NOEGNUD_OPTIONPERM_CONFIGCHANGE,0,noegnud_options_mode_tileset_halfwidth);

    noegnud_options_defadd_int("drawmode.2d.caverns",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1,noegnud_options_mode_tileset_caverns);
    noegnud_options_defadd_int("drawmode.2d.caverns.unexplored",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1,noegnud_options_mode_tileset_caverns_unexplored);
    noegnud_options_defadd_int("drawmode.2d.caverns.cutout",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1,noegnud_options_mode_tileset_caverns_cutout);


    noegnud_options_defadd_int("drawmode.2d.cutout",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1,noegnud_options_render_features_cardboardcutout);

    noegnud_options_defadd_int("drawmode.2d.cutout.raise.monsters",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1,noegnud_options_render_features_cardboardcutout_raise_monsters);
    noegnud_options_defadd_int("drawmode.2d.cutout.raise.objects",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1,noegnud_options_render_features_cardboardcutout_raise_objects);
    noegnud_options_defadd_int("drawmode.2d.cutout.raise.walls",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1,noegnud_options_render_features_cardboardcutout_raise_walls);

    noegnud_options_defadd_float("drawmode.2d.cutout.wall.height",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1.0,noegnud_options_render_features_cardboardcutout_wall_height);
    noegnud_options_defadd_float( "drawmode.2d.cutout.wall.translucency.default",
				  NOEGNUD_OPTIONPERM_CONFIGCHANGE,
				  0.5,
				  noegnud_options_render_features_cardboardcutout_wall_translucency_default);
    noegnud_options_defadd_float( "drawmode.2d.cutout.wall.translucency.current",
				  NOEGNUD_OPTIONPERM_CONFIGCHANGE,
				  0.0,
				  noegnud_options_render_features_cardboardcutout_wall_translucency_current);

    noegnud_options_defadd_float("drawmode.2d.cutout.hole.depth",NOEGNUD_OPTIONPERM_CONFIGCHANGE,0.5,noegnud_options_render_features_cardboardcutout_hole_depth);
    noegnud_options_defadd_float("drawmode.2d.cutout.liquid.depth",NOEGNUD_OPTIONPERM_CONFIGCHANGE,0.8,noegnud_options_render_features_cardboardcutout_liquid_depth);
    noegnud_options_defadd_int("drawmode.2d.cutout.autoface",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1,noegnud_options_render_features_autoface);





    noegnud_options_defadd_string("drawmode.text.font",NOEGNUD_OPTIONPERM_CONFIGCHANGE,"char/nethack",noegnud_options_mode_char_font);



    noegnud_options_defadd_float("engine.multiplier",NOEGNUD_OPTIONPERM_CONFIGCHANGE,10.0,noegnud_options_render_multiplier);

    noegnud_options_defadd_int("engine.fps.limit",NOEGNUD_OPTIONPERM_CONFIGCHANGE,50,noegnud_options_render_engine_fps_limit);
#ifdef NOEGNUDDEBUG
    noegnud_options_defadd_int("engine.fps.show",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1,noegnud_options_render_engine_fps_show);
#else
    noegnud_options_defadd_int("engine.fps.show",NOEGNUD_OPTIONPERM_CONFIGCHANGE,0,noegnud_options_render_engine_fps_show);
#endif

    noegnud_options_defadd_float("opengl.view.min",NOEGNUD_OPTIONPERM_CONFIGCHANGE,10.0,noegnud_options_render_opengl_view_min);
    noegnud_options_defadd_float("opengl.view.max",NOEGNUD_OPTIONPERM_CONFIGCHANGE,3000.0,noegnud_options_render_opengl_view_max);
    noegnud_options_defadd_float("opengl.view.fov",NOEGNUD_OPTIONPERM_CONFIGCHANGE,45.0,noegnud_options_render_opengl_view_fov);
    noegnud_options_defadd_int("opengl.mipmapping",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1,noegnud_options_render_opengl_mipmapping);
    noegnud_options_defadd_int("opengl.texture.max",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1024,noegnud_options_opengl_texture_max);

    noegnud_options_defadd_float("engine.zoom.default",NOEGNUD_OPTIONPERM_CONFIGCHANGE,400.0,noegnud_options_render_opengl_view_zoom_default);
    noegnud_options_defadd_float("engine.zoom.current",NOEGNUD_OPTIONPERM_CONFIGCHANGE,400.0,noegnud_options_render_opengl_view_zoom_current);
    noegnud_options_defadd_float("engine.zoom.delta",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1.15,noegnud_options_render_opengl_view_zoom_delta);
    noegnud_options_defadd_float("engine.zoom.min",NOEGNUD_OPTIONPERM_CONFIGCHANGE,80.0,noegnud_options_render_opengl_view_zoom_min);
    noegnud_options_defadd_float("engine.zoom.max",NOEGNUD_OPTIONPERM_CONFIGCHANGE,2000.0,noegnud_options_render_opengl_view_zoom_max);

    noegnud_options_defadd_float("engine.rotation.y.default",NOEGNUD_OPTIONPERM_CONFIGCHANGE,45.0,noegnud_options_render_opengl_view_angle_y_default);
    noegnud_options_defadd_float("engine.rotation.y.current",NOEGNUD_OPTIONPERM_CONFIGCHANGE,45.0,noegnud_options_render_opengl_view_angle_y_current);
    noegnud_options_defadd_float("engine.rotation.y.min",NOEGNUD_OPTIONPERM_CONFIGCHANGE,5.0,noegnud_options_render_opengl_view_angle_y_min);
    noegnud_options_defadd_float("engine.rotation.y.max",NOEGNUD_OPTIONPERM_CONFIGCHANGE,90.0,noegnud_options_render_opengl_view_angle_y_max);

    noegnud_options_defadd_float("engine.rotation.x.default",NOEGNUD_OPTIONPERM_CONFIGCHANGE,20.0,noegnud_options_render_opengl_view_angle_x_default);
    noegnud_options_defadd_float("engine.rotation.x.current",NOEGNUD_OPTIONPERM_CONFIGCHANGE,20.0,noegnud_options_render_opengl_view_angle_x_current);
    noegnud_options_defadd_float("engine.rotation.x.min",NOEGNUD_OPTIONPERM_CONFIGCHANGE,-45.0,noegnud_options_render_opengl_view_angle_x_min);
    noegnud_options_defadd_float("engine.rotation.x.max",NOEGNUD_OPTIONPERM_CONFIGCHANGE,45.0,noegnud_options_render_opengl_view_angle_x_max);

    noegnud_options_defadd_int("engine.lighting.method",NOEGNUD_OPTIONPERM_CONFIGCHANGE,NOEGNUD_RENDER_LIGHTING_METHOD_CHEAP,noegnud_options_render_opengl_lighting_method);


    noegnud_options_defadd_int("gameplay.autocentre",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1,noegnud_options_render_features_autocentre);
    noegnud_options_defadd_int("gameplay.autocentre.x.only",NOEGNUD_OPTIONPERM_CONFIGCHANGE,0,noegnud_options_render_features_autocentre_xonly);
    noegnud_options_defadd_int("gameplay.autocentre.mouse",NOEGNUD_OPTIONPERM_CONFIGCHANGE,0,noegnud_options_render_features_autocentre_mouse);

    noegnud_options_defadd_int("fun.recording.ask",NOEGNUD_OPTIONPERM_CONFIGCHANGE,0,noegnud_options_fun_recording_ask);
    noegnud_options_defadd_int("fun.bullettime.triggertime",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1024*60,noegnud_options_render_bullettime_trigger);

    noegnud_options_defadd_float("interface.player-highlight.strobe.speed",NOEGNUD_OPTIONPERM_CONFIGCHANGE,1.0,noegnud_options_render_highlight_strobe_speed);
    noegnud_options_defadd_float("interface.player-highlight.block.opacity",NOEGNUD_OPTIONPERM_CONFIGCHANGE,0.25,noegnud_options_render_highlight_block_opacity);

    noegnud_options_defadd_string("i18n.language",NOEGNUD_OPTIONPERM_CONFIGCHANGE,"english",noegnud_options_language);

}

noegnud_optiontype_int *noegnud_options_create_int(int perm, int value) {
    noegnud_optiontype_int *type_int;

    type_int=noegnud_mem_malloc(sizeof(noegnud_optiontype_int));
    type_int->meta.type=NOEGNUD_OPTIONTYPE_INT;
    type_int->meta.perm=perm;
    type_int->value=value;

    return type_int;
}

noegnud_optiontype_float *noegnud_options_create_float(int perm, float value) {
    noegnud_optiontype_float *type_float;

    type_float=noegnud_mem_malloc(sizeof(noegnud_optiontype_float));
    type_float->meta.type=NOEGNUD_OPTIONTYPE_FLOAT;
    type_float->meta.perm=perm;
    type_float->value=value;

    return type_float;
}

noegnud_optiontype_string *noegnud_options_create_string(int perm, char *value) {
    noegnud_optiontype_string *type_string;

    type_string=noegnud_mem_malloc(sizeof(noegnud_optiontype)+256+1);
    type_string->meta.type=NOEGNUD_OPTIONTYPE_STRING;
    type_string->meta.perm=perm;
    strcpy((char *)&type_string->value,value);

    return type_string;
}

noegnud_optiontype_rgb *noegnud_options_create_rgb(int perm, float r, float g, float b) {
    noegnud_optiontype_rgb *type_rgb;

    type_rgb=noegnud_mem_malloc(sizeof(noegnud_optiontype_rgb));
    type_rgb->meta.type=NOEGNUD_OPTIONTYPE_RGB;
    type_rgb->meta.perm=perm;
    type_rgb->r=r;
    type_rgb->g=g;
    type_rgb->b=b;

    return type_rgb;
}

noegnud_optiontype_rgba *noegnud_options_create_rgba(int perm, float r, float g, float b, float a) {
    noegnud_optiontype_rgba *type_rgba;

    type_rgba=noegnud_mem_malloc(sizeof(noegnud_optiontype_rgba));
    type_rgba->meta.type=NOEGNUD_OPTIONTYPE_RGBA;
    type_rgba->meta.perm=perm;
    type_rgba->r=r;
    type_rgba->g=g;
    type_rgba->b=b;
    type_rgba->a=a;

    return type_rgba;
}


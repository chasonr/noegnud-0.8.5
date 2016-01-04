#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

#include "noegnud_collection.h"
#include "noegnud_typeloader.h"
#include "noegnud_extern.h"
#include "noegnud_mem.h"
#include "noegnud_common.h"
#include "noegnud_config.h"
#include "noegnud_textcolour.h"

#include "noegnud_textcolouring.h"

noegnud_tcollection *noegnud_textcolouring=NULL;

typedef struct {
    struct re_pattern_buffer *pattern_buffer;
    float r,g,b;
} noegnud_textcolouring_item;

void noegnud_textcolouring_done();
void noegnud_textcolouring_init() {
	noegnud_textcolouring_load(noegnud_options_gui_text_colouring->value);
	atexit(noegnud_textcolouring_done);
}

void noegnud_textcolouring_done() {
	noegnud_tcollection *stepcoll;

	stepcoll=noegnud_textcolouring;
	while (stepcoll) {
		noegnud_mem_free(stepcoll->data);
		stepcoll=stepcoll->next;
	}
	noegnud_collection_destroy(&noegnud_textcolouring);
}

static void noegnud_textcolouring_load_parser(char *setting, int nparams, char *params, void *data) {
	noegnud_textcolouring_item *textcolouring_item;
	char *regex_error;

	if (!strcmp(setting,"$include$")) {
		noegnud_textcolouring_load(params);
		return;
	}

	if (nparams!=3) {
	    printf("[WARNING] textcolouring syntax error for setting \"%s\" :%d\n",setting,nparams);
	    return;
	}


	if ((textcolouring_item=(noegnud_textcolouring_item *)noegnud_collection_data(noegnud_textcolouring,setting))) {
	    free(textcolouring_item->pattern_buffer);
	} else {
	    textcolouring_item=noegnud_mem_malloc(sizeof(noegnud_textcolouring_item));
	}

	textcolouring_item->pattern_buffer=malloc(sizeof(struct re_pattern_buffer));

	textcolouring_item->pattern_buffer->translate=0;
	textcolouring_item->pattern_buffer->fastmap=0;
	textcolouring_item->pattern_buffer->buffer=0;
	textcolouring_item->pattern_buffer->allocated=0;

	textcolouring_item->r=atof(params);
	while (*params) params++; params++;
	textcolouring_item->g=atof(params);
	while (*params) params++; params++;
	textcolouring_item->b=atof(params);
	while (*params) params++; params++;

	if ((regex_error=(char *)re_compile_pattern(setting,strlen(setting),textcolouring_item->pattern_buffer))) {
	    free(textcolouring_item->pattern_buffer);
	    textcolouring_item->pattern_buffer=NULL;
	    printf("[NOEGNUD_TEXTCOLOURING] regex error attempting to compile regex string \"%s\":\n    %s\n",setting,regex_error);
	}

        if (noegnud_textcolouring) {
	    noegnud_collection_add(noegnud_textcolouring,setting,textcolouring_item);
	} else {
	    noegnud_textcolouring=noegnud_collection_create(setting,textcolouring_item);
	}
}


void noegnud_textcolouring_load(char *textcolouringname) {
	char filename[2048];
	char *szHomedir;

	noegnud_typeloader_IFS=',';
	szHomedir=noegnud_options_get_home();
	sprintf(filename,"%s/textcolouring/%s.colour",szHomedir,textcolouringname);
	noegnud_mem_free(szHomedir);
	if (!noegnud_common_file_exists(filename)) sprintf(filename,"../" NOEGNUD_DATADIR "/textcolouring/%s.colour",textcolouringname);
	printf("* loading textcolouring : %s (%s)\n",textcolouringname,filename);
	re_syntax_options=RE_SYNTAX_EMACS;
	noegnud_typeloader_parsefile(noegnud_textcolouring_load_parser,filename,0);
}

char *noegnud_textcolouring_colourise(const char *string) {
	char *result;
	char *colourisation;
	char *replaced;

	noegnud_tcollection *walk_through;

	result=(char *)string;

	walk_through=noegnud_textcolouring;
	while (walk_through) {

	    replaced=noegnud_textcolour_rgb_pbuffer( result,
						     ((noegnud_textcolouring_item *)walk_through->data)->pattern_buffer,
						     ((noegnud_textcolouring_item *)walk_through->data)->r,
						     ((noegnud_textcolouring_item *)walk_through->data)->g,
						     ((noegnud_textcolouring_item *)walk_through->data)->b,
						     noegnud_options_gui_window_console_text_colour_default->r,
						     noegnud_options_gui_window_console_text_colour_default->g,
						     noegnud_options_gui_window_console_text_colour_default->b);
	    if (result!=replaced) {
		if (result!=string) noegnud_mem_free(result);
		result=replaced;
	    }

	    walk_through=walk_through->next;
	}

	colourisation=noegnud_mem_malloc(strlen(replaced)+1);
	strcpy(colourisation,replaced);

	if (replaced!=string) noegnud_mem_free(replaced);

	return colourisation;
}


#include <stdlib.h>
#include <stdio.h>

#include "noegnud_collection.h"
#include "noegnud_typeloader.h"
#include "noegnud_extern.h"
#include "noegnud_mem.h"
#include "noegnud_config.h"
#include "noegnud_common.h"

#include "noegnud_reference.h"

noegnud_tcollection *noegnud_reference=NULL;
int noegnud_reference_tile_amount=0;

void noegnud_reference_done();
void noegnud_reference_init() {
	noegnud_reference_load(VARIANT_TILETYPE);
	atexit(noegnud_reference_done);
}

void noegnud_reference_done() {
	noegnud_tcollection *stepcoll;

	stepcoll=noegnud_reference;
	while (stepcoll) {
		noegnud_mem_free(stepcoll->data);
		stepcoll=stepcoll->next;
	}
	noegnud_collection_destroy(&noegnud_reference);
}

static void noegnud_reference_load_parser(char *setting, int nparams, char *params, void *data) {
	char *value;


	value=noegnud_mem_malloc(256);
	strcpy(value,params);

	if (noegnud_reference) {
		noegnud_collection_add(noegnud_reference,setting,value);
	} else {
		noegnud_reference=noegnud_collection_create(setting,value);
	}
	noegnud_reference_tile_amount++;

}

void noegnud_reference_load(char *refname) {
	char filename[2048];
	char *szHomedir;

	noegnud_typeloader_IFS=0;
	szHomedir=noegnud_options_get_home();
	sprintf(filename,"%s/variant-reference/%s.txt",szHomedir,refname);
	noegnud_mem_free(szHomedir);
	if (!noegnud_common_file_exists(filename)) sprintf(filename,"../" NOEGNUD_DATADIR "/variant-reference/%s.txt",refname);
	printf("* loading reference file : %s (%s)\n",refname,filename);

	if (!noegnud_common_file_exists((const char *)filename)) {
	    printf("[FATAL ERROR] could not locate reference file [%s].\n",filename);
	    exit(-1);
	}

	noegnud_typeloader_parsefile(noegnud_reference_load_parser,filename,0);
}


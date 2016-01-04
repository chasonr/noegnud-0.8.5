#ifdef NOEGNUDDEBUG

#include <stdlib.h>

#include "noegnud_collection.h"
#include "noegnud_mem.h"

int noegnud_debug_mem_memused=0;
int noegnud_debug_mem_mostmemused=0;
noegnud_tcollection *noegnud_debug_mem_ptrsizemap=0;

void noegnud_debug_done();

#define noegnud_debug_init_string_ "DEBUG_INIT: forced \"memory leak\""
char noegnud_debug_init_string[sizeof(noegnud_debug_init_string_)] = noegnud_debug_init_string_;
void noegnud_debug_init() {
	atexit(noegnud_debug_done);

	noegnud_debug_mem_ptrsizemap=noegnud_collection_create(noegnud_debug_init_string,noegnud_mem_memblock_create((char *)&noegnud_debug_init_string,__LINE__,&noegnud_debug_init_string,0));
}

void noegnud_debug_done() {
	noegnud_tcollection *leaklist;
	tnoegnud_mem_block *memblock;
	noegnud_tcollection *stepcoll;
	int leak_amount=0;
	int leak_total=0;


	printf("------------------------------------------------------------------------------\n");

	printf(" Leak list:\n");
	leaklist=noegnud_debug_mem_ptrsizemap;
	while (leaklist) {
		memblock=leaklist->data;
		if (memblock&&memblock->size) {
			printf("   - %s : %s:%d used %d bytes.\n",leaklist->name,memblock->sourcefile,memblock->linenumber,memblock->size);
			leak_amount++;
			leak_total+=memblock->size;
		}
		leaklist=leaklist->next;
	}
	printf(" End of leak list.\n");
	printf(" memory leaks found: %d\n",leak_amount);
	printf(" memory leak byte tally: %d\n",leak_total);
	printf("------------------------------------------------------------------------------\n");

	noegnud_mem_lock=1;

	stepcoll=noegnud_debug_mem_ptrsizemap;
	while (stepcoll) {
		noegnud_mem_free(stepcoll->data);
		stepcoll=stepcoll->next;
	}

	noegnud_collection_destroy(&noegnud_debug_mem_ptrsizemap);

	printf(" Memory:\n");
	printf("   Most Used: %d\n",noegnud_debug_mem_mostmemused);
	printf("   Used at exit: %d\n",noegnud_debug_mem_memused);
/*	printf("------------------------------------------------------------------------------\n");
	printf(" Summary:\n");                                                              // **************************        102         ???????      ****************************
	printf("   Internal memory tally offset (should always be 0): %d\n",noegnud_debug_mem_memused-(leak_total-sizeof(noegnud_debug_init_string_)));
*/	printf("------------------------------------------------------------------------------\n");
}

#endif

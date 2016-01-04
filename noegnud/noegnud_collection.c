#include <stdlib.h>
#include <stdio.h>

#include "math.h"
#include "string.h"

#include "noegnud_mem.h"
#include "noegnud_collection.h"
  noegnud_tcollection *noegnud_collection_create (const char *name, void *data) {
	noegnud_tcollection *collection;
#ifdef NOEGNUDDEBUG
	char *debug_coll;
	char debug_coll_t;
#endif

#ifdef NOEGNUDDEBUG
	if (data==NULL) {
		printf("[FATAL:%s:%d] collection creation with a NULL data pointer! aborting ...\n",__FILE__,__LINE__);
		exit(-1);
	}
#endif

	collection=noegnud_mem_malloc (sizeof (noegnud_tcollection));
	collection->name=noegnud_mem_malloc (strlen (name) + 1);
	strcpy (collection->name, name);
	collection->data = data;
#ifdef NOEGNUDDEBUG
//	printf("[DEBUG] testing new collection data integrity for [\"%s\"] ...\n",name);
	debug_coll=data;
	debug_coll_t=*debug_coll;
#endif
	collection->origin = NULL;
	collection->next = NULL;
	collection->prev = NULL;

	return collection;
}
noegnud_tcollection * noegnud_collection_add (noegnud_tcollection *collection, const char *name, void *data) {
	noegnud_tcollection *add;
	noegnud_tcollection *made;

	add=collection;
	if (!add) return noegnud_collection_create (name, data);
	while (add->next) {
		if (!strcmp (name, add->name)) {
#ifdef NOEGNUDDEBUG
//			printf("[DEBUG] reassigning data for collection value [\"%s\"]\n",name);
#endif

// ***** FAILED TO FIND REFERENCE ..... well, duh, it might be part of "ram debug" collection ...

			noegnud_mem_free(add->data);
			add->data=data;
			return add;
		}
		add = (noegnud_tcollection *) add->next;
	}
	made=(noegnud_tcollection *) (add->next = (struct noegnud_tcollection *)noegnud_collection_create (name, data));
	return made;
}
void noegnud_collection_destroy (noegnud_tcollection ** collectionp) {
	noegnud_tcollection * collection;
	collection = *collectionp;
	if (!collection) return;
	if (collection->next) noegnud_collection_destroy ((noegnud_tcollection **) & collection->next);
	noegnud_mem_free (collection->name);
	noegnud_mem_free (collection);
	*collectionp = NULL;
}

noegnud_tcollection *noegnud_collection_structure (noegnud_tcollection *collection, const char *name) {
	noegnud_tcollection *get;
	get=collection;
	while (get) {
		if (!strcmp (name, get->name)) {
		    return get;
		}
		get = (noegnud_tcollection *) get->next;
	}
	return NULL;
}

void *noegnud_collection_data (noegnud_tcollection *collection, const char *name) {
	noegnud_tcollection *get;

	get=noegnud_collection_structure(collection,name);
	if (get) return get->data;
	return NULL;
}



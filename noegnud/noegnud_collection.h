#ifndef _NOEGNUD_COLLECTION_H_
#define _NOEGNUD_COLLECTION_H_

typedef struct {
	char *name;
	void *data;
	void *origin;
	void *next;
	void *prev;
} noegnud_tcollection;

noegnud_tcollection *noegnud_collection_create (const char *name, void *data);
noegnud_tcollection *noegnud_collection_add (noegnud_tcollection *collection, const char *name, void *data);
void noegnud_collection_destroy (noegnud_tcollection **collectionp);
noegnud_tcollection *noegnud_collection_structure (noegnud_tcollection *collection, const char *name);
void *noegnud_collection_data (noegnud_tcollection *collection, const char *name);

#endif


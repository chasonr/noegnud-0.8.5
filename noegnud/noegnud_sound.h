#ifndef _NOEGNUD_SOUND_H_
#define _NOEGNUD_SOUND_H_

#include "noegnud_collection.h"

void noegnud_sound_init();

int noegnud_sound_startup();
void noegnud_sound_done();

noegnud_tcollection *noegnud_sound_load(const char *filename);
void noegnud_sound_play(const char *filename,float volume);

#endif


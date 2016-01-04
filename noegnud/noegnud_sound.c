#include <SDL_mixer.h>

#include "noegnud_config.h"
#include "noegnud_mem.h"
#include "noegnud_collection.h"
#include "noegnud_options.h"
#include "noegnud_common.h"
#include "noegnud_sound.h"

typedef struct {
    Mix_Chunk *chunk;
    Mix_Music *music;
} noegnud_sound_tsound;

static noegnud_tcollection *noegnud_sounds=NULL;
int noegnud_sound_initialised=FALSE;

void noegnud_sound_done() {
    noegnud_tcollection *sound;

    if (!noegnud_sound_initialised) return;

    sound=noegnud_sounds;
    while (sound) {
	if (((noegnud_sound_tsound *)(sound->data))->chunk) Mix_FreeChunk(((noegnud_sound_tsound *)(sound->data))->chunk);
	if (((noegnud_sound_tsound *)(sound->data))->music) Mix_FreeMusic(((noegnud_sound_tsound *)(sound->data))->music);
	noegnud_mem_free(sound->data);
	sound=sound->next;
    }
    noegnud_collection_destroy(&noegnud_sounds);
    noegnud_sounds=NULL;
}

int noegnud_sound_startup() {
    noegnud_sounds=NULL;

    if (Mix_OpenAudio(44100,AUDIO_S16,1,512)<0) {
	printf("[SOUND] ERROR: could not initilise sound.\n");
	noegnud_sound_initialised=FALSE;
    } else {
	printf("[SOUND] initialised successfully\n");
	noegnud_sound_initialised=TRUE;
    }

    return noegnud_sound_initialised;
}

void noegnud_sound_init() {
    printf("* initialising sound\n");

    if (noegnud_sound_startup()) atexit(noegnud_sound_done);
}

noegnud_tcollection *noegnud_sound_load(const char *filename) {
    noegnud_sound_tsound *sound;
    char *szHomedir;
    char physical_filename[2048];
    noegnud_tcollection *returnvalue;

    if (!noegnud_sound_initialised) return NULL;

    if ((returnvalue=noegnud_collection_structure(noegnud_sounds,filename))) {
#ifdef NOEGNUDDEBUG
	printf("[SOUND] noegnud_sound_load: cached file found \"%s\"\n",filename);
#endif
	return returnvalue;
    }

    szHomedir=noegnud_options_get_home();
    sprintf((char *)&physical_filename,"%s/%s",szHomedir,filename);
    noegnud_mem_free(szHomedir);
    if (!noegnud_common_file_exists((char *)&physical_filename)) sprintf((char *)&physical_filename,"../" NOEGNUD_DATADIR "/%s",filename);
    if (!noegnud_common_file_exists((char *)&physical_filename)) sprintf((char *)&physical_filename,"%s",filename);
    if (!noegnud_common_file_exists((char *)&physical_filename)) {
	printf("[SOUND] noegnud_sound_load: file not found \"%s\"\n",filename);
	return NULL;
    }

    printf("[SOUND] noegnud_sound_load: loading \"%s\"\n",filename);
    sound=(noegnud_sound_tsound *)noegnud_mem_malloc(sizeof(noegnud_sound_tsound));
    sound->chunk=NULL;
    sound->music=NULL;

    sound->chunk=Mix_LoadWAV(physical_filename);
    if (!sound->chunk) sound->music=Mix_LoadMUS(physical_filename);

    if (!noegnud_sounds) {
	returnvalue=noegnud_sounds=noegnud_collection_create(filename,sound);
    } else {
	returnvalue=noegnud_collection_add(noegnud_sounds,filename,sound);
    }

    return returnvalue;
}

void noegnud_sound_play(const char *filename,float volume) {
    noegnud_tcollection *soundcollection;
    noegnud_sound_tsound *sound;
    int internal_volume;

    if (!noegnud_sound_initialised) return;

    soundcollection=noegnud_sound_load(filename);
    if (soundcollection) {
	if (volume<0) volume=0;
	if (volume>1) volume=1;

	sound=(noegnud_sound_tsound *)soundcollection->data;
	internal_volume=volume*MIX_MAX_VOLUME;
	if (sound->chunk) {
#ifdef NOEGNUDDEBUG
	    printf("[SOUND] noegnud_sound_play: playing chunk \"%s\" (%2.3f)\n",filename,volume);
#endif
	    Mix_VolumeChunk(sound->chunk,internal_volume);
	    Mix_PlayChannel(-1,sound->chunk,0);
	} else if (sound->music) {
#ifdef NOEGNUDDEBUG
	    printf("[SOUND] noegnud_sound_play: playing music \"%s\" (%2.3f)\n",filename,volume);
#endif
	    Mix_VolumeMusic(internal_volume);
	    Mix_PlayMusic(sound->music,0);
	} else {
	    printf("[SOUND] WARNING: noegnud_sound_play: unknown file format/could not load \"%s\"\n",filename);
	}
    } else {
	printf("[SOUND] noegnud_sound_play: could not load/play \"%s\"\n",filename);
    }
}


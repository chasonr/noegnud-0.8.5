#include <SDL.h>
#include <string.h>

#include "noegnud_mem.h"

SDL_Event noegnud_common_pushedevent;

void noegnud_common_pushevent_keypress(SDLKey key) {
/* this is _SO_ ugly :/ i really need to figure out a "cleaner" way ...... (maybe) .... (someday) .... (perhaps) .... */

	((SDL_KeyboardEvent *)&noegnud_common_pushedevent)->type=SDL_KEYDOWN;
	((SDL_KeyboardEvent *)&noegnud_common_pushedevent)->which=0;
	((SDL_KeyboardEvent *)&noegnud_common_pushedevent)->state=SDL_PRESSED;
	((SDL_KeyboardEvent *)&noegnud_common_pushedevent)->keysym.sym=key;
	((SDL_KeyboardEvent *)&noegnud_common_pushedevent)->keysym.scancode=0;
	((SDL_KeyboardEvent *)&noegnud_common_pushedevent)->keysym.unicode=key;
	SDL_PushEvent(&noegnud_common_pushedevent);
}

void noegnud_common_flush_events() {
	SDL_Event event;
	while (SDL_PollEvent (&event)) {};
}

int noegnud_common_power2 (int n) {
  int c;
  int o;
  o = n;
  c = 0;
  while (n)
    {
      n = n / 2;
      c++;
    };
  n = 1;
  while (c--)
    {
      n *= 2;
      if (o == n)
	break;
    }
  return n;
}

char *noegnud_common_str_replace(const char *haystack, const char *needle, const char *camel) {
	char *pos;
	char *result;
	char *resultn;
	char *after;

	result=noegnud_mem_malloc(strlen(haystack)+1);
	strcpy(result,haystack);
	while ((pos=strstr(result,needle))) {
		after=pos+strlen(needle);
		resultn=noegnud_mem_malloc(strlen(result)-strlen(needle)+strlen(camel)+1);

		*pos=0;
		sprintf(resultn,"%s%s%s",result,camel,after);
		*pos=*needle;

		noegnud_mem_free(result);

		result=resultn;
	}
	return result;
}

int noegnud_common_file_exists(const char *filename) {
    FILE *f;

    f=fopen(filename,"r");
    if (f==NULL) return FALSE;
    fclose(f);
    return TRUE;
}


#ifndef _NOEGNUD_COMMON_H_
#define _NOEGNUD_COMMON_H_

void noegnud_common_pushevent_keypress(SDLKey key);
void noegnud_common_flush_events();
int noegnud_common_power2 (int n);
char *noegnud_common_str_replace(const char *haystack, const char *needle, const char *camel);
int noegnud_common_file_exists(const char *filename);

#endif //_NOEGNUD_COMMON_H_

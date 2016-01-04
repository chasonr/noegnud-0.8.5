#ifndef _NOEGNUD_LANG_H_
#define _NOEGNUD_LANG_H_

#define NOEGNUD_LANG_PLNAME "$plname$"

void noegnud_lang_init();
void noegnud_lang_load(char *filename);
char *noegnud_lang_translate(const char *string);

#endif //_NOEGNUD_LANG_H_

#ifndef _TYPELOADER_H_
#define _TYPELOADER_H_
typedef void noegnud_typeloader_output(char *setting, int nparams, char *params, void *data);

int noegnud_typeloader_parsefile(noegnud_typeloader_output *proc, char *filename, void *data);
#endif


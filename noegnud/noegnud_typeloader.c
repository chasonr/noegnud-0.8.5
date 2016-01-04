#include <stdlib.h>
#include <stdio.h>

#include "noegnud_typeloader.h"

#include "hack.h"

#define MAX_LINE_LENGTH 1024*32

char noegnud_typeloader_IFS=',';

static void typeloader_processline(noegnud_typeloader_output *proc, char *line, void *data) {
    char *eline;
    char *params;
    int nparams;

    while (*line==0x20) line++;
    eline=line;
    while (*eline) eline++; eline --;
    while ((eline>line)&&(*eline==0x20)) {
	*eline=0; eline--;
    }
    if (!*line) return;
    if ((*line==';')||(*line=='#')) return; //comment skipit

    eline=line;
    while ((*eline)&&(*eline!='=')) eline++;
    *eline=0; eline++;
    params=eline;
    nparams=1;
    while (*eline) {
	if (*eline==noegnud_typeloader_IFS) {
	    *eline=0; nparams++;
	}
	eline++;
    }

    proc(line,nparams,params,data);
}

int noegnud_typeloader_parsefile(noegnud_typeloader_output *proc, char *filename, void *data) {
    char line[MAX_LINE_LENGTH+1];
    int nchar;
    FILE *inputfile;

    if (!(inputfile=fopen(filename,"r"))) return 0;

    nchar=0;
    while (!feof(inputfile)) {
	fread(&line[nchar],1,1,inputfile);
	switch (line[nchar]) {
	case 0x0D: break;
	case 0x0A: line[nchar]=0; if (line[0]) typeloader_processline(proc,(char *)&line,data); nchar=0; break;
	default: nchar++;
	}
    }
    if (nchar) {
        line[nchar-1]=0;
        if (line[0]) typeloader_processline(proc,(char *)&line,data);
    }
    fclose(inputfile);
    return 1;
}

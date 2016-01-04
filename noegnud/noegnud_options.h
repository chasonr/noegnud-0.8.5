#ifndef _NOEGNUD_OPTIONS_H_
#define _NOEGNUD_OPTIONS_H_

#define NOEGNUD_OPTIONTYPE_INT		1
#define NOEGNUD_OPTIONTYPE_FLOAT	2
#define NOEGNUD_OPTIONTYPE_STRING	3
#define NOEGNUD_OPTIONTYPE_RGB	        4
#define NOEGNUD_OPTIONTYPE_RGBA 	5

#define NOEGNUD_OPTIONPERM_NONE			0x00
#define NOEGNUD_OPTIONPERM_CONFIGCHANGE		0x01
#define NOEGNUD_OPTIONPERM_INGAMECHANGE		0x02
#define NOEGNUD_OPTIONPERM_DONTSAVETHIS		0x04


typedef struct {
    int type;
    int perm;
} noegnud_optiontype;

typedef struct {
    noegnud_optiontype meta;
    int value;
} noegnud_optiontype_int;

typedef struct {
    noegnud_optiontype meta;
    float value;
} noegnud_optiontype_float;

typedef struct {
    noegnud_optiontype meta;
    char value[2048];
} noegnud_optiontype_string;

typedef struct {
    noegnud_optiontype meta;
    float r;
    float g;
    float b;
} noegnud_optiontype_rgb;

typedef struct {
    noegnud_optiontype meta;
    float r;
    float g;
    float b;
    float a;
} noegnud_optiontype_rgba;

void noegnud_options_init();
void noegnud_options_done();

void noegnud_options_load();
void noegnud_options_save();

noegnud_optiontype_int *noegnud_options_create_int(int perm, int value);
noegnud_optiontype_float *noegnud_options_create_float(int perm, float value);
noegnud_optiontype_string *noegnud_options_create_string(int perm, char *value);
noegnud_optiontype_rgb *noegnud_options_create_rgb(int perm, float r, float g, float b);
noegnud_optiontype_rgba *noegnud_options_create_rgba(int perm, float r, float g, float b, float a);

char *noegnud_options_get_home();

#endif //_NOEGNUD_OPTIONS_H_



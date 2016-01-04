#ifndef _NOEGNUD_FONTS_H_
#define _NOEGNUD_FONTS_H_

typedef struct {
	int width;
	float fxd1, fxd2;
} noegnud_fonts_tdynamicchar;

typedef struct {
	GLuint ch[256];
	char imagefilename[1024];
	int width, height;
	GLfloat glwidth, glheight;
	float fx1,fy1;
	float fx2, fy2;
	noegnud_fonts_tdynamicchar dynamic[256];
	int loaded;
} noegnud_fonts_tfont;

noegnud_fonts_tfont *noegnud_fonts_loadfont(const char *name);
void noegnud_fonts_printchar_scaled (noegnud_fonts_tfont *font, unsigned char ch, float scale_x, float scale_y);
void noegnud_fonts_printchar (noegnud_fonts_tfont *font, unsigned char ch);
void noegnud_fonts_print (noegnud_fonts_tfont *font, int x, int y, char *string);
int noegnud_width_dynamic(noegnud_fonts_tfont *font, char *string);
void noegnud_fonts_printchar_dynamic (noegnud_fonts_tfont *font, unsigned char ch);
void noegnud_fonts_print_dynamic (noegnud_fonts_tfont *font, int x, int y, char *string);
void noegnud_fonts_deletefont(noegnud_fonts_tfont **font_);
void noegnud_fonts_cleanup();

#endif // _NOEGNUD_FONTS_H_


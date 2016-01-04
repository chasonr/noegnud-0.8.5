#ifndef _NOEGNUD_GFF_TGA_H_
#define _NOEGNUD_GFF_TGA_H_

void noegnud_gff_tga_header(FILE *f, int width, int height, int bits);
int noegnud_gff_tga_save(const char *filename, int width, int height, void *rgbdata);
int noegnud_gff_tga_save_32(const char *filename, int width, int height, void *rgbdata);

#endif


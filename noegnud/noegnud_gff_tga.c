#include <stdio.h>

#include "noegnud_gff_tga.h"

void noegnud_gff_tga_header(FILE *f, int width, int height, int bits) {

	fputc(0,f); fputc(0,f);
	fputc(2,f);
	fputc(0,f); fputc(0,f);
	fputc(0,f); fputc(0,f);
	fputc(0,f);
	fputc(0,f); fputc(0,f);
	fputc(0,f); fputc(0,f);
	
	fputc(width%256,f);
	fputc((width/256)%256,f);

	fputc(height%256,f);
	fputc((height/256)%256,f);
	
	
	fputc(bits,f); fputc(8,f);
}

int noegnud_gff_tga_save(const char *filename, int width, int height, void *rgbdata) {
	FILE *f;
	int c;
	unsigned char *swapper;
	
	if (!(f=fopen(filename,"w"))) return 0;
		
	noegnud_gff_tga_header(f,width,height,24);
	
	swapper=(unsigned char *)rgbdata;
	
	for(c=0;c<width*height;c++) {
		fputc(swapper[2],f);
		fputc(swapper[1],f);
		fputc(swapper[0],f);
		swapper+=3;
	}
	
//	fwrite(rgbdata,width*height*3,1,f); //uck!
	
	fclose(f);
	return 1;
}

int noegnud_gff_tga_save_32(const char *filename, int width, int height, void *rgbdata) {
	FILE *f;
	int c;
	unsigned char *swapper;
	
	if (!(f=fopen(filename,"w"))) return 0;
		
	noegnud_gff_tga_header(f,width,height,32);
	
	swapper=(unsigned char *)rgbdata;
	
	for(c=0;c<width*height;c++) {
		fputc(swapper[3],f);
		fputc(swapper[2],f);
		fputc(swapper[1],f);
		fputc(swapper[0],f);
		swapper+=4;
	}
	
//	fwrite(rgbdata,width*height*3,1,f); //uck!
	
	fclose(f);
	return 1;
}

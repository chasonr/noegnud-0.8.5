#include <stdlib.h>
#include <stdio.h>
#include "../noegnud_gff_tga.h"

int main(int argc, char **argv) {
	FILE *fi;
	FILE *fo;
	
	int size,bit;
	
	int mh;
	
	unsigned char byte;
	unsigned char value;
	
	unsigned char *fntmap;
	unsigned char *rgbmap;
	
	int x,y,line;
	
	
	if (argc<3) {
		puts("Usage: noegnud_fnt2tga <infile.fnt> <outfile.tga>");
		return 0;
	}
	
	if (!(fi=fopen(argv[1],"r"))) {
		printf("couldn't open %s for reading.\n",argv[1]); 
		exit(-1);
	}
	if (!(fo=fopen(argv[2],"w"))) {
		printf("couldn't open %s for writing.\n",argv[2]); 
		fclose(fo);
		exit(-1);
	}
	
	fclose(fo);

	fseek(fi,0,SEEK_END);
	size=(int)ftell(fi);
	fseek(fi,0,SEEK_SET);
	
	fntmap=(unsigned char *)malloc(size);
	
	fread(fntmap,size,1,fi);
	fclose(fi); 

	mh=0;
	for (y=0;y<16;y++) {
		for (x=0;x<16;x++) {
			for (line=0;line<size/256;line++) {
				byte=fntmap[(y*16+x)*(size/256)+line];
				for (bit=0;bit<8;bit++) {
					if ((byte&(1<<(7-bit)))!=0) {
						if (line>mh) mh=line;
					};
				}
			}
		}
	}
	mh++;
	
	rgbmap=(unsigned char *)malloc(8*size*3);
	
	printf("Converting %s to %s as 128x%d of 8x%d chars\n",argv[1],argv[2],16*mh,mh);
	
	for (y=0;y<16;y++) {
		for (x=0;x<16;x++) {
			for (line=0;line<mh;line++) {
				byte=fntmap[(y*16+x)*(size/256)+line];
				for (bit=0;bit<8;bit++) {
					if ((byte&(1<<(7-bit)))!=0) {
						value=0xFF;
					} else {
						value=0x00;
					};
					rgbmap[((16*mh-1)-(y*mh+line))*(8*16*3) + (x*8 + bit)*3 + 0] = value;
					rgbmap[((16*mh-1)-(y*mh+line))*(8*16*3) + (x*8 + bit)*3 + 1] = value;
					rgbmap[((16*mh-1)-(y*mh+line))*(8*16*3) + (x*8 + bit)*3 + 2] = value;
				}
			}
		}
	}
	
	noegnud_gff_tga_save(argv[2],8*16,16*mh,rgbmap);
	free(fntmap);
	free(rgbmap);
	
	return 1;
}


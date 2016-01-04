#include "noegnud_interface.h"
#include "noegnud_fonts.h"
#include "noegnud_common.h"
#include "noegnud_mem.h"
#include "noegnud_extern.h"
#include "noegnud_collection.h"
#include "noegnud_config.h"
#include "noegnud_textcolour.h"

noegnud_tcollection *noegnud_fonts_fontsloaded=NULL;

static int noegnud_fonts_loadimage(noegnud_fonts_tfont* font) {
    int x, y;
    int dx,dy;

    int req_width, req_height;

    SDL_Surface *image;
    SDL_Surface *tmp;
    SDL_Rect rect;
    SDL_Rect tmprect;

    char *pixels;
    Uint8 r,g,b,a;

    image = noegnud_mem_img_load(font->imagefilename);
    if (!image) return 0;
    if (image->format->BytesPerPixel == 4) SDL_SetAlpha (image, 0, 0);

    font->width=image->w/16;
    font->height=image->h/16;

    font->glwidth = ((float)font->width/32)*noegnud_options_render_multiplier->value;
    font->glheight = ((float)font->height/32)*noegnud_options_render_multiplier->value;

    req_width = noegnud_common_power2 (font->width);
    req_height = noegnud_common_power2 (font->height);

    font->fx1 = 0.0;
    font->fy1 = 0.0;
    font->fx2 = ((float) font->width)/ ((float) (req_width));
    font->fy2 = ((float) font->height)/ ((float) (req_height));

    glGenTextures (256, &font->ch[0]);

    tmp = noegnud_mem_sdl_creatergbsurface (SDL_SWSURFACE, req_width, req_height,
					    image->format->BitsPerPixel,
					    image->format->Rmask, image->format->Gmask,
					    image->format->Bmask, image->format->Amask);
    SDL_SetAlpha (tmp, 0, 0);

    tmprect.x = tmprect.y = 0;
    tmprect.w = tmp->w;
    tmprect.h = tmp->h;
    SDL_FillRect (tmp, &rect, 0xFF000000);

    for (y = 0; y < 16; y++) {
	for (x = 0; x < 16; x++) {

	    rect.x = x * font->width;
	    rect.y = y * font->height;
	    rect.w = font->width;
	    rect.h = font->height;

	    tmprect.w = tmp->w;
	    tmprect.h = tmp->h;
	    tmprect.x = 0; tmprect.y = 0; SDL_BlitSurface (image, &rect, tmp, &tmprect);

	    glBindTexture (GL_TEXTURE_2D, font->ch[y * 16 + x]);
	    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	    SDL_LockSurface (tmp);


	    font->dynamic[y*16+x].fxd1=-1;
	    for (dx=0;dx<font->width;dx++) {
		for (dy=0;dy<font->height;dy++) {
		    pixels=tmp->pixels + (dy*tmp->pitch) + dx*tmp->format->BytesPerPixel;
		    SDL_GetRGBA(*((Uint32 *)pixels),tmp->format,&r,&g,&b,&a);
		    if (a==255) {
			font->dynamic[y*16+x].fxd1=dx;
			break;
		    }
		}
		if (font->dynamic[y*16+x].fxd1!=-1) break;
	    }

	    font->dynamic[y*16+x].fxd2=-1;
	    for (dx=font->width-1;dx>0;dx--) {
		for (dy=0;dy<font->height;dy++) {
		    pixels=tmp->pixels + (dy*tmp->pitch) + dx*tmp->format->BytesPerPixel;
		    SDL_GetRGBA(*((Uint32 *)pixels),tmp->format,&r,&g,&b,&a);
		    if (a) {
			font->dynamic[y*16+x].fxd2=dx;
			break;
		    }
		}
		if (font->dynamic[y*16+x].fxd2!=-1) break;
	    }

	    font->dynamic[y*16+x].width= (font->dynamic[y*16+x].fxd1==-1 ? 0 : font->dynamic[y*16+x].fxd2+1-font->dynamic[y*16+x].fxd1);
	    if (font->dynamic[y*16+x].width) {
		font->dynamic[y*16+x].fxd1 = (font->dynamic[y*16+x].fxd1)/ ((float) (req_width-1));
		font->dynamic[y*16+x].fxd2 = (font->dynamic[y*16+x].fxd2)/ ((float) (req_width-1));
	    }

	    if (noegnud_options_render_opengl_mipmapping->value) {
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, req_width, req_height, GL_RGBA, GL_UNSIGNED_BYTE, tmp->pixels);
	    } else {
		glTexImage2D(GL_TEXTURE_2D, 0, 4, req_width, req_height, 0,GL_RGBA, GL_UNSIGNED_BYTE, tmp->pixels);
	    }
	    SDL_UnlockSurface (tmp);
	}
    }

    noegnud_mem_sdl_freesurface (tmp);
    noegnud_mem_sdl_freesurface (image);

    return 1;
}

noegnud_fonts_tfont *noegnud_fonts_loadfont(const char *name) {
    noegnud_fonts_tfont *font;
    char *szHomedir;
    noegnud_tcollection *font_placement;

    if ((font=noegnud_collection_data(noegnud_fonts_fontsloaded,name))&&font->loaded) {
	font->loaded++;
	printf("* Loading font (cached:%d): %s\n",font->loaded,name);
	return font;
    }

    /*
      TODO: there is a memory leak in this font cache
      "cheap-imitation-implementation" :/ .. perhaps
      ... .. i think ... needs some TLC time ..

    */
    if (font) noegnud_mem_free(font);
    /* ** KLUDGE !!!!!! ** */

    printf("* Loading font: %s",name);
    font=noegnud_mem_malloc(sizeof (noegnud_fonts_tfont));

    szHomedir=noegnud_options_get_home();
    sprintf((char *)&font->imagefilename,"%s/%s.png",szHomedir,name);
    noegnud_mem_free(szHomedir);
    if (!noegnud_common_file_exists((char *)&font->imagefilename)) sprintf((char *)&font->imagefilename,"../" NOEGNUD_DATADIR "/%s.png",name);

    printf(" (%s)\n",(char *)&font->imagefilename);
    noegnud_fonts_loadimage(font);

    if (!(font_placement=noegnud_collection_structure(noegnud_fonts_fontsloaded,name))) {
	if (!noegnud_fonts_fontsloaded) {
	    noegnud_fonts_fontsloaded=noegnud_collection_create(name,font);
	} else {
	    noegnud_collection_add(noegnud_fonts_fontsloaded,name,font);
	}
    } else {
	font_placement->data=font;
    }

    font->loaded=1;

    return font;
}


void noegnud_fonts_printchar_scaled (noegnud_fonts_tfont *font, unsigned char ch, float scale_x, float scale_y) {
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D,font->ch[(int)ch]);
    glEnable(GL_BLEND);

    glBegin (GL_QUADS);

    glTexCoord2f (font->fx1, font->fy2);
    glVertex2i (0, 0);

    glTexCoord2f (font->fx2, font->fy2);
    glVertex2i ((float)font->width*scale_x, 0);

    glTexCoord2f (font->fx2, font->fy1);
    glVertex2i ((float)font->width*scale_x, (float)font->height*scale_y);

    glTexCoord2f (font->fx1, font->fy1);
    glVertex2i (0, (float)font->height*scale_y);

    glEnd ();
}

void noegnud_fonts_printchar (noegnud_fonts_tfont *font, unsigned char ch) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,font->ch[(int)ch]);
    glEnable(GL_BLEND);

    glBegin (GL_QUADS);

    glTexCoord2f (font->fx1, font->fy2);
    glVertex2i (0, 0);

    glTexCoord2f (font->fx2, font->fy2);
    glVertex2i (font->width, 0);

    glTexCoord2f (font->fx2, font->fy1);
    glVertex2i (font->width, font->height);

    glTexCoord2f (font->fx1, font->fy1);
    glVertex2i (0, font->height);

    glEnd ();
}

void noegnud_fonts_print (noegnud_fonts_tfont *font, int x, int y, char *string) {
    unsigned char *ch;
    float r,g,b,a;

    glTranslated (x, y, 0);

    ch = string;
    while (*ch) {
	while (*ch==NOEGNUD_TEXTCOLOUR_CONTROL) {
	    ch++;
	    switch (*ch) {
	    case NOEGNUD_TEXTCOLOUR_CODE_RGB:
		ch++;
		r=((float)*ch-1)/254; ch++;
		g=((float)*ch-1)/254; ch++;
		b=((float)*ch-1)/254; ch++;
		glColor3f(r,g,b);
		break;
	    case NOEGNUD_TEXTCOLOUR_CODE_RGBA:
		ch++;
		r=((float)*ch-1)/254; ch++;
		g=((float)*ch-1)/254; ch++;
		b=((float)*ch-1)/254; ch++;
		a=((float)*ch-1)/254; ch++;
		glColor4f(r,g,b,a);
		break;
		/*
		  default:
		  printf("[ERROR] internal font colour syntax\n\t\t\"%s\"\n",string);
		*/
	    }
	}
	noegnud_fonts_printchar_dynamic (font,(char)(*ch));
	glTranslated (font->width, 0, 0);
	ch++;
    }

}

int noegnud_width_dynamic(noegnud_fonts_tfont *font, char *string) {
    int width_count=0;
    char *ch;

    ch=string;
    while (*ch) {
	    width_count+=(font->dynamic[(int)*ch].width?font->dynamic[(int)*ch].width+1:4);
	    ch++;
    }
    return width_count;
}


void noegnud_fonts_printchar_dynamic (noegnud_fonts_tfont *font, unsigned char ch) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,font->ch[(int)ch]);
    glEnable(GL_BLEND);

    glBegin (GL_QUADS);

    glTexCoord2f (font->dynamic[(int)ch].fxd1, font->fy2);
    glVertex2i (0, 0);

    glTexCoord2f (font->dynamic[(int)ch].fxd2, font->fy2);
    glVertex2i (font->dynamic[(int)ch].width, 0);

    glTexCoord2f (font->dynamic[(int)ch].fxd2, font->fy1);

    glVertex2i (font->dynamic[(int)ch].width, font->height);

    glTexCoord2f (font->dynamic[(int)ch].fxd1, font->fy1);
    glVertex2i (0, font->height);

    glEnd ();
}

void noegnud_fonts_print_dynamic (noegnud_fonts_tfont *font, int x, int y, char *string) {
    unsigned char *ch;
    float r,g,b,a;

    glTranslated (x, y, 0);

    ch = string;
    while (*ch) {
	while (*ch==NOEGNUD_TEXTCOLOUR_CONTROL) {
	    ch++;
	    switch (*ch) {
	    case NOEGNUD_TEXTCOLOUR_CODE_RGB:
		ch++;
		r=((float)*ch-1)/254; ch++;
		g=((float)*ch-1)/254; ch++;
		b=((float)*ch-1)/254; ch++;
		glColor3f(r,g,b);
		break;
	    case NOEGNUD_TEXTCOLOUR_CODE_RGBA:
		ch++;
		r=((float)*ch-1)/254; ch++;
		g=((float)*ch-1)/254; ch++;
		b=((float)*ch-1)/254; ch++;
		a=((float)*ch-1)/254; ch++;
		glColor4f(r,g,b,a);
		break;
	    default:
		printf("[ERROR] internal font colour syntax\n\t\t\"%s\"\n",string);
	    }
	}
	noegnud_fonts_printchar_dynamic (font,(char)(*ch));
	glTranslated (font->dynamic[(int)*ch].width?font->dynamic[(int)*ch].width+1:4, 0, 0);
	ch++;
    }

}

 void noegnud_fonts_deletefont(noegnud_fonts_tfont **font_) {
    noegnud_fonts_tfont *font;

    font=*font_;
    font->loaded--;
    if (!font->loaded) {
	printf("* Unloading font: %s\n",font->imagefilename);
	glDeleteTextures (256, &font->ch[0]);
    } else printf("* Unloading font (cached:%d): %s\n",font->loaded,font->imagefilename);
}

void noegnud_fonts_cleanup() {
    noegnud_tcollection *font;

    font=noegnud_fonts_fontsloaded;
    while (font) {
	noegnud_mem_free(font->data);
	font=font->next;
    }
    noegnud_collection_destroy(&noegnud_fonts_fontsloaded);
}

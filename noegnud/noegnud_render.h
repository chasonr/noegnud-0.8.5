#ifndef _NOEGNUD_RENDER_H_
#define _NOEGNUD_RENDER_H_

#define NOEGNUD_RENDER_DRAWMODE_NONE 1
#define NOEGNUD_RENDER_DRAWMODE_CHAR 2
#define NOEGNUD_RENDER_DRAWMODE_TILE 3
#define NOEGNUD_RENDER_DRAWMODE_3D   4

#define NOEGNUD_RENDER_LIGHTING_METHOD_NONE 0
#define NOEGNUD_RENDER_LIGHTING_METHOD_CHEAP 1

void noegnud_render_loop (void);
void noegnud_render_map (void);
void noegnud_render_selection(int x, int y);

#endif // _NOEGNUD_RENDER_H_

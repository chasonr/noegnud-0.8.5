#ifndef _NOEGNUD_DEBUG_H_
#define _NOEGNUD_DEBUG_H_

#include "noegnud_mem.h"

#ifdef NOEGNUDDEBUG

# define noegnud_debug_mem_printused noegnud_mem_debug_printused(__FILE__,__LINE__)

void noegnud_debug_init();

#else

# define noegnud_debug_mem_printused

#endif // NOEGNUDDEBUG

#endif //  _NOEGNUD_DEBUG_H_

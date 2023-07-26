#ifndef _FONT_H_
#define _FONT_H_

#include "surface.h"

//font is 7x7, from ascii code 32 to 126 (inclusive)

extern const char font_7x7[];
void font_print (Surface *surface, char *text, uint16_t x, uint16_t y, uint16_t colour);

#endif

#ifndef _FONT_H_
#define _FONT_H_

#include "surface.h"

extern const char font_5x5[];
extern const char font_7x7[];


typedef struct {
    char *data;
    uint8_t width, height, spacing;
    uint8_t ascii_start, ascii_end;
} Font;

void font_print (Surface *surface, Font *font, char *text, uint16_t x, uint16_t y, uint16_t colour);

#endif

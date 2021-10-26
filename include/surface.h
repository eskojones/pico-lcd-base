#ifndef _SURFACE_H_
#define _SURFACE_H_

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "types.h"


typedef struct {
    uint16_t *pixels;
    uint16_t width;
    uint16_t height;
    uint32_t size;
} Surface;



Surface *   surface_create          (int width, int height);
void        surface_destroy         (Surface *surface);
void        surface_fill            (Surface *surface, uint16_t colour);
void        surface_fill_rgb        (Surface *surface, uint8_t r, uint8_t g, uint8_t b);
void        surface_putpixel        (Surface *surface, uint16_t x, uint16_t y, uint16_t colour);
void        surface_putpixel_rgb    (Surface *surface, uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b);
uint16_t    surface_getpixel        (Surface *surface, uint16_t x, uint16_t y);
void        surface_blit            (Surface *dest, Surface *src, Rect *destRect, Rect *srcRect);
void        surface_blit_mask       (Surface *dest, Surface *src, Rect *destRect, Rect *srcRect, uint16_t mask);
void        surface_scaleblit       (Surface *dest, Surface *src, Rect *destRect, Rect *srcRect);
void        surface_scaleblit_mask  (Surface *dest, Surface *src, Rect *destRect, Rect *srcRect, uint16_t mask);
void        surface_load            (Surface *dest, char *src, uint16_t len, uint16_t colour, uint16_t mask);


#endif

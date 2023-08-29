#include "font.h"
#include "surface.h"
#include "lcd.h"

/*
*  Create a new surface and allocate memory
*/
Surface *surface_create (int width, int height) {
    Surface *surface = (Surface *)malloc(sizeof(Surface));
    surface->size = width * height;
    surface->width = width;
    surface->height = height;
    surface->pixels = (uint16_t *)malloc(width * height * 2);
    return surface;
}


/*
*  Unallocate the memory used by a surface
*/
void surface_destroy (Surface *surface) {
    free(surface->pixels);
    free(surface);
}


/*
*  Fills a surface with the specified 16-bit colour
*/
void surface_fill (Surface *surface, uint16_t colour) {
    colour = ((colour << 8) & 0xff00) | (colour >> 8);
    for (int i = 0; i < surface->size; i++) surface->pixels[i] = colour;
}


/*
*  Fills a surface with the specified r,g,b colour
*/
void surface_fill_rgb (Surface *surface, uint8_t r, uint8_t g, uint8_t b) {
    uint16_t colour = ((r / 8) << 11) + ((g / 8) << 6) + (b / 8);
    colour = ((colour << 8) & 0xff00) | (colour >> 8);
    for (int i = 0; i < surface->size; i++) surface->pixels[i] = colour;
}


void surface_putpixel (Surface *surface, uint16_t x, uint16_t y, uint16_t colour) {
    surface->pixels[y * surface->width + x] = ((colour << 8) & 0xff00) | (colour >> 8);
}


void surface_putpixel_rgb (Surface *surface, uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) {
    uint16_t colour = ((r / 8) << 11) + ((g / 8) << 6) + (b / 8);
    surface->pixels[y * surface->width + x] = ((colour << 8) & 0xff00) | (colour >> 8);
}


uint16_t surface_getpixel (Surface *surface, uint16_t x, uint16_t y) {
    return surface->pixels[y * surface->width + x];
}


/*
    draw a line of `colour` from (sx,sy) to (dx,dy) on a given surface
    using Bresenham's line drawing algorithm
    https://en.wikipedia.org/wiki/Bresenham's_line_algorithm
*/
void surface_line(Surface *surface, uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint16_t colour) {
    int16_t diffx = abs(dx - sx),
            dirx = sx < dx ? 1 : -1;
    int16_t diffy = -abs(dy - sy),
            diry = sy < dy ? 1 : -1;
    int16_t error = diffx + diffy;
    uint16_t cx = sx, cy = sy;

    while (1) {
        if (cx >= 0 && cx < LCD_WIDTH 
         && cy >= 0 && cy < LCD_HEIGHT) {
            surface_putpixel(surface, cx, cy, colour);
        }
        if (cx == dx && cy == dy) break;
        int16_t e2 = 2 * error;
        if (e2 >= diffy) {
            if (cx == dx) break;
            error += diffy;
            cx += dirx;
        }
        if (e2 <= diffx) {
            if (cy == dy) break;
            error += diffx;
            cy += diry;
        }
    }
}


void surface_circle (Surface *surface, uint16_t x0, uint16_t y0, uint16_t r, uint16_t colour) {
    int x = r, y = 0, err = 0;

    while (x >= y) {
        surface_putpixel(surface, x0 + x, y0 + y, colour);
        surface_putpixel(surface, x0 + y, y0 + x, colour);
        surface_putpixel(surface, x0 - y, y0 + x, colour);
        surface_putpixel(surface, x0 - x, y0 + y, colour);
        surface_putpixel(surface, x0 - x, y0 - y, colour);
        surface_putpixel(surface, x0 - y, y0 - x, colour);
        surface_putpixel(surface, x0 + y, y0 - x, colour);
        surface_putpixel(surface, x0 + x, y0 - y, colour);
        if (err <= 0) {
            y++;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x--;
            err -= 2 * x + 1;
        }
    }
}


/*
*  Copies a region of 'src' surface to an offset within 'dest' surface
*  Note that this doesnt scale ('destRect' width and height are unused)
*/
void surface_blit (Surface *dest, Surface *src, Rect *destRect, Rect *srcRect) {
    for (int y = 0; y < srcRect->h; y++) {
        int srcY = srcRect->y + y;
        if (srcY < 0 || srcY >= src->height) continue;
        int destY = destRect->y + y;
        if (destY < 0 || destY >= dest->height) continue;
        for (int x = 0; x < srcRect->w; x++) {
            int srcX = srcRect->x + x;
            if (srcX < 0 || srcX >= src->width) continue;
            int srcIdx = srcY * src->width + srcX;
            int destX = destRect->x + x;
            if (destX < 0 || destX >= dest->width) continue;
            int destIdx = destY * dest->width + destX;
            dest->pixels[destIdx] = src->pixels[srcIdx];
        }
    }
}


/*
*  Copies a region of 'src' surface to an offset within 'dest' surface, 
*  ignoring pixels in the 'src' that are of colour 'mask'.
*  Note that this doesnt scale ('destRect' width and height are unused)
*/
void surface_blit_mask (Surface *dest, Surface *src, Rect *destRect, Rect *srcRect, uint16_t mask) {
    for (int y = 0; y < srcRect->h; y++) {
        int srcY = srcRect->y + y;
        if (srcY < 0 || srcY >= src->height) continue;
        int destY = destRect->y + y;
        if (destY < 0 || destY >= dest->height) continue;
        for (int x = 0; x < srcRect->w; x++) {
            int srcX = srcRect->x + x;
            if (srcX < 0 || srcX >= src->width) continue;
            int srcIdx = srcY * src->width + srcX;
            if (src->pixels[srcIdx] == mask) continue;
            int destX = destRect->x + x;
            if (destX < 0 || destX >= dest->width) continue;
            int destIdx = destY * dest->width + destX;
            dest->pixels[destIdx] = src->pixels[srcIdx];
        }
    }
}


void surface_scaleblit (Surface *dest, Surface *src, Rect *destRect, Rect *srcRect) {
    float scalex = (float)destRect->w / (float)srcRect->w;
    float scaley = (float)destRect->h / (float)srcRect->h;
    int srcIdx, destX, destY;

    for (float y = 0; y < destRect->h; y += scaley) {
        for (float x = 0; x < destRect->w; x += scalex) {
            int srcX = (float)x / scalex;
            int srcY = (float)y / scaley;
            srcIdx = srcY * src->width + srcX;
            for (int sy = 0; sy < scaley; sy++) {
                destY = destRect->y + y + sy;
                if (destY < 0 || destY >= dest->height) continue;
                for (int sx = 0; sx < scalex; sx++) {
                    destX = destRect->x + x + sx;
                    if (destX < 0 || destY >= dest->width) continue;
                    dest->pixels[destY * dest->width + destX] = src->pixels[srcIdx];
                }
            }
        }
    }
}


void surface_scaleblit_mask (Surface *dest, Surface *src, Rect *destRect, Rect *srcRect, uint16_t mask) {
    float scalex = (float)destRect->w / (float)srcRect->w;
    float scaley = (float)destRect->h / (float)srcRect->h;
    int srcIdx, destX, destY;

    for (float y = 0; y < destRect->h; y += scaley) {
        for (float x = 0; x < destRect->w; x += scalex) {
            int srcX = (float)x / scalex;
            int srcY = (float)y / scaley;
            srcIdx = srcY * src->width + srcX;
            if (src->pixels[srcIdx] == mask) continue;
            for (int sy = 0; sy < scaley; sy++) {
                destY = destRect->y + y + sy;
                if (destY < 0 || destY >= dest->height) continue;
                for (int sx = 0; sx < scalex; sx++) {
                    destX = destRect->x + x + sx;
                    if (destX < 0 || destY >= dest->width) continue;
                    dest->pixels[destY * dest->width + destX] = src->pixels[srcIdx];
                }
            }
        }
    }
}


/*
*  Iterates 'src' and interprets non-space characters as 'colour' pixels, while all other
*  characters result in a 'mask' pixel colour. 
*/
void surface_load (Surface *dest, char *src, uint16_t len, uint16_t colour, uint16_t mask) {
    colour = ((colour << 8) & 0xff00) | (colour >> 8);
    mask = ((mask << 8) & 0xff00) | (mask >> 8);
    for (int i=0; i < len; i++) {
        dest->pixels[i] = src[i] == ' ' ? mask : colour;
    }
}



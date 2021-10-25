#include "surface.h"



Surface *surface_create (int width, int height) {
    Surface *surface = (Surface *)malloc(sizeof(Surface));
    surface->size = width * height;
    surface->width = width;
    surface->height = height;
    surface->pixels = (uint16_t *)malloc(width * height * 2);
    return surface;
}


void surface_destroy (Surface *surface) {
    free(surface->pixels);
    free(surface);
}


void surface_fill (Surface *surface, uint16_t colour) {
    colour = ((colour << 8) & 0xff00) | (colour >> 8);
    for (int i = 0; i < surface->size; i++) surface->pixels[i] = colour;
}


void surface_fill_rgb (Surface *surface, uint8_t r, uint8_t g, uint8_t b) {
    uint16_t colour = ((r / 8) << 11) + ((g / 8) << 6) + (b / 8);
    colour = ((colour << 8) & 0xff00) | (colour >> 8);
    for (int i = 0; i < surface->size; i++) surface->pixels[i] = colour;
}


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
            int destX = destRect->x + x;
            if (destX < 0 || destX >= dest->width) continue;
            if (src->pixels[srcIdx] == mask) continue;
            int destIdx = destY * dest->width + destX;
            dest->pixels[destIdx] = src->pixels[srcIdx];
        }
    }
}


void surface_load (Surface *dest, char *src, uint16_t len, uint16_t colour, uint16_t mask) {
    colour = ((colour << 8) & 0xff00) | (colour >> 8);
    mask = ((mask << 8) & 0xff00) | (mask >> 8);
    for (int i=0; i < len; i++) {
        dest->pixels[i] = src[i] == ' ' ? mask : colour;
    }
}



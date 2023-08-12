#include "surface.h"
#include "font.h"
#include "sprite.h"
#include "types.h"



void font_print (Surface *surface, Font *font, char *text, uint16_t x, uint16_t y, uint16_t colour) {
    uint8_t font_size = font->width * font->height, px = 0, py = 0;
    for (uint8_t i = 0, l = strlen(text); i < l; i++) {
        uint8_t code = (text[i] >= font->ascii_start ? text[i] - font->ascii_start : '?');
        if (code < 0 || code >= font->ascii_end - font->ascii_start) continue;
        uint16_t idx = font_size * code;
        px = i * (font->width + font->spacing);
        py = 0;
        for (uint16_t offset = 0; offset < font_size; offset++) {
            char pixel = font->data[idx + offset];
            if (pixel != ' ' 
             && x + px < surface->width 
             && y + py < surface->height) {
                surface_putpixel(surface, x + px, y + py, colour);
            }
            if (offset % font->width == font->width - 1) {
                py++;
                px = i * (font->width + font->spacing);
            } else {
                px++;
            }
        }
    }
}

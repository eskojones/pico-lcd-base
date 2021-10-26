#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "types.h"
#include "surface.h"
#include "pico/time.h"
#include "hardware/timer.h"


typedef struct {
    Surface *atlas;                 //source atlas
    uint8_t  framesPerRow;          //how many frames wide is the atlas? (must be exact multiple)
    uint16_t width, height;         //pixel width and height of each frame
    uint16_t startIndex, stopIndex; //frame index of first and last frame in sprite
    uint16_t currentIndex;          //frame index of current frame
    bool playing, loop;             //is playing? is looping?
    float lastFrameTime, delay;     //time in seconds since last frame change, and the time per frame
    Surface *frame;                 //current frame
} Sprite;


Sprite *    sprite_create       (Surface *atlas, uint16_t width, uint16_t height, uint16_t startIdx, uint16_t stopIdx, float delay);
void        sprite_set_frame    (Sprite *sprite, uint16_t frameIndex);
void        sprite_update       (Sprite *sprite);
void        sprite_draw         (Surface *dest, Sprite *sprite, Rect *destRect);
void        sprite_draw_mask    (Surface *dest, Sprite *sprite, Rect *destRect, uint16_t mask);

#endif

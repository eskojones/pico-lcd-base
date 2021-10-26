#include "sprite.h"


Sprite *sprite_create (Surface *atlas, uint16_t width, uint16_t height, uint16_t startIdx, uint16_t stopIdx, float delay) {
    Sprite *sprite = (Sprite *)malloc(sizeof(Sprite));
    sprite->atlas = atlas;
    sprite->width = width;
    sprite->height = height;
    sprite->framesPerRow = floor((float)atlas->width / (float)width);
    sprite->startIndex = startIdx;
    sprite->stopIndex = stopIdx;
    sprite->currentIndex = startIdx;
    sprite->lastFrameTime = time_us_64() / 1000000.0f;
    sprite->delay = delay;
    sprite->playing = false;
    sprite->loop = false;
    sprite->frame = surface_create(width, height);
    sprite_set_frame(sprite, sprite->startIndex);
    return sprite;
}


void sprite_set_frame(Sprite *sprite, uint16_t frameIndex) {
    if (frameIndex < sprite->startIndex) return;
    while (frameIndex > sprite->stopIndex) frameIndex -= (sprite->stopIndex - sprite->startIndex);
    Rect atlasRect, frameRect;
    frameRect.x = 0;
    frameRect.y = 0;
    frameRect.w = atlasRect.w = sprite->width;
    frameRect.h = atlasRect.h = sprite->height;
    atlasRect.x = (frameIndex % sprite->framesPerRow) * sprite->width;
    atlasRect.y = floor((float)frameIndex / (float)sprite->framesPerRow) * sprite->height;
    atlasRect.w = sprite->width;
    atlasRect.h = sprite->height;
    surface_blit(sprite->frame, sprite->atlas, &frameRect, &atlasRect);
    sprite->currentIndex = frameIndex;
}


void sprite_update (Sprite *sprite) {
    // if (sprite->playing == false) return;
    // if (sprite->currentIndex == sprite->stopIndex) return;
    float tNow = time_us_64() / 1000000.0f;
    if (tNow - sprite->lastFrameTime >= sprite->delay) {
        sprite->lastFrameTime = tNow;
        int nextFrameIndex = sprite->currentIndex == sprite->stopIndex ? sprite->startIndex : ++sprite->currentIndex;
        sprite_set_frame(sprite, nextFrameIndex);
        sprite->currentIndex = nextFrameIndex;
    }
}


void sprite_draw (Surface *dest, Sprite *sprite, Rect *destRect) {
    Rect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = sprite->width;
    srcRect.h = sprite->height;
    surface_scaleblit(dest, sprite->frame, destRect, &srcRect);
}


void sprite_draw_mask (Surface *dest, Sprite *sprite, Rect *destRect, uint16_t mask) {
    Rect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = sprite->width;
    srcRect.h = sprite->height;
    surface_scaleblit_mask(dest, sprite->frame, destRect, &srcRect, mask);
}


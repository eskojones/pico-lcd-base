#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pico/time.h"
#include "hardware/timer.h"

#include "types.h"
#include "lcd.h"
#include "surface.h"


//plz no lawyers its just a joke
char LogoChars[] = { //52x8
    "##   ## ##                             ##           " \
    "###  ## ##        ##                   ##        ## " \
    "###  ##          ####                  ##       #  #" \
    "## # ## ## ## ##  ##  ####  ## ##   #####  ####  ## " \
    "## # ## ## ### ## ## ##  ## ### ## ##  ## ##  ##    " \
    "##  ### ## ##  ## ## ###### ##  ## ##  ## ##  ##    " \
    "##  ### ## ##  ## ## ##     ##  ## ##  ## ##  ##    " \
    "##   ## ## ##  ## ##  ##### ##  ##  #####  ####     "
};


//struct to pass between update() and render() functions
typedef struct {
    uint64_t frames;
    Surface *screen, *logo;
    Rect screenRect, logoRect;
    uint8_t logoModeX, logoModeY;
    uint8_t r,g,b;
} State;


void update (float delta, State *state) {
    state->r++;// = rand() % 256;
    state->g++;// = rand() % 256;
    state->b--;// = rand() % 256;

    if (state->logoModeX == 1) {
        state->screenRect.x += 2;
        if (state->screenRect.x > LCD_WIDTH - state->logo->width) state->logoModeX = -1;
    } else {
        state->screenRect.x -= 2;
        if (state->screenRect.x < 0) state->logoModeX = 1;
    }

    if (state->logoModeY == 1) {
        state->screenRect.y += 2;
        if (state->screenRect.y > LCD_HEIGHT - state->logo->height) state->logoModeY = -1;
    } else {
        state->screenRect.y -= 2;
        if (state->screenRect.y < 0) state->logoModeY = 1;
    }

}


void render (float delta, State *state) {
    //primes to use with lcd_draw_surface_checkered()
    uint32_t primes[] = { 
           223, 738797, 255253, 947357, 760231, 558611, 349211, 185593, 895171, 532163,
        825779, 199819, 493523, 101383, 918949, 254827, 340577, 893603, 707249, 346369,
        276173,  71761,  92639,  86293,  22343,  50111,  76673,  47351,  20981,  32633 
    };

    //Clear screen to current r,g,b value
    surface_fill_rgb(state->screen, state->r, state->g, state->b);

    //Alternatively, you can surface_fill() with a 16-bit colour value
    //surface_fill(state->screen, WHITE);

    //Copy the 'logo' surface to the 'screen', using the logoRect to specify the region of the logo
    //to be copied, while the screenRect specifies the destination position (no scaling is done)
    //The mask colour will be ignored in the 'logo' surface, allowing for transparent backgrounds
    surface_blit_mask(state->screen, state->logo, &state->screenRect, &state->logoRect, WHITE);

    //Send the 'screen' surface to the LCD panel (sequential pixels)
    //lcd_draw_surface(state->screen);

    //Send the 'screen' surface to the LCD panel in subdivided tiles
    lcd_draw_surface_checkered(state->screen, 64 + (rand() % 2 == 0 ? 64 : 0), primes[state->frames % 30]);

    state->frames++;
}


int main(void)
{
    State *state = (State *)malloc(sizeof(State));

    //configure the LCD for use
    lcd_init();
    lcd_set_backlight(100); 

    //create the surface to use as the screen buffer
    state->screen = surface_create(LCD_WIDTH, LCD_HEIGHT);
    surface_fill(state->screen, WHITE);

    state->logo = surface_create(52, 8);
    surface_load(state->logo, LogoChars, 52 * 8, RED, WHITE);

    state->r = 255;
    state->g = 127;
    state->b = 31;

    state->logoRect.x = 0;
    state->logoRect.y = 0;
    state->logoRect.w = 52;
    state->logoRect.h = 8;
    state->screenRect.x = rand() % (LCD_WIDTH - 52);
    state->screenRect.y = rand() % (LCD_HEIGHT - 8);
    state->logoModeX = -1 + rand() % 3;
    state->logoModeY = -1 + rand() % 3;

    state->frames = 0;

    uint64_t tLastUpdate = time_us_64() + 1, tLastRender = time_us_64() + 1;

    while (true) {
        uint64_t t = time_us_64();
        float deltaUpdate = (t - tLastUpdate) / 1000000.0f;
        float deltaRender = (t - tLastRender) / 1000000.0f;

        if (deltaUpdate >= 0.05f) {
            tLastUpdate = t;
            update(deltaUpdate, state);
        }

        if (deltaRender >= 0.05f) {
            tLastRender = t;
            render(deltaRender, state);
        }
    }

    return 0;
}


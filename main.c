#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "stdbool.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "font.h"
#include "lcd.h"
#include "sprite.h"
#include "surface.h"
#include "types.h"


int main () {
    //init std in/out
    stdio_init_all();

    //configure the ADC so we can read temp sensor
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_gpio_init(PICO_VSYS_PIN);
    adc_select_input(4); //temp

    //init the LCD panel and the surface we'll use to draw
    lcd_init();
    lcd_set_backlight(1023);
    Surface *screen = surface_create(LCD_WIDTH, LCD_HEIGHT);

    srand(1337);
    char str[256];

    while(1) {
        //clear screen
        surface_fill(screen, BLACK);

        //read temp sensor and convert to celsius
        float adc_result = (float)adc_read() * (3.3f / (1 << 12));
        float tempC = 27.0f - (adc_result - 0.706f) / 0.001721f;

        //print temperature to screen with font
        sprintf(str, "Temp: %.2f C", tempC);
        font_print(screen, str, 1, 1 + (rand() % (LCD_HEIGHT - 8)), WHITE);

        //draw some random lines on the screen
        for (int i = 0; i < 20; i++) {
            surface_line(
                screen, 
                rand() % LCD_WIDTH, 
                rand() % LCD_HEIGHT, 
                rand() % LCD_WIDTH, 
                rand() % LCD_HEIGHT, 
                rand() % 0xffff
            );
        }

        //send the surface to the lcd
        lcd_draw_surface(screen);

        sleep_ms(200);
    }
    return 0;
}
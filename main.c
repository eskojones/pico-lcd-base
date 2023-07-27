#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "stdbool.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/float.h"
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

    //init the LCD panel and the surface we'll use to draw
    lcd_init();
    lcd_set_backlight(25);
    Surface *screen = surface_create(LCD_WIDTH, LCD_HEIGHT);

    srand(1337);
    char str[256];
    int frame = 0;
    while(1) {
        //clear screen
        surface_fill(screen, BLACK);
        int offs = frame % 16;
        for (int i = 0; i <= 20; i++) {
            surface_line(
                screen, 
                1, offs + (i * 15),
                LCD_WIDTH, offs + (i * 15),
                CYAN
            );
            surface_line(
                screen, 
                offs + (i * 15), 1,
                offs + (i * 15), LCD_HEIGHT,
                CYAN
            );
        }

        //read temp sensor and convert to celsius
        adc_select_input(4); //temp
        float adc_result = (float)adc_read() * (3.3f / (1 << 12));
        float tempC = 27.0f - (adc_result - 0.706f) / 0.001721f;
        //print temperature to screen with font
        sprintf(str, "Temp: %.2f C", tempC);
        font_print(screen, str, 1, 1, GREEN);

        adc_gpio_init(PICO_VSYS_PIN);
        adc_select_input(PICO_VSYS_PIN - 26);
    
        adc_fifo_setup(true, false, 0, false, false);
        adc_run(true);

        // We seem to read low values initially - this seems to fix it
        int ignore_count = 10;
        while (!adc_fifo_is_empty() || ignore_count-- > 0) {
            (void)adc_fifo_get_blocking();
        }

        // read vsys
        uint32_t vsys = 0;
        for(int i = 0; i < 10; i++) {
            uint16_t val = adc_fifo_get_blocking();
            vsys += val;
        }

        adc_run(false);
        adc_fifo_drain();

        vsys /= 10;
        float voltage_result = vsys * 3 * (3.3f / (1 << 12));
        sprintf(str, "VSYS: %f V", voltage_result);
        font_print(screen, str, 1, 9, GREEN);

        //send the surface to the lcd
        lcd_draw_surface(screen);
        sleep_ms(100);
        frame++;
    }
    return 0;
}

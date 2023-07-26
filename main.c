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

#include "surface.h"
#include "lcd.h"
#include "types.h"
#include "sprite.h"
#include "font.h"



int main () {
    stdio_init_all();

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_gpio_init(PICO_VSYS_PIN);
    adc_select_input(4); //temp

    lcd_init();
    lcd_set_backlight(50);
    Surface *screen = surface_create(LCD_WIDTH, LCD_HEIGHT);

    char str[256];

    while(1) {
        surface_fill(screen, BLACK);
        float adc_result = (float)adc_read() * (3.3f / (1 << 12));
        float tempC = 27.0f - (adc_result - 0.706f) / 0.001721f;
        sprintf(str, "Temp: %.2fc", tempC);
        font_print(screen, str, 1, 1, GREEN);
        lcd_draw_surface(screen);
        sleep_ms(500);
    }
    return 0;
}
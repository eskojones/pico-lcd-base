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

#if CYW43_USES_VSYS_PIN
#include "pico/cyw43_arch.h"
#endif

// Pin used for ADC 0
#define PICO_FIRST_ADC_PIN 26


#include "font.h"
#include "lcd.h"
#include "sprite.h"
#include "surface.h"
#include "types.h"


float adc_read_temp () {
    adc_select_input(4);
    return 27.0f - ((float)adc_read() * (3.3f / (1 << 12)) - 0.706f) / 0.001721f;
}


float adc_read_vsys () {
#if CYW43_USES_VSYS_PIN
    cyw43_thread_enter();
    // Make sure cyw43 is awake
    cyw43_arch_gpio_get(CYW43_WL_GPIO_VBUS_PIN);
#endif

    adc_gpio_init(PICO_VSYS_PIN);
    adc_select_input(PICO_VSYS_PIN - PICO_FIRST_ADC_PIN);
    adc_fifo_setup(true, false, 0, false, false);
    adc_run(true);
    uint32_t vsys = 0;
    int ignore_count = 10;
    while (!adc_fifo_is_empty() || ignore_count-- > 0) adc_fifo_get_blocking();
    for(int i = 0; i < 10; i++) vsys += adc_fifo_get_blocking();
    adc_run(false);
    adc_fifo_drain();
#if CYW43_USES_VSYS_PIN
    cyw43_thread_exit();
#endif
    return (vsys / 10) * 3 * (3.3f / (1 << 12));
}

int main () {
    //init std in/out
    stdio_init_all();

    //configure the ADC so we can read temp sensor
    adc_init();
    adc_set_temp_sensor_enabled(true);
    // Pico W uses a CYW43 pin to get VBUS so we need to initialise it
    #if CYW43_USES_VSYS_PIN
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }
    #endif
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

        float tempC = adc_read_temp();
        sprintf(str, "Temp: %.2f C", tempC);
        font_print(screen, str, 1, 1, GREEN);

        float vsys = adc_read_vsys();
        sprintf(str, "VSYS: %f V", vsys);
        font_print(screen, str, 1, 9, GREEN);

        //send the surface to the lcd
        lcd_draw_surface(screen);
        sleep_ms(100);
        frame++;
    }

    #if CYW43_USES_VSYS_PIN
        cyw43_arch_deinit();
    #endif
    return 0;
}

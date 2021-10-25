#include "lcd.h"


void gpio_mode (int pin, int mode) {
    gpio_init(pin);
    gpio_set_dir(pin, mode == 0 || mode == GPIO_IN ? GPIO_IN : GPIO_OUT);
}


void lcd_send_command (uint8_t reg) {
    gpio_put(EPD_DC_PIN, 0);
    gpio_put(EPD_CS_PIN, 0);
    spi_write_blocking(LCD_SPI_PORT, &reg, 1);
    gpio_put(EPD_CS_PIN, 1);
}


void lcd_send_byte (uint8_t val) {
    gpio_put(EPD_DC_PIN, 1);
    gpio_put(EPD_CS_PIN, 0);
    spi_write_blocking(LCD_SPI_PORT, &val, 1);
    gpio_put(EPD_CS_PIN, 1);
}


void lcd_send_bytes (uint8_t *vals, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) lcd_send_byte(vals[i]);
}


void lcd_send_word (uint16_t word) {
    char bytes[] = {
        word >> 8 & 0xff,
        word & 0xff
    };
    gpio_put(EPD_DC_PIN, 1);
    gpio_put(EPD_CS_PIN, 0);
    spi_write_blocking(LCD_SPI_PORT, (char *)bytes, 1);
    spi_write_blocking(LCD_SPI_PORT, (char *)(bytes+1), 1);
    gpio_put(EPD_CS_PIN, 1);
}


void lcd_set_scan (uint8_t scan_dir) {
    //Get the screen scan direction
    //LCD_1IN8.SCAN_DIR = scan_dir;
    uint8_t MemoryAccessReg = 0x00;
    //Get GRAM and LCD width and height
    if (scan_dir == HORIZONTAL) {
        //LCD_1IN8.HEIGHT	= LCD_1IN8_WIDTH;
        //LCD_1IN8.WIDTH   = LCD_1IN8_HEIGHT;
        MemoryAccessReg = 0X70;
    } else {
        //LCD_1IN8.HEIGHT	= LCD_1IN8_HEIGHT;       
        //LCD_1IN8.WIDTH   = LCD_1IN8_WIDTH;
        MemoryAccessReg = 0X00;
    }
    // Set the read / write scan direction of the frame memory
    lcd_send_command(0x36); //MX, MY, RGB mode
    lcd_send_byte(MemoryAccessReg);	//0x08 set RGB
}


void lcd_set_window (Rect *rect) {
    //set the X coordinates
    lcd_send_command(0x2A);
    lcd_send_byte(0x00);
    lcd_send_byte(rect->x+1);
	lcd_send_byte(0x00);
    lcd_send_byte(rect->x+rect->w);

    //set the Y coordinates
    lcd_send_command(0x2B);
    lcd_send_byte(0x00);
	lcd_send_byte(rect->y+1);
	lcd_send_byte(0x00);
    lcd_send_byte(rect->y+rect->h);

    lcd_send_command(0X2C);
}


int lcd_init () {
    stdio_init_all();
    //GPIO PIN
    EPD_RST_PIN     = 12;  //Reset
    EPD_DC_PIN      = 8;   //Data or Command
    EPD_BL_PIN      = 13;  //Backlight
    EPD_CS_PIN      = 9;   //Chip Select
    EPD_CLK_PIN     = 10;  
    EPD_MOSI_PIN    = 11;  
    EPD_SCL_PIN     = 7;   
    EPD_SDA_PIN     = 6;   
    
    // SPI Config
    spi_init(LCD_SPI_PORT, 10000 * 1000);
    gpio_set_function(EPD_CLK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(EPD_MOSI_PIN, GPIO_FUNC_SPI);
    
    // GPIO Config
    gpio_mode(EPD_RST_PIN, 1);
    gpio_mode(EPD_DC_PIN, 1);
    gpio_mode(EPD_CS_PIN, 1);
    gpio_mode(EPD_BL_PIN, 1);

    gpio_put(EPD_CS_PIN, 1);
    gpio_put(EPD_DC_PIN, 1);
    gpio_put(EPD_BL_PIN, 1);
    
    // PWM Config
    gpio_set_function(EPD_BL_PIN, GPIO_FUNC_PWM);
    LCD_BacklightSlice = pwm_gpio_to_slice_num(EPD_BL_PIN);
    pwm_set_wrap(LCD_BacklightSlice, 100);
    pwm_set_chan_level(LCD_BacklightSlice, PWM_CHAN_B, 1);
    pwm_set_clkdiv(LCD_BacklightSlice, 50);
    pwm_set_enabled(LCD_BacklightSlice, true);
    
    //I2C Config
    i2c_init(i2c1, 300 * 1000);
    gpio_set_function(EPD_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(EPD_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(EPD_SDA_PIN);
    gpio_pull_up(EPD_SCL_PIN);

    pwm_set_chan_level(LCD_BacklightSlice, PWM_CHAN_B, 90);

    //Hardware reset
    gpio_put(EPD_RST_PIN, 1);
    sleep_ms(100);
    gpio_put(EPD_RST_PIN, 0);
    sleep_ms(100);
    gpio_put(EPD_RST_PIN, 1);
    sleep_ms(100);

    //Set the resolution and scanning method of the screen
    lcd_set_scan(HORIZONTAL);

    //Set the initialization register
	lcd_send_command(0x3A);
    lcd_send_byte(0x05);

     //ST7735R Frame Rate
    lcd_send_command(0xB1);
    lcd_send_byte(0x01);
    lcd_send_byte(0x2C);
    lcd_send_byte(0x2D);

    lcd_send_command(0xB2);
    lcd_send_byte(0x01);
    lcd_send_byte(0x2C);
    lcd_send_byte(0x2D);

    lcd_send_command(0xB3);
    lcd_send_byte(0x01);
    lcd_send_byte(0x2C);
    lcd_send_byte(0x2D);
    lcd_send_byte(0x01);
    lcd_send_byte(0x2C);
    lcd_send_byte(0x2D);

    lcd_send_command(0xB4); //Column inversion
    lcd_send_byte(0x07);

    //ST7735R Power Sequence
    lcd_send_command(0xC0);
    lcd_send_byte(0xA2);
    lcd_send_byte(0x02);
    lcd_send_byte(0x84);

    lcd_send_command(0xC1);
    lcd_send_byte(0xC5);

    lcd_send_command(0xC2);
    lcd_send_byte(0x0A);
    lcd_send_byte(0x00);

    lcd_send_command(0xC3);
    lcd_send_byte(0x8A);
    lcd_send_byte(0x2A);

    lcd_send_command(0xC4);
    lcd_send_byte(0x8A);
    lcd_send_byte(0xEE);

    lcd_send_command(0xC5); //VCOM
    lcd_send_byte(0x0E);

    //ST7735R Gamma Sequence
    lcd_send_command(0xe0);
    lcd_send_byte(0x0f);
    lcd_send_byte(0x1a);
    lcd_send_byte(0x0f);
    lcd_send_byte(0x18);
    lcd_send_byte(0x2f);
    lcd_send_byte(0x28);
    lcd_send_byte(0x20);
    lcd_send_byte(0x22);
    lcd_send_byte(0x1f);
    lcd_send_byte(0x1b);
    lcd_send_byte(0x23);
    lcd_send_byte(0x37);
    lcd_send_byte(0x00);
    lcd_send_byte(0x07);
    lcd_send_byte(0x02);
    lcd_send_byte(0x10);

    lcd_send_command(0xe1);
    lcd_send_byte(0x0f);
    lcd_send_byte(0x1b);
    lcd_send_byte(0x0f);
    lcd_send_byte(0x17);
    lcd_send_byte(0x33);
    lcd_send_byte(0x2c);
    lcd_send_byte(0x29);
    lcd_send_byte(0x2e);
    lcd_send_byte(0x30);
    lcd_send_byte(0x30);
    lcd_send_byte(0x39);
    lcd_send_byte(0x3f);
    lcd_send_byte(0x00);
    lcd_send_byte(0x07);
    lcd_send_byte(0x03);
    lcd_send_byte(0x10);

    lcd_send_command(0xF0); //Enable test command
    lcd_send_byte(0x01);

    lcd_send_command(0xF6); //Disable ram power save mode
    lcd_send_byte(0x00);

	//sleep out
    lcd_send_command(0x11);
    sleep_ms(120);

    //Turn on the LCD display
    lcd_send_command(0x29);
	sleep_ms(120);

    return 0;
}


void lcd_set_backlight(uint16_t val) {
    pwm_set_chan_level(LCD_BacklightSlice, PWM_CHAN_B, val > 1023 ? 1023 : val);
}


void lcd_draw_surface(Surface *surface) {
    Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = surface->width;
    rect.h = surface->height;
    lcd_set_window(&rect);
    gpio_put(EPD_DC_PIN, 1);
    gpio_put(EPD_CS_PIN, 0);
    spi_write_blocking(LCD_SPI_PORT, (uint8_t *)&surface->pixels[0], surface->size * 2);
    gpio_put(EPD_CS_PIN, 1);
    lcd_send_command(0x29);
}


void lcd_draw_surface_checkered(Surface *surface, uint8_t size, uint32_t prime) {
    Rect rect;
    int tcol = ceil((surface->width * 1.0f) / (size * 1.0f));
    int trow = ceil((surface->height * 1.0f) / (size * 1.0f));
    int tsize = tcol * trow;
    for (int ty = 0; ty < trow; ty++) {
        for (int tx = 0; tx < tcol; tx++) {
            uint16_t tile_idx = ((ty * tcol + tx) * prime) % tsize;
            rect.x = (tile_idx % tcol) * size;
            rect.y = (tile_idx / tcol) * size;
            rect.w = rect.x > surface->width - size ? surface->width - rect.x : size;
            rect.h = rect.y > surface->height - size ? surface->height - rect.y : size;
            lcd_set_window(&rect);
            uint16_t pixel_idx = rect.y * surface->width + rect.x;
            gpio_put(EPD_DC_PIN, 1);
            gpio_put(EPD_CS_PIN, 0);
            for (int line = 0; line < rect.h; line++) {
                spi_write_blocking(LCD_SPI_PORT, (uint8_t *)&surface->pixels[pixel_idx], rect.w * 2);
                pixel_idx += surface->width;
            }
            gpio_put(EPD_CS_PIN, 1);
        }
    }
}

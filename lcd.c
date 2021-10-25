#include "lcd.h"


/*  
*  Waveshare Pico LCD 1.8inch (C)
*  https://www.waveshare.com/wiki/Pico-LCD-1.8
*  https://www.waveshare.com/w/upload/e/e2/ST7735S_V1.1_20111121.pdf
*/


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
    gpio_put(EPD_DC_PIN, 1);
    gpio_put(EPD_CS_PIN, 0);
    spi_write_blocking(LCD_SPI_PORT, vals, length);
    gpio_put(EPD_CS_PIN, 1);
}


/*
*  Set the portrait/landscape mode of the LCD
*/
void lcd_set_scan (uint8_t scan_dir) {
    uint8_t MemoryAccessReg = 0x00;
    if (scan_dir == HORIZONTAL) {
        MemoryAccessReg = 0X70;
    } else {
        MemoryAccessReg = 0X00;
    }
    lcd_send_command(CMD_MDACONTROL);//MX, MY, RGB mode
    lcd_send_byte(MemoryAccessReg);	//0x08 set RGB
}


/*
*  Sets the pixel data pointer on the LCD driver
*/
void lcd_set_window (Rect *rect) {
    lcd_send_command(CMD_COLADDRSET);
    lcd_send_byte(0x00);            //X Address Start
    lcd_send_byte(rect->x+1);       //X Address Start
	lcd_send_byte(0x00);            //X Address End
    lcd_send_byte(rect->x+rect->w); //X Address End

    lcd_send_command(CMD_ROWADDRSET);
    lcd_send_byte(0x00);            //Y Address Start
	lcd_send_byte(rect->y+1);       //Y Address Start
	lcd_send_byte(0x00);            //Y Address End
    lcd_send_byte(rect->y+rect->h); //Y Address End

    lcd_send_command(CMD_MEMWRITE);
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
    pwm_set_chan_level(LCD_BacklightSlice, PWM_CHAN_B, 100);

    //Hardware reset
    gpio_put(EPD_RST_PIN, 1);
    sleep_ms(100);
    gpio_put(EPD_RST_PIN, 0);
    sleep_ms(100);
    gpio_put(EPD_RST_PIN, 1);
    sleep_ms(100);

    lcd_set_scan(HORIZONTAL);

	lcd_send_command(CMD_PIXELFMT);
    lcd_send_byte(0x05); //IFPF (3 bits, b011 = 12-bit, b101 = 16-bit, b110 = 18-bit)

    lcd_send_command(CMD_FRMCTR1);
    lcd_send_byte(0x01); //RTNA - Set 1-line period (4 bits)
    lcd_send_byte(0x2C); //Front Porch (6 bits)
    lcd_send_byte(0x2D); //Back Porch (6 bits)

    lcd_send_command(CMD_FRMCTR2);
    lcd_send_byte(0x01); //RTNB - Set 1-line period (4 bits)
    lcd_send_byte(0x2C); //Front Porch (6 bits)
    lcd_send_byte(0x2D); //Back Porch (6 bits)

    lcd_send_command(CMD_FRMCTR3); 
    lcd_send_byte(0x01); //RTNC - Set 1-line period (4 bits)
    lcd_send_byte(0x2C); //Front Porch (6 bits)
    lcd_send_byte(0x2D); //Back Porch (6 bits)
    lcd_send_byte(0x01); //RTND - Set 1-line period (4 bits)
    lcd_send_byte(0x2C); //Front Porch (6 bits)
    lcd_send_byte(0x2D); //Back Porch (6 bits)

    lcd_send_command(CMD_INVCTR);
    lcd_send_byte(0x07); //NLA NLB NLC (3 bits)

    lcd_send_command(CMD_PWCTR1);
    lcd_send_byte(0xA2); //AVDD (3 bits) + VRHP (5 bits)
    lcd_send_byte(0x02); //VRHN (5 bits)
    lcd_send_byte(0x84); //MODE (high 2 bits, with lower 6 bits to b000100)

    lcd_send_command(CMD_PWCTR2);
    lcd_send_byte(0xC5); //Refer to ST7735S doc

    lcd_send_command(CMD_PWCTR3);
    lcd_send_byte(0x0A); //Refer to ST7735S doc
    lcd_send_byte(0x00);

    lcd_send_command(CMD_PWCTR4);
    lcd_send_byte(0x8A); //Refer to ST7735S doc
    lcd_send_byte(0x2A);

    lcd_send_command(CMD_PWCTR5);
    lcd_send_byte(0x8A); //Refer to ST7735S doc
    lcd_send_byte(0xEE);

    lcd_send_command(CMD_VMCTR1);
    lcd_send_byte(0x0E); //VCOM Voltage (6 bits)

    lcd_send_command(CMD_GAMCTRP1); 
    lcd_send_byte(0x0f); //Refer to ST7735S doc for full argument table
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

    lcd_send_command(CMD_GAMCTRN1);
    lcd_send_byte(0x0f); //Refer to ST7735S doc for full argument table
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

    lcd_send_command(CMD_TEST);
    lcd_send_byte(0x01); //unknown, see ST7735S doc, assume 1-bit on/off

    lcd_send_command(CMD_RAMPWRSAVE);
    lcd_send_byte(0x00); //unknown, see ST7735S doc, assume 1-bit on/off

    lcd_send_command(CMD_SLEEPOUT);
    sleep_ms(120);

    lcd_send_command(CMD_DISPON);
	sleep_ms(120);

    return 0;
}


void lcd_set_backlight(uint16_t val) {
    pwm_set_chan_level(LCD_BacklightSlice, PWM_CHAN_B, val > 1023 ? 1023 : val);
}


/*
*  Sends the specified Surface to the LCD
*  Surface must be same size as LCD
*/
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


/*
*  Send the specified Surface to the LCD in non-sequential segments of pixels
*  Surface must be same size as LCD
*
*  The surface is divided into tiles of size pixels wide and tall,
*  the tiles are sent in a scrambled order by multiplying the tile index with a
*  supplied prime number, modulo the total number of tiles.
*  This guarentees that each tile will be sent assuming `prime` is actually a prime.
*
*  If the tile size is too small the overhead diminishes performance too noticably,
*  however depending on the type of graphics being animated this may still be 
*  favourable to the sequential drawing mode of the lcd_draw_surface() function which
*  suffers from visible tearing.
*/
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

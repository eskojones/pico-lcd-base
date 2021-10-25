#ifndef _LCD_H_
#define _LCD_H_

#include "types.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "surface.h"

#define LCD_SPI_PORT spi1
#define LCD_WIDTH 160
#define LCD_HEIGHT 130

#define HORIZONTAL 0
#define VERTICAL 1

#define WHITE   0xFFFF
#define BLACK   0x0000
#define BLUE    0x001F
#define BRED    0XF81F
#define GRED    0XFFE0
#define GBLUE   0X07FF
#define RED     0xF800
#define MAGENTA 0xF81F
#define GREEN   0x07E0
#define CYAN    0x7FFF
#define YELLOW  0xFFE0
#define BROWN   0XBC40
#define BRRED   0XFC07
#define GRAY    0X8430

int LCD_BacklightSlice;
int EPD_RST_PIN;
int EPD_DC_PIN;
int EPD_BL_PIN;
int EPD_CS_PIN;
int EPD_CLK_PIN;
int EPD_MOSI_PIN;
int EPD_SCL_PIN;
int EPD_SDA_PIN;


int lcd_init ();
void lcd_set_backlight(uint16_t val);
void lcd_draw_surface(Surface *surface);
void lcd_draw_surface_checkered(Surface *surface, uint8_t size, uint32_t prime);


#endif
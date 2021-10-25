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


/*
*  Function Commands taken from ST7735S documentation
*  Arguments are described partially in lcd_init()
*  See: https://www.waveshare.com/w/upload/e/e2/ST7735S_V1.1_20111121.pdf
*  *** NOTE: This is not a full list of commands! ***
*/
#define CMD_SLEEPIN    0x10 //Sleep In & Booster Off
#define CMD_SLEEPOUT   0x11 //Sleep Out & Booster On
#define CMD_DISPOFF    0x28 //Display Off
#define CMD_DISPON     0x29 //Display On
#define CMD_COLADDRSET 0x2A //Column Address Set
#define CMD_ROWADDRSET 0x2B //Row Address Set
#define CMD_MEMWRITE   0x2C //Memory Write
#define CMD_MDACONTROL 0x36 //Memory Direct Access Control
#define CMD_PIXELFMT   0x3A //Pixel Format
#define CMD_FRMCTR1    0xB1 //In Normal Mode (full colour), Frame Controls
#define CMD_FRMCTR2    0xB2 //In Idle Mode (8-colour), Frame Controls
#define CMD_FRMCTR3    0xB3 //In Partial Mode (full colour), Frame Controls
#define CMD_INVCTR     0xB4 //Display Inversion Control
#define CMD_PWCTR1     0xC0 //Power Control Setting, AVDD, VRHP, VRHN, MODE
#define CMD_PWCTR2     0xC1 //Power Control Setting, see ST7735S doc
#define CMD_PWCTR3     0xC2 //In Normal Mode (full colour), Adjust OpAmp+Booster Voltage
#define CMD_PWCTR4     0xC3 //In Idle Mode (8-colour), Adjust OpAmp+Booster Voltage
#define CMD_PWCTR5     0xC4 //In Partial Mode (full colour), Adjust OpAmp+Booster Circuit
#define CMD_VMCTR1     0xC5 //VCOM Control 1
#define CMD_GAMCTRP1   0xE0 //Gamma Adjustment (+ Polarity)
#define CMD_GAMCTRN1   0xE1 //Gamma Adjustment (- Polarity)
#define CMD_TEST       0xF0 //unknown, Waveshare example claims "Test Command"
#define CMD_RAMPWRSAVE 0xF6 //unknown, Waveshare example claims "RAM Power Save Mode"


int LCD_BacklightSlice;
int EPD_RST_PIN;
int EPD_DC_PIN;
int EPD_BL_PIN;
int EPD_CS_PIN;
int EPD_CLK_PIN;
int EPD_MOSI_PIN;


int lcd_init ();
void lcd_set_backlight(uint16_t val);
void lcd_draw_surface(Surface *surface);
void lcd_draw_surface_checkered(Surface *surface, uint8_t size, uint32_t prime);
void lcd_send_command (uint8_t reg);
void lcd_send_byte (uint8_t val);

#endif
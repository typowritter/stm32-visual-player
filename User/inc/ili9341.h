//*****************************************************************************
//
// ili9341.h
//
//*****************************************************************************

#ifndef __ILI9341_H
#define __ILI9341_H

#include "stm32f10x.h"
#include "fonts.h"

#define SOFT_SELECT         0

#define TFT_APBxClock       RCC_APB2PeriphClockCmd // all pins are on APB2
#define TFT_SPIx            SPI1
#define TFT_SPI_CLK         RCC_APB2Periph_SPI1
#define TFT_GPIO_CLK        (RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC)

#define SCLK_PORT           GPIOA
#define SCLK_PIN            GPIO_Pin_5
#define MOSI_PORT           GPIOA
#define MOSI_PIN            GPIO_Pin_7
#define RES_PORT            GPIOC
#define RES_PIN             GPIO_Pin_13
#define DC_PORT             GPIOA
#define DC_PIN              GPIO_Pin_6
#if HARD_SELECT
#define CS_PORT             GPIOC
#define CS_PIN              GPIO_Pin_4
#endif


#define MAX_W       320
#define MAX_H       240

#define AQUA        0x07FF
#define BLACK       0x0000
#define BLUE        0x001F
#define CYAN        0x07FF
#define DARKCYAN    0x03EF
#define DARKGREEN   0x03E0
#define DARKGREY    0x7BEF
#define FUCHSIA     0xF81F
#define GRAY1       0x8410
#define GRAY2       0x4208
#define GRAY3       0x2104
#define GREEN       0x07e0
#define LIGHTGREEN  0xAFE5
#define LIGHTGREY   0xC618
#define LIME        0x07E0
#define MAGENTA     0xF81F
#define MAROON      0x7800
#define NAVY        0x000F
#define OLIVE       0x8400
#define ORANGE      0xFD20
#define PURPLE      0x780F
#define RED         0xF800
#define SILVER      0xC618
#define TEAL        0x0410
#define WHITE       0xFFFF
#define YELLOW      0xFFE0

#define FONT_W          (get_font()->width)
#define FONT_H          (get_font()->height)

#define LINE(x)         ((x) * (FONT_W))
#define LINEY(x)        ((x) * (FONT_H))

extern uint16_t x_Max;
extern uint16_t y_Max;

void TFT_init();
void clear_region(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void disp_string(uint16_t x, uint16_t y, char *pStr);
sFONT* get_font();

#endif /* __ILI9341_H */
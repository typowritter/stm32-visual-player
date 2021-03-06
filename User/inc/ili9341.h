/**
  ******************************************************************************
  * @file    ili9341.h
  * @author  杨向南 yxnan@pm.me
  * @date    2020-11-14
  * @brief   SPI driver for ili9341 controller
  ******************************************************************************
  */

#ifndef __ILI9341_H
#define __ILI9341_H

#include "stm32f10x.h"
#include "fonts.h"

/* 因为只有一个SPI设备，直接把CS引脚接地，不再分配端口进行读写 */
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
#define RES_PIN             GPIO_Pin_7
#define DC_PORT             GPIOC
#define DC_PIN              GPIO_Pin_6
#if SOFT_SELECT
#define CS_PORT             GPIOC
#define CS_PIN              GPIO_Pin_4
#endif

#define MAX_W       320
#define MAX_H       240

// RGB565 (16-bit)
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
void draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t filled);
void disp_string(uint16_t x, uint16_t y, char *pStr);
void set_font(sFONT* font);
void set_backColor(uint16_t color);
void set_foreColor(uint16_t color);
sFONT* get_font();

#endif /* __ILI9341_H */
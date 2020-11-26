/**
  ******************************************************************************
  * @file    ili9341.c
  * @author  杨向南 yxnan@pm.me
  * @date    2020-11-14
  * @brief   SPI driver for ili9341 controller
  ******************************************************************************
  */

#include "stm32f10x.h"
#include "ili9341.h"
#include "delay.h"

static void SPI_init();
static void TFT_reset();
static void TFT_config();
static void sendCmd(uint8_t number);
static void sendParameter(uint8_t number);
static void sendColor(uint16_t color);
static void set_gram(uint8_t ucOption);
static void set_cursor(uint16_t x, uint16_t y);
static void draw_pixel(uint16_t x, uint16_t y);
static void fill_window(uint32_t pointCnt, uint16_t color);
static void open_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
static void disp_char(uint16_t x, uint16_t y, const char cChar);
#if SOFT_SELECT
static void TFT_select();
static void TFT_unselect();
#endif

static uint16_t foreColor;
static uint16_t backColor;
static sFONT*   currentFont;

uint16_t x_Max;
uint16_t y_Max;

void set_font(sFONT* font)
{
    currentFont = font;
}

sFONT* get_font()
{
    return currentFont;
}

void set_backColor(uint16_t color)
{
    backColor = color;
}

void set_foreColor(uint16_t color)
{
    foreColor = color;
}

static void set_gram(uint8_t ucOption)
{
    if (ucOption % 2 == 0)
    {
        x_Max = MAX_H;
        y_Max = MAX_W;
    }
    else
    {
        x_Max = MAX_W;
        y_Max = MAX_H;
    }

    sendCmd(0x36);
    sendParameter(0x08 | (ucOption << 5));
}

static void sendCmd(uint8_t number)
{
    GPIO_ResetBits(DC_PORT, DC_PIN);

    while (RESET == SPI_I2S_GetFlagStatus(TFT_SPIx, SPI_I2S_FLAG_TXE))
        ;

    SPI_I2S_SendData(TFT_SPIx, number);
}

static void sendParameter(uint8_t number)
{
    GPIO_SetBits(DC_PORT, DC_PIN);

    while (RESET == SPI_I2S_GetFlagStatus(TFT_SPIx, SPI_I2S_FLAG_TXE))
        ;

    SPI_I2S_SendData(TFT_SPIx, number);
}

static void sendColor(uint16_t color)
{
    sendParameter(color >> 8);
    sendParameter(color & 0xFF);
}

#if SOFT_SELECT
static void TFT_select()
{
    GPIO_ResetBits(CS_PORT, CS_PIN);
}

static void TFT_unselect()
{
    GPIO_SetBits(CS_PORT, CS_PIN);
}
#else
#define TFT_select()
#define TFT_unselect()
#endif

static void TFT_reset()
{
    GPIO_ResetBits(RES_PORT, RES_PIN);
    delay_us(12); // tRW_min = 10us

    GPIO_SetBits(RES_PORT, RES_PIN);
    delay_ms(10); // tRT_max = 5ms
}

static void open_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    sendCmd(0x2A);
    sendParameter( x >> 8  );
    sendParameter( x & 0xff  );
    sendParameter( ( x + w - 1 ) >> 8  );
    sendParameter( ( x + w - 1 ) & 0xff );

    sendCmd(0x2B);
    sendParameter( y >> 8  );
    sendParameter( y & 0xff  );
    sendParameter( ( y + h - 1 ) >> 8 );
    sendParameter( ( y + h - 1) & 0xff );
}

static void set_cursor(uint16_t x, uint16_t y)
{
    open_window(x, y, 1, 1);
}

static void draw_pixel(uint16_t x, uint16_t y)
{
    if ((x < MAX_W) && (y < MAX_H))
    {
        set_cursor(x, y);
        fill_window(1, foreColor);
    }
}

static void fill_window(uint32_t pointCnt, uint16_t color)
{
    sendCmd(0x2C);
    for (; pointCnt > 0; pointCnt--)
        sendColor(color);
}

void clear_region(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    open_window(x, y, w, h);
    fill_window((uint32_t)w * h, backColor);
}

// Bresenham algorithm
void draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint16_t us;
    uint16_t cur_x, cur_y;

    int32_t err_x = 0, err_y = 0, delta_x, delta_y, distance;
    int32_t inc_x, inc_y;

    delta_x = x2 - x1;
    delta_y = y2 - y1;

    cur_x = x1;
    cur_y = y1;


    if (delta_x > 0) inc_x = 1;
    else if (delta_x == 0) inc_x = 0;
    else {
        inc_x = -1;
        delta_x = - delta_x;
    }


    if (delta_y > 0) inc_y = 1;
    else if (delta_y == 0) inc_y = 0;
    else {
        inc_y = -1;
        delta_y = - delta_y;
    }


    if (delta_x > delta_y)
        distance = delta_x;
    else
        distance = delta_y;


    for (us = 0; us <= distance + 1; us++)
    {
        draw_pixel(cur_x, cur_y);

        err_x += delta_x ;
        err_y += delta_y ;

        if (err_x > distance)
        {
            err_x -= distance;
            cur_x += inc_x;
        }

        if (err_y > distance)
        {
            err_y -= distance;
            cur_y += inc_y;
        }

    }
}

static inline void fill_screen()
{
    clear_region(0, 0, x_Max, y_Max);
}

static void disp_char(uint16_t x, uint16_t y, const char cChar)
{
    uint8_t  byteCount, bitCount, fontLength;
    uint16_t ucRelativePositon;
    uint8_t *Pfont;

    // 对ascii码表偏移（字模表不包含ASCII表的前32个非图形符号）
    ucRelativePositon = cChar - ' ';

    // 每个字模的字节数
    fontLength = (FONT_W * FONT_H)/8;

    // 字模首地址
    Pfont = (uint8_t *)&(currentFont->table)[ucRelativePositon * fontLength];

    open_window(x, y, FONT_W, FONT_H);
    sendCmd(0x2C);

    // 按字节读取字模数据
    for (byteCount = 0; byteCount < fontLength; byteCount++)
    {
        for (bitCount = 0; bitCount < 8; bitCount++)
        {
            if ( Pfont[byteCount] & (0x80>>bitCount) )
                sendColor(foreColor);
            else
                sendColor(backColor);
        }
    }
}

void disp_string(uint16_t x, uint16_t y, char *pStr)
{
    while (*pStr != '\0')
    {
        if (x + FONT_W > x_Max)
        {
            x = 0;
            y += FONT_H;
        }

        if (y + FONT_H > y_Max)
        {
            x = 0;
            y = 0;
        }

        disp_char(x, y, *pStr);
        pStr ++;
        x += FONT_W;
    }
}

static void SPI_init()
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    TFT_APBxClock(TFT_SPI_CLK|TFT_GPIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin     = RES_PIN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_Init(RES_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = DC_PIN;
    GPIO_Init(DC_PORT, &GPIO_InitStructure);

    #if SOFT_SELECT
    GPIO_InitStructure.GPIO_Pin     = CS_PIN;
    GPIO_Init(CS_PORT, &GPIO_InitStructure);
    #endif

    GPIO_InitStructure.GPIO_Pin     = SCLK_PIN;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_Init(SCLK_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = MOSI_PIN;
    GPIO_Init(MOSI_PORT, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(TFT_SPIx, &SPI_InitStructure);

    SPI_Cmd(TFT_SPIx, ENABLE);
}

static void TFT_config()
{
    TFT_select();
    TFT_reset();

    sendCmd(0xCB);  // POWER CONTROL A
    sendParameter(0x39);    // 1. (default)
    sendParameter(0x2C);    // 2. (default)
    sendParameter(0x00);    // 3. (default)
    sendParameter(0x34);    // 4. (Vcore Controll: 1.55V, default)
    sendParameter(0x02);    // 5. (DDVDH: 5.8V, default)

    sendCmd(0xCF);  // POWER CONTROL B
    sendParameter(0x00);    // 1. (default)
    sendParameter(0xC1);    // 2. (PC&EQ operation for power saving enabled), 0x81 def.
    sendParameter(0x30);    // 3. (default)

    sendCmd(0xE8);  // DRIVER TIMING CONTROL A
    sendParameter(0x85);    // 1. (gate driver non-overlap timing control), 0x84 def.
    sendParameter(0x00);    // 2. (EQ timing control), 0x11 def.
    sendParameter(0x78);    // 3. (pre-charge timing control), 0x7A def.

    sendCmd(0xEA);  // DRIVER TIMING CONTROL A
    sendParameter(0x00);    // 1. (gate driver timing control), 0x66 def.
    sendParameter(0x00);    // 2. (default)

    sendCmd(0xED);  // POWER ON SEQUENCE CONTROL
    sendParameter(0x64);    // 1. (soft start control), 0x55 def.
    sendParameter(0x03);    // 2. (power on sequence control), 0x01 def.
    sendParameter(0x12);    // 3. (power on sequence control), 0x23 def.
    sendParameter(0x81);    // 4. (DDVDH enhance mode(only for 8 external capacitors)), enabled, 0x01 def.

    sendCmd(0xF7);  // PUMP RATION CONTROL
    sendParameter(0x20);    // 1. (ratio control)

    sendCmd(0xC0);  // POWER CONTROL 1
    sendParameter(0x23);    // 1. (set the GVDD level, 4.6V), 0x21 def.

    sendCmd(0xC1);  // POWER CONTROL 2
    sendParameter(0x10);    // 1. (sets the factor used in the step-up circuits) default

    sendCmd(0xC5);  // VCOM CONTROL 1
    sendParameter(0x3e);    // 1. (VCOMH voltage = 5.85V), 0x31 def.
    sendParameter(0x28);    // 2. (VCOML voltage = -1.50V), 0x3C def.

    sendCmd(0xC7);  // VCOM CONTROL 2
    sendParameter(0x86);    // 1. (VCOM offset voltage), 0xC0 def.

    sendCmd(0x36);  // MEMORY ACCESS CONTROL / Orientation
    sendParameter(0xE8);    // 1. ( ), 0x00 def.

    sendCmd(0x3A);  // COLMOD: PIXEL FORMAT SET
    sendParameter(0x55);    // 1. (sets the pixel format for the RGB image data used by the interface, 16bits/pixel)

    sendCmd(0xB1);  // FRAME RATE CONTROL (IN NORMAL MODE / FULL COLORS)
    sendParameter(0x00);    // 1. (division ratio for internal clocks when Normal mode), default
    sendParameter(0x18);    // 2. (frame frequency, 79Hz), 0x1B def. (70Hz)

    sendCmd(0xB6);  // DISPLAY FUNCTION CONTROL
    sendParameter(0x08);    // 1. (), 0x0A def.
    sendParameter(0x82);    // 2. (default)
    sendParameter(0x27);    // 3. (default)

    sendCmd(0xF2);  // 3GAMMA CONTROL
    sendParameter(0x00);    // 1. (disabled), 0x02 def.

    sendCmd(0x26);  // GAMMA SET
    sendParameter(0x01);    // 1. (default)

    sendCmd(0xE0);  // POSITIVE GAMMA CORRECTION
    sendParameter(0x0F);    // 1.
    sendParameter(0x31);    // 2.
    sendParameter(0x2B);    // 3.
    sendParameter(0x0C);    // 4.
    sendParameter(0x0E);    // 5.
    sendParameter(0x08);    // 6.
    sendParameter(0x4E);    // 7.
    sendParameter(0xF1);    // 8.
    sendParameter(0x37);    // 9.
    sendParameter(0x07);    // 10.
    sendParameter(0x10);    // 11.
    sendParameter(0x03);    // 12.
    sendParameter(0x0E);    // 13.
    sendParameter(0x09);    // 14.
    sendParameter(0x00);    // 15.

    sendCmd(0xE1);  // NEGATIVE GAMMA CORRECTION
    sendParameter(0x00);    // 1.
    sendParameter(0x0E);    // 2.
    sendParameter(0x14);    // 3.
    sendParameter(0x03);    // 4.
    sendParameter(0x11);    // 5.
    sendParameter(0x07);    // 6.
    sendParameter(0x31);    // 7.
    sendParameter(0xC1);    // 8.
    sendParameter(0x48);    // 9.
    sendParameter(0x08);    // 10.
    sendParameter(0x0F);    // 11.
    sendParameter(0x0C);    // 12.
    sendParameter(0x31);    // 13.
    sendParameter(0x36);    // 14.
    sendParameter(0x0F);    // 15.

    sendCmd(0x11);  // SLEEP OUT
    delay_ms(15);   // min = 5ms

    sendCmd(0x29);  // DISPLAY ON
}

void TFT_init()
{
    SPI_init();
    TFT_config();

    set_gram(1);
    set_foreColor(BLACK);
    set_backColor(WHITE);
    set_font(&Font8x16);

    fill_screen();
}

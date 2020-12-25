/**
  ******************************************************************************
  * @file    gui.c
  * @author  杨向南 yxnan@pm.me
  * @date    2020-11-15
  * @brief   stm32f103 simple GUI interface
  ******************************************************************************
  */

#include "gui.h"
#include "ili9341.h"
#include <stdio.h>
#include <string.h>

extern __IO uint16_t AudioBuffer[];
extern uint16_t Chunks;

static uint8_t label_artist [25];
static uint8_t label_title  [25];
static uint8_t label_period [16];

static uint16_t sec_elap;
__IO   uint16_t sec_total;

static uint16_t raw_buffer[FFT_SIZE];
static int16_t  fft_buffer[FFT_SIZE * 2];
static uint16_t mag_buffer[FFT_SIZE];

static draw_button(uint8_t text[], uint16_t x, uint16_t y);


void GUI_Init(void)
{
    uint8_t btn_strbuf[8];

    // 描绘图像边界
    set_foreColor(TEXT_COLOR);

    draw_line(GRAPH_START_X,
              GRAPH_START_Y,
              GRAPH_END_X,
              GRAPH_START_Y);

    draw_line(GRAPH_START_X,
              GRAPH_END_Y,
              GRAPH_END_X,
              GRAPH_END_Y);

    // 描绘标签
    sprintf(label_artist, "Artist: ak+q");
    sprintf(label_title,  "Title : ikasu");
    sprintf(label_period, "00:00 | 00:00");
    disp_string(LABEL_ARTIST_X, LABEL_ARTIST_Y, label_artist);
    disp_string(LABEL_TITLE_X,  LABEL_TITLE_Y,  label_title);
    disp_string(LABEL_PERIOD_X, LABEL_PERIOD_Y, label_period);

    /* 描绘按钮 */

    set_foreColor(GRAY1);
    draw_button("Prev",
                BUTTON_START_X, BUTTON_START_Y);
    draw_button("P/R",
                BUTTON_START_X + (BUTTON_WIDTH+BUTTON_GAP), BUTTON_START_Y);
    draw_button("Next",
                BUTTON_START_X + (BUTTON_WIDTH+BUTTON_GAP)*2, BUTTON_START_Y);
    // draw_button("EQ", BUTTON_EQ_X, BUTTON_EQ_Y);
}

static draw_button(uint8_t text[], uint16_t x, uint16_t y)
{
    uint16_t w = BUTTON_WIDTH;
    uint16_t h = BUTTON_HEIGHT;
    // 按钮内部
    set_foreColor(SILVER);
    draw_rect(x, y, w, h, 1);

    // 按钮边框
    set_foreColor(BLUE);
    draw_rect(x, y, w, h, 0);

    // 按钮文字
    set_foreColor(TEXT_COLOR);
    set_backColor(SILVER);
    disp_string(x + (w - strlen(text)*FONT_W)/2,
                y + (h - FONT_H)/2,
                text);

}

void update_msg(void)
{
    sec_elap = sec_total - Chunks*0.032;
    sprintf(label_period, "%02d:%02d | %02d:%02d",
        sec_elap/60, sec_elap%60,
        sec_total/60, sec_total%60);

    clear_region(LABEL_PERIOD_X, LABEL_PERIOD_Y,
        FONT_W*12, FONT_H);

    set_foreColor(TEXT_COLOR);
    set_backColor(WHITE);
    disp_string(LABEL_PERIOD_X, LABEL_PERIOD_Y, label_period);
}

void plot_graph()
{
    clear_region(GRAPH_START_X+17, GRAPH_START_Y+1,
        GRAPH_WIDTH-33, GRAPH_HEIGHT-2);

    // arm_rfft_q15(&arm_rfft_sR_q15_len256, raw_buffer, fft_buffer);
    // arm_cmplx_mag_q15(fft_buffer, mag_buffer, FFT_SIZE);

    // int16_t real, imag;
    // int16_t *pSrc = fft_buffer;
    // uint16_t *pDst = mag_buffer;
    // int32_t acc0, acc1;
    // uint16_t numSamples = FFT_SIZE/2;

    // while (numSamples > 0)
    // {
    //     /* out = sqrt(real * real + imag * imag) */
    //     real = *pSrc++;
    //     imag = *pSrc++;

    //     acc0 = (real * real);
    //     acc1 = (imag * imag);

    //     // arm_sqrt_q15((int16_t) (((int64_t) acc0 + acc1) >> 17), pDst++);
    //     *pDst = sqrt((float)(acc0 + acc1));
    //     pDst++;

    //     numSamples--;
    // }

    uint16_t *pt = AudioBuffer;
    uint16_t pt_x, pt_y;
    // uint16_t prev_x, prev_y;

    // prev_x = pt_x = GRAPH_START_X+17;
    // prev_y = pt_y = GRAPH_END_Y - ((*pt/4096.0) * GRAPH_HEIGHT) - 1 + (GRAPH_HEIGHT/2);
    // set_foreColor(GRAPH_COLOR);
    // for (uint16_t i = 0; i < 64; i++)
    // {
    //     pt += 4;
    //     pt_x += 4;
    //     pt_y = GRAPH_END_Y - ((*pt/4096.0) * GRAPH_HEIGHT) - 1 + (GRAPH_HEIGHT/2);

    //     if (pt_y >= GRAPH_END_Y)
    //         pt_y = GRAPH_END_Y*2 - pt_y;

    //     draw_line(prev_x, prev_y,
    //         pt_x, pt_y);

    //     prev_x = pt_x;
    //     prev_y = pt_y;
    // }

    pt_x = GRAPH_START_X+17;
    set_foreColor(GRAPH_COLOR);
    for (uint16_t i = 0; i < 64; i++)
    {
        pt_y = GRAPH_END_Y - ((*pt/4096.0) * GRAPH_HEIGHT) - 1 + (GRAPH_HEIGHT/2);
        if (pt_y >= GRAPH_END_Y)
            pt_y = GRAPH_END_Y*2 - pt_y;

        draw_rect(pt_x, pt_y, 4, GRAPH_END_Y-pt_y, 1);

        pt += 4;
        pt_x += 4;
    }
}
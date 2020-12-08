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

static uint8_t label_artist [25];
static uint8_t label_title  [25];
static uint8_t label_period [16];

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
    sprintf(label_period, "01:21 | 03:09");
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

static void update_msg(void)
{
    // LCD_SetBackColor(CL_WHITE);
    // ILI9341_Clear(LABEL1_START_X, LABEL1_START_Y, LCD_X_LENGTH, 4*HEIGHT_CH_CHAR);

    // // 文字信息
    // LCD_SetFont(&Font8x16);
    // LCD_SetTextColor(CL_RED);

    // ILI9341_DispString_EN(LABEL1_START_X, LABEL1_START_Y, labelMode);
    // ILI9341_DispString_EN(LABEL2_START_X, LABEL2_START_Y, labelTHD);
    // ILI9341_DispString_EN(LABEL3_START_X, LABEL3_START_Y, labelVpp);
    // ILI9341_DispString_EN(LABEL4_START_X, LABEL4_START_Y, labelFreq);
}

#ifdef DEBUG
static void serial_send()
{

}
#endif

static void clear_graph(void)
{

}

static void plot_graph(uint16_t buffer[])
{
    // uint16_t *pt = buffer;
    // uint16_t pt_x, pt_y;
    // uint16_t prev_x, prev_y;
    // uint16_t linecolor;
    // uint16_t x_scalar;
    // float y_scalar;

    // if (graphType == TIME_DOMAIN)
    // {
    //     x_scalar = 1;
    //     y_scalar = (float)ADC_RANGE;
    //     linecolor = CH1_COLOR;
    // }
    // else
    // {
    //     x_scalar = FFT_SIZE/1024;
    //     y_scalar = 500.0f;  // 经验值
    //     linecolor = CH2_COLOR;
    // }

    // LCD_SetTextColor(linecolor);
    // prev_x = pt_x = GRAPH_START_X;
    // prev_y = LCD_Y_LENGTH - ((*pt/y_scalar)*GRAPH_HEIGHT + GRAPH_BOTTOM + 1);
    // if (prev_y <= GRAPH_START_Y)
    //     prev_y = GRAPH_START_Y + 1;

    // for (uint16_t i = 1; pt_x<=GRAPH_END_X && i<ADC_BUFFER_SIZE; i++)
    // {
    //     // 保证画FFT图像时不会跳过峰值点
    //     if (graphType == FREQ_DOMAIN)
    //     {
    //         uint16_t j;
    //         for (j = 1; j < x_scalar; j++)
    //             if (*(pt+j) > MAG_LOWER_THRESH) break;

    //         pt += j;
    //     }
    //     else
    //     {
    //         pt += x_scalar;
    //     }

    //     pt_x++;
    //     pt_y = LCD_Y_LENGTH - ((*pt/y_scalar)*GRAPH_HEIGHT + GRAPH_BOTTOM + 1);
    //     if (pt_y <= GRAPH_START_Y)
    //         pt_y = GRAPH_START_Y + 1;

    //     // ILI9341_SetPointPixel(pt_x, pt_y);
    //     ILI9341_DrawLine(prev_x, prev_y, pt_x, pt_y);
    //     prev_x = pt_x;
    //     prev_y = pt_y;
    // }
}


static void start_sampling(void)
{
    // ADCx_Mode_Config();
    // ADC_SoftwareStartConvCmd(ADC_x, ENABLE);

    // while (DMA_GetFlagStatus(ADC_DMA_FLAG_TC) != SET);
    // DMA_ClearFlag(ADC_DMA_FLAG_TC);

    // clear_graph();
    // plot_graph(ADC_Buffer, TIME_DOMAIN);

    // // FFT会改变ADC_Buffer，所以要先把时域图画好
    // calculate_params();
    // plot_graph(FFT_MagBuffer, FREQ_DOMAIN);
    // update_msg();

}

static void calculate_params(void)
{
    // // Vpp
    // float vpp = vpp_q15(ADC_Buffer);
    // sprintf(labelVpp, "Vpp = %.3fV", vpp);

    // // THD
    // float THD = 0.00f;
    // for (uint16_t i = 0; i < FFT_REPEAT; i++)
    // {
    //     fft_q15(ADC_Buffer + i*FFT_SIZE, FFT_Buffer);
    //     mag_q15(FFT_Buffer, FFT_MagBuffer);
    //     THD += THD_to_nth(FFT_MagBuffer, 5) / FFT_REPEAT;
    // }

    // sprintf(labelTHD, "THD = %.2f%%", THD*100);

    // // freq
    // float freq = freq_u16(FFT_MagBuffer);
    // sprintf(labelFreq, "Freq = %.3fHz", freq);
}

static void switch_mode()
{
    // set_switch_mode(btn->param);

    // switch (btn->param)
    // {
    //     case F1_NORMAL:
    //         sprintf(labelMode, "F1: Normal");
    //         break;
    //     case F2_TOP_DISTORTION:
    //         sprintf(labelMode, "F2: Top Distortion");
    //         break;
    //     case F3_BOTTOM_DISTORTION:
    //         sprintf(labelMode, "F3: Bottom Distortion");
    //         break;
    //     case F4_TWOWAY_DISTORTION:
    //         sprintf(labelMode, "F4: Two-way Distortion");
    //         break;
    //     case F5_CROSSOVER_DISTORTION:
    //         sprintf(labelMode, "F5: Crossover Distortion");
    //         break;
    //     default: break;
    // }

    // if (!AutoMode)
    //     start_sampling();
}

/**
  ******************************************************************************
  * @file    gui.h
  * @author  杨向南 yxnan@pm.me
  * @date    2020-11-15
  * @brief   stm32f103 simple GUI interface header
  ******************************************************************************
  */

#ifndef __GUI_H
#define __GUI_H

#include "stm32f10x.h"
#include "ili9341.h"

#define LABEL_BUFFER_SIZE       25

#define GRAPH_START_X           15
#define GRAPH_START_Y           10
#define GRAPH_END_X             (x_Max - GRAPH_START_X)
#define GRAPH_END_Y             120
#define GRAPH_WIDTH             (GRAPH_END_X - GRAPH_START_X)
#define GRAPH_HEIGHT            (GRAPH_END_Y - GRAPH_START_Y)

#define LABEL_ARTIST_X          (x_Max/2 - 50)
#define LABEL_ARTIST_Y          (GRAPH_END_Y + 5)
#define LABEL_TITLE_X           LABEL_ARTIST_X
#define LABEL_TITLE_Y           (LABEL_ARTIST_Y + FONT_H + 4)
#define LABEL_PERIOD_X          LABEL_ARTIST_X
#define LABEL_PERIOD_Y          (LABEL_TITLE_Y + FONT_H + 4) // xx:xx | xx:xx

#define BUTTON_WIDTH            60
#define BUTTON_HEIGHT           35
#define BUTTON_ROW_NUM          3

#define BUTTON_GAP              ((x_Max - BUTTON_WIDTH*BUTTON_ROW_NUM) / (BUTTON_ROW_NUM+1))
#define BUTTON_START_X          BUTTON_GAP
#define BUTTON_START_Y          (LABEL_PERIOD_Y + FONT_H + 8)
#define BUTTON_EQ_X             240
#define BUTTON_EQ_Y             (LABEL_ARTIST_Y + 10)

#define TEXT_COLOR              BLACK
#define GRAPH_COLOR             PURPLE



void GUI_Init(void);

#endif /* __GUI_H */


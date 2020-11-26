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
#define GRAPH_START_Y           15
#define GRAPH_END_X             (x_Max - GRAPH_START_X)
#define GRAPH_END_Y             120
#define GRAPH_WIDTH             (GRAPH_END_X - GRAPH_START_X)
#define GRAPH_HEIGHT            (GRAPH_END_Y - GRAPH_START_Y)

#define LABEL1_START_X+
#define LABEL1_START_Y


#define TEXT_COLOR              BLACK
#define GRAPH_COLOR             PURPLE


typedef enum
{
  TIME_DOMAIN,
  FREQ_DOMAIN
} GraphTypeEnm;

void GUI_Init(void);

#endif /* __GUI_H */


#ifndef __DAC_H
#define __DAC_H

#include "stm32f10x.h"

// DAC_DHR12R1寄存器，12位、右对齐、通道1
#define DAC_DHR12R1_ADDR      (DAC_BASE+0x08)
#define DAC_Output_Port       GPIOA
#define DAC_Output_Pin        GPIO_Pin_4
#define DAC_Output_Channel    DAC_Channel_1
#define DAC_DMA_Channel       DMA2_Channel3


void DAC_init(void);


#endif /* __DAC_H */


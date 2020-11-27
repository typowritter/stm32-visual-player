#ifndef __ADC_H
#define __ADC_H


#include "stm32f10x.h"

#define    ADC_x                         ADC1
#define    ADC_APBxClock_FUN             RCC_APB2PeriphClockCmd
#define    ADC_CLK                       RCC_APB2Periph_ADC1

#define    ADC_GPIO_APBxClock_FUN        RCC_APB2PeriphClockCmd
#define    ADC_GPIO_CLK                  RCC_APB2Periph_GPIOA
#define    ADC_PORT                      GPIOA
#define    ADC_PIN                       GPIO_Pin_2
#define    ADC_CHANNEL                   ADC_Channel_2

#define    ADC_IRQ                       ADC1_2_IRQn
#define    ADC_IRQHandler                ADC1_2_IRQHandler

void ADCx_Init (void);


#endif /* __ADC_H */


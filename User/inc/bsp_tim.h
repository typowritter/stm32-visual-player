#ifndef __BSP_TIMEBASE_H
#define __BSP_TIMEBASE_H


#include "stm32f10x.h"


/********************基本定时器TIM参数定义，只限TIM6、7************/
#define BASIC_TIM6 // 如果使用TIM7，注释掉这个宏即可

/*
    我们把定时器自动重装载寄存器 ARR 的值设为 125，设置时钟预分频器为 71，则
    驱动计数器的时钟：
        CK_CNT = CK_INT / (71+1) = 1M
    计数器计数一次的时间等于：
        1/CK_CNT = 1us
    当计数器计数到 ARR 的值 125 时，产生一次中断，中断一次的时间为：
        1/CK_CNT*ARR = 125us
    正好为采样频率 8000Hz 的倒数，据此利用中断驱动 DAC 输出
*/

#ifdef  BASIC_TIM6 // 使用基本定时器TIM6
#define            BASIC_TIM                   TIM6
#define            BASIC_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            BASIC_TIM_CLK               RCC_APB1Periph_TIM6
#define            BASIC_TIM_Period            125
#define            BASIC_TIM_Prescaler         (72-1)
#define            BASIC_TIM_IRQ               TIM6_IRQn
#define            BASIC_TIM_IRQHandler        TIM6_IRQHandler

#else  // 使用基本定时器TIM7
#define            BASIC_TIM                   TIM7
#define            BASIC_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            BASIC_TIM_CLK               RCC_APB1Periph_TIM7
#define            BASIC_TIM_Period            125
#define            BASIC_TIM_Prescaler         (72-1)
#define            BASIC_TIM_IRQ               TIM7_IRQn
#define            BASIC_TIM_IRQHandler        TIM7_IRQHandler

#endif
/**************************函数声明********************************/

void BASIC_TIM_Init(void);


#endif  /* __BSP_TIMEBASE_H */



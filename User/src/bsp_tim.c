
// 基本定时器TIMx, x[6,7]定时初始化函数

#include "bsp_tim.h"

// 中断优先级配置
static void TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    // 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = UPDATE_TIM_IRQ;
    // 设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    // 设置抢占优先级为3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // RELOAD 定时器优先级高于 UPDATE
    // NVIC_InitStructure.NVIC_IRQChannel = RELOAD_TIM_IRQ;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    // NVIC_Init(&NVIC_InitStructure);
}

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 *typedef struct
 *{ TIM_Prescaler            都有
 *  TIM_CounterMode              TIMx,x[6,7]没有，其他都有
 *  TIM_Period               都有
 *  TIM_ClockDivision        TIMx,x[6,7]没有，其他都有
 *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]才有
 *}TIM_TimeBaseInitTypeDef;
 *-----------------------------------------------------------------------------
 */


static void TIM_Mode_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    // 开启定时器时钟,即内部时钟CK_INT=72M
    TIM_APBxClock(UPDATE_TIM_CLK, ENABLE);

    // 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_Period = UPDATE_TIM_Period;
    // 时钟预分频数
    TIM_TimeBaseStructure.TIM_Prescaler = UPDATE_TIM_Prescaler;
    // 初始化定时器
    TIM_TimeBaseInit(UPDATE_TIM, &TIM_TimeBaseStructure);

    // TIM_TimeBaseStructure.TIM_Period = RELOAD_TIM_Period;
    // TIM_TimeBaseStructure.TIM_Prescaler = RELOAD_TIM_Prescaler;
    // TIM_TimeBaseInit(RELOAD_TIM, &TIM_TimeBaseStructure);

    // 清除计数器中断标志位
    TIM_ClearFlag(UPDATE_TIM, TIM_FLAG_Update);
    // TIM_ClearFlag(RELOAD_TIM, TIM_FLAG_Update);

    // 开启计数器中断
    TIM_ITConfig(UPDATE_TIM, TIM_IT_Update, ENABLE);
    // TIM_ITConfig(RELOAD_TIM, TIM_IT_Update, ENABLE);

    // 使能计数器
    TIM_Cmd(UPDATE_TIM, ENABLE);
    // TIM_Cmd(RELOAD_TIM, ENABLE);

    // 暂时关闭定时器的时钟,等待使用
    TIM_APBxClock(UPDATE_TIM_CLK, DISABLE);
}

void TIM_Init(void)
{
    TIM_NVIC_Config();
    TIM_Mode_Config();
}
/*********************************************END OF FILE**********************/

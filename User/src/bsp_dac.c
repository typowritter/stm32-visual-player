#include "bsp_dac.h"

#define POINTS_NUM  32

// const uint16_t square_12bit[POINTS_NUM] = {
//     4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000,
//     4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000,
//     4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000,
//     4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000,
//     4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000,
//     4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000,
//     4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000,
//     4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000,
//     4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000,
//     4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000,
//     4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000,
//     4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000,
//     4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000,
//     4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000,
//     4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000,
//     4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000,
//     100, 100, 100, 100, 100, 100, 100, 100,
//     100, 100, 100, 100, 100, 100, 100, 100,
//     100, 100, 100, 100, 100, 100, 100, 100,
//     100, 100, 100, 100, 100, 100, 100, 100,
//     100, 100, 100, 100, 100, 100, 100, 100,
//     100, 100, 100, 100, 100, 100, 100, 100,
//     100, 100, 100, 100, 100, 100, 100, 100,
//     100, 100, 100, 100, 100, 100, 100, 100,
//     100, 100, 100, 100, 100, 100, 100, 100,
//     100, 100, 100, 100, 100, 100, 100, 100,
//     100, 100, 100, 100, 100, 100, 100, 100,
//     100, 100, 100, 100, 100, 100, 100, 100,
//     100, 100, 100, 100, 100, 100, 100, 100,
//     100, 100, 100, 100, 100, 100, 100, 100,
//     100, 100, 100, 100, 100, 100, 100, 100,
//     100, 100, 100, 100, 100, 100, 100, 100
// };

const uint16_t square_12bit[POINTS_NUM] = {
    2048    , 2460  , 2856  , 3218  , 3532  , 3786  , 3969  , 4072  ,
    4093    , 4031  , 3887  , 3668  , 3382  , 3042  , 2661  , 2255  ,
    1841    , 1435  , 1054  , 714   , 428   , 209   , 65    , 3     ,
    24      , 127   , 310   , 564   , 878   , 1240  , 1636  , 2048
};

static void DAC_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    /* 防止寄生干扰，配置为模拟输入 */
    GPIO_InitStructure.GPIO_Pin  = DAC_Output_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(DAC_Output_Port, &GPIO_InitStructure);

    /* DAC 通道1 */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable; // 启用DAC输出缓冲，直接驱动负载
    DAC_Init(DAC_Output_Channel, &DAC_InitStructure);

    /* 使能通道1 由PA4输出 */
    DAC_Cmd(DAC_Output_Channel, ENABLE);

    /* 使能DAC的DMA请求 */
    DAC_DMACmd(DAC_Output_Channel, ENABLE);
}

static void DAC_TIM_Config(void)
{

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    /* 使能TIM2时钟，TIM2CLK 为72M */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* TIM2基本定时器配置 */
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = (225-1);
    TIM_TimeBaseStructure.TIM_Prescaler = (10-1);
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    /* 配置TIM2触发源 */
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);

    /* 使能TIM2 */
    TIM_Cmd(TIM2, ENABLE);
}

static void DAC_DMA_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

    /* 配置DMA2 */
    DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_ADDR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&square_12bit;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = POINTS_NUM;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DAC_DMA_Channel, &DMA_InitStructure);

    /* 使能DMA2-3通道 */
    DMA_Cmd(DAC_DMA_Channel, ENABLE);
}

void DAC_init(void)
{
    DAC_Config();
    DAC_TIM_Config();

    DAC_DMA_Config();
}
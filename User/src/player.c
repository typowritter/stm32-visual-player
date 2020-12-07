#include "player.h"
#include "bsp_led.h"

#define POINTS_NUM  256

__IO uint8_t AudioBuffer1[POINTS_NUM];
__IO uint8_t AudioBuffer2[POINTS_NUM];

FATFS fs;           /* FatFs文件系统对象 */
FIL file_mu;        /* 文件对象 */
FRESULT res_sd;     /* 文件操作结果 */
UINT fnum;          /* 成功读写BYTE数量 */
// 文件头
uint8_t  Artist [16];
uint8_t  Title  [16];
uint16_t Chunks;

uint8_t dmaindex = 0;
uint8_t wavecounter = 0;

void player_init(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    NVIC_InitTypeDef    NVIC_InitStructure;
    DAC_InitTypeDef     DAC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6 | RCC_APB1Periph_DAC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

    /* GPIO: PA4 = DAC_Channel_1 */
    // 防止寄生干扰，配置为模拟输入
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 中断配置 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    // DMA2_Channel3_IRQn
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // TIM6_IRQn
    // NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&NVIC_InitStructure);

    /* DAC 通道1 */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    // 使能通道1，连接到PA4
    DAC_Cmd(DAC_Channel_1, ENABLE);

    /* File system */
    f_mount(&fs, "0:", 1);
}

static void media_parse(void)
{
    f_open(&file_mu, "0:ikasu.mu", FA_OPEN_EXISTING | FA_READ);
    f_read(&file_mu, Artist,  sizeof(Artist), &fnum);
    f_read(&file_mu, Title,   sizeof(Title),  &fnum);
    f_read(&file_mu, &Chunks, sizeof(uint16_t), &fnum);
}

void mediainfo(uint8_t* pStr)
{
    sprintf(pStr, "%s, %s, %d", Artist, Title, Chunks);
}

void player_start(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    DMA_InitTypeDef DMA_InitStructure;

    media_parse();
    f_read(&file_mu, AudioBuffer1, POINTS_NUM, &fnum);

    // TIM_DeInit(TIM6);

    /* DMA2 通道3 */
    // DMA_DeInit(DMA2_Channel3);
    // DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_ADDR;
    // DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&AudioBuffer1;
    // DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    // DMA_InitStructure.DMA_BufferSize = POINTS_NUM;
    // DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    // DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    // DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    // DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    // DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    // DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    // DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    // DMA_Init(DMA2_Channel3, &DMA_InitStructure);
    // DMA_Cmd(DMA2_Channel3, ENABLE);

    // // 启用通道1的DMA
    // DAC_DMACmd(DAC_Channel_1, ENABLE);

    /* TIM6 */
    // TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    // TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
    // TIM_SetAutoreload(TIM6, 3000);  // 8kHz = 24MHz / 3000

    // TIM_Cmd(TIM6, ENABLE);
}

void player_pause(void)
{
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    TIM_ITConfig(TIM6, TIM_IT_Update, DISABLE);
}

void player_stop(void)
{
    TIM_ITConfig(TIM6, TIM_IT_Update, DISABLE);
    TIM_Cmd(TIM6, DISABLE);
}

void player_resume(void)
{
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
}

void DMA2_Channel3_IRQHandler(void)
{
    DMA2_Channel3->CCR   = 0x0;
    DMA2_Channel3->CNDTR = POINTS_NUM;
    DMA2_Channel3->CPAR  = DAC_DHR8R1_ADDR;

    if (dmaindex == 0)
    {
        DMA2_Channel3->CMAR = (uint32_t)&AudioBuffer2;
        DMA2_Channel3->CCR = 0x2093;
        f_read(&file_mu, AudioBuffer1, POINTS_NUM, &fnum);
        dmaindex++;
    }
    else
    {
        DMA2_Channel3->CMAR = (uint32_t)&AudioBuffer1;
        DMA2_Channel3->CCR = 0x2093;
        f_read(&file_mu, AudioBuffer2, POINTS_NUM, &fnum);
        dmaindex--;
    }
    DMA1->IFCR = DMA2_IT_TC3;
}

void Trigger(void)
{
    DAC_SetChannel1Data(DAC_Align_8b_R, AudioBuffer1[wavecounter++]);
    DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);
    if (wavecounter == 255)
    {
        wavecounter = 0;
        f_read(&file_mu, AudioBuffer1, POINTS_NUM, &fnum);
        Chunks--;
    }

    if (Chunks == 0)
    {
        player_stop();
    }
}

void TIM6_IRQHandler(void)
{
    if (RESET != TIM_GetITStatus(TIM6, TIM_IT_Update))
    {
        LED_GREEN();
        DAC_SetChannel1Data(DAC_Align_8b_R, AudioBuffer1[wavecounter++]);
        if (wavecounter == 255)
        {
            wavecounter = 0;
            f_read(&file_mu, AudioBuffer1, POINTS_NUM, &fnum);
            Chunks--;
        }

        if (Chunks == 0)
        {
            player_stop();
        }
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
}
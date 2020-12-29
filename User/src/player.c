/**
  ******************************************************************************
  * @file    player.c
  * @author  丁天立
  * @date    2020-11-25
  * @brief   音乐播放逻辑
  ******************************************************************************
  */

#include "player.h"
#include "bsp_sdio_sdcard.h"
#include "ff.h"
#include "gui.h"

typedef uint16_t    Sample_t;
#define POINTS_NUM  1024
#define READ_SIZE   (sizeof(Sample_t)*POINTS_NUM)

__IO Sample_t AudioBuffer[POINTS_NUM];

static FATFS fs;           /* FatFs文件系统对象 */
static FIL file_mu;        /* 文件对象 */
// static FRESULT res_sd;     /* 文件操作结果 */
static UINT fnum;          /* 成功读写BYTE数量 */

// 文件头
uint8_t  Artist [16];   // 歌手名
uint8_t  Title  [16];   // 歌曲名
uint16_t Chunks;        // 歌曲块数（每个块256个采样点）

// DMA半传输、传输完成后分别设置这两个标志，主函数里轮询检测并更新 AudioBuffer
uint8_t flag_UpdateHalf_1 = 0;
uint8_t flag_UpdateHalf_2 = 0;

static PlayerStatu_t PlayerStatu = PAUSED;  // 播放状态

// 音频文件列表
#define SONG_COUNT 2
uint8_t file_idx = 0;
uint8_t fileList[SONG_COUNT][16] =
{
    "0:ikasu.mu",
    "0:record.mu",
};

/**
  * @brief  初始化播放器
  * @param  无
  * @retval 无
  */
void player_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* GPIO: PA4 = DAC_Channel_1 */
    // 防止寄生干扰，配置为模拟输入
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* DMA 中断配置 */
    // 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel3_IRQn;
    // 设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    // 设置抢占优先级为3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* File system */
    f_mount(&fs, "0:", 1);
}


/**
  * @brief  配置 DAC 的 DMA 请求
  * @param  无
  * @retval 无
  */
static void DAC_DMA_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    /* 使能DMA2时钟 */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

    // 清除中断标志位，避免干扰
    DMA_ClearFlag(DMA2_FLAG_TC3 | DMA2_FLAG_HT3 | DMA2_FLAG_GL3 | DMA2_FLAG_TE3);
    DMA_Cmd(DMA2_Channel3, DISABLE);

    /* 配置DMA2 */
    DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_ADDR; // -------------- 外设数据地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&AudioBuffer; // ------------ 内存数据地址 AudioBuffer
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; // ------------------------ 数据传输方向内存至外设
    DMA_InitStructure.DMA_BufferSize = POINTS_NUM; // ---------------------------- 缓存大小为POINTS_NUM字节
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // ---------- 外设数据地址固定
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // ------------------- 内存数据地址自增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 外设数据以字为单位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // ------- 内存数据以字为单位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; // ----------------------------- 单次传输模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; // ------------------- 高DMA通道优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // ------------------------------ 非内存至内存模式
    DMA_Init(DMA2_Channel3, &DMA_InitStructure);

    // 启用DMA半传输中断（HT）和传输完成中断（TC）
    DMA_ITConfig(DMA2_Channel3,
        DMA_IT_HT | DMA_IT_TC, ENABLE);

    /* 使能DMA通道 */
    DMA_Cmd(DMA2_Channel3, ENABLE);
}


/**
  * @brief  配置 DAC 的 触发定时器
  * @param  无
  * @retval 无
  */
static void DAC_TIM_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    /* 使能TIM6时钟，TIM6CLK 为72M */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    TIM_DeInit(TIM6);

    /* TIM6基本定时器配置 */
    // TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = 125;           // 定时周期 125
    TIM_TimeBaseStructure.TIM_Prescaler = (72-1);     // 预分频，72M / (72) = 1M
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    /* 配置TIM6触发源 */
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
}


/**
  * @brief  配置 DAC 工作模式
  * @param  无
  * @retval 无
  */
static void DAC_Config(void)
{
    DAC_InitTypeDef DAC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    DAC_DeInit();

    /* DAC 通道1 */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;            // 触发源：TIM6
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None; // 关闭自动波形生成
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;   // 启用输出缓冲
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    /* 打开DAC电源 */
    DAC_Cmd(DAC_Channel_1, ENABLE);

    /* 使能DAC的DMA请求 */
    DAC_DMACmd(DAC_Channel_1, ENABLE);
}


/**
  * @brief  读入音频文件头
  * @param  无
  * @retval 无
  */
static void media_parse(void)
{
    // 关闭之前打开的文件
    f_close(&file_mu);

    /* 打开文件并读取文件头 */
    f_open(&file_mu, fileList[file_idx], FA_OPEN_EXISTING | FA_READ);
    f_read(&file_mu, Artist,  sizeof(Artist), &fnum);
    f_read(&file_mu, Title,   sizeof(Title),  &fnum);
    f_read(&file_mu, &Chunks, sizeof(uint16_t), &fnum);
    f_read(&file_mu, AudioBuffer, READ_SIZE/2, &fnum);

    file_idx = (file_idx + 1) % SONG_COUNT; // 指向下一首歌（列表循环）
}


/**
  * @brief  开始播放
  * @param  无
  * @retval 无
  */
void player_start(void)
{
    media_parse();

    DAC_Config();
    DAC_TIM_Config();
    DAC_DMA_Config();

    /* 使能TIM6 */
    TIM_Cmd(TIM6, ENABLE);

    PlayerStatu = PLAYING;
    update_msg();
}


/**
  * @brief  暂停播放
  * @param  无
  * @retval 无
  */
void player_pause(void)
{
    PlayerStatu = PAUSED;
}


/**
  * @brief  停止播放
  * @param  无
  * @retval 无
  */
void player_stop(void)
{
    PlayerStatu = PAUSED;
    // 关闭DAC
    DAC_Cmd(DAC_Channel_1, DISABLE);
}


/**
  * @brief  继续播放
  * @param  无
  * @retval 无
  */
void player_resume(void)
{
    PlayerStatu = PLAYING;
    DAC_DMA_Config();
}


/**
  * @brief  取得当前播放状态
  * @param  无
  * @retval 无
  */
PlayerStatu_t get_statu(void)
{
    return PlayerStatu;
}


/**
  * @brief  更新AudioBuffer前半部分
  * @param  无
  * @retval 无
  */
void update_half_1(void)
{
    f_read(&file_mu, AudioBuffer, READ_SIZE/2, &fnum);
    Chunks -= 2;
    if (Chunks < 6)  // 剩余块数小于6（160ms以内）则停止播放
        player_stop();
}


/**
  * @brief  更新AudioBuffer后半部分
  * @param  无
  * @retval 无
  */
void update_half_2(void)
{
    f_read(&file_mu, &AudioBuffer[POINTS_NUM/2], READ_SIZE/2, &fnum);
    Chunks -= 2;
}


/**
  * @brief  DMA中断服务函数
  * @param  无
  * @retval 无
  */
void DMA2_Channel3_IRQHandler(void)
{
    // 检查中断原因
    if (DMA_GetITStatus(DMA2_IT_HT3) != RESET)
    {
        flag_UpdateHalf_1 = 1;
    }
    else
    {
        flag_UpdateHalf_2 = 1;
        PlayerStatu == PLAYING?
            DAC_DMA_Config():
            0;
    }
    DMA_ClearITPendingBit(DMA2_IT_GL3); // 清除中断标志位
}
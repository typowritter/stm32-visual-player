#include "player.h"
#include "bsp_sdio_sdcard.h"
#include "ff.h"
#include "tty.h"
#include "bsp_led.h"
#include "bsp_tim.h"

#define POINTS_NUM  256
#define READ_SIZE   (2*POINTS_NUM)

__IO uint16_t AudioBuffer[POINTS_NUM];

FATFS fs;           /* FatFs文件系统对象 */
FIL file_mu;        /* 文件对象 */
FRESULT res_sd;     /* 文件操作结果 */
UINT fnum;          /* 成功读写BYTE数量 */
// 文件头
uint8_t  Artist [16];
uint8_t  Title  [16];
uint16_t Chunks;

uint8_t wavecounter = 0;
uint16_t wave_sec   = 0;
PlayerStatu player_statu = PAUSED;

void player_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* GPIO: PA4 = DAC_Channel_1 */
    // 防止寄生干扰，配置为模拟输入
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    BASIC_TIM_Init();

    /* File system */
    f_mount(&fs, "0:", 1);
}

static void DAC_Config(void)
{
    DAC_InitTypeDef DAC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    /* DAC 通道1 */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
}

static void media_parse(void)
{
    f_open(&file_mu, "0:ikasu16.mu", FA_OPEN_EXISTING | FA_READ);
    f_read(&file_mu, Artist,  sizeof(Artist), &fnum);
    f_read(&file_mu, Title,   sizeof(Title),  &fnum);
    f_read(&file_mu, &Chunks, sizeof(uint16_t), &fnum);
}

void player_start(void)
{
    media_parse();
    f_read(&file_mu, AudioBuffer, READ_SIZE, &fnum);

    wave_sec = Chunks / SAMPLE_RATE * POINTS_NUM;

    DAC_Config();
    DAC_Cmd(DAC_Channel_1, ENABLE);

    BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, ENABLE);
    player_resume();
}

void player_pause(void)
{
    player_statu = PAUSED;
}

void player_stop(void)
{
    player_statu = PAUSED;
    DAC_Cmd(DAC_Channel_1, DISABLE);
    BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, DISABLE);
}

void player_resume(void)
{
    player_statu = PLAYING;
}

PlayerStatu get_statu(void)
{
    return player_statu;
}

void BASIC_TIM_IRQHandler(void)
{
    if (player_statu == PLAYING)
    {
        // 将音频数据送到 DAC
        DAC_SetChannel1Data(DAC_Align_12b_R, AudioBuffer[wavecounter++]);
        DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);  // 触发 DAC 转换
    }

    // 清除中断标志位
    TIM_ClearITPendingBit(BASIC_TIM , TIM_IT_Update);
}

void player_update(void)
{
    if (player_statu == PLAYING)
    {
        // 最后一个采样播放完成时，上溢至 0
        if (wavecounter == 0)
        {
            f_read(&file_mu, AudioBuffer, READ_SIZE, &fnum);
            Chunks--;
        }
        else if (Chunks == 0)
        {
            player_stop();
        }
    }
}
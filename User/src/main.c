#include "stm32f10x.h"
#include "ili9341.h"
#include "delay.h"
#include "gui.h"
#include "bsp_led.h"
#include "bsp_dac.h"
#include "bsp_sdio_sdcard.h"
#include "ff.h"
#ifdef DEBUG
#include "bsp_adc.h"
#include "tty.h"
#endif

extern __IO uint16_t ADC_ConvertedValue;
extern SD_CardInfo SDCardInfo;
extern __IO uint16_t AudioBuffer[];

FATFS fs;           /* FatFs文件系统对象 */
FIL fnew;           /* 文件对象 */
FRESULT res_sd;     /* 文件操作结果 */
UINT fnum;          /* 文件成功读写数量 */
// 文件头
BYTE Artist [16];
BYTE Title  [16];
BYTE Length [16];

int main(void)
{
    delay_init();
    DAC_init();
    LED_GPIO_Config();

    // debug
    tty_init();
    ADCx_Init();

    TFT_init();
    GUI_Init();

    res_sd = f_mount(&fs, "0:", 1);

    if (FR_OK != res_sd)
    {
        // mount failed
        LED_RED();
        while (1)
            ;
    }
    else
    {
        // mount successful
        LED_GREEN();
    }

    res_sd = f_open(&fnew, "0:ikasu.mu", FA_OPEN_EXISTING | FA_READ);
    if (FR_OK == res_sd)
    {
        // open successful
        res_sd  = f_read(&fnew, Artist, sizeof(Artist), &fnum);
        res_sd &= f_read(&fnew, Title,  sizeof(Title),  &fnum);
        res_sd &= f_read(&fnew, Length, sizeof(Length), &fnum);
        if(FR_OK == res_sd)
        {
            LED_GREEN();
        }
        else
        {
        }
    }
    else
    {
        LED_RED();
    }

    while (1)
    {
        res_sd  = f_read(&fnew, AudioBuffer, 512, &fnum);
        delay_ms(160);
        if (FR_OK != res_sd)
        {
            /* 不再读写，关闭文件 */
            f_close(&fnew);

             // 不再使用文件系统，取消挂载文件系统
            f_mount(NULL, "0:", 1);

            while (1);
        }
        // DAC_SetChannel1Data(0, square_12bit[i++]);
        // DAC_SoftwareTriggerCmd(DAC_Output_Channel, ENABLE);
        // tty_print("%d, %s, %s\r\n", fnum, Artist, Title);
        // ADC_SoftwareStartConvCmd(ADC_x, ENABLE);
        // tty_print("here: %d\r\n", ADC_ConvertedValue);
    }
}

/* ------------------------------------------end of file---------------------------------------- */


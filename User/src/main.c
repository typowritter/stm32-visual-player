#include "stm32f10x.h"
#include "ili9341.h"
#include "delay.h"
#include "gui.h"
#include "bsp_led.h"
#include "bsp_sdio_sdcard.h"
#include "ff.h"
#ifdef DEBUG
#include "bsp_adc.h"
#include "tty.h"
#endif

extern __IO uint16_t ADC_ConvertedValue;
extern SD_CardInfo SDCardInfo;

FATFS fs;           /* FatFs文件系统对象 */
FIL fnew;           /* 文件对象 */
FRESULT res_sd;     /* 文件操作结果 */
UINT fnum;                        /* 文件成功读写数量 */
BYTE ReadBuffer[1024] = {0};      /* 读缓冲区 */
BYTE WriteBuffer[] =              /* 写缓冲区 */
"New file content test.\nnewline.";

int main(void)
{
    LED_GPIO_Config();
    delay_init();

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

    res_sd = f_open(&fnew, "0:newfile.txt", FA_OPEN_EXISTING | FA_READ);
    if (FR_OK == res_sd)
    {
        // open successful
        res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
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
    /* 不再读写，关闭文件 */
    f_close(&fnew);

    /* 不再使用文件系统，取消挂载文件系统 */
    f_mount(NULL, "0:", 1);

    while (1)
    {
        tty_print("%d, %s", fnum, ReadBuffer);
        // ADC_SoftwareStartConvCmd(ADC_x, ENABLE);
        // tty_print("here: %d\r\n", ADC_ConvertedValue);
        delay_ms(100);
    }
}

/* ------------------------------------------end of file---------------------------------------- */


#include "stm32f10x.h"
#include "ili9341.h"
#include "delay.h"
#include "gui.h"
#include "bsp_led.h"
#include "player.h"
#ifdef DEBUG
#include "tty.h"
#endif

extern __IO uint16_t ADC_ConvertedValue;

uint32_t cnt = 0;
uint8_t strbuf[20];

int main(void)
{
    delay_init();
    player_init();

    // debug
    tty_init();

    TFT_init();
    GUI_Init();

    player_start();
    while (1)
    {
        Trigger();
        delay_us(125);
        // tty_print("%s\r\n", strbuf);
    }
}

void assert_failed(uint8_t* file, uint32_t line)
{
    while (1);
}

// void DMA2_Channel3_IRQHandler(void)
// {
//     DMA_ClearITPendingBit(DMA2_IT_TC3);
//     DMA_ClearFlag(DMA2_FLAG_TC3);
//     cnt++;
//     sprintf(strbuf, "%d", cnt);
//     disp_string(0, 0, strbuf);
//     res_sd = f_read(&fnew, AudioBuffer, 512, &fnum);
//     DAC_DMACmd(DAC_Output_Channel, ENABLE);
//     if (FR_OK != res_sd)
//     {
//         LED_CYAN();
//         /* 不再读写，关闭文件 */
//         f_close(&fnew);

//         // 不再使用文件系统，取消挂载文件系统
//         f_mount(NULL, "0:", 1);
//         while (1);
//     }
// }

/* ------------------------------------------end of file---------------------------------------- */


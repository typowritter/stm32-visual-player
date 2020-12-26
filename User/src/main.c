#include "stm32f10x.h"
#include "ili9341.h"
#include "delay.h"
#include "gui.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "player.h"
#ifdef DEBUG
#include "tty.h"
#endif

// 更新图像、显示信息的标志
uint8_t graph_updCounter;
uint8_t disp_updCounter;

int main(void)
{
    delay_init();
    Key_GPIO_Config();
    LED_GPIO_Config();
    tty_init();
    player_init();

    TFT_init();
    GUI_Init();

    player_start();
    while (1)
    {
        // 检测到暂停键按下
        if (Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
        {
            get_statu() == PLAYING?
                player_pause():
                player_resume();
        }

        // 前半部分播放完成
        if (flag_UpdateHalf_1)
        {
            update_half_1();
            flag_UpdateHalf_1 = 0;

            // 每完全更新2次缓冲区（128ms x 2 = 256ms）就刷新一次频谱显示
            if (graph_updCounter > 1)
            {
                plot_graph();
                graph_updCounter = 0;
            }

            // 每完全更新6次缓冲区（128ms x 6 = 768ms）就刷新一次进度显示
            if (disp_updCounter > 5)
            {
                update_msg();
                disp_updCounter = 0;
            }

        }
        // 后半部分播放完成
        else if (flag_UpdateHalf_2)
        {
            update_half_2();
            flag_UpdateHalf_2 = 0;
            graph_updCounter++;
            disp_updCounter++;
        }

    }
}

/* ------------------------------------------end of file---------------------------------------- */


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

uint32_t cnt = 0;
uint8_t strbuf[20];

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
        if (Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
        {
            get_statu() == PLAYING?
                player_pause():
                player_resume();
        }
        if (flag_UpdateHalf_1)
        {
            update_half_1();
            flag_UpdateHalf_1 = 0;
        }
        else if (flag_UpdateHalf_2)
        {
            update_half_2();
            flag_UpdateHalf_2 = 0;
        }

        // check_reload();
        // delay_us(125);
        // tty_print("%s\r\n", strbuf);
    }
}

/* ------------------------------------------end of file---------------------------------------- */


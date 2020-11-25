#include "stm32f10x.h"
#include "bsp_led.h"
#include "ili9341.h"
// #include "bsp_tim.h"
#include "delay.h"
#ifdef DEBUG
#include "tty.h"
#endif

int main(void)
{
    LED_GPIO_Config();
    delay_init();
    #ifdef DEBUG
    tty_init();
    #endif
    LED_GREEN();
    TFT_init();

    draw_line(20, 40, 220, 200);
    disp_string(20, 20, "1234567890");
    LED_BLUE();

    while (1)
    {

    }
}


/* ------------------------------------------end of file---------------------------------------- */


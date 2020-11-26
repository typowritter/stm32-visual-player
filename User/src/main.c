#include "stm32f10x.h"
#include "bsp_led.h"
#include "ili9341.h"
// #include "bsp_tim.h"
#include "delay.h"
#include "gui.h"
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
    TFT_init();

    GUI_Init();
    // LED_BLUE();

    while (1)
    {

    }
}


/* ------------------------------------------end of file---------------------------------------- */


#include "stm32f10x.h"
#include "bsp_led.h"
#include "ili9341.h"
// #include "bsp_tim.h"
#include "delay.h"
#include "gui.h"
#ifdef DEBUG
#include "bsp_adc.h"
#include "tty.h"
#endif

extern __IO uint16_t ADC_ConvertedValue;

int main(void)
{
    LED_GPIO_Config();
    delay_init();
    TFT_init();

    // debug
    tty_init();
    ADCx_Init();

    // LED_BLUE();

    GUI_Init();
    while (1)
    {
        ADC_SoftwareStartConvCmd(ADC_x, ENABLE);
        tty_print("here: %d\r\n", ADC_ConvertedValue);
        delay_ms(100);
    }
}

/* ------------------------------------------end of file---------------------------------------- */


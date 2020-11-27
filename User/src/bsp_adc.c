#include "bsp_adc.h"

__IO uint16_t ADC_ConvertedValue;

/**
  * @brief  ADC GPIO 初始化
  * @param  无
  * @retval 无
  */
static void ADCx_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 打开 ADC IO端口时钟
    ADC_GPIO_APBxClock_FUN ( ADC_GPIO_CLK, ENABLE );

    // 配置 ADC IO 引脚模式
    GPIO_InitStructure.GPIO_Pin = ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;

    // 初始化 ADC IO
    GPIO_Init(ADC_PORT, &GPIO_InitStructure);
}

/**
  * @brief  配置ADC工作模式
  * @param  无
  * @retval 无
  */
static void ADCx_Mode_Config(void)
{
    ADC_InitTypeDef ADC_InitStructure;

    ADC_APBxClock_FUN ( ADC_CLK, ENABLE );

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;

    ADC_Init(ADC_x, &ADC_InitStructure);

    // 配置ADC时钟为CLK2的8分频，即9MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL, 1, ADC_SampleTime_71Cycles5);

    // ADC 转换结束产生中断，在中断服务程序中读取转换值
    ADC_ITConfig(ADC_x, ADC_IT_EOC, ENABLE);

    ADC_Cmd(ADC_x, ENABLE);

    // 初始化ADC 校准寄存器
    ADC_ResetCalibration(ADC_x);
    // 等待校准寄存器初始化完成
    while(ADC_GetResetCalibrationStatus(ADC_x));

    // ADC开始校准
    ADC_StartCalibration(ADC_x);
    // 等待校准完成
    while(ADC_GetCalibrationStatus(ADC_x));

    // 由于没有采用外部触发，所以使用软件触发ADC转换
    // ADC_SoftwareStartConvCmd(ADC_x, ENABLE);
}

static void ADC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    // 优先级分组
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    // 配置中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  ADC初始化
  * @param  无
  * @retval 无
  */
void ADCx_Init(void)
{
    ADCx_GPIO_Config();
    ADCx_Mode_Config();
    ADC_NVIC_Config();
}
/*********************************************END OF FILE**********************/

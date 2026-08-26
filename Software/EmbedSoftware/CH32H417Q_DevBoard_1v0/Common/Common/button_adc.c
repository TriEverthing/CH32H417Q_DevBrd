#include "button_adc.h"
#include "hardware.h"

//void ADC1_2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void ADC1_2_IRQHandler(void) __attribute__((interrupt()));

#if MCU_ADC_DMA_ENABLE
uint16_t u16ButtonAdcRaw;
#endif

uint8_t u8ButtoniIsPressed=0;
int8_t u8ButtonAdcVal = -1;

#if BUTTONADC_ENABLE_TIMER
uint16_t u16ButtonPressedTimes=0;
#endif

#if BUTTONADC_ENABLE_TIMER
/*********************************************************************
 * @fn      Button_Timer_Init
 *
 * @brief   Initializes Button Timer.
 *
 * @return  none
 */
void Button_Timer_Init(void)
{
    /* Enables the High Speed HB1 peripheral clock. */
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_TIM6, ENABLE);

    /* Configures the TIMx Prescaler. 120MHz/12000 = 10kHz */
    TIM_PrescalerConfig(TIM6, 12000 - 1, TIM_PSCReloadMode_Update);

    /* Sets the TIMx Autoreload Register value. */
    TIM_SetAutoreload(TIM6, 0xFFFF);

    /* Disable the specified TIM peripheral. */
    TIM_Cmd(TIM6, DISABLE);
}
#endif

/*********************************************************************
 * @fn      Mcu_Adc_Init
 *
 * @brief   Initializes ADC collection.
 *
 * @return  none
 */
void Button_Adc_Init(void)
{
    uint32_t TimesOut;
    ADC_InitTypeDef ADC_InitStructure={0};
    GPIO_InitTypeDef GPIO_InitStructure={0};
#if MCU_ADC_DMA_ENABLE
    DMA_InitTypeDef DMA_InitStructure = {0};
#endif

    /* Enables or disables the HB peripheral clock */
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);
    /* Enables or disables the HB2 peripheral clock */
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC1|RCC_HB2Periph_GPIOA, ENABLE );

    /* Configures the ADC clock source. */
    RCC_ADCCLKConfig(RCC_ADCCLKSource_HCLK);
    /* Configures the ADC clock (ADCCLK), the PLL clock source is USBHSPLL(480MHz). */
    //RCC_ADCUSBHSPLLCLKAsSourceConfig(RCC_USBHS_Div36);
    /* Configures the ADC_clock (ADCCLK), ADC clock = HCLK/PPRE2_DIV/RCC_ADC_DIV. */
    RCC_ADCHCLKCLKAsSourceConfig(RCC_PPRE2_DIV16,RCC_HCLK_ADCPRE_DIV8);
 
    //PA2,ADCIN2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Deinitializes the ADCx peripheral registers to their default reset values. */
    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    /* Initializes the ADCx peripheral according to the specified parameters in the ADC_InitStruct. */
    ADC_Init(ADC1, &ADC_InitStructure);

#if(ADC_SPEED == ADC_SPEED_High)
    ADC_LowPowerModeCmd(ADC1,DISABLE); //For > 2Msa/s
#else
    ADC_LowPowerModeCmd(ADC1,ENABLE);  //For < 2Msa/s
#endif 

    /* Enables the ADCx TO SMP SEL. */
    ADC_SMP_ModeConfig(ADC1,ADC_Channel_2,ADC_SMP_CFG_MODE0);
    /* Enables the specified ADC peripheral. */
    ADC_Cmd(ADC1, ENABLE);
    /* Enables or disables the ADCx buffer. */
    ADC_BufferCmd(ADC1, DISABLE);

    /* Resets the selected ADC calibration registers. */
    ADC_ResetCalibration(ADC1);
    /* Wait Resets the selected ADC calibration registers Complete */
    TimesOut = 0xFFFFFF;
    while(ADC_GetResetCalibrationStatus(ADC1))
    {
        TimesOut--;
        if( TimesOut == 0 )
        {
            return;
        } 
    }
    /* Starts the selected ADC calibration process. */
    ADC_StartCalibration(ADC1);

    /* Wait ADC calibration Complete */
    TimesOut = 0xFFFFFF;
    while(ADC_GetCalibrationStatus(ADC1))
    {
        TimesOut--;
        if( TimesOut == 0 )
        {
            return;
        } 
    }

#if (ADC_SPEED == ADC_SPEED_High)
    RCC_ADCUSBHSPLLCLKAsSourceConfig(RCC_USBHS_Div6);
#endif   

    /* Configures for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_CyclesMode7);//239.5 ADC Clocks,Total=239.5+12.5. About 14.940kSa/s

    /* Configures the high and low thresholds of the analog watchdog. */
    ADC_AnalogWatchdogThresholdsConfig(ADC1, MCU_ADC_HighThreshold, MCU_ADC_LowThreshold);
    /* Configures the analog watchdog guarded single channel. */
    ADC_AnalogWatchdogSingleChannelConfig(ADC1,ADC_Channel_2);
    /* Enables the analog watchdog on single/all regular or injected channels. */
    ADC_AnalogWatchdogCmd(ADC1,ADC_AnalogWatchdog_SingleRegEnable);

    /* ADC Interrupt Config */
    /* Set Interrupt Priority */
    NVIC_SetPriority(ADC1_2_IRQn, 0);
    /* Enable Interrupt */
    NVIC_EnableIRQ(ADC1_2_IRQn);
    /* Enables the specified ADC interrupts. */
    ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE);

#if MCU_ADC_DMA_ENABLE
    /* Deinitializes the DMAy Channelx registers to their default reset values */
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (int32_t)& ADC1->RDATAR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&u16ButtonAdcRaw;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = sizeof(u16ButtonAdcRaw)/sizeof(uint16_t);
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    /*  Initializes the DMAy Channelx according to the specified parameters in the DMA_InitStruct. */
    DMA_Init(DMA1_Channel1, &DMA_InitStructure); 

    /* Configures the DMA input channel , DMA_MuxChannelx equal 0x78 for ADC1. */
    DMA_MuxChannelConfig(DMA_MuxChannel1, 0x78); 

    /* Enables or disables the specified ADC DMA request. */
    ADC_DMACmd(ADC1, ENABLE);

    /* Enables or disables the specified DMAy Channelx. */
    DMA_Cmd(DMA1_Channel1, ENABLE);  
#endif
    /* Enables or disables the selected ADC software start conversion. */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

#if BUTTONADC_ENABLE_TIMER    
    /* Initializes Button Timer. */
    Button_Timer_Init();
#endif
}

/*********************************************************************
 * @fn      Mcu_Adc_GetVolatge_mV
 *
 * @brief   Get MCU's ADC voltage convert result, unit is mV
 *
 * @return  none
 */
uint16_t Button_Adc_GetVolatge_mV(void)
{
#if MCU_ADC_DMA_ENABLE
    return (uint16_t)(u16ButtonAdcRaw*MCU_ADC_VREF>>MCU_ADC_BITS);
#else
    return (uint16_t)(ADC1->RDATAR*MCU_ADC_VREF>>MCU_ADC_BITS);
#endif
}

/*********************************************************************
 * @fn      ADC_AnalogWatchdog_Isr
 *
 * @brief   This function handles ADC AnalogWatchDog exception.
 *
 * @return  none
 */
void ADC_AnalogWatchDog_Isr(void)
{
    static uint8_t u8ButtonAdcCount=0;
    u8ButtonAdcCount++;
    //printf("Button Count: %d.\r\n",u8ButtonAdcCount); 
    if( u8ButtonAdcCount == 16 )
    {
        if( u8ButtoniIsPressed == 0 )
        {
            ADC1->WDHTR = MCU_ADC_HighThreshold-205;
            ADC1->WDLTR = 0;
#if BUTTONADC_ENABLE_TIMER
            TIM6->CNT = 0;
            TIM6->CTLR1 |= TIM_CEN;
#endif
            u8ButtonAdcVal =  AdcRaw2ButtonNb(ADC1->RDATAR);
            printf("Button ADC:Button %d pressed.\r\n",u8ButtonAdcVal); 
            u8ButtoniIsPressed = 1;
        }
        else
        {
#if BUTTONADC_ENABLE_TIMER
            u16ButtonPressedTimes = TIM6->CNT;
            TIM6->CTLR1 &= (uint16_t)(~((uint16_t)TIM_CEN));
#endif
            ADC1->WDHTR = MCU_ADC_HighThreshold;
            ADC1->WDLTR = MCU_ADC_LowThreshold;
            printf("Button ADC:Button %d realease,%d.%dms.\r\n", u8ButtonAdcVal, u16ButtonPressedTimes/10, u16ButtonPressedTimes%10); 
            u8ButtoniIsPressed = 0;
        }
        u8ButtonAdcCount = 0;
    }
}

/*********************************************************************
 * @fn      ADC1_2_IRQHandler
 *
 * @brief   This function handles ADC1_2 exception.
 *
 * @return  none
 */
void ADC1_2_IRQHandler(void)
{
    // if(ADC_GetITStatus( ADC1, ADC_IT_AWD))
    // {
       ADC_AnalogWatchDog_Isr();
    // }
    ADC_ClearITPendingBit( ADC1, ADC_IT_AWD);
}
/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
//button adc
#include "button_adc.h"

#define SFU_DEMO_TEST 0
#define XIP_DEMO_TEST 0

#if Core_V3F
//st7735s
#include "st7735s_bsp.h"
//sfud
#include "sfud.h"
//tf card
#include "sd_spi_driver.h"
//audio sai
#include "audio_sai.h"

#define SFUD_DEMO_TEST_BUFFER_SIZE      1024
uint8_t sfud_demo_test_buf[SFUD_DEMO_TEST_BUFFER_SIZE];
#endif

#if Core_V3F && V3F_USE_FreeRTOS
/* FreeRTOS Stack Size , Unit Word , Total=4KB */
#define LED_TASK_STACK_SIZE             ( 128/4)    /* 128Byte  */
#define V3F_MAIN_TASK_STACK_SIZE        (2048/4)    /* 2048Byte */
/* FreeRTOS Task Priorities */
#define LED_TASK_PRIORITIES             (configMAX_PRIORITIES-3)
#define V3F_MAIN_TASK_PRIORITIES        (configMAX_PRIORITIES-3)
#endif


/*********************************************************************
 * @fn      LED_Init
 *
 * @brief   Initializes LED's GPIO
 *
 * @return  none
 */
void LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure={0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      QuadSpi_Init
 * 
 * @brief   Initializes Quad Spi
 * 
 * @param   none
 */
void QuadSpi_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    QSPI_InitTypeDef QSPI_InitStructure = {0};

    /* Enables the High Speed HB2 peripheral clock. */
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    //RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOF, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOC, ENABLE);
    //RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOE, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_QSPI2, ENABLE);

    // QSPI2_SCK PF6(AF4)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource6, GPIO_AF4);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // QSPI2_SCSN PF7(AF4)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource7, GPIO_AF4);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // QSPI2_SIO0 PF8(AF4)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF4);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // SPI2_SIO1 PF9(AF4)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF4);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // QSPI2_SIO2 PF10(AF4)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource10, GPIO_AF4);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // QSPI2_SIO3 PC0(AF10)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource0, GPIO_AF10);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Initializes the QSPI peripheral according to the specified parameters in the QSPI_InitStruct. */
    QSPI_InitStructure.QSPI_Prescaler = 3;
    QSPI_InitStructure.QSPI_CKMode    = QSPI_CKMode_Mode3;
    QSPI_InitStructure.QSPI_CSHTime   = QSPI_CSHTime_8Cycle;
    // size = 2 ** (FSize + 1) = 2 ** 25 = 32MB
    QSPI_InitStructure.QSPI_FSize = 24;
    QSPI_InitStructure.QSPI_FSelect = QSPI_FSelect_1;
    QSPI_InitStructure.QSPI_DFlash  = QSPI_DFlash_Disable;
    QSPI_Init(QSPI2, &QSPI_InitStructure);
    /* Sets the FIFO Threshold */
    QSPI_SetFIFOThreshold(QSPI2, 10);

    /* Enables QSPI peripheral. */
    QSPI_Cmd(QSPI2, ENABLE);
}


#if Core_V3F && V3F_USE_FreeRTOS

void LED_Run_Task(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(500); 
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_0,!GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_0));
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void V3F_Main_Task(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(500); 

    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        //printf("KEY ADC:%dmV.\r\n",Mcu_Adc_GetVolatge_mV()); 
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/*******************************************************************************
 * FreeRTOS Hook function
 ******************************************************************************/
#if configUSE_IDLE_HOOK
void vApplicationIdleHook(void) 
{
    //EnterStopMode_NoOperation_TimeOut();
}
#endif


#if configUSE_TICK_HOOK
void vApplicationTickHook(void) 
{
   extern volatile uint64_t ticks_count_overflow;
   ticks_count_overflow ++ ;
}
#endif

#if configUSE_MALLOC_FAILED_HOOK
void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS(); 
    while(1);
}
#endif

#if configCHECK_FOR_STACK_OVERFLOW  
void vApplicationStackOverflowHook(xTaskHandle pxTask, char *pcTaskName) 
{
    while(1);
}
#endif  

#if configUSE_DAEMON_TASK_STARTUP_HOOK
void vApplicationDaemonTaskStartupHook( void )
{

}
#endif

#endif

// #if Core_V3F
// const uint16_t fill_buf[4096] = { [0 ... 4095] = ST7735_RED };
// #endif

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @return  none
 */
void Hardware(void)
{
#if Core_V3F
    LED_Init();
    Button_Adc_Init();

    Lcd_Init();
    Lcd_Backlight(BLACKLIGHT_ON);
    //St7735s_Fill(&st773s_lcd0, 0, 63, 0, 63, (uint8_t *)fill_buf, 4096*2);
    St7735s_DrawFillRectangle(&st773s_lcd0, 0, 159, 0, 79, ST7735_RED);

    /*SFUD initialize */
    if (sfud_init() == SFUD_SUCCESS)
    {
        printf("SFUD_SUCCESS.\r\n");
#if SFU_DEMO_TEST
        sfud_demo(0, SFUD_DEMO_TEST_BUFFER_SIZE, (uint8_t*)sfud_demo_test_buf);
#endif
    }
    else
    {
        printf("SFUD_SFUD_FAILED.\r\n");
    }

    Audio_SAI_Init();

    /* spi sdcard driver */
    sd_spi_lib_init();
    struct sd_card* card = sd_card_find("card0");
    if(card == NULL)
    {
        printf("card not found\r\n");
    }
    else
    {
        printf("card(\"%s\") found\r\n", card->name);
        if(sd_card_init(card) != Sd_Err_OK)
        {
            printf("card init failed\r\n");
        }
        else
        {
            printf("card init success, type: %s\r\n", sd_get_capacity_class_name(sd_card_get_type(card)));
        }
    }
        
#if V3F_USE_FreeRTOS
    /* led blink task */
    xTaskCreate((TaskFunction_t )LED_Run_Task ,
                        (const char*    )"LED_Task" ,
                        (uint16_t       )LED_TASK_STACK_SIZE ,  //stack size
                        (void*          )NULL ,
                        (UBaseType_t    )LED_TASK_PRIORITIES,     //task pro level
                        (TaskHandle_t*  )NULL);

    /* Main task */
    xTaskCreate((TaskFunction_t )V3F_Main_Task ,
                        (const char*    )"Main_Task" ,
                        (uint16_t       )V3F_MAIN_TASK_STACK_SIZE ,  //stack size
                        (void*          )NULL ,
                        (UBaseType_t    )V3F_MAIN_TASK_PRIORITIES,     //task pro level
                        (TaskHandle_t*  )NULL);

    vTaskStartScheduler();
#endif

    while(1)
    {
        Delay_Ms(500);
        GPIO_WriteBit(GPIOA, GPIO_Pin_0,!GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_0));
    }
#endif

#if Core_V5F

    while(1)
    {

    }
#endif

}

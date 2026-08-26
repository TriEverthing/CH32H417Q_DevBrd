#ifndef __BUTTON_ADC_H__
#define __BUTTON_ADC_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* include files ------------------------------------------------------------*/
#include "ch32h417.h"
#include "debug.h"

/* exported constants --------------------------------------------------------*/


/* Exported macro ------------------------------------------------------------*/
#define BUTTONADC_ENABLE_TIMER  1

#define ADC_SPEED_High          0
#define ADC_SPEED_Low           1

#define MCU_ADC_VREF            3000     //Unit:mV
#define MCU_ADC_BITS            12

#define MCU_ADC_HighThreshold   4095
#define MCU_ADC_LowThreshold    3800
/* Exported macro ------------------------------------------------------------*/
//#define ADC_SPEED               ADC_SPEED_High
#define ADC_SPEED               ADC_SPEED_Low
#define MCU_ADC_DMA_ENABLE      0

#define AdcRaw2ButtonNb(raw)    (( raw * 10 / 896 + 5 ) / 10)

/* Exported define -----------------------------------------------------------*/


/* External variables --------------------------------------------------------*/
extern uint8_t u8ButtoniIsPressed;
extern int8_t u8ButtonAdcVal;
/* Exported functions ------------------------------------------------------- */
void Button_Adc_Init(void);

uint16_t Button_Adc_GetVolatge_mV(void);

#ifdef __cplusplus
}
#endif

#endif  /* __BUTTON_ADC_H__ */
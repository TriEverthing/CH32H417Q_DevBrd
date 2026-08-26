/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file contains all the functions prototypes for the 
*                      hardware.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __HARDWARE_H
#define __HARDWARE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32h417.h"
#include "printf.h"
#include "debug.h"

/* exported constants --------------------------------------------------------*/

/* exported macro ------------------------------------------------------------*/
/* V3F Core Functions */
#define V3F_DEBUG_ON_OFF            1
#define V3F_DEBUG_TO_UART           0
#define V3F_DEBUG_TO_USB_CDC        1
#define V3F_USE_LETTER_SHELL        0
#define V3F_USE_EasyLogger          0
#define V3F_USE_FreeRTOS            1
#define V3F_Usb_To_Uart             0
#define V3F_UART_DMA_TX             1
#define V3F_USB_Stack_CherryUSB     0   
#define V3F_USB_Stack_TinyUSB       0
/* V5F Core Functions */
#define V5F_DEBUG_ON_OFF            1
#define V5F_DEBUG_TO_UART           0
#define V5F_DEBUG_TO_USB_CDC        1
#define V5F_USE_LETTER_SHELL        0
#define V5F_USE_EasyLogger          0
#define V5F_USE_FreeRTOS            0
#define V5F_Usb_To_Uart             0
#define V5F_UART_DMA_TX             1
#define V5F_USB_Stack_CherryUSB     0   
#define V5F_USB_Stack_TinyUSB       0
/* Exported macro ------------------------------------------------------------*/
#if (V3F_USE_FreeRTOS && Core_V3F) || (V5F_USE_FreeRTOS && Core_V5F)
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#endif
/* Exported define -----------------------------------------------------------*/


/* External variables --------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */
	 
void Hardware(void);

void QuadSpi_Init(void);

#ifdef __cplusplus
}
#endif

#endif 






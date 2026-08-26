/**
 *     st7735 display library
 *
 *     Copyright (c) 2020 Vitaliy Nimych (Cvetaev) @ cvetaevvitaliy@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _ST7735S_BSP_H
#define _ST7735S_BSP_H

/* includes -----------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "ch32h417.h"
#include "debug.h"
#include "st7735s.h"
/* private includes -------------------------------------------------------------*/

/* exported types -------------------------------------------------------------*/

/* exported constants --------------------------------------------------------*/

/* exported macro ------------------------------------------------------------*/
#define LCD_SPI_DMA_ENABLE              0
#define USE_LVGL_ENABLE                 0
/**
 * @defgroup ST7735S Hardware definition 
 * @{
 */
#define LCD_SPI_COMM_TIMEOUT_VAL        (0x20000000UL)
/* SPI definition */
#define LCD_SPI                         (SPI1)
#define LCD_SPI_RCC_SPI                 (RCC_HB2Periph_SPI1)
#define RCC_LCDSPIPeriphClockCmd        RCC_HB2PeriphClockCmd
/* CS = PA4 */
#define LCD_SPI_CSX_RCC_GPIO            (RCC_HB2Periph_GPIOA)
#define LCD_SPI_CSX_PORT                (GPIOA)
#define LCD_SPI_CSX_PINSRC              (GPIO_PinSource4)
#define LCD_SPI_CSX_PIN                 (GPIO_Pin_4)
#define LCD_SPI_CSX_FUNC                (GPIO_AF5) //CSX
/* SCK = PA5 */
#define LCD_SPI_SCK_RCC_GPIO            (RCC_HB2Periph_GPIOA)
#define LCD_SPI_SCK_PORT                (GPIOA)
#define LCD_SPI_SCK_PINSRC              (GPIO_PinSource5)
#define LCD_SPI_SCK_PIN                 (GPIO_Pin_5)
#define LCD_SPI_SCK_FUNC                (GPIO_AF5) //SCK
/* MOSI = PA7 */
#define LCD_SPI_SDI_RCC_GPIO            (RCC_HB2Periph_GPIOA)
#define LCD_SPI_SDI_PORT                (GPIOA)
#define LCD_SPI_SDI_PINSRC              (GPIO_PinSource7)
#define LCD_SPI_SDI_PIN                 (GPIO_Pin_7)
#define LCD_SPI_SDI_FUNC                (GPIO_AF5) //MOSI
/**
 * @defgroup ST7735S Others Hardware definition 
 * @{```````````````
 */
/* ST7735S DC : Data & Command Select */
/* DC = PA6 */
#define LCD_DC_RCC_GPIO                 (RCC_HB2Periph_GPIOA)
#define LCD_DC_PORT                     (GPIOA)
#define LCD_DC_PIN                      (GPIO_Pin_6)
/* RST = PA10 */
// #define LCD_RST_RCC_GPIO                (RCC_HB2Periph_GPIOA)
// #define LCD_RST_PORT                    (GPIOA)
// #define LCD_RST_PIN                     (GPIO_Pin_10)
/* BLON = PA1 */
#define LCD_BLON_RCC_GPIO               (RCC_HB2Periph_GPIOA)
#define LCD_BLON_PORT                   (GPIOA)
#define LCD_BLON_PIN                    (GPIO_Pin_1)
/* DC Low Level , Command */
#define LCD_ST7735S_DC_Cmd()			LCD_DC_PORT->BCR = LCD_DC_PIN
/* DC High Level , Data */
#define LCD_ST7735S_DC_Data()			LCD_DC_PORT->BSHR = LCD_DC_PIN
/**
 * @defgroup SPI DMA definition 
 * @{
 */
/* DMA definition */
#define LCD_SPI_DMA                     (DMA1)
#define LCD_SPI_RCC_DMA                 (RCC_HBPeriph_DMA1)
#define LCD_SPI_DMA_TX_CH               (DMA1_Channel1)
#define LCD_SPI_DMA_TX_CH_MUX           (DMA_MuxChannel1)//for DMA1_Channel1
#define LCD_SPI_DMA_TX_CH_REQ           (63)  //for SPI1_TX
#define LCD_SPI_DMA_TX_CH_FLAG_TC       (DMA1_FLAG_TC1)
#define LCD_SPI_DMA_TX_CH_IT_TC         (DMA1_IT_TC1)
#define LCD_SPI_DMA_TX_CH_IRQ           (DMA1_Channel1_IRQn)
#define LCD_SPI_DMA_TX_CH_IRQHandler    DMA1_Channel1_IRQHandler
/* Exported define -----------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void Lcd_Init(void);

void Lcd_Backlight(uint8_t u8State);

/* External variables --------------------------------------------------------*/
extern lcd_st7735s_t st773s_lcd0;

#endif //_ST7735S_H

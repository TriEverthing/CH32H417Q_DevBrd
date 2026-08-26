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
#ifndef _ST7735S_H
#define _ST7735S_H

/* includes -----------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "st7735s_settings.h"

/* private includes -------------------------------------------------------------*/
/* add user code begin private includes */

/* add user code end private includes */

/* exported types -------------------------------------------------------------*/
typedef enum
{
    LCD_R0,
    LCD_R90,
    LCD_R180,
    LCD_R270
} st7735s_rotation_t;

typedef enum
{
    BLACKLIGHT_OFF,
    BLACKLIGHT_ON
}Blacklight_State_t;

typedef enum
{
    ST7735S_CMD,
    ST7735S_DATA
}st7735s_CmdOrData_t;

typedef struct ST7735s
{
    uint8_t width;
    uint8_t height;
    uint8_t xstart;
    uint8_t ystart;
    void (*LowLevelInit)(void);
    void (*St7735s_CmdData)(uint8_t);//0-Command,1-Data
    void (*Transmit)(uint8_t *, uint32_t);
    void (*Fill)(uint8_t *, uint16_t,uint8_t);
    void (*Blacklight)(uint8_t);
    void (*DelyaMs)(uint32_t);
} lcd_st7735s_t;
/* exported constants --------------------------------------------------------*/
/* add user code begin exported constants */

/* add user code end exported constants */

/* exported macro ------------------------------------------------------------*/
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36 /* Memory Data Access Control */
#define ST7735_VSCSAD  0x37 /* Vertical Scroll Start Address of RAM */
#define ST7735_SCRLAR  0x33 /* Scroll Area Set */

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

//// Color definitions
#define	ST7735_BLACK   0x0000
#define	ST7735_BLUE    0x001F
#define	ST7735_RED     0xF800
#define	ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF
//#define ST7735_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

/* add user code begin exported functions */
void St7735s_Init(lcd_st7735s_t *lcdst7735s);

void St7735s_Power_OnOff(lcd_st7735s_t *lcdst7735s, uint8_t u8OnOff);

void St7735s_SetAddressWindow(lcd_st7735s_t *lcdst7735s, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

void St7735s_DrawPixel(lcd_st7735s_t *lcdst7735s, int16_t x, int16_t y, uint16_t color);

void St7735s_DrawFillRectangle(lcd_st7735s_t *lcdst7735s, uint8_t u8XStart, uint8_t u8XEnd,uint8_t u8YStart, uint8_t u8YEnd, uint16_t color);

void St7735s_Set_Orientation(lcd_st7735s_t *lcdst7735s, st7735s_rotation_t rotation);

void St7735s_Scroll(const lcd_st7735s_t *lcdst7735s,uint8_t u8line);

void St7735s_ScrollArea(const lcd_st7735s_t *lcdst7735s, uint8_t u8XStart, uint8_t u8XEnd);

void LCD_ST7735S_Clear(void);

void St7735s_Fill(lcd_st7735s_t *lcdst7735s, uint8_t u8XStart, uint8_t u8XEnd,uint8_t u8YStart, uint8_t u8YEnd,uint8_t * u8pColor, uint16_t u16plength);



#endif //_ST7735S_H

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
#include "st7735s.h"

#define DELAY 0x80

static const uint8_t init_cmds1[] =
{   // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
    150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
    255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
    0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
    0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
    0x01, 0x2C, 0x2D,       //     Dot inversion mode
    0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
    0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
    0xA2,
    0x02,                   //     -4.6V
    0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
    0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
    0x0A,                   //     Opamp current small
    0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
    0x8A,                   //     BCLK/2, Opamp current small & Medium low
    0x2A,
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
    0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
    0x0E,
    ST7735_INVON , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
    ST7735_ROTATION,        //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
    0x05
};//     16-bit color

#if (defined(ST7735_IS_128X128) || defined(ST7735_IS_160X128))
static const uint8_t init_cmds2[] =
{            // Init for 7735R, part 2 (1.44" display)
    2,                  //  2 commands in list:
    ST7735_CASET , 4 ,  //  1: Column addr set, 4 args, no delay:
    0x00, 0x00 ,        //     XSTART = 0
    0x00, 0x7F ,        //     XEND = 127
    ST7735_RASET , 4  , //  2: Row addr set, 4 args, no delay:
    0x00, 0x00,         //     XSTART = 0
    0x00, 0x7F
};           //     XEND = 127
#endif // ST7735_IS_128X128

#ifdef ST7735_IS_160X80
static const uint8_t init_cmds2[] =
{            // Init for 7735S, part 2 (160x80 display)
    2,                  //  3 commands in list:
    ST7735_CASET , 4 ,  //  1: Column addr set, 4 args, no delay:
    0x00, 0x00 ,        //  XSTART = 0
    0x00, 0x4F ,        //  XEND = 79
    ST7735_RASET , 4 ,  //  2: Row addr set, 4 args, no delay:
    0x00, 0x00 ,        //  XSTART = 0
    0x00, 0x9F ,        //  XEND = 159
    //ST7735_INVON, 0

};        //  3: Invert colors
#endif
static const uint8_t init_cmds3[] =
{            // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
    0x02, 0x1c, 0x07, 0x12,
    0x37, 0x32, 0x29, 0x2d,
    0x29, 0x25, 0x2B, 0x39,
    0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
    0x03, 0x1d, 0x07, 0x06,
    0x2E, 0x2C, 0x29, 0x2D,
    0x2E, 0x2E, 0x37, 0x3F,
    0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,  DELAY, //  3: Normal display on, no args, w/delay
    10,                     //     10 ms delay
    ST7735_DISPON ,  DELAY, //  4: Main screen turn on, no args w/delay
    100
};                  //     100 ms delay


static void ST7735_WriteCommand(const lcd_st7735s_t *lcdst7735s, uint8_t u8Cmd)
{
    //int32_t i32Ret = LL_ERR;
    /* DC Low Level , Command */
    lcdst7735s->St7735s_CmdData(ST7735S_CMD);
    /* SPI Transmit Command */
    lcdst7735s->Transmit(&u8Cmd, 1U);
}


static void ST7735_WriteData(const lcd_st7735s_t *lcdst7735s, uint8_t* u8Data, size_t sizeTxlen)
{
    //int32_t i32Ret = LL_ERR;
    /* DC High Level , Data */
    lcdst7735s->St7735s_CmdData(ST7735S_DATA);
    /* SPI Transmit Data */
    lcdst7735s->Transmit(u8Data, sizeTxlen);
}

static void ST7735_ExecuteCommandList(const lcd_st7735s_t *lcdst7735s, const uint8_t *addr)
{
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while(numCommands--)
    {
        uint8_t cmd = *addr++;
        ST7735_WriteCommand(lcdst7735s, cmd);

        numArgs = *addr++;

        /** If high bit set, delay follows args */
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        if(numArgs)
        {
            ST7735_WriteData(lcdst7735s, (uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if(ms)
        {
            ms = *addr++;
            if(ms == 255) ms = 500;
            	lcdst7735s->DelyaMs(ms);
        }
    }
}

void St7735s_Init(lcd_st7735s_t *lcdst7735s)
{
    /* Low Level Hardware initialztion */
    lcdst7735s->LowLevelInit();
    /* Execute ST7735 initialztion command list */
	ST7735_ExecuteCommandList(lcdst7735s, init_cmds1);
	ST7735_ExecuteCommandList(lcdst7735s, init_cmds2);
	ST7735_ExecuteCommandList(lcdst7735s, init_cmds3);
    /*  Swicth ON Backlight */
	lcdst7735s->Blacklight(BLACKLIGHT_OFF);
}

void St7735s_Power_OnOff(lcd_st7735s_t *lcdst7735s, uint8_t u8OnOff)
{
    if( u8OnOff )
    {
        ST7735_WriteCommand(lcdst7735s, ST7735_SLPOUT);
    }
    else
    {
        ST7735_WriteCommand(lcdst7735s, ST7735_SLPIN);
    }
}

void St7735s_SetAddressWindow(lcd_st7735s_t *lcdst7735s, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    uint8_t data[] = { 0x00, x0 + lcdst7735s->xstart, 0x00, x1 + lcdst7735s->xstart };
    /* column address set */
    ST7735_WriteCommand(lcdst7735s, ST7735_CASET);
    ST7735_WriteData(lcdst7735s, data, sizeof(data));

    // row address set
    ST7735_WriteCommand(lcdst7735s, ST7735_RASET);
    data[1] = y0 + lcdst7735s->ystart;
    data[3] = y1 + lcdst7735s->ystart;
    ST7735_WriteData(lcdst7735s, data, sizeof(data));

    // write to RAM
    ST7735_WriteCommand(lcdst7735s, ST7735_RAMWR);
}

void St7735s_DrawPixel(lcd_st7735s_t *lcdst7735s, int16_t x, int16_t y, uint16_t color)
{
    /* Swap Low Byte and High Byte */
    uint8_t data[] = { color >> 8, color & 0xFF };
    if((x >= lcdst7735s->width) || (y >= lcdst7735s->height))
        return;

    St7735s_SetAddressWindow(lcdst7735s, x, y, x+1, y+1);
    ST7735_WriteData(lcdst7735s, data, 2U);
}


void St7735s_Set_Orientation(lcd_st7735s_t *lcdst7735s, st7735s_rotation_t rotation)
{
    uint8_t madctl;

    switch ((uint8_t)rotation)
    {
        case   LCD_R0:
        {
            madctl = 0b01100000;
            lcdst7735s->width = ST7735_WIDTH;
            lcdst7735s->height = ST7735_HEIGHT;
            lcdst7735s->xstart = ST7735_XSTART;
            lcdst7735s->ystart = ST7735_YSTART;
            break;
        }
        case  LCD_R90:
        {
            madctl = 0b11000000;
            lcdst7735s->width = ST7735_HEIGHT;
            lcdst7735s->height = ST7735_WIDTH;
            lcdst7735s->xstart = ST7735_YSTART;
            lcdst7735s->ystart = ST7735_XSTART;
            break;
        }
        case LCD_R180:
        {
            madctl = 0b10100000;
            lcdst7735s->width = ST7735_WIDTH;
            lcdst7735s->height = ST7735_HEIGHT;
            lcdst7735s->xstart = ST7735_XSTART;
            lcdst7735s->ystart = ST7735_YSTART;

            break;
        }
        case LCD_R270:
        {
            madctl = 0b01000000;
            lcdst7735s->width = ST7735_HEIGHT;
            lcdst7735s->height = ST7735_WIDTH;
            lcdst7735s->xstart = ST7735_YSTART;
            lcdst7735s->ystart = ST7735_XSTART;
            break;
        }
    }
    ST7735_WriteCommand(lcdst7735s, ST7735_MADCTL);
    ST7735_WriteData(lcdst7735s, &madctl, sizeof(madctl));
}



void St7735s_Scroll(const lcd_st7735s_t *lcdst7735s, uint8_t u8line)
{
    if ( u8line < 160 )
    {
        uint8_t data[] = {u8line >> 8, u8line & 0xFF};
        ST7735_WriteCommand(lcdst7735s, ST7735_VSCSAD);
        ST7735_WriteData(lcdst7735s, data, 2);
    }
}


void St7735s_ScrollArea(const lcd_st7735s_t *lcdst7735s, uint8_t u8XStart, uint8_t u8XEnd)
{
    /** tfa: top fixed area: nr of line from top of the frame mem and display) */
    uint16_t tfa = lcdst7735s->width - u8XEnd + lcdst7735s->xstart;
    /** vsa: height of the vertical scrolling area in nr of line of the frame mem
       (not the display) from the vertical scrolling address. the first line appears
       immediately after the bottom most line of the top fixed area. */
    uint16_t vsa = u8XEnd - u8XStart + lcdst7735s->xstart;
    /** bfa: bottom fixed are in nr of lines from bottom of the frame memory and display */
    uint16_t bfa = u8XStart + lcdst7735s->xstart;

    if (tfa+vsa+bfa < 160)
        return;

    uint8_t u8CmdList[] = { tfa >> 8, tfa & 0xFF,
                      vsa >> 8, vsa & 0xFF,
                      bfa >> 8, bfa & 0xFF };

    ST7735_WriteCommand(lcdst7735s, ST7735_SCRLAR);
    ST7735_WriteData(lcdst7735s, u8CmdList, 6U);

}

void St7735s_DrawFillRectangle(lcd_st7735s_t *lcdst7735s, uint8_t u8XStart, uint8_t u8XEnd,uint8_t u8YStart, uint8_t u8YEnd, uint16_t color)
{
#if 1
    uint16_t length;
    // uint8_t data[] = { color >> 8, color & 0xFF };
    length = ( u8YEnd - u8YStart + 1)*( u8XEnd - u8XStart + 1)*2;
    St7735s_SetAddressWindow(lcdst7735s, u8XStart, u8YStart, u8XEnd, u8YEnd);
    /* DC High Level , Data */
    lcdst7735s->St7735s_CmdData(ST7735S_DATA);
    /* Fill Buffer */
    lcdst7735s->Fill((uint8_t *)&color, length, true);
#else
    for( uint8_t i = u8XStart; i <= u8XEnd; i++ )
    {
        for( uint8_t j = u8YStart; j <= u8YEnd; j++ )
        {
            St7735s_DrawPixel(lcdst7735s, i, j, color);
        }
    }
#endif
}


void St7735s_Fill(lcd_st7735s_t *lcdst7735s, uint8_t u8XStart, uint8_t u8XEnd,uint8_t u8YStart, uint8_t u8YEnd,uint8_t * u8pColor, uint16_t u16plength)
{
    St7735s_SetAddressWindow(lcdst7735s, u8XStart, u8YStart, u8XEnd, u8YEnd);
    /* DC High Level , Data */
    lcdst7735s->St7735s_CmdData(ST7735S_DATA);
    /* Fill Buffer */
    lcdst7735s->Fill(u8pColor, u16plength, false);
}


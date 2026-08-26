#include "st7735s_bsp.h"

lcd_st7735s_t st773s_lcd0;

#if USE_LVGL_ENABLE
void LCD_SPI_DMA_TX_CH_IRQHandler(void) __attribute__((interrupt()));

extern void lvgl_lcd_dma_cb(void);
/**
 * @brief  DMA transmit complete callback.
 * @param  None
 * @retval None
 */
static void LCD_SPI_DMA_TransCompleteCallback(void)
{
    /* signal LVGL that we're done */
    //lvgl_lcd_dma_cb();
}

void LCD_SPI_DMA_TX_CH_IRQHandler(void)
{
    LCD_SPI_DMA_TransCompleteCallback();
    /* Clear DMA transfer flag */
    DMA_ClearITPendingBit(LCD_SPI_DMA, LCD_SPI_DMA_TX_CH_IT_TC);
}
#endif
/**
 * @brief  LCD panel low level hardware initial.
 * @param  None.
 * @retval None.
 */
static void Lcd_St7735s_Low_Level_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef  SPI_InitStructure  = {0};
#if LCD_SPI_DMA_ENABLE
    DMA_InitTypeDef DMA_InitStructure = {0};
    uint16_t DumyByte = 0xFFFF;
#endif
    
    /* Enables the AFIO and GPIO peripheral clock. */
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | LCD_DC_RCC_GPIO | LCD_BLON_RCC_GPIO | \
        LCD_SPI_CSX_RCC_GPIO | LCD_SPI_SCK_RCC_GPIO | LCD_SPI_SDI_RCC_GPIO , ENABLE);
    /* Enables the SPI peripheral clock. */
    RCC_LCDSPIPeriphClockCmd(LCD_SPI_RCC_SPI, ENABLE);

    /* Initialize LCD GPIO config structure. */
    //LCD DC
    GPIO_InitStructure.GPIO_Pin   = LCD_DC_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(LCD_DC_PORT, &GPIO_InitStructure);
    //BLON
    GPIO_InitStructure.GPIO_Pin = LCD_BLON_PIN;
    GPIO_Init(LCD_BLON_PORT, &GPIO_InitStructure);

    /* Initialize SPI GPIO config structure. */
    /* Port multiplexing function. */
    GPIO_PinAFConfig(LCD_SPI_CSX_PORT, LCD_SPI_CSX_PINSRC, LCD_SPI_CSX_FUNC);
    GPIO_PinAFConfig(LCD_SPI_SCK_PORT, LCD_SPI_SCK_PINSRC, LCD_SPI_SCK_FUNC);
    GPIO_PinAFConfig(LCD_SPI_SDI_PORT, LCD_SPI_SDI_PINSRC, LCD_SPI_SDI_FUNC);
    /* Initializes the GPIO peripheral according to the specified parameters in the GPIO_InitStruct. */
    // CSX
    GPIO_InitStructure.GPIO_Pin   = LCD_SPI_CSX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(LCD_SPI_CSX_PORT, &GPIO_InitStructure);
    // MOSI
    GPIO_InitStructure.GPIO_Pin   = LCD_SPI_SDI_PIN;
    GPIO_Init(LCD_SPI_SDI_PORT, &GPIO_InitStructure);
    // SCK
    GPIO_InitStructure.GPIO_Pin  = LCD_SPI_SCK_PIN;
    GPIO_Init(LCD_SPI_SCK_PORT, &GPIO_InitStructure);

    /* Configuration SPI */
    /* Enables the SS output for the selected SPI. */
    SPI_SSOutputCmd( LCD_SPI, ENABLE ); //must be call at before the SPI_Init() 
    /* Set a default value for the SPI initialization structure. */
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;//SCK pin output low in idle state;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;//MOSI/MISO pin data valid in odd edge
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Hard;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_Mode2;//HCLK/RatePrescaler:120MHz/8=15Mbps
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial     = 7;
    SPI_InitStructure.SPI_Direction         = SPI_Direction_1Line_Tx;
    /*  Initializes the SPIx peripheral according to the specified parameters in the SPI_InitStruct. */
    SPI_Init(LCD_SPI, &SPI_InitStructure);
    /* Enables the specified SPI peripheral. */
    SPI_Cmd(LCD_SPI, ENABLE);

#if LCD_SPI_DMA_ENABLE
    /* DMA configuration */
    /* Enables the DMA peripheral clock. */
    RCC_HBPeriphClockCmd(LCD_SPI_RCC_DMA, ENABLE);
    /* Configure TX */
    /* Deinitializes the DMAy Channelx registers to their default reset values. */
    DMA_DeInit(LCD_SPI_DMA_TX_CH);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&LCD_SPI->DATAR);
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)&DumyByte;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = 1;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    /* Initializes the DMAy Channelx according to the specified parameters in the DMA_InitStruct. */
    DMA_Init(LCD_SPI_DMA_TX_CH, &DMA_InitStructure);
    /* Configures the DMA input channel. */
    DMA_MuxChannelConfig(LCD_SPI_DMA_TX_CH_MUX, LCD_SPI_DMA_TX_CH_REQ);

    /* Enables the SPIx/I2Sx DMA interface. */
    SPI_I2S_DMACmd(LCD_SPI, SPI_I2S_DMAReq_Tx, ENABLE);

    /* Disables the specified DMAy Channelx. */
    DMA_Cmd(LCD_SPI_DMA_TX_CH, ENABLE);
#if USE_LVGL_ENABLE
    /* DMA receive NVIC configure */
    /* Set Interrupt Priority */
    NVIC_SetPriority(LCD_SPI_DMA_TX_CH_IRQ, 0);
    /* Enable Interrupt */
    NVIC_EnableIRQ(LCD_SPI_DMA_TX_CH_IRQ);
    /* Enables the specified DMAy Channelx interrupts. */
    DMA_ITConfig(LCD_SPI_DMA_TX_CH, DMA_IT_TC, ENABLE);
#endif
#endif    

    /*  Swicth OFF Backlight */
	Lcd_Backlight(BLACKLIGHT_OFF);
    /* Hardware Rest ST7735s */
}

/**
 * @brief  LCD panel switch command and data mode
 * @param  u8State, 0->command Mode, 1 and others->Data Mode
 * @retval None.
 */
static void Lcd_Switch_CmdData(uint8_t u8CmdorData)
{
    //0-Command,1-Data
    if( u8CmdorData )
        LCD_DC_PORT->BSHR = LCD_DC_PIN;
    else
        LCD_DC_PORT->BCR = LCD_DC_PIN;
}

static void Lcd_SPI_Transmit(uint8_t *u8Tran, uint32_t u32Txlength)
{
    uint32_t i=0;
    uint32_t u32timeout = LCD_SPI_COMM_TIMEOUT_VAL;
    /* SPI Width , 8Bit */
    LCD_SPI->CTLR1 &= ~SPI_CTLR1_DFF;
    /* SPI Transmit */
    while(i < u32Txlength ) 
    {
        u32timeout--;
        if( u32timeout == 0 )
        {
            /* Times out */
            return;
        }

    	if( SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_TXE) == SET )
        {
            SPI_I2S_SendData( LCD_SPI, u8Tran[i] );
            i++;
            u32timeout = LCD_SPI_COMM_TIMEOUT_VAL;
        }
    }

    u32timeout = LCD_SPI_COMM_TIMEOUT_VAL;
    while( SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_BSY))
    {
        u32timeout--;
        if( u32timeout == 0 )
        {
            /* Times out */
            return;
        }
    }
    /* SPI Width , 16Bit */
    LCD_SPI->CTLR1 |= SPI_CTLR1_DFF;
}

/**
 * @brief  Fill Color to LCD , this function very improtant for some gui libiary.
 * @param  u16Color , RGB565 format color vaule
 * @param  u16length , the length of color 
 * @retval None.
 */
void Lcd_Fill(uint8_t *u8pColor, uint16_t u16length, uint8_t Mode)
{
#if LCD_SPI_DMA_ENABLE
    uint32_t u32timeout = LCD_SPI_COMM_TIMEOUT_VAL;
    // while( LCD_SPI_DMA_TX_CH->CNTR )
    // {
    //     /* DMA Channel Busy */
    //     u32timeout--;
    //     if( u32timeout == 0 ) return;
    // }

    // u32timeout = LCD_SPI_COMM_TIMEOUT_VAL;
    while(DMA_GetFlagStatus(LCD_SPI_DMA, LCD_SPI_DMA_TX_CH_FLAG_TC)==RESET) 
    {
        /* DMA Channel Busy */
        u32timeout--;
        if( u32timeout == 0 ) return;    
    }
    /* Disable DMA */
    LCD_SPI_DMA_TX_CH->CFGR &= (uint16_t)(~DMA_CFGR1_EN);

    if( Mode )
    {
        LCD_SPI_DMA_TX_CH->CFGR &= ~DMA_CFGR1_MINC;
    }
    else
    {
        LCD_SPI_DMA_TX_CH->CFGR |= DMA_CFGR1_MINC;           
    }
    
    LCD_SPI_DMA_TX_CH->MADDR = (uint32_t)u8pColor;
    LCD_SPI_DMA_TX_CH->CNTR = u16length/2;    
    /* Enable DMA */
    LCD_SPI_DMA_TX_CH->CFGR |= DMA_CFGR1_EN;
#else
    uint32_t i=0;
    uint32_t u32timeout = LCD_SPI_COMM_TIMEOUT_VAL;
    u16length = u16length/2;
    uint16_t *ptr = (uint16_t *)u8pColor;
    /* SPI Transmit */
    while(i < u16length ) 
    {
    	if( SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_TXE) == SET )
        {
            if( Mode )
            {
                LCD_SPI->DATAR = ptr[0];
            }
            else
            {
                LCD_SPI->DATAR = ptr[i];                
            }

            i++;
            u32timeout = LCD_SPI_COMM_TIMEOUT_VAL;
        }

        u32timeout--;
        if( u32timeout == 0 )
        {
            /* Times out */
            return;
        }
    }
#endif
}

/**
 * @brief  LCD panel initial.
 * @param  None.
 * @retval None.
 */
void Lcd_Init(void)
{
    st773s_lcd0.width = ST7735_WIDTH;
    st773s_lcd0.height = ST7735_HEIGHT;
    st773s_lcd0.xstart = ST7735_XSTART;
    st773s_lcd0.ystart = ST7735_YSTART;
    st773s_lcd0.LowLevelInit = Lcd_St7735s_Low_Level_Init;
    st773s_lcd0.St7735s_CmdData = Lcd_Switch_CmdData;
    st773s_lcd0.Transmit = Lcd_SPI_Transmit;
    st773s_lcd0.Fill = Lcd_Fill;
    st773s_lcd0.Blacklight = Lcd_Backlight;
    st773s_lcd0.DelyaMs = Delay_Ms;
    St7735s_Init(&st773s_lcd0);
}

/**
 * @brief  LCD panel blacklight switch on/off.
 * @param  u8State, 0->Switch OFF, 1 and others->Switch ON
 * @retval None.
 */
void Lcd_Backlight(uint8_t u8State)
{
    if (u8State)
    {
        /* High Level , Switch ON Blacklight */
        LCD_BLON_PORT->BSHR = LCD_BLON_PIN;
    }
    else
    {
        /* Low Level , Switch OFF Blacklight */
        LCD_BLON_PORT->BCR = LCD_BLON_PIN;
    }
}
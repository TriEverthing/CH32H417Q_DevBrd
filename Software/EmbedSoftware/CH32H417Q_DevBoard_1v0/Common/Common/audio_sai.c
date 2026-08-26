#include "audio_sai.h"
#include "hardware.h"
#include "math.h"

#define PI              3.1415926535f
#define TAU             (2.0f * PI)
#define SAMPLE_RATE     (48000) //48KHz
#define DATA_SIZE       SAMPLE_RATE
int32_t SAI_Data[DATA_SIZE];

/*********************************************************************
 * @fn      Audio_SAI_Init
 * 
 * @brief   Configure the SAI.
 * 
 * @param   SampleRate - the sampling rate of the SAI.
 * 
 * @return  none
 */
void Audio_SAI_Init(void)
{
    GPIO_InitTypeDef     GPIO_InitStructure     = {0};
    SAI_InitTypeDef      SAI_InitStructure      = {0};
    SAI_FrameInitTypeDef SAI_FrameInitStructure = {0};
    SAI_SlotInitTypeDef  SAI_SlotInitStructure  = {0};
    RCC_ClocksTypeDef    RCC_ClocksStatus       = {0};
    DMA_InitTypeDef      DMA_InitStructure      = {0};

    // Generate the sine wave data
    // The sine wave frequency is 440Hz(A4) and the sampling rate is 48000Hz.
    for (int i = 0; i < DATA_SIZE; i += 2)
    {
        float t         = ((float)i) / ((float)SAMPLE_RATE);
        SAI_Data[i]     = (int32_t)((float)(0x7FFFFF) * sinf(440.0f * TAU * t));
        SAI_Data[i + 1] = SAI_Data[i];
    }

    /* Enables High Speed HB2 peripheral clock. */
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC | RCC_HB2Periph_GPIOE, ENABLE);
	
    // FS_A PC3(AF7)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF7);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // SCK_A PC2(AF7)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF7);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // SD_A PB2(AF6)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource2, GPIO_AF6);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // MCLK_A PE2(AF6)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource2, GPIO_AF6);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* Enables the High Speed HB2 SAI peripheral clock. */
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_SAI, ENABLE);

    /* The SAI Clock configuration is calculated as follows:
    SAI_CK_x  = SysClk / 6
    MCLK_x = SAI_CK_x / MCKDIV[5:0] with MCLK_x = 256 * FS
    FS = SAI_CK_x / (MCKDIV[5:0] * 256)
    MCKDIV[5:0] = (SysClk / 6) / (FS * 256) */

    /* The result of this function could be not correct when using fractional value for HSE crystal. */
    RCC_GetClocksFreq(&RCC_ClocksStatus);
    /* Initializes the SAI Block x peripheral according to the specified parameters in the SAI_InitStruct. */
    const uint32_t tmpdiv = ((RCC_ClocksStatus.SYSCLK_Frequency / 6 * 10 ) / (SAMPLE_RATE * 256) + 5 ) / 10;
    SAI_InitStructure.SAI_MasterDivider = tmpdiv;
    SAI_InitStructure.SAI_NoDivider     = SAI_MasterDivider_Enabled;
    SAI_InitStructure.SAI_AudioMode     = SAI_Mode_MasterTx;
    SAI_InitStructure.SAI_Protocol      = SAI_Free_Protocol;
    SAI_InitStructure.SAI_DataSize      = SAI_DataSize_24b;
    SAI_InitStructure.SAI_FirstBit      = SAI_FirstBit_MSB;
    SAI_InitStructure.SAI_ClockStrobing = SAI_ClockStrobing_RisingEdge;
    SAI_InitStructure.SAI_Synchro       = SAI_Asynchronous;
    SAI_InitStructure.SAI_FIFOThreshold = SAI_Threshold_FIFOEmpty;
    SAI_Init(SAI_Block_A, &SAI_InitStructure);
    /* Initializes the SAI Block Audio frame according to the specified parameters in the SAI_FrameInitStruct. */
    SAI_FrameInitStructure.SAI_FrameLength       = 64;
    SAI_FrameInitStructure.SAI_ActiveFrameLength = 24;
    SAI_FrameInitStructure.SAI_FSDefinition      = I2S_FS_ChannelIdentification;
    SAI_FrameInitStructure.SAI_FSPolarity        = SAI_FS_ActiveLow;
    SAI_FrameInitStructure.SAI_FSOffset          = SAI_FS_BeforeFirstBit;
    SAI_FrameInit(SAI_Block_A, &SAI_FrameInitStructure);
    /*  Initializes the SAI Block audio Slot according to the specified parameters in the SAI_SlotInitStruct. */
    SAI_SlotInitStructure.SAI_FirstBitOffset = 0;
    SAI_SlotInitStructure.SAI_SlotSize       = SAI_SlotSize_32b;
    SAI_SlotInitStructure.SAI_SlotNumber     = 2;
    SAI_SlotInitStructure.SAI_SlotActive     = SAI_SlotActive_0|SAI_SlotActive_1;
    SAI_SlotInit(SAI_Block_A, &SAI_SlotInitStructure);
    
    /* Reinitialize the FIFO pointer */
    SAI_FlushFIFO(SAI_Block_A);

    RCC_HBPeriphClockCmd(SAI_RCC_DMA, ENABLE);

    DMA_DeInit(SAI_DMA_TX_CH);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&SAI_Block_A->DATAR);
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)SAI_Data;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = DATA_SIZE;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(SAI_DMA_TX_CH, &DMA_InitStructure);

    DMA_MuxChannelConfig(SAI_DMA_TX_CH_MUX, SAI_DMA_TX_CH_REQ);

    DMA_Cmd(SAI_DMA_TX_CH, ENABLE);
    SAI_DMACmd(SAI_Block_A, ENABLE);
    
    SAI_Cmd(SAI_Block_A, ENABLE);
}
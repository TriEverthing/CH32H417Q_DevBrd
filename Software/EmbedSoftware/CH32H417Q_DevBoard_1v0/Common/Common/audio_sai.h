#ifndef _AUDIO_SAI_H_
#define _AUDIO_SAI_H_

/* includes -----------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
/* private includes -------------------------------------------------------------*/

/* exported types -------------------------------------------------------------*/

/* exported constants --------------------------------------------------------*/

/* exported macro ------------------------------------------------------------*/
/* SAI DMA definition */
#define SAI_DMA                         (DMA1)
#define SAI_RCC_DMA                     (RCC_HBPeriph_DMA1)
/* SAI TX Channel */
#define SAI_DMA_TX_CH                   (DMA1_Channel4)
#define SAI_DMA_TX_CH_MUX               (DMA_MuxChannel4)//for DMA1_Channel1
#define SAI_DMA_TX_CH_REQ               (112)  //for SAI_A_TX
#define SAI_DMA_TX_CH_FLAG_TC           (DMA1_FLAG_TC4)
#define SAI_DMA_TX_CH_IT_TC             (DMA1_IT_TC4)
#define SAI_DMA_TX_CH_IRQ               (DMA1_Channel4_IRQn)
#define SAI_DMA_TX_CH_IRQHandler        DMA1_Channel4_IRQHandler

/* Exported define -----------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void Audio_SAI_Init(void);
/* External variables --------------------------------------------------------*/


#endif //_AUDIO_SAI_H_

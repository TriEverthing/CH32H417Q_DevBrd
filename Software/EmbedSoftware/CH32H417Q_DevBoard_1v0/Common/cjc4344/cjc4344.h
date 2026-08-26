#ifndef _CJC4344_H_
#define _CJC4344_H_

/* includes -----------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
/* private includes -------------------------------------------------------------*/

/* exported types -------------------------------------------------------------*/

/* exported constants --------------------------------------------------------*/

/* exported macro ------------------------------------------------------------*/
/* CJC4344 Register Addr and Bits Definition */
/* CJC4344 Power Management1 Register, Default=0xA0 */
#define CJC4344_PM1                                 0x00
/* Bits[7] VMID: Vmid divider enable, 0 = Vmid Disabled, 1 = Vmid Enable */
#define CJC4344_PM2_VMID_Mask                       0x80 

/* CJC4344 Power Management2 Register, Default=0x1B */
#define CJC4344_PM2                                 0x01
/* Bits[0] DACR: DAC Right. 0 = DAC Power down, 1 = DAC Power up */
#define CJC4344_PM2_DACR_Mask                       0x01
/* Bits[1] DACL: DAC Left. 0 = DAC Power down, 1 = DAC Power up */
#define CJC4344_PM2_DACL_Mask                       0x02
/* Bits[3] VOUTR: CTF Enable (VCC/2-centered Right Output), 0 = VOUT Disabled, 1 = VOUT Enalbe */
#define CJC4344_PM2_VOUTL_Mask                      0x80
/* Bits[4] VOUTL: CTF Enable (VCC/2-centered Left Output), 0 = VOUT Disabled, 1 = VOUT Enalbe */
#define CJC4344_PM2_VOUTR_Mask                      0x10

/* CJC4344 Digital Audio Interface Format Register, Default=0x0A */
#define CJC4344_Digital_Audio_Interface_Format      0x02
/* Bits[1:0] FORMAT1-0: Audio Data Format Select */
#define CJC4344_Format_Mask                         0x03
#define CJC4344_Format_DSP                          0x00
#define CJC4344_Format_I2S                          0x01
#define CJC4344_Format_Left_Justified               0x02
#define CJC4344_Format_Reserved                     0x03
/* Bits[3:2] WL1-0: Audio Data Word Length */
#define CJC4344_Data_Width_Mask                     0x0C
#define CJC4344_Data_Width_16Bits                   0x00
#define CJC4344_Data_Width_20Bits                   0x04
#define CJC4344_Data_Width_24Bits                   0x08
#define CJC4344_Data_Width_Reserved                 0x0C
/* Bits[4] LRP: right, left and I2S modes ¨C LRCLK polarity */
#define CJC4344_LRP_Mask                            0x10
/* Bits[5] LRSWAP: Left/Right channel swap */
#define CJC4344_LRSWAP_Mask                         0x20
/* Bits[7] BCLKINV: BCLK invert bit (for master and slave modes) */
#define CJC4344_BCLKINV_Mask                        0x80

/* CJC4344 DAC Control Register, Default=0x00 */
#define CJC4344_DAC_Control                         0x03
/* Bits[2:1] DEEMP1-0: De-emphasis Control */
#define CJC4344_DEEMP_Mask                          0x07
#define CJC4344_DEEMP_NoDeemphasis                  0x00
#define CJC4344_DEEMP_32kHz                         0x02
#define CJC4344_DEEMP_44p1kHz                       0x04
#define CJC4344_DEEMP_48kHz                         0x07

/* CJC4344 Additional Control Register, Default=0x24 */
#define CJC4344_Additional_Control                  0x04
/* Bits[0] GAINSE: Out gain select, 0 = 2VRMS, 1 = 3VRMS */
#define CJC4344_GAINSE_Mask                         0x01
/* Bits[2] OTD: Thermal Shutdown Enable */
#define CJC4344_OTD_Mask                            0x04
/* Bits[5] OTD: MUTE: Analog OUT MUTE, 0 = mute (signal active), 1 = no mute */
#define CJC4344_MUTE_Mask                           0x20
/* Bits[7] ClockAUTO: SYSTEM Clock discern, 0 : Auto discern MCLK(default), 1 : manual SET MCLK */
#define CJC4344_ClockAUTO_Mask                      0x80

/* CJC4344 Left Channel Digital Volume1 Register Default=0xFF */
#define CJC4344_Left_Channel_Digital_Volume1        0x05
/* Bits[7:0] LDACVOL7-0: Left DAC Digital Volume Control */
#define CJC4344_LDACVOL_Mask                        0xFF

/* CJC4344 Left Channel Digital Volume2 Register  Default=0x00 */
#define CJC4344_Left_Channel_Digital_Volume2        0x06
/* Bits[0] LDVU: Left DAC Volume Update 
 * 0 = Store LDACVOL in intermediate latch (no gain change)
 * 1 = Update left and right channel gains (left = LDACVOL, right = intermediate latch)
*/
#define CJC4344_LDVU_Mask                           0x01

/* CJC4344 Right Channel Digital Volume1 Register Default=0xFF */
#define CJC4344_Right_Channel_Digital_Volume1       0x07
/* Bits[7:0] LDACVOR7-0: Right DAC Digital Volume Control */
#define CJC4344_LDACVOR_Mask                        0xFF

/* CJC4344 Right Channel Digital Volume2 Register Default=0x00 */
#define CJC4344_Right_Channel_Digital_Volume2       0x08
/* Bits[0] RDVU: Right DAC Volume Update 
 * 0 = Store RDACVOL in intermediate latch (no gain change)
 * 1 = Update left and right channel gains (left = RDACVOL, right = intermediate latch)
*/
#define CJC4344_RDVU_Mask                           0x01

/* CJC4344 Clocking and Sample Rate Control Register Default=0x06 */
#define CJC4344_Clocking_Sample_Rate_Control        0x09
/* Bits[5:0] SR5-0: Sample Rate Control */
#define CJC4344_Sample_Rate_Mask                    0x3F
/* Bits[6] CLKDIV2: Master Clock Divide by 2, 0: MCLK is not divided, 1: MCLK is divided by 2 */
#define CJC4344_CLKDIV2_Mask                        0x40
/* Bits[7] CLKDIV4: Master Clock Divide by 4, 0: MCLK is not divided, 1: MCLK is divided by 4 */
#define CJC4344_CLKDIV4_Mask                        0x80

/* Exported define -----------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

/* External variables --------------------------------------------------------*/


#endif //_CJC4344_H_

/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016-2018, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2016-04-23
 */

#include <sfud.h>
#include <sfud_def.h>
#include <stdarg.h>
#include <stdio.h>

#include "hardware.h"

void sfud_log_info(const char *format, ...);
void sfud_log_debug(const char *file, const long line, const char *format, ...);

/**
 * @brief  none
 * @param  none
 * @return none
 */
static void qspi_lock(const sfud_spi *spi)
{
    //__disable_irq();
}

/**
 * @brief  none
 * @param  none
 * @return none
 */
static void qspi_unlock(const sfud_spi *spi)
{
    //__enable_irq();
}

/**
  * @brief  QSPI single line write data then read data
  * @param  none
  * @retval none
  */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf, size_t read_size , bool addr_in_4_byte)
{
    sfud_err result = SFUD_SUCCESS ;
    QSPI_ComConfig_InitTypeDef Cmdhandler = {0};
    uint8_t *ptr = (uint8_t *)write_buf ;
    size_t count = 0 ;
    uint32_t addr;
    uint32_t TimesOut;

    if (write_size)
    {
        SFUD_ASSERT(write_buf);
    }

    if (read_size)
    {
        SFUD_ASSERT(read_buf);
    }

    /* Instruction */
    Cmdhandler.QSPI_ComConfig_Ins = ptr[0] ;
    /* Instruction mode */
    Cmdhandler.QSPI_ComConfig_IMode = QSPI_ComConfig_IMode_1Line ;
    /* Address Mode */
    Cmdhandler.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_1Line ;
    Cmdhandler.QSPI_ComConfig_ADSize = QSPI_ComConfig_ADSize_24bit ;//Uint Bytes
    /* Alternate Byte */
    Cmdhandler.QSPI_ComConfig_ABSize = QSPI_ComConfig_ABSize_8bit;//Uint Bytes
    Cmdhandler.QSPI_ComConfig_ABMode = QSPI_ComConfig_ABMode_NoAlternateByte;
    /* DummyCycles */
    Cmdhandler.QSPI_ComConfig_DummyCycles = 0; //Uint Cycles
    /* Data mode */
    Cmdhandler.QSPI_ComConfig_DMode = QSPI_ComConfig_DMode_1Line ;
    /* Flash Mode: Indirect Read */
    Cmdhandler.QSPI_ComConfig_FMode = QSPI_ComConfig_FMode_Indirect_Read;
    /* Others Setting */
    Cmdhandler.QSPI_ComConfig_SIOOMode = QSPI_ComConfig_SIOOMode_Disable;

    count += 1 ; //Instruction append

    TimesOut = 0xFFFFFF;
    while(QSPI_GetFlagStatus(QSPI2, QSPI_FLAG_BUSY))
    {
        TimesOut--;
        if( TimesOut == 0 )
        {
            return SFUD_ERR_TIMEOUT;
        }
    }

    /* get address */
    if ( write_size == 1 )
    {
        /* no address stage */
        Cmdhandler.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_NoAddress ;
    }
    else if ( write_size >= 4 )
    {
        if( addr_in_4_byte && write_size >= 5 )
        {
            Cmdhandler.QSPI_ComConfig_ADSize = QSPI_ComConfig_ADSize_32bit ;
            /* address size is 4 Byte */
            addr = (ptr[1] << 24) | (ptr[2] << 16) | (ptr[3] << 8) | (ptr[4]) ;
            /* Sets the value of the Address */
            QSPI_SetAddress(QSPI2, addr);
            count += 4 ; //4Byte Address append
        }
        else
        {
            /* address size is 3 Byte */
            addr = (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]) ;
            /* Sets the value of the Address */
            QSPI_SetAddress(QSPI2, addr);
            count += 3 ;//3Byte Address append
        }
    }
    else
    {
        return SFUD_ERR_READ ;
    }

    if ( write_size && read_size ) /* Frist Write Data, and Read */
    {
        /* recv data */
        /* set dummy cycles */
        if ( count < write_size )
        {
            Cmdhandler.QSPI_ComConfig_DummyCycles = ( write_size - count ) * 8 ;
        }
        else
        {
            Cmdhandler.QSPI_ComConfig_DummyCycles = 0 ;
        }

        /* set recv size */
        /* Sets data number of Bytes to be transferred */
        QSPI_SetDataLength(QSPI2, read_size);
        /* Initializes the QSPI CCR according to the specified parameters in the QSPI_ComConfig_InitStruct. */
        QSPI_ComConfig_Init(QSPI2, &Cmdhandler);

        /* Start a single QSPI transfer. */
        QSPI_Start(QSPI2);

        /* read data */
        uint32_t i = 0;
        TimesOut = 0xFFFFFF;
        while (i < read_size)
        {
            /* FIFO vaild bytes more than FIFO's thread */
            if(QSPI_GetFlagStatus(QSPI2, QSPI_FLAG_FT))
            {
                /* Returns the most recent received 8bit data by the QSPI peripheral. */
                read_buf[i] = QSPI_ReceiveData8(QSPI2);
                i++;
                TimesOut = 0xFFFFFF;
            }

            TimesOut--;
            if( TimesOut == 0 )
            {
                result = SFUD_ERR_READ;
                break;
            }
        }
        /* Clears the QSPI flag. */
        QSPI_ClearFlag(QSPI2, QSPI_FLAG_FT);
        QSPI_ClearFlag(QSPI2, QSPI_FLAG_TC);
    }
    else /* Only Write Data */
    {
        /* Flash Mode: Indirect Write */
        Cmdhandler.QSPI_ComConfig_FMode = QSPI_ComConfig_FMode_Indirect_Write;
        /* set dummy cycles */
        Cmdhandler.QSPI_ComConfig_DummyCycles = 0 ;

        if (  write_size > count )
        {
            /* Sets data number of Bytes to be transferred */
            write_size = write_size - count;
            QSPI_SetDataLength(QSPI2, write_size);
            ptr += count ;
            
        }
        else
        {
            /* Sets number of Bytes to be transferred */
            QSPI_SetDataLength(QSPI2, 0);
            Cmdhandler.QSPI_ComConfig_DMode = QSPI_ComConfig_DMode_NoData;
            ptr = NULL ;
        }

        /* Initializes the QSPI CCR according to the specified parameters in the QSPI_ComConfig_InitStruct. */
        QSPI_ComConfig_Init(QSPI2, &Cmdhandler);

        /* Start a single QSPI transfer. */
        QSPI_Start(QSPI2);

        if( ptr != NULL )
        {
            uint32_t i = 0;
            TimesOut = 0xFFFFFF;
            while (i < write_size )
            {
                /* FIFO empty bytes more than FIFO's thread */
                if (QSPI_GetFlagStatus(QSPI2, QSPI_FLAG_FT))
                {
                    /* Transmits a 8bit Data through the QSPI peripheral. */
                    QSPI_SendData8(QSPI2, ptr[i]);
                    i++;
                    TimesOut = 0xFFFFFF;
                }

                TimesOut--;
                if( TimesOut == 0 )
                {
                    result = SFUD_ERR_WRITE;
                    break;
                }
            }
            /* Clears the QSPI flag. */
            QSPI_ClearFlag(QSPI2, QSPI_FLAG_FT);
            QSPI_ClearFlag(QSPI2, QSPI_FLAG_TC);
        }
    }

    return result;
}

#ifdef SFUD_USING_QSPI

/**
  * @brief  QSPI fast read data
  * @param  none
  * @retval none
  */
/**
 * QSPI fast read data
 */
static sfud_err qspi_read(const struct __sfud_spi *spi, uint32_t addr, sfud_qspi_read_cmd_format *qspi_read_cmd_format, uint8_t *read_buf, size_t read_size)
{
    uint32_t TimesOut;
    sfud_err result = SFUD_SUCCESS;
    QSPI_ComConfig_InitTypeDef Cmdhandler = {0};

    /* Alternate Byte */
    Cmdhandler.QSPI_ComConfig_ABSize = QSPI_ComConfig_ABSize_8bit;
    Cmdhandler.QSPI_ComConfig_ABMode = QSPI_ComConfig_ABMode_NoAlternateByte;
    /* Dummy Cycles */
    Cmdhandler.QSPI_ComConfig_DummyCycles = qspi_read_cmd_format->dummy_cycles;
    /* Others Setting */
    Cmdhandler.QSPI_ComConfig_FMode = QSPI_ComConfig_FMode_Indirect_Read;
    Cmdhandler.QSPI_ComConfig_SIOOMode = QSPI_ComConfig_SIOOMode_Disable;

    /* set cmd struct */
    Cmdhandler.QSPI_ComConfig_Ins = qspi_read_cmd_format->instruction;
    if(qspi_read_cmd_format->instruction_lines == 2)
    {
        Cmdhandler.QSPI_ComConfig_IMode = QSPI_ComConfig_IMode_2Line ;
    }
    else if(qspi_read_cmd_format->instruction_lines == 4)
    {
        Cmdhandler.QSPI_ComConfig_IMode = QSPI_ComConfig_IMode_4Line ;
    }
    else
    {
        Cmdhandler.QSPI_ComConfig_IMode = QSPI_ComConfig_IMode_1Line ;
    }

    if( qspi_read_cmd_format->address_size == 24 )
    {
        Cmdhandler.QSPI_ComConfig_ADSize = QSPI_ComConfig_ADSize_24bit ;
    }
    else if( qspi_read_cmd_format->address_size == 32 )
    {
        Cmdhandler.QSPI_ComConfig_ADSize = QSPI_ComConfig_ADSize_32bit ;
    }
    else
    {
        Cmdhandler.QSPI_ComConfig_ADSize = QSPI_ComConfig_ADSize_24bit ;
    }

    if(qspi_read_cmd_format->address_lines == 0)
    {
        Cmdhandler.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_NoAddress ;
    }
    else if(qspi_read_cmd_format->address_lines == 1)
    {
        Cmdhandler.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_1Line ;
    }
    else if(qspi_read_cmd_format->address_lines == 2)
    {
        Cmdhandler.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_2Line ;
    }
    else if(qspi_read_cmd_format->address_lines == 4)
    {
        Cmdhandler.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_4Line ;
    }

    if(qspi_read_cmd_format->data_lines == 0)
    {
        Cmdhandler.QSPI_ComConfig_DMode = QSPI_ComConfig_DMode_NoData ;
    }
    else if(qspi_read_cmd_format->data_lines == 1)
    {
        Cmdhandler.QSPI_ComConfig_DMode = QSPI_ComConfig_DMode_1Line ;
    }
    else if(qspi_read_cmd_format->data_lines == 2)
    {
        Cmdhandler.QSPI_ComConfig_DMode = QSPI_ComConfig_DMode_2Line ;
        /* Enables QSPI 4-line Mode. */
        QSPI_EnableQuad(QSPI2, ENABLE);
    }
    else if(qspi_read_cmd_format->data_lines == 4)
    {
        Cmdhandler.QSPI_ComConfig_DMode = QSPI_ComConfig_DMode_4Line ;
        /* Enables QSPI 4-line Mode. */
        QSPI_EnableQuad(QSPI2, ENABLE);
    }

    TimesOut = 0xFFFFFF;
    while(QSPI_GetFlagStatus(QSPI2, QSPI_FLAG_BUSY))
    {
        TimesOut--;
        if( TimesOut == 0 )
        {
            return SFUD_ERR_TIMEOUT;
        }
    }

    /* Sets the value of the Address */
    QSPI_SetAddress(QSPI2, addr);

    /* Sets number of Bytes to be transferred */
    QSPI_SetDataLength(QSPI2, read_size);

    /* Initializes the QSPI CCR according to the specified parameters in the QSPI_ComConfig_InitStruct. */
    QSPI_ComConfig_Init(QSPI2, &Cmdhandler);

    /* Start a single QSPI transfer. */
    QSPI_Start(QSPI2);

    /* read data */
    uint32_t i = 0;
    TimesOut = 0xFFFFFF;
    while(i < read_size)
    {
        /* Wait FIFO Threshold flag */
        if(QSPI_GetFlagStatus(QSPI2, QSPI_FLAG_FT))
        {
            /* Returns the most recent received 8bit data by the QSPI peripheral. */
            read_buf[i] = QSPI_ReceiveData8(QSPI2);
            i++;
            TimesOut = 0xFFFFFF;
        }

        TimesOut--;
        if( TimesOut == 0 )
        {
            result = SFUD_ERR_READ;
            break;
        }
    }
    /* Clears the QSPI flag. */
    QSPI_ClearFlag(QSPI2, QSPI_FLAG_FT);
    QSPI_ClearFlag(QSPI2, QSPI_FLAG_TC);

    /* Disables QSPI 4-line Mode. */
    QSPI_EnableQuad(QSPI2, DISABLE);

    return result;
}

#endif
/**
  * @brief  about 100 microsecond delay
  * @param  none
  * @retval none
  */
static void retry_delay_100us(void)
{
    Delay_Us(100);
}



/**
  * @brief  none
  * @param  none
  * @retval none
  */
sfud_err sfud_spi_port_init(sfud_flash *flash)
{
    sfud_err result = SFUD_SUCCESS;

    switch (flash->index)
    {
        case SFUD_W25_DEVICE_INDEX :
        {
            QuadSpi_Init();
            /* set the interfaces and data */
            flash->spi.wr = spi_write_read ;
#ifdef SFUD_USING_QSPI
            flash->spi.qspi_read = qspi_read ;
#endif
            flash->spi.lock = qspi_lock ;
            flash->spi.unlock = qspi_unlock ;
            flash->spi.user_data = NULL ;
            /* about 100 microsecond delay */
            flash->retry.delay = retry_delay_100us ;
            /* adout 60 seconds timeout */
            flash->retry.times = 60 * 10000 ;

            break;
        }
    }
    return result;
}

static char log_buf[256];

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 */
void sfud_log_debug(const char *file, const long line, const char *format, ...)
{
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    //printf("[SFUD](%s:%ld) ", file, line);
    printf("[SFUD](%s:%ld)", file, line);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\r\n",log_buf);
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfud_log_info(const char *format, ...)
{
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD]");
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\r\n",log_buf);
    va_end(args);
}

/**
 * SFUD demo for the first flash device test.
 *
 * @param addr flash start address
 * @param size test flash size
 * @param size test flash data buffer
 */
void sfud_demo(uint32_t addr, size_t size, uint8_t *data)
{
    sfud_err result = SFUD_SUCCESS;
    const sfud_flash *flash = sfud_get_device_table() + 0;
    size_t i;

    /* prepare write data */
    for (i = 0; i < size; i++)
    {
        data[i] = i;
    }
    /* erase test */
    result = sfud_erase(flash, addr, size);
    if (result == SFUD_SUCCESS)
    {
        printf("Erase the %s flash data finish. Start from 0x%08X, size is %u.\r\n", flash->name, addr,(uint32_t)size);
    }
    else
    {
    	printf("Erase the %s flash data failed.\r\n", flash->name);
        return;
    }

#if 0
    uint8_t cmd , status_1 , status_2 , status_3 ;
    //Flash Write Enable
    //cmd  = SFUD_CMD_WRITE_ENABLE ;
    //flash->spi.wr( &flash->spi, &cmd, 1 , NULL, 0, false);
    //Read Status Register-1
    cmd  = SFUD_CMD_READ_STATUS_REGISTER ;
    flash->spi.wr( &flash->spi, &cmd, 1 , &status_1, 1, false);
    //Read Status Register-2
    cmd  = 0x35 ;
    flash->spi.wr( &flash->spi, &cmd, 1 , &status_2, 1, false);
    //Read Status Register-3
    cmd  = 0x15 ;
    flash->spi.wr( &flash->spi, &cmd, 1 , &status_3, 1, false);
#if V3F_USE_EasyLogger
    elog_debug("SFUD","Status Register-1 : 0x%02X." , status_1 );
    elog_debug("SFUD","Status Register-2 : 0x%02X." , status_2 );
    elog_debug("SFUD","Status Register-3 : 0x%02X." , status_3 );
#else
    printf("Status Register-1 : 0x%02X.\r\n" , status_1 );
    printf("Status Register-2 : 0x%02X.\r\n" , status_2 );
    printf("Status Register-3 : 0x%02X.\r\n" , status_3 );
#endif
#endif

    /* write test */
    result = sfud_write(flash, addr, size, data);
    if (result == SFUD_SUCCESS)
    {
    	printf("Write the %s flash data finish. Start from 0x%08X, size is %u.\r\n", flash->name, (uint32_t)addr, (uint32_t)size);
    }
    else
    {
    	printf("Write the %s flash data failed.\r\n", flash->name);
        return;
    }

    /* read test */
    result = sfud_read(flash, addr, size, data);
    if (result == SFUD_SUCCESS)
    {
    	printf("Read the %s flash data success. Start from 0x%08X, size is %u. The data is:\r\n", flash->name, (uint32_t)addr,(uint32_t)size);
    	printf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
        for (i = 0; i < size; i++)
        {
            if (i % 16 == 0)
            {
            	printf("[%08X] ", (uint32_t)addr + i);
            }
            printf("%02X ", data[i]);
            if (((i + 1) % 16 == 0) || i == size - 1)
            {
            	printf("\r\n");
            }
        }
        printf("\r\n");
    }
    else
    {
    	printf("Read the %s flash data failed.\r\n", flash->name);
    }

    /* data check */
    for (i = 0; i < size; i++)
    {
        if (data[i] != i % 256)
        {
        	printf("Read and check write data has an error. Write the %s flash data failed.\r\n", flash->name);
            break;
        }
    }
    if (i == size)
    {
    	printf("The %s flash test is success.\r\n", flash->name);
    }
}

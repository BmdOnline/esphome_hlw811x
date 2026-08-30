/**
 **********************************************************************************
 * @file   HLW811x_platform_bk72xx.h
 * @author @BmdOnline using Claude Sonnet 5
 * @brief  A sample Platform dependent layer for HLW811x Driver - Beken BK72xx
 **********************************************************************************
 *
 * Copyright (c) 2024 Mahda Embedded System (MIT License)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **********************************************************************************
 */

/* Define to prevent recursive inclusion ----------------------------------------*/
#ifndef _HLW811X_PLATFORM_BK72XX_H_
#define _HLW811X_PLATFORM_BK72XX_H_

#ifdef USE_BK72XX

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ---------------------------------------------------------------------*/
#include "HLW811x_lib.h"
#include <stdint.h>

#if (HLW811X_CONFIG_SUPPORT_SPI)
/**
 * @brief  Specify IO Pins of ESP connected to HLX8112
 */

// Fréquence SPI par défaut
#ifndef HLW811X_SPI_RATE
#define HLW811X_SPI_RATE       1000000
#endif

#ifndef HLW811X_SPI_CLK_GPIO
#define HLW811X_SPI_CLK_GPIO   14  // Corresponding to P14
#endif

#ifndef HLW811X_SPI_MISO_GPIO
#define HLW811X_SPI_MISO_GPIO  17  // Corresponding to P17
#endif

#ifndef HLW811X_SPI_MOSI_GPIO
#define HLW811X_SPI_MOSI_GPIO  16  // Corresponding to P16
#endif

#ifndef HLW811X_SPI_CSN_GPIO
#define HLW811X_SPI_CSN_GPIO   9   // Corresponding to P9
#endif

#endif

#if (HLW811X_CONFIG_SUPPORT_UART)
#define HLW811X_UART_NUM        UART_NUM_1
#define HLW811X_UART_BAUD       0  // 0: 9600, 1: 19200, 2: 38400, other: invalid
#define HLW811X_UART_TX_GPIO    GPIO_NUM_2
#define HLW811X_UART_RX_GPIO    GPIO_NUM_36
#endif

/**
 ==================================================================================
                               ##### Functions #####
 ==================================================================================
 */

#if (HLW811X_CONFIG_SUPPORT_SPI)

void HLW811x_SPIconfig(int mosi_io_num, int miso_io_num, int sclk_io_num, int spics_io_num);

/**
 * @brief  Initialize platform device to communicate HLW811x through SPI.
 * @param  Handler: Pointer to handler
 * @retval None
 */
void
HLW811x_Platform_Init_SPI(HLW811x_Handler_t *Handler);
#endif

#if (HLW811X_CONFIG_SUPPORT_UART)
/**
 * @brief  Initialize platform device to communicate HLW811x through UART.
 * @param  Handler: Pointer to handler
 * @retval None
 */
void
HLW811x_Platform_Init_UART(HLW811x_Handler_t *Handler);
#endif

#ifdef __cplusplus
}
#endif

#endif //! USE_BK72XX
#endif //! _HLW811X_PLATFORM_BK72XX_H_

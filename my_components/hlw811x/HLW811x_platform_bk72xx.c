/**
 **********************************************************************************
 * @file   HLW811x_platform_bk72xx.c
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
 *
 * NOTE (2026): Neither ESPHome's own `spi` component nor LibreTiny's Arduino
 * `SPIClass` currently offer a working hardware SPI backend for the Beken
 * BK72xx family (LibreTiny's own TODO list still carries "SPI" as unfinished
 * for this platform, and the ESPHome `spi` component only has a clang-tidy
 * stub for LibreTiny, no real hardware transport). This port therefore
 * bit-bangs SPI on plain GPIOs using the Arduino-compatible core that
 * LibreTiny does fully support (pinMode / digitalWrite / digitalRead).
 *
 * A native, higher-performance alternative would be to call the underlying
 * Beken SDK driver directly (beken378/driver/spi, e.g. spi_pub.h, bundled by
 * ESPHome under .esphome/platformio/packages/framework-beken-bdk/), the same
 * way HLW811x_platform_esp32.c calls ESP-IDF's driver/spi_master.h directly
 * instead of Arduino's SPI.h. That path was not explored here; note it for
 * the later optimization pass.
 *
 **********************************************************************************
 */

#ifdef USE_BK72XX

/* Includes ---------------------------------------------------------------------*/
#include "HLW811x_platform_bk72xx.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <Arduino.h>

#ifdef __cplusplus
};
#endif

/* Private Variables ------------------------------------------------------------*/
#if (HLW811X_CONFIG_SUPPORT_SPI)

// Half-period delay (µs) derived from HLW811X_SPI_RATE, floored at 1µs.
static const uint32_t half_period_us = (500000UL / (HLW811X_SPI_RATE > 0 ? HLW811X_SPI_RATE : 200000UL)) > 0
                                    ? (500000UL / (HLW811X_SPI_RATE > 0 ? HLW811X_SPI_RATE : 200000UL))
                                    : 1;

typedef struct SPIconfig_s {
  int mosi_io_num;
  int miso_io_num;
  int sclk_io_num;
  int scsn_io_num;
} SPIconfig_t;

SPIconfig_t SPIconfig = {
  .mosi_io_num = HLW811X_SPI_MOSI_GPIO,
  .miso_io_num = HLW811X_SPI_MISO_GPIO,
  .sclk_io_num = HLW811X_SPI_CLK_GPIO,
  .scsn_io_num = HLW811X_SPI_CSN_GPIO
};

void HLW811x_SPIconfig(int mosi_io_num, int miso_io_num, int sclk_io_num, int scsn_io_num) {
  SPIconfig.mosi_io_num = mosi_io_num;
  SPIconfig.miso_io_num = miso_io_num;
  SPIconfig.sclk_io_num = sclk_io_num;
  SPIconfig.scsn_io_num = scsn_io_num;
}

#endif

/**
 ==================================================================================
                           ##### Private Functions #####
 ==================================================================================
 */

static int8_t
HLW811x_Platform_DelayMs(uint8_t Delay)
{
  delay(Delay);
  return 0;
}

#if (HLW811X_CONFIG_SUPPORT_SPI)

static int8_t
HLW811x_Platform_SPI_Init(void)
{
  pinMode(SPIconfig.scsn_io_num, OUTPUT);
  digitalWrite(SPIconfig.scsn_io_num, HIGH);

  pinMode(SPIconfig.sclk_io_num, OUTPUT);
  digitalWrite(SPIconfig.sclk_io_num, LOW);   // CPOL=0: clock idles low (mode 1)

  pinMode(SPIconfig.mosi_io_num, OUTPUT);
  digitalWrite(SPIconfig.mosi_io_num, LOW);

  pinMode(SPIconfig.miso_io_num, INPUT);

  return 0;
}

static int8_t
HLW811x_Platform_SPI_DeInit(void)
{
  pinMode(SPIconfig.sclk_io_num, INPUT);
  pinMode(SPIconfig.mosi_io_num, INPUT);
  pinMode(SPIconfig.miso_io_num, INPUT);
  pinMode(SPIconfig.scsn_io_num, INPUT);
  return 0;
}

// SPI mode 1 (CPOL=0, CPHA=1): data is presented on the rising edge
// of CLK and sampled just before the falling edge. MSB first.
static uint8_t
spi_bitbang_transfer_byte(uint8_t out_byte)
{
  uint8_t in_byte = 0;
  for (int8_t bit = 7; bit >= 0; bit--) {
    digitalWrite(SPIconfig.mosi_io_num, (out_byte >> bit) & 0x01);
    digitalWrite(SPIconfig.sclk_io_num, HIGH);
    delayMicroseconds(half_period_us);
    in_byte = (uint8_t) ((in_byte << 1) | (digitalRead(SPIconfig.miso_io_num) ? 1 : 0));
    digitalWrite(SPIconfig.sclk_io_num, LOW);
    delayMicroseconds(half_period_us);
  }
  return in_byte;
}

static int8_t
HLW811x_Platform_SPI_SendReceive(uint8_t *SendData,
                                  uint8_t *ReceiveData,
                                  uint8_t Len)
{
  for (uint8_t i = 0; i < Len; i++) {
    uint8_t tx = SendData ? SendData[i] : 0xFF;
    uint8_t rx = spi_bitbang_transfer_byte(tx);
    if (ReceiveData)
      ReceiveData[i] = rx;
  }
  return 0;
}

static int8_t
HLW811x_Platform_SPI_SetLevelSCSN(uint8_t Level)
{
  digitalWrite(SPIconfig.scsn_io_num, Level);
  return 0;
}
#endif

#if (HLW811X_CONFIG_SUPPORT_UART)
static int8_t
HLW811x_Platform_UART_Init(void)
{
  /*
  esp_err_t err = ESP_OK;
  uart_config_t uart_config = {0};

  uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
  uart_config.source_clk = UART_SCLK_APB;
  uart_config.data_bits = UART_DATA_8_BITS;
  uart_config.baud_rate = BaudRateTable[HLW811X_UART_BAUD];
  uart_config.parity = UART_PARITY_EVEN;
  uart_config.stop_bits = UART_STOP_BITS_1;

  // Configure UART parameters
  err = uart_param_config(HLW811X_UART_NUM, &uart_config);
  if (err != ESP_OK)
  {
    return -1;
  }

  // Set UART pins(TX, RX, RTS, CTS)
  err = uart_set_pin(HLW811X_UART_NUM,
                     HLW811X_UART_TX_GPIO, HLW811X_UART_RX_GPIO,
                     UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  if (err != ESP_OK)
  {
    return -2;
  }

  // Install UART driver using an event queue here
  err = uart_driver_install(HLW811X_UART_NUM, 256, 256,
                            0, NULL, 0);
  if (err != ESP_OK)
  {
    return -3;
  }
  */

  return 0;
}

static int8_t
HLW811x_Platform_UART_DeInit(void)
{
  /*
  uart_driver_delete(HLW811X_UART_NUM);
  */
  return 0;
}

static int8_t
HLW811x_Platform_UART_Send(uint8_t *Data, uint8_t Len)
{
  /*
  int Result = 0;

  Result = uart_write_bytes(HLW811X_UART_NUM, (const void *)Data, Len);
  if (Result == -1)
    return -1;
  else if (Result != Len)
    return -2;
  */

  return 0;
}

static int8_t
HLW811x_Platform_UART_Receive(uint8_t *Data, uint8_t Len)
{
  /*
  int Result = 0;

  Result = uart_read_bytes(HLW811X_UART_NUM, (void *)Data, Len, 10 / portTICK_PERIOD_MS);
  if (Result == -1)
    return -1;
  else if (Result != Len)
    return -2;
  */

  return 0;
}
#endif



/**
 ==================================================================================
                            ##### Public Functions #####
 ==================================================================================
 */

#if (HLW811X_CONFIG_SUPPORT_SPI)
/**
 * @brief  Initialize platform device to communicate HLW811x through SPI.
 * @param  Handler: Pointer to handler
 * @retval None
 */
void
HLW811x_Platform_Init_SPI(HLW811x_Handler_t *Handler)
{
  HLW811X_PLATFORM_LINK_INIT(Handler, HLW811x_Platform_SPI_Init);
  HLW811X_PLATFORM_LINK_DEINIT(Handler, HLW811x_Platform_SPI_DeInit);
  HLW811X_PLATFORM_LINK_DELAYMS(Handler, HLW811x_Platform_DelayMs);
  HLW811X_PLATFORM_LINK_SPI_SENDRECEIVE(Handler, HLW811x_Platform_SPI_SendReceive);
  HLW811X_PLATFORM_LINK_SPI_SETLEVELSCSN(Handler, HLW811x_Platform_SPI_SetLevelSCSN);
  HLW811X_PLATFORM_SET_COMMUNICATION(Handler, HLW811X_COMMUNICATION_SPI);
}
#endif

#if (HLW811X_CONFIG_SUPPORT_UART)
/**
 * @brief  Initialize platform device to communicate HLW811x through UART.
 * @param  Handler: Pointer to handler
 * @retval None
 */
void
HLW811x_Platform_Init_UART(HLW811x_Handler_t *Handler)
{
  HLW811X_PLATFORM_LINK_INIT(Handler, HLW811x_Platform_UART_Init);
  HLW811X_PLATFORM_LINK_DEINIT(Handler, HLW811x_Platform_UART_DeInit);
  HLW811X_PLATFORM_LINK_DELAYMS(Handler, HLW811x_Platform_DelayMs);
  HLW811X_PLATFORM_LINK_UART_SEND(Handler, HLW811x_Platform_UART_Send);
  HLW811X_PLATFORM_LINK_UART_RECEIVE(Handler, HLW811x_Platform_UART_Receive);
  HLW811X_PLATFORM_SET_COMMUNICATION(Handler, HLW811X_COMMUNICATION_UART);
}
#endif
#endif //! USE_BK72XX

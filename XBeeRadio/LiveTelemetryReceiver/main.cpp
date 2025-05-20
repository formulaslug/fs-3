/* mbed Microcontroller Library
 * Copyright (c) 2025 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"
#include "radio.hpp"

SPI spi(PA_7, PA_6, PA_5); // mosi, miso, sclk[, ss]
DigitalOut cs(PB_0);
// changed from default on PCB due to nucleo solder bridges - see Slack message
DigitalIn spi_attn(PB_1);

int main() {
  spi.format(8, 0);
  spi.frequency(1000000);
  spi.set_default_write_value(0x00);

  ThisThread::sleep_for(10ms);

  XBeeRadio radio(spi, cs, spi_attn);
  while (1) {
    ThisThread::sleep_for(10ms);

    printf("%d\n", radio.get_temp());
    // printf("%d\n", 27);
  }
}

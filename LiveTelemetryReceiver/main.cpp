/* mbed Microcontroller Library
 * Copyright (c) 2025 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

SPI spi(PA_7, PA_6, PA_5); // mosi, miso, sclk
DigitalOut cs(PB_0);

int main()
{
    cs.write(1);

    spi.format(8, 0);
    spi.frequency(1000000);

    cs.write(0);

    

    spi.write()

    cs.write(1);

    printf("Hello, Mbed!\n");
    return 0;
}

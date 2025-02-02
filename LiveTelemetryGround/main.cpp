/* mbed Microcontroller Library
 * Copyright (c) 2025 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

#define BUF_SIZE 1000

static BufferedSerial radio_serial(PA_9, PA_10);
static BufferedSerial usb_serial(USBTX, USBRX);
static DigitalOut led(LED1);

int main()
{
    radio_serial.set_baud(9600);
    radio_serial.set_format(8, BufferedSerial::None, 1);
    usb_serial.set_baud(9600);
    usb_serial.set_format(8, BufferedSerial::None, 1);

    /* printf("hello world!\n"); */

    char buf[BUF_SIZE] = {0};

    while (true) {
        if (uint32_t num = radio_serial.read(buf, sizeof(buf))) {
            led = !led;
            usb_serial.write(buf, num);
        }
    }

    return 0;

}

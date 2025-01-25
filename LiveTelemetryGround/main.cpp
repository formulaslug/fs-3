/* mbed Microcontroller Library
 * Copyright (c) 2025 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

#define BUF_SIZE 1000

static BufferedSerial radio_serial(A7, D12);
static BufferedSerial usb_serial(USBTX, USBRX);
static DigitalOut led(LED1);

int main()
{
    radio_serial.set_baud(9600);
    radio_serial.set_format(
        /* bits */ 8,
        /* parity */ BufferedSerial::None,
        /* stop bit */ 1
    );
    usb_serial.set_baud(9600);
    usb_serial.set_format(
        /* bits */ 8,
        /* parity */ BufferedSerial::None,
        /* stop bit */ 1
    );

    char w[] = "hello world\n";
    printf("%s", w);
    usb_serial.write(w, strlen(w));

    char buf[BUF_SIZE] = {0};

    while (true) {
        if (uint32_t num = radio_serial.read(buf, sizeof(buf))) {
            led = !led;
            usb_serial.write(buf, num);
        }
    }

    return 0;

}

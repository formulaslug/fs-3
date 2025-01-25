/* mbed Microcontroller Library
 * Copyright (c) 2025 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

#define BUF_SIZE 1000

static BufferedSerial radio_serial(D5, D4);
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

    char w[] = "hellwo woasda\n";
    printf("%s", w);
    usb_serial.write(w, strlen(w));

    char buf[BUF_SIZE] = {0};

    thread_sleep_for(2000);
    char at_command_mode[] = "+++";
    radio_serial.write(at_command_mode, sizeof(at_command_mode));
    thread_sleep_for(2000);


    while (true) {
        if (uint32_t num = usb_serial.read(buf, sizeof(buf))) {
            led = !led;
            /* usb_serial.write(buf, num); */
            radio_serial.write(buf, num);
        }
        
        if (uint32_t num = radio_serial.read(buf, sizeof(buf))) {
            usb_serial.write(buf, num);
        }
    }
    
    return 0;
}

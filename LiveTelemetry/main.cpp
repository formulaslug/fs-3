/* mbed Microcontroller Library
 * Copyright (c) 2025 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

#define BUF_SIZE 1000

static BufferedSerial radio_serial(PC_10, PC_11);
static BufferedSerial usb_serial(USBTX, USBRX);
static DigitalOut led(LED1);
static DigitalOut pb1(PB_1);

int main()
{
    radio_serial.set_baud(9600);
    radio_serial.set_format(8, BufferedSerial::None, 1);
    radio_serial.set_flow_control(BufferedSerial::RTSCTS, PB_14, PB_13);
    usb_serial.set_baud(9600);
    usb_serial.set_format(8, BufferedSerial::None, 1);

    // pb1.write(1);

    printf("hello world!\n");

    char buf[BUF_SIZE] = {0};

    // thread_sleep_for(2000);
    // char at_command_mode[] = "+++";
    // radio_serial.write(at_command_mode, sizeof(at_command_mode));
    // usb_serial.write(at_command_mode, sizeof(at_command_mode));
    // thread_sleep_for(2000);


    while (true) {
        if (usb_serial.readable()) {
            uint32_t num = usb_serial.read(buf, sizeof(buf));
            if (num) {
                led = !led;
                // usb_serial.write(buf, num);
                radio_serial.write(buf, num);
            }
        }

        if (radio_serial.readable()) {
            uint32_t num = radio_serial.read(buf, sizeof(buf));
            if (num) {
                usb_serial.write(buf, num);
            }
        }
    }

    return 0;

}

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
    printf("\n\n\n\n\n\n");
    int i = 0;
    while (1) {
        i++;

        char payload[] = {'T', 'x', 'D', 'a', 't', 'a'};
        int response = radio.transmit(0, payload, sizeof(payload), 52);

        switch (response) {
            case DELIVERY_SUCCESS:
                printf("Success!\n");
                break;
            case NO_ACK_FAILURE:
                printf("No ack received!\n");
                break;
        }
        printf("Response: %d\n", response);
        // printf("%d\n", radio.get_temp());
        // printf("%d\n", 27);
        ThisThread::sleep_for(9000ms);
    }
}

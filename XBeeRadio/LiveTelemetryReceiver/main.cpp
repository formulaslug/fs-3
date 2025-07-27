/* mbed Microcontroller Library
 * Copyright (c) 2025 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"
#include "stdio.h"
#include "radio.hpp"

SPI spi(PA_7, PA_6, PA_5); // mosi, miso, sclk[, ss]
DigitalOut cs(PB_0);
// changed from default on PCB due to nucleo solder bridges - see Slack message
DigitalIn spi_attn(PB_1);

int main() {
    spi.format(8, 0);
    spi.frequency(100000);
    spi.set_default_write_value(0x00);

    ThisThread::sleep_for(10ms);

    XBeeRadio radio(spi, cs, spi_attn);
    printf("HELLO WORLD\n\n\n\n\n\n");

    char network_identifier[] = {'E', 'n', 'd', ' ', 'D', 'e', 'v', 'i', 'c', 'e'};

    // radio.set_network_identifier(network_identifier, sizeof(network_identifier));

    // radio.baud_rate_set(0x08);
    // radio.set_repeat_transmissions(0);
    // radio.set_network_hops(0);
    
    printf("Temp: %d\n", radio.get_temp());
    radio.set_streaming_limit(3);
    // radio.baud_rate_set(2);
    //

    // printf("tEMP: %d\n", radio.get_temp());

    // while (1) {};

    int i = 0;
    while (1) {
        i++;

        // uint8_t payload[240] = {0};

        // for (unsigned int i = 0; i < sizeof(payload); i++) {
        //     payload[i] = 'M';
        // }

        uint8_t payload[350] = {};

        for (unsigned int i = 0; i < sizeof(payload); i++) {
            if (rand() > rand()) {
                payload[i] = 'X';
            } else {
                payload[i] = 'M';
            }
        }

        int response = radio.transmit(payload, sizeof(payload));

        switch (response) {
            case DELIVERY_SUCCESS:
                // printf("Success!\n");
                break;
            case NO_ACK_FAILURE:
                // printf("No ack received!\n");
                break;
        }

        // printf("Response: %d\n", response);
        // printf("%d\n", radio.get_temp());
        // printf("%d\n", 27);
        // ThisThread::sleep_for(9000ms);
    }

}

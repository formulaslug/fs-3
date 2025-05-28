/* mbed Microcontroller Library
 * Copyright (c) 2025 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"
#include "radio.hpp"
#include <cstdint>

DigitalIn spi_attn(PA_9);
DigitalOut cs(PC_8);
SPI spi(PA_7, PA_6, PA_5);

int main() {
    printf("Main() - LiveTelemetryMain");
    cs.write(1);

    spi.format(8, 0);
    spi.frequency(1000000);
    spi.set_default_write_value(0x00);

    XBeeRadio radio(spi, cs, spi_attn);
    ThisThread::sleep_for(200ms);

    // Turns out this wasn't even neccessary to receive data, also this causes seg fault
    // uint8_t parameters[] = { 0x00, 0x00, 0x00, 0x00 };
    // radio.send_at_command(DESTINATION_SET_LOW, parameters, sizeof(parameters), nullptr, 0);
    while (true) {

        if (spi_attn.read()) {
            continue;
        }
        uint8_t resp_buf[100] = {0};
        cs.write(0);
        spi.write(nullptr, 0, resp_buf, sizeof(resp_buf));
        cs.write(1);

        printf("Received data: ");
        for (uint i = 15; i < sizeof(resp_buf) - 15; i++) {
            printf("%c", resp_buf[i]);
        }

        printf("\nAttention:%d\n", spi_attn.read());
        printf("\n");
    }

    return 0;
}


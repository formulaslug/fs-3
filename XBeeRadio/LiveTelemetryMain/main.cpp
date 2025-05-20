/* mbed Microcontroller Library
 * Copyright (c) 2025 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"

DigitalIn spi_attn(PA_9);
DigitalOut cs(PC_8);
SPI spi(PA_7, PA_6, PA_5);

int main() {
  printf("Main() - LiveTelemetryMain");
  cs.write(1);

  spi.format(8, 0);
  spi.frequency(1000000);
  spi.set_default_write_value(0x00);

  ThisThread::sleep_for(200ms);
  while (true) {
    // Example directly out of datasheet
    const uint8_t at_command_TP[] = {
        0x7E, 0x00, 0x04, 0x08, 0x17, 0x54, 0x50, 0x3C, // AT command - "TP"
    };
    uint8_t resp_buf[11] = {0}; // TP returns an 11-byte frame

    // // Done manually by Jack
    // const uint8_t at_command_SL[] = {
    //     0x7E, // Start delimeter
    //     0x00, // LEN - MSB
    //     0x04, // LEN - LSB
    //
    //     // ---- BEGIN DATA ----
    //     0x08, // frametype
    //     0x77, // frameid
    //     // 0x53, 0x48, // AT command - "SH"
    //     0x53, 0x4C, // AT command - "SL"
    //     // (no parameter in this example)
    //     // ---- END DATA ----
    //
    //     (0xFF - ((0x08 + 0x77 + 0x53 + 0x4C) & 0xFF)), // Checksum
    //
    // };
    // uint8_t resp_buf[13] = {0}; // SH/SL returns an 11-byte frame

    // while (!spi_attn.read()) {
    //   printf("Waiting on spi_attn to assert...\n");
    // }

    // Write message and read response
    cs.write(0);
    spi.write(at_command_TP, sizeof(at_command_TP), resp_buf, sizeof(resp_buf));
    cs.write(1);

    // cs.write(0);
    // spi.write(nullptr, 0, resp_buf, sizeof(resp_buf));
    // cs.write(1);

    // spi.write(nullptr, 0, resp_buf, sizeof(resp_buf));
    printf("Buffer:");
    for (uint i = 0; i < sizeof(resp_buf); i++) {
      printf("%x ", resp_buf[i]);
    }
    printf("\nAttention:%d\n", spi_attn.read());
    printf("\n");
  }

  return 0;
}


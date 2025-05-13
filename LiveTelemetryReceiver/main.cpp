/* mbed Microcontroller Library
 * Copyright (c) 2025 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"

DigitalIn spi_attn(PB_1);
// PB_6 is shorted to PA_6 in the default SB configuration of L432KC
// PB 7 is shorted to PA_5 in the default SB configuration of L432KC
DigitalOut cs(PB_0);
SPI spi(PB_5, PB_4, PB_3); // mosi, miso, sclk[, ss]

int main() {
  printf("Main()");
  cs.write(1);

  spi.format(8, 0);
  spi.frequency(1000000);
  spi.set_default_write_value(0x00);

  ThisThread::sleep_for(200ms);
  // while ()
  while (true) {
    // ThisThread::sleep_for(10ms);
    const uint8_t local_at_command[] = {
      // Done manually by Jack
        // 0x7E, // Start delimeter
        // 0x00, // LEN - MSB
        // 0x04, // LEN - LSB
        //
        // // ---- BEGIN DATA ----
        // 0x08,       // frametype
        // 0x77,       // frameid
        // 0x53, 0x48, // AT command - "SH"
        // // (no parameter in this example)
        // // ---- END DATA ----
        //
        // (0xFF - ((0x08 + 0x77 + 0x53 + 0x48) & 0xFF)), // Checksum
      // Out of datasheet directly
        0x7E, 0x00, 0x04, 0x08, 0x17, 0x54, 0x50, 0x3C,
    };

    uint8_t resp_buf[100] = {0};


    // Write message and read response
    cs.write(0);
    spi.write(local_at_command, sizeof(local_at_command), resp_buf, sizeof(resp_buf));
    cs.write(1);

    // printf("%d, %s", spi_attn.read(), str);
    // Idea by wesley that didn't work
    // if (!spi_attn) {
    //   cs.write(0);
    //   spi.write(NULL, 0, (char *)resp_buf, sizeof(resp_buf));
    //   cs.write(1);
    // } else {
    //   cs.write(0);
    //   spi.write((char *)local_at_command, sizeof(local_at_command),
    //             (char *)resp_buf, sizeof(resp_buf));
    //   cs.write(1);
    // }

    // spi.write(nullptr, 0, resp_buf, sizeof(resp_buf));
    printf("Buffer:");
    for(int i = 0; i < 100; i++) {
      printf("%x ",resp_buf[i]);
    }
    printf("\nAttention:%d\n", spi_attn.read());
    printf("\n");


  }

  return 0;
}


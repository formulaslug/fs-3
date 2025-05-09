/* mbed Microcontroller Library
 * Copyright (c) 2025 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

DigitalIn spi_attn(PB_6);
DigitalOut cs(PB_0);
SPI spi(PA_7, PA_6, PA_5); // mosi, miso, sclk[, ss]

// DigitalOut reset(PA_4);
DigitalOut dout(PA_3);

// DigitalOut orange(A0);
// DigitalOut red(A1);
// BufferedSerial test_serial(orange, red);

char str[100] = {0};

int main() {
  cs.write(1);

  

  // spi.format(8, 0);
  // spi.frequency(1000000);
  // spi.set_default_write_value(0x00);

  printf("%d", spi_attn.read());
  dout = 0;
  // reset = 0;
  ThisThread::sleep_for(1s);
  // reset = 1;

  while (true) {
    const uint8_t local_at_command[] = {
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
        0x7E, 0x00, 0x04, 0x08, 0x17, 0x54, 0x50, 0x3C,
    };

    uint8_t resp_buf[10] = {0};

    cs.write(0);
    spi.write(local_at_command, sizeof(local_at_command), resp_buf, sizeof(resp_buf));
    // printf("%d, %s", spi_attn.read(), str);

    cs.write(1);


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

    printf("%x, %d", *resp_buf, spi_attn.read());
    printf("\n");
  }

  return 0;
}


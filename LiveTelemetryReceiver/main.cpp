/* mbed Microcontroller Library
 * Copyright (c) 2025 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

DigitalOut cs(PB_0);
SPI spi(PA_7, PA_6, PA_5); // mosi, miso, sclk[, ss]

bool to_hex(char *dest, size_t dest_len, const uint8_t *values,
            size_t val_len) {
  if (dest_len < (val_len * 2 + 1)) /* check that dest is large enough */
    return false;
  *dest = '\0'; /* in case val_len==0 */
  while (val_len--) {
    /* sprintf directly to where dest points */
    sprintf(dest, "%02X", *values);
    dest += 2;
    ++values;
  }
  return true;
}

int main() {
  // cs.write(1);

  spi.format(8, 0);
  spi.frequency(1000000);

  while (1) {
    // HAL_GPIO_WritePin(GPIOC, 8, 1);
    const uint8_t local_at_command[] = {
        0x7E, // Start delimeter
        0x00, // LEN - MSB
        0x04, // LEN - LSB

        // ---- BEGIN DATA ----
        0x08,       // frametype
        0x77,       // frameid
        0x53, 0x48, // AT command - "SH"
        // (no parameter in this example)
        // ---- END DATA ----

        (0xFF - ((0x08 + 0x77 + 0x53 + 0x48) & 0xFF)), // Checksum
        // 0x7E, 0x00, 0x04, 0x08, 0x17, 0x54, 0x50, 0x3C,
    };

    cs.write(0);

    uint8_t resp_buf[1024] = {0};

    spi.write((char *)local_at_command, sizeof(local_at_command),
              (char *)resp_buf, sizeof(resp_buf));
    // spi.transfer(const Type *tx_buffer, int tx_length, Type *rx_buffer, int rx_length, const event_callback_t &callback)

    cs.write(1);

    HAL_Delay(10);
    char str[1024] = {0};
    to_hex(str, sizeof(str), resp_buf, sizeof(resp_buf));

    printf("0x%s", str);
    printf("\n");
  }

  return 0;
}

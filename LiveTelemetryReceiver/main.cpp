/* mbed Microcontroller Library
 * Copyright (c) 2025 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

DigitalIn spi_attn(PB_6);
DigitalOut cs(PB_0);
SPI spi(PA_7, PA_6, PA_5); // mosi, miso, sclk[, ss]

DigitalOut reset(PA_4);
DigitalOut dout(PA_3);

DigitalOut orange(A0);
DigitalOut red(A1);
BufferedSerial test_serial(orange, red);


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

char str[100] = {0};

int main() {
  cs.write(1);

  

  // spi.format(8, 0);
  // spi.frequency(1000000);
  // spi.set_default_write_value(0x00);

  printf("%d", spi_attn.read());
  dout = 0;
  reset = 0;
  ThisThread::sleep_for(1s);
  reset = 1;

  // while (1) {
  //   // uint8_t resp_buf[10] = {0};
  //   // char str[15] = {0};
  //   // cs.write(0);
  //   // spi.write(NULL, 0, (char *)resp_buf, sizeof(resp_buf));
  //   // cs.write(1);
  //   // to_hex(str, 15, resp_buf, 10);
  //   // printf("%d, %s", spi_attn.read(), str);
  //
  //   // HAL_GPIO_WritePin(GPIOC, 8, 1);
  //   const uint8_t local_at_command[] = {
  //       // 0x7E, // Start delimeter
  //       // 0x00, // LEN - MSB
  //       // 0x04, // LEN - LSB
  //       //
  //       // // ---- BEGIN DATA ----
  //       // 0x08,       // frametype
  //       // 0x77,       // frameid
  //       // 0x53, 0x48, // AT command - "SH"
  //       // // (no parameter in this example)
  //       // // ---- END DATA ----
  //       //
  //       // (0xFF - ((0x08 + 0x77 + 0x53 + 0x48) & 0xFF)), // Checksum
  //       0x7E, 0x00, 0x04, 0x08, 0x17, 0x54, 0x50, 0x3C,
  //   };
  //
  //   uint8_t resp_buf[10] = {0};
  //
  //   // cs.write(0);
  //
  //   // spi.write((char *)local_at_command, sizeof(local_at_command),
  //   // printf("%d, %s", spi_attn.read(), str);
  //
  //   //           (char *)resp_buf, sizeof(resp_buf));
  //
  //   // spi.transfer(local_at_command, sizeof(local_at_command), resp_buf,
  //   // sizeof(resp_buf), [&](int _) {
  //   //   to_hex(str, sizeof(str), resp_buf, sizeof(resp_buf));
  //   // });
  //   //
  //   // while (spi_attn.read()==1) {}
  //   // cs.write(1);
  //   //
  //
  //   if (!spi_attn) {
  //     cs.write(0);
  //     spi.write(NULL, 0, (char *)resp_buf, sizeof(resp_buf));
  //     cs.write(1);
  //   } else {
  //     cs.write(0);
  //     spi.write((char *)local_at_command, sizeof(local_at_command),
  //               (char *)resp_buf, sizeof(resp_buf));
  //     cs.write(1);
  //   }
  //
  //   HAL_Delay(10);
  //   char str[1024] = {0};
  //   to_hex(str, sizeof(str), resp_buf, sizeof(resp_buf));
  //
  //   printf("0x%s, %d", str, spi_attn.read());
  //   printf("\n");
  // }

  return 0;
}

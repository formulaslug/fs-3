#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <stdbool.h>

bool to_hex(char* dest, size_t dest_len, const uint8_t* values, size_t val_len) {
    if(dest_len < (val_len*2+1)) /* check that dest is large enough */
        return false;
    *dest = '\0'; /* in case val_len==0 */
    while(val_len--) {
        /* sprintf directly to where dest points */
        sprintf(dest, "%02X", *values);
        dest += 2;
        ++values;
    }
    return true;
}


int app_main(SPI_HandleTypeDef hspi1, UART_HandleTypeDef huart2) {

  while (1) {
    // HAL_GPIO_WritePin(GPIOC, 8, 1);
    const uint8_t local_at_command[] = {
      0x7E, // Start delimeter
      0x00,  // LEN - MSB
      0x08,  // LEN - LSB

      // ---- BEGIN DATA ----
      0x08, // frametype
      0x77, // frameid
      0x41, 0x41, // AT command - "AA"
      // (no parameter in this example)
      // ---- END DATA ----

      (0xFF - ((0x08 + 0x01 + 0x41 + 0x41) & 0xFF)) // Checksum
    };
    
    // uint8_t frame[] = {
    //   0x7E, // Start delimeter
    //   0x??,  // LEN - MSB
    //   0x??,  // LEN - LSB
    //
    //   // ---- BEGIN DATA ----
    //   0x10, // Frametype (0x10: transmit request)
    //   0x52, // FrameID   (to associate with a response frame)
    //   0x000000000000FFFF, // 64-bit dest (0x000000000000FFFF: bcast)
    //   0xFFFE, // Reserved
    //   0x00, // Bcastradius  (0x00: use NH option instead - recommended)
    //   0x00, // Transmit Options
    //   0x547844617461, // RFdata
    //   // ---- END DATA ----
    //
    //   0x?? // Checksum
    // };


    // CHIP SELECT LOW

    // HAL_SPI_Transmit(&hspi1, frame, sizeof(frame), 100);
    uint8_t resp_buf[1024] = {0};
    HAL_SPI_TransmitReceive(&hspi1, local_at_command, resp_buf, sizeof(local_at_command), 100);

    HAL_Delay(1000);
    // uint8_t buf[] = "Hello World!\r\n";
    // HAL_UART_Transmit(&huart2, buf, sizeof(buf), 100);
    char str[1024] = {0};
    // to_hex(str, sizeof(str), resp_buf, sizeof(resp_buf));

    HAL_UART_Transmit(&huart2, (uint8_t*)resp_buf, sizeof(resp_buf), 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 100);
  }
}

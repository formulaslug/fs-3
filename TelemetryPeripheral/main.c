/**
 * @file main.c
 * @author jack
 * @date 2025-02-22
 * @brief Main function
 */

#include "mcc_generated_files/mcc.h"
#include <avr/io.h>


int main() {

  // Add your code here and press Ctrl + Shift + B to build
  SYSTEM_Initialize();

  IO_PC3_SetHigh();

  while (true) {

    SPI0_OpenConfiguration(MASTER0_CONFIG);

    IO_PC3_SetLow();
    // SPI0.INTFLAGS;
    char buf[] = {0xA0, 0x00, 0x00, 0x00};
    SPI0_ExchangeBlock(buf, sizeof(buf));
    // SPI0_WriteByte(0xA0);
    // SPI0_WaitDataready();
    // const uint8_t recv = SPI0_ReadByte();
    // SPI0_WaitDataready();
    // const uint8_t recv2 = SPI0_ReadByte();

    IO_PC3_SetHigh();

    SPI0_Close();

  }

  return 0;
};

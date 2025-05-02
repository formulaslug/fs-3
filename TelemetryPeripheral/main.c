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

  DELAY_milliseconds(100);

  SPI0_OpenConfiguration(MASTER0_CONFIG);

  IO_PC3_SetLow();

  char buf[] = {0b00000010, 0x28, 0b0000001, 0b10000000, 0b00000100 };
  SPI0_WriteBlock(buf, sizeof(buf));

  IO_PC3_SetHigh();

  // IO_PC3_SetLow();
  //
  // char buf2[] = {0b00000010, 0x0F, 0x00 };
  // SPI0_WriteBlock(buf2, sizeof(buf2));
  //
  // IO_PC3_SetHigh();

  SPI0_Close();

  DELAY_milliseconds(500);

  while (true) {

    SPI0_OpenConfiguration(MASTER0_CONFIG);

    IO_PC3_SetLow();

    // clang-format: ignore
    //            WRITE       ADDR        CTRL        SIDH        SIDL        EID8  EID0  DLC         DATA                                           
    char buf[] = {0b00000010, 0b00110000, 0b00001011, 0b01010101, 0b01000000, 0x00, 0x00, 0b00001000, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x00};
    SPI0_WriteBlock(buf, sizeof(buf));

    IO_PC3_SetHigh();

    // IO_PC3_SetLow();
    //
    // SPI0_WriteByte(0x81);
    //
    // IO_PC3_SetHigh();

    SPI0_Close();

    DELAY_milliseconds(100);

  }

  return 0;
};

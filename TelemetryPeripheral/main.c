/**
 * @file main.c
 * @author jack
 * @date 2025-02-22
 * @brief Main function
 */

#include "mcc_generated_files/mcc.h"
#include <avr/io.h>
#include <math.h>

// #include "MCP_CAN_lib-master/mcp_can.hpp"


int main() {

  // Add your code here and press Ctrl + Shift + B to build
  SYSTEM_Initialize();

  DELAY_milliseconds(100);

  SPI0_OpenConfiguration(MASTER0_CONFIG);
  IO_PC3_SetLow();
  char clear[] = {0b00000010, 0b00110000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  SPI0_WriteBlock(clear, sizeof(clear));
  IO_PC3_SetHigh();
  SPI0_Close();

  DELAY_milliseconds(10);

  SPI0_OpenConfiguration(MASTER0_CONFIG);
  IO_PC3_SetLow();

  //             WRITE       ADDR  CNF3        CNF2        CNF1
  char buf[] = { 0b00000010, 0x28, 0b00000001, 0b10000000, 0b00000100 }; // from our old peripheral board revision (math done with Cole)
  // char buf[] = { 0b00000010, 0x28, 0x81, 0xD1, 0x00 }; // from the arduino mcp_can library
  // char buf[] = { 0b00000010, 0x28, 0b00000001, 0b10001010, 0b00000000 }; // new code
  SPI0_WriteBlock(buf, sizeof(buf));
  IO_PC3_SetHigh();

  // IO_PC3_SetLow();
  //
  // char buf2[] = {0b00000010, 0x0F, 0x00 };
  // SPI0_WriteBlock(buf2, sizeof(buf2));
  //
  // IO_PC3_SetHigh();

  SPI0_Close();

  ADC1_Enable();
  // ADC1_StartConversion(ADC_MUXPOS_AIN5_gc);

  DELAY_milliseconds(500);

  while (true) {
    const uint16_t voltage = ADC1_GetConversion(ADC_MUXPOS_AIN0_gc) * pow(2, 16 - 10); // ADC_MUXPOS_AIN5_gc

    SPI0_OpenConfiguration(MASTER0_CONFIG);
    IO_PC3_SetLow();

    // clang-format: ignore
    //            WRITE       ADDR        CTRL        SIDH        SIDL        EID8  EID0  DLC         DATA                                           
    // char buf[] = {0b00000010, 0b00110000, 0b00001011, 0b10101010, 0b01000000, 0x00, 0x00, 0b00001000, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x00};
    char buf[] = {0b00000010, 0b00110000, 0b00001011, 0b10101010, 0b01000000, 0x00, 0x00, 0b00000010, voltage >> 8, voltage & 0xFF, 0x00, 0x00};
    SPI0_WriteBlock(buf, sizeof(buf));

    IO_PC3_SetHigh();
    SPI0_Close();

    DELAY_milliseconds(100);

  }

  return 0;
};

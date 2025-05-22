/**
 * @file main.c
 * @author jack
 * @date 2025-02-22
 * @brief Main function
 */

#include "mcc_generated_files/delay.h"
#include "mcc_generated_files/mcc.h"
#include <avr/io.h>
#include <math.h>

#include "d6t-8lh.h"

// #include "MCP_CAN_lib-master/mcp_can.hpp"


int main() {

  // Add your code here and press Ctrl + Shift + B to build
  SYSTEM_Initialize();

  DELAY_milliseconds(100);

  d6t_8lh_setup();

  SPI0_OpenConfiguration(MASTER0_CONFIG);

  IO_PA4_SetLow();
  char bufreset[] = { 0b11000000}; // newer code
  SPI0_WriteBlock(bufreset, sizeof(bufreset));
  IO_PA4_SetHigh();
  DELAY_milliseconds(10);

  IO_PA4_SetLow();
  char clear[] = {0b00000010, 0b00110000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  SPI0_WriteBlock(clear, sizeof(clear));
  IO_PA4_SetHigh();
  // SPI0_Close();

  DELAY_milliseconds(100);

  // SPI0_OpenConfiguration(MASTER0_CONFIG);
  IO_PA4_SetLow();

  // char buf[] = { 0b00000010, 0x28, 0b00000001, 0b10000000, 0b00000100 }; // from our old peripheral board revision (math done with Cole)
  // char buf[] = { 0b00000010, 0x28, 0x81, 0xD1, 0x00 }; // from the arduino mcp_can library
  //             WRITE       ADDR  CNF3        CNF2        CNF1
  // char buf[] = { 0b00000010, 0x28, 0b00000001, 0b10001001, 0b00000000 }; // new code

  //             WRITE       ADDR  CNF3        CNF2        CNF1
  char buf[] = { 0b00000010, 0x28, 0b00000001, 0b10010001, 0b00000000 }; // newer code
  SPI0_WriteBlock(buf, sizeof(buf));
  IO_PA4_SetHigh();

  // IO_PA4_SetLow();
  //
  // char buf2[] = {0b00000010, 0x0F, 0x00 };
  // SPI0_WriteBlock(buf2, sizeof(buf2));
  //
  // IO_PA4_SetHigh();

  // SPI0_Close();

  // ADC1_Enable();
  // ADC1_StartConversion(ADC_MUXPOS_AIN5_gc); // not needed if using ADC1_GetConversion

  DELAY_milliseconds(500);

  while (true) {
    // const uint16_t voltage = ADC1_GetConversion(ADC_MUXPOS_AIN0_gc) * pow(2, 16 - 10); // ADC_MUXPOS_AIN5_gc
    // I2C0_example_writeNBytes()
    // I2C0_SetAddress(0x0A << 1 );
    // char i2c_buf[1] = {0x4C};
    // // I2C0_SetBuffer(i2c_buf, 1);
    // I2C0_MasterWrite();
    // I2C0_SetBuffer(i2c_buf, 1);
    // I2C0_MasterRead();

    double degC = d6t_8lh_loop();

    IO_PA4_SetLow();

    // clang-format: ignore
    //                     WRITE       ADDR        CTRL        SIDH        SIDL[7:5]   EID8  EID0  DLC         DATA                                            CAN          
    char buf_deadbeef[] = {0b00000010, 0b00110000, 0b00001000, 0b10101010, 0b01000000, 0x00, 0x00, 0b00001000, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x00};
    // Should check that CTRL[3] is 0 before moving on (message sent)
    // char buf_adc[] = {0b00000010, 0b00110000, 0b00001011, 0b10101010, 0b01000000, 0x00, 0x00, 0b00000010, voltage >> 8, voltage & 0xFF, 0x00, 0x00};
    SPI0_WriteBlock(buf_deadbeef, sizeof(buf_deadbeef));

    char buf_degC[] = {0b00000010, 0b00110000, 0b00001011, 0b10101010, 0b01000000, 0x00, 0x00, 0b00000010, (long)degC >> 8, (long)degC & 0x0F};
    SPI0_WriteBlock(buf_degC, sizeof(buf_deadbeef));

    IO_PA4_SetHigh();

    DELAY_milliseconds(100);

  }

  SPI0_Close();

  return 0;
};

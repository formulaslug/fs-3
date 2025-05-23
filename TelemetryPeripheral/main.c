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

    // d6t_8lh_setup();

    SPI0_OpenConfiguration(MASTER0_CONFIG);

    // RESET instruction. Resets registers and also enters configuration mode
    char reset[] = {0b11000000};
    IO_PA4_SetLow();
    SPI0_WriteBlock(reset, sizeof(reset));
    IO_PA4_SetHigh();
    DELAY_milliseconds(10);

    // clang-format off
    char clear_tx0[] = {0b00000010, 0b00110000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    char clear_tx1[] = {0b00000010, 0b01000000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    char clear_tx2[] = {0b00000010, 0b01010000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    char clear_rxctrl1[] = {0b00000010, 0b01100000, 0x00};
    char clear_rxctrl2[] = {0b00000010, 0b01110000, 0x00};
    // clang-format on
    IO_PA4_SetLow();
    SPI0_WriteBlock(clear_tx0, sizeof(clear_tx0));
    IO_PA4_SetHigh();
    IO_PA4_SetLow();
    SPI0_WriteBlock(clear_tx1, sizeof(clear_tx1));
    IO_PA4_SetHigh();
    IO_PA4_SetLow();
    SPI0_WriteBlock(clear_tx2, sizeof(clear_tx2));
    IO_PA4_SetHigh();
    IO_PA4_SetLow();
    SPI0_WriteBlock(clear_rxctrl1, sizeof(clear_rxctrl1));
    IO_PA4_SetHigh();
    IO_PA4_SetLow();
    SPI0_WriteBlock(clear_rxctrl2, sizeof(clear_rxctrl2));
    IO_PA4_SetHigh();
    // SPI0_Close();

    DELAY_milliseconds(100);

    // clang-format off
    // //             WRITE       ADDR  CNF3        CNF2        CNF1
    // char buf[] = { 0b00000010, 0x28, 0b00000001, 0b10000000, 0b00000100 }; // from our old peripheral board revision (math done with Cole)
    // char buf[] = { 0b00000010, 0x28, 0x81, 0xD1, 0x00 }; // from the arduino mcp_can library
    // clang-format on

    // clang-format off
    //                 WRITE       ADDR  CNF3        CNF2        CNF1
    char set_cnf[] = {0b00000010, 0x28, 0b00000001, 0b10010001, 0b00000000};
    // clang-format on
    IO_PA4_SetLow();
    SPI0_WriteBlock(set_cnf, sizeof(set_cnf));
    IO_PA4_SetHigh();

    char set_normal_mode[] = {0b00000010, 0x0F, 0x00};
    IO_PA4_SetLow();
    SPI0_WriteBlock(set_normal_mode, sizeof(set_normal_mode));
    IO_PA4_SetHigh();

    // SPI0_Close();

    // ADC1_Enable();
    // ADC1_StartConversion(ADC_MUXPOS_AIN5_gc); // not needed if using
    // ADC1_GetConversion

    while (true) {
        // const uint16_t voltage = ADC1_GetConversion(ADC_MUXPOS_AIN0_gc) *
        // pow(2, 16 - 10); // ADC_MUXPOS_AIN5_gc I2C0_example_writeNBytes()
        // I2C0_SetAddress(0x0A << 1 );
        // char i2c_buf[1] = {0x4C};
        // // I2C0_SetBuffer(i2c_buf, 1);
        // I2C0_MasterWrite();
        // I2C0_SetBuffer(i2c_buf, 1);
        // I2C0_MasterRead();

        // double degC = d6t_8lh_loop();

        // clang-format off
        //                     WRITE       ADDR        CTRL        SIDH        SIDL[7:5]   EID8  EID0  DLC         DATA                                          
        char buf_deadbeef[] = {0b00000010, 0b00110000, 0b00001000, 0b11111111, 0b11101011, 0xff, 0xff, 0b00001000, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF};
        // clang-format on
        IO_PA4_SetLow();
        SPI0_WriteBlock(buf_deadbeef, sizeof(buf_deadbeef));
        SPI0_ReadByte();
        IO_PA4_SetHigh();

        // Should check that CTRL[3] is 0 before moving on (message sent)

        // clang-format off
        // char buf_adc[] = {0b00000010, 0b00110000, 0b00001011, 0b10101010, 0b01000000, 0x00, 0x00, 0b00000010, voltage >> 8, voltage & 0xFF, 0x00, 0x00};
        // clang-format on

        // // clang-format off
        // char buf_degC[] = {0b00000010, 0b00110000, 0b00001011, 0b10101010,
        // 0b01000000, 0x00, 0x00, 0b00000010, (long)degC >> 8, (long)degC &
        // 0x0F};
        // // clang-format on
        // IO_PA4_SetLow();
        // SPI0_WriteBlock(buf_degC, sizeof(buf_deadbeef));
        // IO_PA4_SetHigh();

        DELAY_milliseconds(100);
    }

    SPI0_Close();

    return 0;
};

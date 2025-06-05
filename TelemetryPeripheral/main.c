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
#include "d6t-1a.h"

// #include "MCP_CAN_lib-master/mcp_can.hpp"

int main() {

    // Add your code here and press Ctrl + Shift + B to build
    SYSTEM_Initialize();

    DELAY_milliseconds(100);

    // Will not succeed initialization if sensor is not connected!!!
    // d6t_8lh_setup();
    // d6t_1a_setup();

    SPI0_OpenConfiguration(MASTER0_CONFIG);

    // RESET instruction. Resets registers and also enters configuration mode
    char reset[] = {0b11000000};
    IO_PA4_SetLow();
    SPI0_WriteBlock(reset, sizeof(reset));
    IO_PA4_SetHigh();
    DELAY_milliseconds(10);

    // clang-format off
    //             WRITE       ADDR  CNF3        CNF2        CNF1
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

    ADC1_Enable();
    // ADC1_StartConversion(ADC_MUXPOS_AIN5_gc); // not needed if using
    // ADC1_GetConversion

    char buf_rts[] = {0b10000001};
    while (true) {
        // --- Code to read from an ADC ---
        // refer to https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/40001893B.pdf#_OPENTOPIC_TOC_PROCESSING_d114e14239 for ADC
        // AIN4, AIN5, AIN10, and AIN11 DNE for attiny1616

        // ADC1
        //  ADC_MUXPOS_AIN0_gc = PA_4
        //  ADC_MUXPOS_AIN1_gc = PA_5
        //  ADC_MUXPOS_AIN2_gc = PA_6
        //  ADC_MUXPOS_AIN3_gc = PA_7
        //  ADC_MUXPOS_AIN6_gc = PC_0
        //  ADC_MUXPOS_AIN7_gc = PC_1
        //  ADC_MUXPOS_AIN8_gc = PC_2
        //  ADC_MUXPOS_AIN9_gc = PC_3

        //ADC2
        //  ADC_MUXPOS_AIN0_gc = PA_0
        //  ADC_MUXPOS_AIN1_gc = PA_1
        //  ADC_MUXPOS_AIN2_gc = PA_2
        //  ADC_MUXPOS_AIN3_gc = PA_3
        //  ADC_MUXPOS_AIN4_gc = PA_4
        //  ADC_MUXPOS_AIN5_gc = PA_5
        //  ADC_MUXPOS_AIN6_gc = PA_6
        //  ADC_MUXPOS_AIN7_gc = PA_7
        //  ADC_MUXPOS_AIN8_gc = PB_5
        //  ADC_MUXPOS_AIN9_gc = PB_4
        // ADC_MUXPOS_AIN10_gc = PB_1
        // ADC_MUXPOS_AIN11_gc = PB_0

        const uint16_t voltage = ADC1_GetConversion(ADC_MUXPOS_AIN6_gc) * pow(2, 16 - 10); 
        // clang-format off
        //                WRITE       ADDR        SIDH        SIDL[7:5]   EID8  EID0  DLC         DATA                                          
        char buf_adc[] = {0b00000010, 0b00110001, 0b00001011, 0b10100000, 0x00, 0x00, 0b00000100,  0x00, voltage >> 8, voltage & 0xFF, 0x00};
        // clang-format on
        IO_PA4_SetLow();
        SPI0_WriteBlock(buf_adc, sizeof(buf_adc));
        IO_PA4_SetHigh();

        IO_PA4_SetLow();
        SPI0_WriteBlock(buf_rts, sizeof(buf_rts));
        IO_PA4_SetHigh();

        // ADC_MUXPOS_AIN5_gc I2C0_example_writeNBytes()
        // I2C0_SetAddress(0x0A << 1 );
        // char i2c_buf[1] = {0x4C};
        // I2C0_SetBuffer(i2c_buf, 1);
        // I2C0_MasterWrite();
        // I2C0_SetBuffer(i2c_buf, 1);
        // I2C0_MasterRead();

        // clang-format off
        //                     WRITE       ADDR        SIDH        SIDL[7:5]   EID8  EID0  DLC         DATA                                          
        // char buf_deadbeef[] = {0b00000010, 0b00110001, 0b11111111, 0b11100011, 0x00, 0x00, 0b00001000, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF};
        // // clang-format on
        // IO_PA4_SetLow();
        // SPI0_WriteBlock(buf_deadbeef, sizeof(buf_deadbeef));
        // IO_PA4_SetHigh();

        // IO_PA4_SetLow();
        // SPI0_WriteBlock(buf_rts, sizeof(buf_rts));
        // IO_PA4_SetHigh();

        // --- CODE TO READ FROM AN MCP2515 REGISTER ---
        // IO_PA4_SetLow();
        // const char buf0[3] = {0x03, 0x31};
        // SPI0_ExchangeBlock((void*)buf0, sizeof(buf0));
        // IO_PA4_SetHigh();



        // --- Code to read from temp sensors using I2C ---
        

        // Should check that CTRL[3] is 0 before moving on (message sent)

       

        // // Re-read from the d6t-8lh's pixel buffers
        // d6t_8lh_loop();
        // d6t_1a_loop();

        // // uint8_t degC = (uint8_t)d6t_8lh_ptat;
        // uint8_t pixels8lh[N_PIXEL] = {0};
        // for (int i=0; i<N_PIXEL; i++) {
        //     pixels8lh[i] = (uint8_t)(d6t_8lh_pix_data[i]);
        // }
        // uint8_t pixels1a[N_PIXEL] = {0};
        // for (int i=0; i<N_PIXEL; i++) {
        //     pixels1a[i] = (uint8_t)(d6t_1a_pix_data[i]);
        // }

        // // char i2c_buf[36] = {0};
        // // I2C0_example_readDataBlock(0x0a, 0x4c, i2c_buf, 36);

        // // clang-format off
        // //                 WRITE       ADDR        SIDH        SIDL[7:5]   EID8  EID0  DLC         DATA                                          
        // // char buf_degC[] = {0b00000010, 0b00110001, 0b10101010, 0b01000000, 0x00, 0x00, 0b00000001, degC};
        // char buf_pixels8lh[] = {0b00000010, 0b00110001, 0b10101010, 0b01000000, 0x00, 0x00, 0b00001000, pixels8lh[0], pixels8lh[1], pixels8lh[2], pixels8lh[3], pixels8lh[4], pixels8lh[5], pixels8lh[6], pixels8lh[7]};
        // char buf_pixels1a[] = {0b00000010, 0b00110001, 0b10101010, 0b01000000, 0x00, 0x00, 0b00001000, pixels1a[0], pixels1a[1], pixels1a[2], pixels1a[3], pixels1a[4], pixels1a[5], pixels1a[6], pixels1a[7]};
        // // clang-format on

        // // For some reason this value isn't very good
        // // IO_PA4_SetLow();
        // // SPI0_WriteBlock(buf_degC, sizeof(buf_degC));
        // // IO_PA4_SetHigh();

        // // IO_PA4_SetLow();
        // // SPI0_WriteBlock(buf_rts, sizeof(buf_rts));
        // // IO_PA4_SetHigh();

        // IO_PA4_SetLow();
        // SPI0_WriteBlock(buf_pixels8lh, sizeof(buf_pixels8lh));
        // IO_PA4_SetHigh();

        // IO_PA4_SetLow();
        // SPI0_WriteBlock(buf_rts, sizeof(buf_rts));
        // IO_PA4_SetHigh();

        // IO_PA4_SetLow();
        // SPI0_WriteBlock(buf_pixels1a, sizeof(buf_pixels1a));
        // IO_PA4_SetHigh();

        // IO_PA4_SetLow();
        // SPI0_WriteBlock(buf_rts, sizeof(buf_rts));
        // IO_PA4_SetHigh();

        DELAY_milliseconds(100);
    }

    SPI0_Close();

    return 0;
};

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

void mcp2515_reset() {
    // RESET instruction. Resets registers and also enters configuration mode
    static char reset[] = {0b11000000};
    IO_PA4_SetLow();
    SPI0_WriteBlock(reset, sizeof(reset));
    IO_PA4_SetHigh();
    DELAY_milliseconds(10);
}

void mcp2515_initialize() {
    // Set CNF registers
    // clang-format off
    //                WRITE       ADDR  CNF3        CNF2        CNF1
    // char buf[] = { 0b00000010, 0x28, 0b00000001, 0b10000000, 0b00000100 }; // from our old peripheral board revision (math done with Cole)
    // char buf[] = { 0b00000010, 0x28, 0x81,       0xD1,       0x00       }; // from the arduino mcp_can library
    static char set_cnf[] = {0b00000010, 0x28, 0b00000001, 0b10010001, 0b00000000 };
    // clang-format on
    IO_PA4_SetLow();
    SPI0_WriteBlock(set_cnf, sizeof(set_cnf));
    IO_PA4_SetHigh();

    // Set MCP2515 to Normal mode
    static char set_normal_mode[] = {0b00000010, 0x0F, 0x00};
    IO_PA4_SetLow();
    SPI0_WriteBlock(set_normal_mode, sizeof(set_normal_mode));
    IO_PA4_SetHigh();
}

void mcp2515_request_to_send(bool txb0, bool txb1, bool txb2) {
    char buf_rts[] = {0b10000000 | (txb2 ? 0b100 : 0) | (txb1 ? 0b010 : 0) | (txb0 ? 0b001 : 0)};
    IO_PA4_SetLow();
    SPI0_WriteBlock(buf_rts, sizeof(buf_rts));
    IO_PA4_SetHigh();
}

void mcp2515_fill_txbuf0(uint32_t id, uint8_t data[], uint8_t dlc) {
    //                  WRITE       ADDR        SIDH                       SIDL[7:5]          EID8  EID0  DLC  DATA
    // char write_buf[] = {0b00000010, 0b00110001, (id & 0b11111111000) >> 3, (id & 0b111) << 5, 0x00, 0x00, dlc & 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    //                  LOAD TX     SIDH                       SIDL[7:5]          EID8  EID0  DLC         DATA
    char write_buf[] = {0b01000000, (id & 0b11111111000) >> 3, (id & 0b111) << 5, 0x00, 0x00, dlc & 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    for (int i=0; i < dlc; i++) {
        write_buf[6+i] = data[i];
    }
    // clang-format on
    IO_PA4_SetLow();
    SPI0_WriteBlock(write_buf, sizeof(write_buf));
    IO_PA4_SetHigh();
};
void mcp2515_fill_txbuf1(uint32_t id, uint8_t data[], uint8_t dlc) {
    //                  LOAD TX     SIDH                       SIDL[7:5]          EID8  EID0  DLC         DATA
    char write_buf[] = {0b01000010, (id & 0b11111111000) >> 3, (id & 0b111) << 5, 0x00, 0x00, dlc & 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    for (int i=0; i < dlc; i++) {
        write_buf[6+i] = data[i];
    }
    // clang-format on
    IO_PA4_SetLow();
    SPI0_WriteBlock(write_buf, sizeof(write_buf));
    IO_PA4_SetHigh();
};
void mcp2515_fill_txbuf2(uint32_t id, uint8_t data[], uint8_t dlc) {
    //                  LOAD TX     SIDH                       SIDL[7:5]          EID8  EID0  DLC         DATA
    char write_buf[] = {0b01000100, (id & 0b11111111000) >> 3, (id & 0b111) << 5, 0x00, 0x00, dlc & 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    for (int i=0; i < dlc; i++) {
        write_buf[6+i] = data[i];
    }
    // clang-format on
    IO_PA4_SetLow();
    SPI0_WriteBlock(write_buf, sizeof(write_buf));
    IO_PA4_SetHigh();
};

// --- Code to read from an ADC ---
// Refer to https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/40001893B.pdf#_OPENTOPIC_TOC_PROCESSING_d114e14239 for ADC
// Note that AIN4, AIN5, AIN10, and AIN11 do not exist for attiny1616
// ADC1
//  ADC_MUXPOS_AIN0_gc = PA_4
//  ADC_MUXPOS_AIN1_gc = PA_5
//  ADC_MUXPOS_AIN2_gc = PA_6
//  ADC_MUXPOS_AIN3_gc = PA_7
//  ADC_MUXPOS_AIN6_gc = PC_0
//  ADC_MUXPOS_AIN7_gc = PC_1
//  ADC_MUXPOS_AIN8_gc = PC_2
//  ADC_MUXPOS_AIN9_gc = PC_3
// ADC2
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
//  ADC_MUXPOS_AIN10_gc = PB_1
//  ADC_MUXPOS_AIN11_gc = PB_0

#define FR 0
#define FL 1
#define BR 2
#define BL 3

#if WHEEL_POSITION == FR
    #define TPERIPH_TPDO_DATA_ID 0x1A3
    #define TPERIPH_TPDO_TIRETEMP_ID 0x2A2
    #define HAS_TIRETEMP_1x8 false
    #define HAS_TIRETEMP_1x1 false
#elif WHEEL_POSITION == FL
    #define TPERIPH_TPDO_DATA_ID 0x1A2
    #define TPERIPH_TPDO_TIRETEMP_ID 0x2A1
    #define HAS_TIRETEMP_1x8 true
    #define HAS_TIRETEMP_1x1 false
#elif WHEEL_POSITION == BR
    #define TPERIPH_TPDO_DATA_ID 0x1A5
    #define TPERIPH_TPDO_TIRETEMP_ID 0x2A4
    #define HAS_TIRETEMP_1x8 false
    #define HAS_TIRETEMP_1x1 false
#elif WHEEL_POSITION == BL
    #define TPERIPH_TPDO_DATA_ID 0x1A4
    #define TPERIPH_TPDO_TIRETEMP_ID 0x2A3
    #define HAS_TIRETEMP_1x8 true
    #define HAS_TIRETEMP_1x1 false
#else
    #error "WHEEL_POSITION must be one of BR/BL/FR/FL!"
#endif

int main() {
    SYSTEM_Initialize();
    DELAY_milliseconds(100);

    // Setup will block if sensor is not connected!!!
    if (HAS_TIRETEMP_1x8) d6t_8lh_setup();
    if (HAS_TIRETEMP_1x1) d6t_1a_setup();

    SPI0_OpenConfiguration(MASTER0_CONFIG);

    mcp2515_reset();
    mcp2515_initialize();

    ADC1_Initialize(); // Use ADC1_GetConversion() to read

    while (true) {

        const uint16_t wheel_speed = ADC1_GetConversion(ADC_MUXPOS_AIN9_gc) * pow(2, 16 - 10); 
        const uint16_t sus_travel_voltage = ADC1_GetConversion(ADC_MUXPOS_AIN2_gc) * pow(2, 16 - 10); 
        const uint16_t sus_travel = (pow(2, 16) - sus_travel_voltage) / pow(2, 16) * 5000;

        // // clang-format off
        // //                WRITE       ADDR        SIDH        SIDL[7:5]   EID8  EID0  DLC         DATA                                          
        // // 0b110100011
        // char buf_adc[] = {0b00000010, 0b00110001, 0b00110100, 0b01100000, 0x00, 0x00, 0b00000100,  0x00, voltage >> 8, voltage & 0xFF, 0x00};
        // // clang-format on
        // IO_PA4_SetLow();
        // SPI0_WriteBlock(buf_adc, sizeof(buf_adc));
        // IO_PA4_SetHigh();

        if (HAS_TIRETEMP_1x8) {
            d6t_1a_loop();

            uint8_t pixels1a[N_PIXEL] = {0};
            for (int i=0; i<N_PIXEL; i++) {
                pixels1a[i] = (uint8_t)(d6t_1a_pix_data[i]);
            }
        }
        if (HAS_TIRETEMP_1x8) {
            d6t_8lh_loop();

            uint8_t pixels8lh[N_PIXEL] = {0};
            for (int i=0; i<N_PIXEL; i++) {
                pixels8lh[i] = (uint8_t)(d6t_8lh_pix_data[i]);
            }

            mcp2515_fill_txbuf0(TPERIPH_TPDO_TIRETEMP_ID, pixels8lh, 8);
        }

        uint8_t tpdo_data[] = {
            wheel_speed & 0xFF,
            (wheel_speed & 0xFF00) >> 8,
            sus_travel & 0xFF, 
            (sus_travel & 0xFF00) >> 8,
            0x00,
            0x00,
            0x00,
        };
        mcp2515_fill_txbuf1(TPERIPH_TPDO_DATA_ID, tpdo_data, 7);

        mcp2515_request_to_send(HAS_TIRETEMP_1x8, true, false);

        DELAY_milliseconds(100);
    }

    SPI0_Close();

    return 0;
};

// --- CODE TO READ FROM AN MCP2515 REGISTER ---
// IO_PA4_SetLow();
// const char buf0[3] = {0x03, 0x31};
// SPI0_ExchangeBlock((void*)buf0, sizeof(buf0));
// IO_PA4_SetHigh();

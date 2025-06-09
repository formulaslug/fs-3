#include "mbed.h"

#define COLS 100
#define ROWS 8

static const char *col_names[] = { "SME_THROTL_TorqueDemand", "SME_THROTL_MaxSpeed", "SME_THROTL_Forward", "SME_THROTL_Reverse", "SME_THROTL_UNUSED_BIT_1", "SME_THROTL_PowerReady", "SME_THROTL_UNUSED_BIT_2", "SME_THROTL_UNUSED_BIT_3", "SME_THROTL_UNUSED_BIT_4", "SME_THROTL_UNUSED_BIT_5", "SME_THROTL_MBB_Alive", "SME_THROTL_UNUSED_BIT_6", "SME_THROTL_UNUSED_BIT_7", "SME_THROTL_UNUSED_BIT_8", "SME_THROTL_UNUSED_BIT_9", "SME_THROTL_UNUSED_SHORT_1", "SME_CURRLIM_ChargeCurrentLim", "SME_CURRLIM_DischargeCurrentLim", "SME_CURRLIM_UNUSED_INT_1", "SME_TRQSPD_Speed", "SME_TRQSPD_Torque", "SME_TRQSPD_SOC_Low_Traction", "SME_TRQSPD_SOC_Low_Hydraulic", "SME_TRQSPD_Reverse", "SME_TRQSPD_Forward", "SME_TRQSPD_Park_Brake", "SME_TRQSPD_Pedal_Brake", "SME_TRQSPD_Controller_Overtermp", "SME_TRQSPD_Key_switch_overvolt", "SME_TRQSPD_Key_switch_undervolt", "SME_TRQSPD_Running", "SME_TRQSPD_Traction", "SME_TRQSPD_Hydraulic", "SME_TRQSPD_Powering_Enabled", "SME_TRQSPD_Powering_Ready", "SME_TRQSPD_Precharging", "SME_TRQSPD_contactor_closed", "SME_TRQSPD_MotorFlags", "SME_TEMP_MotorTemperature", "SME_TEMP_ControllerTemperature", "SME_TEMP_DC_Bus_V", "SME_TEMP_FaultCode", "SME_TEMP_FaultLevel", "SME_TEMP_BusCurrent", "ACC_STATUS_BMS_FAULT", "ACC_STATUS_IMD_FAULT", "ACC_STATUS_SHUTDOWN_STATE", "ACC_STATUS_PRECHARGE_DONE", "ACC_STATUS_PRECHARING", "ACC_STATUS_CELL_TOO_LOW", "ACC_STATUS_CELL_TOO_HIGH", "ACC_STATUS_TEMP_TOO_LOW", "ACC_STATUS_TEMP_TOO_HIGH", "ACC_POWER_PACK_VOLTAGE", "ACC_POWER_SOC", "ACC_POWER_CURRENT", "ACC_SEG0_VOLTS_CELL0", "ACC_SEG0_VOLTS_CELL1", "ACC_SEG0_VOLTS_CELL2", "ACC_SEG0_VOLTS_CELL3", "ACC_SEG0_VOLTS_CELL4", "ACC_SEG0_VOLTS_CELL5", "ACC_SEG0_TEMPS_CELL0", "ACC_SEG0_TEMPS_CELL1", "ACC_SEG0_TEMPS_CELL2", "ACC_SEG0_TEMPS_CELL3", "ACC_SEG0_TEMPS_CELL4", "ACC_SEG0_TEMPS_CELL5", "ACC_SEG1_VOLTS_CELL0", "ACC_SEG1_VOLTS_CELL1", "ACC_SEG1_VOLTS_CELL2", "ACC_SEG1_VOLTS_CELL3", "ACC_SEG1_VOLTS_CELL4", "ACC_SEG1_VOLTS_CELL5", "ACC_SEG1_TEMPS_CELL0", "ACC_SEG1_TEMPS_CELL1", "ACC_SEG1_TEMPS_CELL2", "ACC_SEG1_TEMPS_CELL3", "ACC_SEG1_TEMPS_CELL4", "ACC_SEG1_TEMPS_CELL5", "ACC_SEG2_VOLTS_CELL0", "ACC_SEG2_VOLTS_CELL1", "ACC_SEG2_VOLTS_CELL2", "ACC_SEG2_VOLTS_CELL3", "ACC_SEG2_VOLTS_CELL4", "ACC_SEG2_VOLTS_CELL5", "ACC_SEG2_TEMPS_CELL0", "ACC_SEG2_TEMPS_CELL1", "ACC_SEG2_TEMPS_CELL2", "ACC_SEG2_TEMPS_CELL3", "ACC_SEG2_TEMPS_CELL4", "ACC_SEG2_TEMPS_CELL5", "ACC_SEG3_VOLTS_CELL0", "ACC_SEG3_VOLTS_CELL1", "ACC_SEG3_VOLTS_CELL2", "ACC_SEG3_VOLTS_CELL3", "ACC_SEG3_VOLTS_CELL4", "ACC_SEG3_VOLTS_CELL5", "ACC_SEG3_TEMPS_CELL0", "ACC_SEG3_TEMPS_CELL1" };
static const uint8_t col_name_sizes[] = { 23, 19, 18, 18, 23, 21, 23, 23, 23, 23, 20, 23, 23, 23, 23, 25, 28, 31, 24, 16, 17, 27, 28, 18, 18, 21, 22, 31, 30, 31, 18, 19, 20, 27, 25, 22, 27, 21, 25, 30, 17, 18, 19, 19, 20, 20, 25, 25, 21, 23, 24, 23, 24, 22, 13, 17, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20 };
static const char *col_name_types[] = { "i4", "i4", "b0", "b0", "b0", "b0", "b0", "b0", "b0", "b0", "u3", "b0", "b0", "b0", "b0", "u4", "u4", "u4", "u5", "u4", "i4", "b0", "b0", "b0", "b0", "b0", "b0", "b0", "b0", "b0", "b0", "b0", "b0", "b0", "b0", "b0", "b0", "u4", "u3", "u3", "f5", "u3", "u3", "f5", "b0", "b0", "b0", "b0", "b0", "b0", "b0", "b0", "b0", "f5", "u3", "f5", "f5", "f5", "f5", "f5", "f5", "f5", "i3", "i3", "i3", "i3", "i3", "i3", "f5", "f5", "f5", "f5", "f5", "f5", "i3", "i3", "i3", "i3", "i3", "i3", "f5", "f5", "f5", "f5", "f5", "f5", "i3", "i3", "i3", "i3", "i3", "i3", "f5", "f5", "f5", "f5", "f5", "f5", "i3", "i3" };

struct Values {
    int16_t SME_THROTL_TorqueDemand[ROWS];
    int16_t SME_THROTL_MaxSpeed[ROWS];
    uint8_t SME_THROTL_Forward[ROWS/8];
    uint8_t SME_THROTL_Reverse[ROWS/8];
    uint8_t SME_THROTL_UNUSED_BIT_1[ROWS/8];
    uint8_t SME_THROTL_PowerReady[ROWS/8];
    uint8_t SME_THROTL_UNUSED_BIT_2[ROWS/8];
    uint8_t SME_THROTL_UNUSED_BIT_3[ROWS/8];
    uint8_t SME_THROTL_UNUSED_BIT_4[ROWS/8];
    uint8_t SME_THROTL_UNUSED_BIT_5[ROWS/8];
    uint8_t SME_THROTL_MBB_Alive[ROWS];
    uint8_t SME_THROTL_UNUSED_BIT_6[ROWS/8];
    uint8_t SME_THROTL_UNUSED_BIT_7[ROWS/8];
    uint8_t SME_THROTL_UNUSED_BIT_8[ROWS/8];
    uint8_t SME_THROTL_UNUSED_BIT_9[ROWS/8];
    uint16_t SME_THROTL_UNUSED_SHORT_1[ROWS];
    uint16_t SME_CURRLIM_ChargeCurrentLim[ROWS];
    uint16_t SME_CURRLIM_DischargeCurrentLim[ROWS];
    uint32_t SME_CURRLIM_UNUSED_INT_1[ROWS];
    uint16_t SME_TRQSPD_Speed[ROWS];
    int16_t SME_TRQSPD_Torque[ROWS];
    uint8_t SME_TRQSPD_SOC_Low_Traction[ROWS/8];
    uint8_t SME_TRQSPD_SOC_Low_Hydraulic[ROWS/8];
    uint8_t SME_TRQSPD_Reverse[ROWS/8];
    uint8_t SME_TRQSPD_Forward[ROWS/8];
    uint8_t SME_TRQSPD_Park_Brake[ROWS/8];
    uint8_t SME_TRQSPD_Pedal_Brake[ROWS/8];
    uint8_t SME_TRQSPD_Controller_Overtermp[ROWS/8];
    uint8_t SME_TRQSPD_Key_switch_overvolt[ROWS/8];
    uint8_t SME_TRQSPD_Key_switch_undervolt[ROWS/8];
    uint8_t SME_TRQSPD_Running[ROWS/8];
    uint8_t SME_TRQSPD_Traction[ROWS/8];
    uint8_t SME_TRQSPD_Hydraulic[ROWS/8];
    uint8_t SME_TRQSPD_Powering_Enabled[ROWS/8];
    uint8_t SME_TRQSPD_Powering_Ready[ROWS/8];
    uint8_t SME_TRQSPD_Precharging[ROWS/8];
    uint8_t SME_TRQSPD_contactor_closed[ROWS/8];
    uint16_t SME_TRQSPD_MotorFlags[ROWS];
    uint8_t SME_TEMP_MotorTemperature[ROWS];
    uint8_t SME_TEMP_ControllerTemperature[ROWS];
    float SME_TEMP_DC_Bus_V[ROWS];
    uint8_t SME_TEMP_FaultCode[ROWS];
    uint8_t SME_TEMP_FaultLevel[ROWS];
    float SME_TEMP_BusCurrent[ROWS];
    uint8_t ACC_STATUS_BMS_FAULT[ROWS/8];
    uint8_t ACC_STATUS_IMD_FAULT[ROWS/8];
    uint8_t ACC_STATUS_SHUTDOWN_STATE[ROWS/8];
    uint8_t ACC_STATUS_PRECHARGE_DONE[ROWS/8];
    uint8_t ACC_STATUS_PRECHARING[ROWS/8];
    uint8_t ACC_STATUS_CELL_TOO_LOW[ROWS/8];
    uint8_t ACC_STATUS_CELL_TOO_HIGH[ROWS/8];
    uint8_t ACC_STATUS_TEMP_TOO_LOW[ROWS/8];
    uint8_t ACC_STATUS_TEMP_TOO_HIGH[ROWS/8];
    float ACC_POWER_PACK_VOLTAGE[ROWS];
    uint8_t ACC_POWER_SOC[ROWS];
    float ACC_POWER_CURRENT[ROWS];
    float ACC_SEG0_VOLTS_CELL0[ROWS];
    float ACC_SEG0_VOLTS_CELL1[ROWS];
    float ACC_SEG0_VOLTS_CELL2[ROWS];
    float ACC_SEG0_VOLTS_CELL3[ROWS];
    float ACC_SEG0_VOLTS_CELL4[ROWS];
    float ACC_SEG0_VOLTS_CELL5[ROWS];
    int8_t ACC_SEG0_TEMPS_CELL0[ROWS];
    int8_t ACC_SEG0_TEMPS_CELL1[ROWS];
    int8_t ACC_SEG0_TEMPS_CELL2[ROWS];
    int8_t ACC_SEG0_TEMPS_CELL3[ROWS];
    int8_t ACC_SEG0_TEMPS_CELL4[ROWS];
    int8_t ACC_SEG0_TEMPS_CELL5[ROWS];
    float ACC_SEG1_VOLTS_CELL0[ROWS];
    float ACC_SEG1_VOLTS_CELL1[ROWS];
    float ACC_SEG1_VOLTS_CELL2[ROWS];
    float ACC_SEG1_VOLTS_CELL3[ROWS];
    float ACC_SEG1_VOLTS_CELL4[ROWS];
    float ACC_SEG1_VOLTS_CELL5[ROWS];
    int8_t ACC_SEG1_TEMPS_CELL0[ROWS];
    int8_t ACC_SEG1_TEMPS_CELL1[ROWS];
    int8_t ACC_SEG1_TEMPS_CELL2[ROWS];
    int8_t ACC_SEG1_TEMPS_CELL3[ROWS];
    int8_t ACC_SEG1_TEMPS_CELL4[ROWS];
    int8_t ACC_SEG1_TEMPS_CELL5[ROWS];
    float ACC_SEG2_VOLTS_CELL0[ROWS];
    float ACC_SEG2_VOLTS_CELL1[ROWS];
    float ACC_SEG2_VOLTS_CELL2[ROWS];
    float ACC_SEG2_VOLTS_CELL3[ROWS];
    float ACC_SEG2_VOLTS_CELL4[ROWS];
    float ACC_SEG2_VOLTS_CELL5[ROWS];
    int8_t ACC_SEG2_TEMPS_CELL0[ROWS];
    int8_t ACC_SEG2_TEMPS_CELL1[ROWS];
    int8_t ACC_SEG2_TEMPS_CELL2[ROWS];
    int8_t ACC_SEG2_TEMPS_CELL3[ROWS];
    int8_t ACC_SEG2_TEMPS_CELL4[ROWS];
    int8_t ACC_SEG2_TEMPS_CELL5[ROWS];
    float ACC_SEG3_VOLTS_CELL0[ROWS];
    float ACC_SEG3_VOLTS_CELL1[ROWS];
    float ACC_SEG3_VOLTS_CELL2[ROWS];
    float ACC_SEG3_VOLTS_CELL3[ROWS];
    float ACC_SEG3_VOLTS_CELL4[ROWS];
    float ACC_SEG3_VOLTS_CELL5[ROWS];
    int8_t ACC_SEG3_TEMPS_CELL0[ROWS];
    int8_t ACC_SEG3_TEMPS_CELL1[ROWS];
};

inline void write_fsdaq_schema(FILE *file) {
    uint32_t m = COLS;
    uint32_t n = ROWS;
    fwrite(&m, sizeof(m), 1, file);
    fwrite(&n, sizeof(n), 1, file);

    for (int i=0; i<COLS; i++) {
        const uint8_t col_len = col_name_sizes[i];
        const char *col = col_names[i];

        fwrite(&col_len, sizeof(col_len), 1, file);
        fwrite(col, col_len, 1, file);
    }

    for (int i=0; i<COLS; i++) {
        const char *col_type = col_name_types[i];
        fwrite(col_type, 2, 1, file);
    }
}

inline void write_fsdaq_batch(Values *vals, FILE *file) {

    fwrite(&vals, sizeof(*vals), 1, file);
}


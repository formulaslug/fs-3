#include "can_processor_generated.hpp"
#include "encoder_generated.hpp"
#include "mbed.h"

namespace fsdaq {

CANProcessor::CANProcessor(CAN &can)
    : current_row(std::make_unique<DataRow>()), can(can) {}

void CANProcessor::process_message() {
    CANMessage msg;
    if (!can.read(msg)) return;
    switch (static_cast<fsdaq::MessageID>(msg.id)) {
    case SYNC:
        break;

    case SME_RPDO_Throttle_Demand:
        uint16_t raw_SME_THROTL_TorqueDemand;

        uint16_t raw_SME_THROTL_MaxSpeed;

        uint8_t raw_SME_THROTL_Forward;

        uint8_t raw_SME_THROTL_Reverse;

        uint8_t raw_SME_THROTL_UNUSED_BIT_1;

        uint8_t raw_SME_THROTL_PowerReady;

        uint8_t raw_SME_THROTL_UNUSED_BIT_2;

        uint8_t raw_SME_THROTL_UNUSED_BIT_3;

        uint8_t raw_SME_THROTL_UNUSED_BIT_4;

        uint8_t raw_SME_THROTL_UNUSED_BIT_5;

        uint8_t raw_SME_THROTL_MBB_Alive;

        uint8_t raw_SME_THROTL_UNUSED_BIT_6;

        uint8_t raw_SME_THROTL_UNUSED_BIT_7;

        uint8_t raw_SME_THROTL_UNUSED_BIT_8;

        uint8_t raw_SME_THROTL_UNUSED_BIT_9;

        uint16_t raw_SME_THROTL_UNUSED_SHORT_1;

        break;

    case SME_RPDO_Max_Currents:
        uint16_t raw_SME_CURRLIM_ChargeCurrentLim;

        uint16_t raw_SME_CURRLIM_DischargeCurrentLim;

        uint32_t raw_SME_CURRLIM_UNUSED_INT_1;

        break;

    case SME_TPDO_Torque_speed:
        uint16_t raw_SME_TRQSPD_Speed;

        uint16_t raw_SME_TRQSPD_Torque;

        uint8_t raw_SME_TRQSPD_SOC_Low_Traction;

        uint8_t raw_SME_TRQSPD_SOC_Low_Hydraulic;

        uint8_t raw_SME_TRQSPD_Reverse;

        uint8_t raw_SME_TRQSPD_Forward;

        uint8_t raw_SME_TRQSPD_Park_Brake;

        uint8_t raw_SME_TRQSPD_Pedal_Brake;

        uint8_t raw_SME_TRQSPD_Controller_Overtermp;

        uint8_t raw_SME_TRQSPD_Key_switch_overvolt;

        uint8_t raw_SME_TRQSPD_Key_switch_undervolt;

        uint8_t raw_SME_TRQSPD_Running;

        uint8_t raw_SME_TRQSPD_Traction;

        uint8_t raw_SME_TRQSPD_Hydraulic;

        uint8_t raw_SME_TRQSPD_Powering_Enabled;

        uint8_t raw_SME_TRQSPD_Powering_Ready;

        uint8_t raw_SME_TRQSPD_Precharging;

        uint8_t raw_SME_TRQSPD_contactor_closed;

        uint16_t raw_SME_TRQSPD_MotorFlags;

        break;

    case SME_TPDO_Temperature:
        uint8_t raw_SME_TEMP_MotorTemperature;

        uint8_t raw_SME_TEMP_ControllerTemperature;

        uint16_t raw_SME_TEMP_DC_Bus_V;

        uint8_t raw_SME_TEMP_FaultCode;

        uint8_t raw_SME_TEMP_FaultLevel;

        uint16_t raw_SME_TEMP_BusCurrent;

        break;

    case ACC_TPDO_STATUS:
        uint8_t raw_ACC_STATUS_BMS_FAULT;

        uint8_t raw_ACC_STATUS_IMD_FAULT;

        uint8_t raw_ACC_STATUS_SHUTDOWN_STATE;

        uint8_t raw_ACC_STATUS_PRECHARGE_DONE;

        uint8_t raw_ACC_STATUS_PRECHARGING;

        uint8_t raw_ACC_STATUS_CHARGING;

        uint8_t raw_ACC_STATUS_CELL_TOO_LOW;

        uint8_t raw_ACC_STATUS_CELL_TOO_HIGH;

        uint8_t raw_ACC_STATUS_TEMP_TOO_LOW;

        uint8_t raw_ACC_STATUS_TEMP_TOO_HIGH;

        uint8_t raw_ACC_STATUS_TEMP_TOO_HIGH_CRG;

        uint8_t raw_ACC_STATUS_BALANCING;

        uint16_t raw_ACC_STATUS_GLV_VOLTAGE;

        uint32_t raw_ACC_STATUS_CELL_FAULT_INDEX;

        break;

    case ACC_TPDO_POWER:
        uint16_t raw_ACC_POWER_PACK_VOLTAGE;

        uint8_t raw_ACC_POWER_SOC;

        uint16_t raw_ACC_POWER_CURRENT;

        break;

    case ACC_TPDO_SEG0_VOLTS:
        uint8_t raw_ACC_SEG0_VOLTS_CELL0;

        uint8_t raw_ACC_SEG0_VOLTS_CELL1;

        uint8_t raw_ACC_SEG0_VOLTS_CELL2;

        uint8_t raw_ACC_SEG0_VOLTS_CELL3;

        uint8_t raw_ACC_SEG0_VOLTS_CELL4;

        uint8_t raw_ACC_SEG0_VOLTS_CELL5;

        break;

    case ACC_TPDO_SEG0_TEMPS:
        uint8_t raw_ACC_SEG0_TEMPS_CELL0;

        uint8_t raw_ACC_SEG0_TEMPS_CELL1;

        uint8_t raw_ACC_SEG0_TEMPS_CELL2;

        uint8_t raw_ACC_SEG0_TEMPS_CELL3;

        uint8_t raw_ACC_SEG0_TEMPS_CELL4;

        uint8_t raw_ACC_SEG0_TEMPS_CELL5;

        break;

    case ACC_TPDO_SEG1_VOLTS:
        uint8_t raw_ACC_SEG1_VOLTS_CELL0;

        uint8_t raw_ACC_SEG1_VOLTS_CELL1;

        uint8_t raw_ACC_SEG1_VOLTS_CELL2;

        uint8_t raw_ACC_SEG1_VOLTS_CELL3;

        uint8_t raw_ACC_SEG1_VOLTS_CELL4;

        uint8_t raw_ACC_SEG1_VOLTS_CELL5;

        break;

    case ACC_TPDO_SEG1_TEMPS:
        uint8_t raw_ACC_SEG1_TEMPS_CELL0;

        uint8_t raw_ACC_SEG1_TEMPS_CELL1;

        uint8_t raw_ACC_SEG1_TEMPS_CELL2;

        uint8_t raw_ACC_SEG1_TEMPS_CELL3;

        uint8_t raw_ACC_SEG1_TEMPS_CELL4;

        uint8_t raw_ACC_SEG1_TEMPS_CELL5;

        break;

    case ACC_TPDO_SEG2_VOLTS:
        uint8_t raw_ACC_SEG2_VOLTS_CELL0;

        uint8_t raw_ACC_SEG2_VOLTS_CELL1;

        uint8_t raw_ACC_SEG2_VOLTS_CELL2;

        uint8_t raw_ACC_SEG2_VOLTS_CELL3;

        uint8_t raw_ACC_SEG2_VOLTS_CELL4;

        uint8_t raw_ACC_SEG2_VOLTS_CELL5;

        break;

    case ACC_TPDO_SEG2_TEMPS:
        uint8_t raw_ACC_SEG2_TEMPS_CELL0;

        uint8_t raw_ACC_SEG2_TEMPS_CELL1;

        uint8_t raw_ACC_SEG2_TEMPS_CELL2;

        uint8_t raw_ACC_SEG2_TEMPS_CELL3;

        uint8_t raw_ACC_SEG2_TEMPS_CELL4;

        uint8_t raw_ACC_SEG2_TEMPS_CELL5;

        break;

    case ACC_TPDO_SEG3_VOLTS:
        uint8_t raw_ACC_SEG3_VOLTS_CELL0;

        uint8_t raw_ACC_SEG3_VOLTS_CELL1;

        uint8_t raw_ACC_SEG3_VOLTS_CELL2;

        uint8_t raw_ACC_SEG3_VOLTS_CELL3;

        uint8_t raw_ACC_SEG3_VOLTS_CELL4;

        uint8_t raw_ACC_SEG3_VOLTS_CELL5;

        break;

    case ACC_TPDO_SEG3_TEMPS:
        uint8_t raw_ACC_SEG3_TEMPS_CELL0;

        uint8_t raw_ACC_SEG3_TEMPS_CELL1;

        uint8_t raw_ACC_SEG3_TEMPS_CELL2;

        uint8_t raw_ACC_SEG3_TEMPS_CELL3;

        uint8_t raw_ACC_SEG3_TEMPS_CELL4;

        uint8_t raw_ACC_SEG3_TEMPS_CELL5;

        break;

    case ACC_TPDO_SEG4_VOLTS:
        uint8_t raw_ACC_SEG4_VOLTS_CELL0;

        uint8_t raw_ACC_SEG4_VOLTS_CELL1;

        uint8_t raw_ACC_SEG4_VOLTS_CELL2;

        uint8_t raw_ACC_SEG4_VOLTS_CELL3;

        uint8_t raw_ACC_SEG4_VOLTS_CELL4;

        uint8_t raw_ACC_SEG4_VOLTS_CELL5;

        break;

    case ACC_TPDO_SEG4_TEMPS:
        uint8_t raw_ACC_SEG4_TEMPS_CELL0;

        uint8_t raw_ACC_SEG4_TEMPS_CELL1;

        uint8_t raw_ACC_SEG4_TEMPS_CELL2;

        uint8_t raw_ACC_SEG4_TEMPS_CELL3;

        uint8_t raw_ACC_SEG4_TEMPS_CELL4;

        uint8_t raw_ACC_SEG4_TEMPS_CELL5;

        break;

    case VDM_GPS_LAT_LONG:
        uint32_t raw_VDM_GPS_Latitude;

        uint32_t raw_VDM_GPS_Longitude;

        break;

    case VDM_GPS_DATA:
        uint16_t raw_VDM_GPS_SPEED;

        uint16_t raw_VDM_GPS_ALTITUDE;

        uint16_t raw_VDM_GPS_TRUE_COURSE;

        uint8_t raw_VDM_GPS_SATELLITES_IN_USE;

        uint8_t raw_VDM_GPS_VALID1;

        break;

    case VDM_DATE_TIME:
        uint8_t raw_VDM_GPS_VALID2;

        uint8_t raw_VDM_UTC_DATE_YEAR;

        uint8_t raw_VDM_UTC_DATE_MONTH;

        uint8_t raw_VDM_UTC_DATE_DAY;

        uint8_t raw_VDM_UTC_TIME_HOURS;

        uint8_t raw_VDM_UTC_TIME_MINUTES;

        uint8_t raw_VDM_UTC_TIME_SECONDS;

        break;

    case VDM_ACCELERATION:
        uint16_t raw_VDM_X_AXIS_ACCELERATION;

        uint16_t raw_VDM_Y_AXIS_ACCELERATION;

        uint16_t raw_VDM_Z_AXIS_ACCELERATION;

        break;

    case VDM_YAW_RATE:
        uint16_t raw_VDM_X_AXIS_YAW_RATE;

        uint16_t raw_VDM_Y_AXIS_YAW_RATE;

        uint16_t raw_VDM_Z_AXIS_YAW_RATE;

        break;

    case ETC_TPDO_STATUS:
        uint16_t raw_ETC_STATUS_HE1;

        uint16_t raw_ETC_STATUS_HE2;

        uint16_t raw_ETC_STATUS_BRAKE_SENSE_VOLTAGE;

        uint8_t raw_ETC_STATUS_PEDAL_TRAVEL;

        uint8_t raw_ETC_STATUS_RTD_BUTTON;

        uint8_t raw_ETC_STATUS_RTDS;

        uint8_t raw_ETC_STATUS_REVERSE;

        uint8_t raw_ETC_STATUS_BRAKELIGHT;

        uint8_t raw_ETC_STATUS_RTD;

        uint8_t raw_ETC_STATUS_IMPLAUSIBILITY;

        uint8_t raw_ETC_STATUS_TS_ACTIVE;

        break;

    case PDB_TPDO_POWER_A:
        uint16_t raw_PDB_POWER_A_GLV_VOLTAGE;

        uint8_t raw_PDB_POWER_A_CURRENT_SHUTDOWN;

        uint8_t raw_PDB_POWER_A_CURRENT_ACC;

        uint8_t raw_PDB_POWER_A_CURRENT_ETC;

        uint8_t raw_PDB_POWER_A_CURRENT_BPS;

        uint8_t raw_PDB_POWER_A_CURRENT_TRACTIVE;

        uint8_t raw_PDB_POWER_A_CURRENT_BSPD;

        break;

    case PDB_TPDO_POWER_B:
        uint8_t raw_PDB_POWER_B_CURRENT_TELEMETRY;

        uint8_t raw_PDB_POWER_B_CURRENT_PDB;

        uint8_t raw_PDB_POWER_B_CURRENT_DASH;

        uint8_t raw_PDB_POWER_B_CURRENT_RTML;

        uint8_t raw_PDB_POWER_B_CURRENT_EXTRA_1;

        uint8_t raw_PDB_POWER_B_CURRENT_EXTRA_2;

        break;

    case TMAIN_TPDO_DATA:
        uint16_t raw_TMAIN_DATA_BRAKES_F;

        uint16_t raw_TMAIN_DATA_BRAKES_R;

        break;

    case TPERIPH_FL_TPDO_DATA:
        uint16_t raw_TPERIPH_FL_DATA_WHEELSPEED;

        uint16_t raw_TPERIPH_FL_DATA_SUSTRAVEL;

        uint16_t raw_TPERIPH_FL_DATA_STRAIN;

        uint8_t raw_TPERIPH_FL_DATA_SIDE_TIRE_TEMP;

        break;

    case TPERIPH_FR_TPDO_DATA:
        uint16_t raw_TPERIPH_FR_DATA_WHEELSPEED;

        uint16_t raw_TPERIPH_FR_DATA_SUSTRAVEL;

        uint16_t raw_TPERIPH_FR_DATA_STRAIN;

        uint8_t raw_TPERIPH_FR_DATA_SIDE_TIRE_TEMP;

        break;

    case TPERIPH_BL_TPDO_DATA:
        uint16_t raw_TPERIPH_BL_DATA_WHEELSPEED;

        uint16_t raw_TPERIPH_BL_DATA_SUSTRAVEL;

        uint16_t raw_TPERIPH_BL_DATA_STRAIN;

        uint8_t raw_TPERIPH_BL_DATA_SIDE_TIRE_TEMP;

        break;

    case TPERIPH_BR_TPDO_DATA:
        uint16_t raw_TPERIPH_BR_DATA_WHEELSPEED;

        uint16_t raw_TPERIPH_BR_DATA_SUSTRAVEL;

        uint16_t raw_TPERIPH_BR_DATA_STRAIN;

        uint8_t raw_TPERIPH_BR_DATA_SIDE_TIRE_TEMP;

        break;

    case TPERIPH_FL_TPDO_TIRETEMP:
        uint8_t raw_TPERIPH_FL_TIRETEMP_1;

        uint8_t raw_TPERIPH_FL_TIRETEMP_2;

        uint8_t raw_TPERIPH_FL_TIRETEMP_3;

        uint8_t raw_TPERIPH_FL_TIRETEMP_4;

        uint8_t raw_TPERIPH_FL_TIRETEMP_5;

        uint8_t raw_TPERIPH_FL_TIRETEMP_6;

        uint8_t raw_TPERIPH_FL_TIRETEMP_7;

        uint8_t raw_TPERIPH_FL_TIRETEMP_8;

        break;

    case TPERIPH_FR_TPDO_TIRETEMP:
        uint8_t raw_TPERIPH_FR_TIRETEMP_1;

        uint8_t raw_TPERIPH_FR_TIRETEMP_2;

        uint8_t raw_TPERIPH_FR_TIRETEMP_3;

        uint8_t raw_TPERIPH_FR_TIRETEMP_4;

        uint8_t raw_TPERIPH_FR_TIRETEMP_5;

        uint8_t raw_TPERIPH_FR_TIRETEMP_6;

        uint8_t raw_TPERIPH_FR_TIRETEMP_7;

        uint8_t raw_TPERIPH_FR_TIRETEMP_8;

        break;

    case TPERIPH_BL_TPDO_TIRETEMP:
        uint8_t raw_TPERIPH_BL_TIRETEMP_1;

        uint8_t raw_TPERIPH_BL_TIRETEMP_2;

        uint8_t raw_TPERIPH_BL_TIRETEMP_3;

        uint8_t raw_TPERIPH_BL_TIRETEMP_4;

        uint8_t raw_TPERIPH_BL_TIRETEMP_5;

        uint8_t raw_TPERIPH_BL_TIRETEMP_6;

        uint8_t raw_TPERIPH_BL_TIRETEMP_7;

        uint8_t raw_TPERIPH_BL_TIRETEMP_8;

        break;

    case TPERIPH_BR_TPDO_TIRETEMP:
        uint8_t raw_TPERIPH_BR_TIRETEMP_1;

        uint8_t raw_TPERIPH_BR_TIRETEMP_2;

        uint8_t raw_TPERIPH_BR_TIRETEMP_3;

        uint8_t raw_TPERIPH_BR_TIRETEMP_4;

        uint8_t raw_TPERIPH_BR_TIRETEMP_5;

        uint8_t raw_TPERIPH_BR_TIRETEMP_6;

        uint8_t raw_TPERIPH_BR_TIRETEMP_7;

        uint8_t raw_TPERIPH_BR_TIRETEMP_8;

        break;

    case LSS_COMMAND:
        uint8_t raw_COMMAND_COMMAND_SPECIFIER;

        uint8_t raw_COMMAND_MODE;

        break;

    case LSS_RESPONSE:
        uint8_t raw_RESPONSE_COMMAND_SPECIFIER;

        break;

    case SMPC_RPDO_CONTROL:
        uint8_t raw_SMPC_CONTROL_DEST_NODE_ID;

        uint8_t raw_SMPC_CONTROL_ENABLE;

        uint8_t raw_SMPC_CONTROL_CURRENT_10X_MULT;

        uint8_t raw_SMPC_CONTROL_EVSE_OVERRIDE;

        uint32_t raw_SMPC_CONTROL_PACK_VOLTAGE;

        break;

    case SMPC_RPDO_MAX_VOLTAGE_CURRENT:
        uint8_t raw_SMPC_MAX_DEST_NODE_ID;

        uint32_t raw_SMPC_MAX_CHRG_VOLTAGE;

        uint16_t raw_SMPC_MAX_CHRG_CURRENT;

        uint8_t raw_SMPC_MAX_INPUT_EVSE_OVERRIDE;

        break;

    case SMPC_TPDO_STATUS:
        uint8_t raw_SMPC_STATUS_READY;

        uint8_t raw_SMPC_STATUS_CHARGE_COMPLETE;

        uint8_t raw_SMPC_STATUS_OVERTEMP_FAULT;

        uint8_t raw_SMPC_STATUS_OVERCURRENT_FAULT;

        uint8_t raw_SMPC_STATUS_OVERVOLTAGE_FAULT;

        uint8_t raw_SMPC_STATUS_UNDERVOLTAGE_FAULT;

        uint8_t raw_SMPC_STATUS_INTERNAL_FAULT;

        uint8_t raw_SMPC_STATUS_CHARGING;

        uint8_t raw_SMPC_STATUS_CHARGER_UNPLUGGED;

        uint8_t raw_SMPC_STATUS_REQUEST_EXCLUSIVE;

        uint8_t raw_SMPC_STATUS_J1772_DISCONN;

        uint32_t raw_SMPC_STATUS_VOLTAGE_MV;

        uint16_t raw_SMPC_STATUS_CURRENT_MA;

        break;

    case SMPC_TPDO_MAX_VOLTAGE_CURRENT:
        uint32_t raw_SMPC_MAX2_MAX_VOLTAGE_MV;

        uint16_t raw_SMPC_MAX2_MAX_CURRENT_MA;

        uint8_t raw_SMPC_MAX2_EVSE_CURRENT;

        uint8_t raw_SMPC_MAX2_INPUT_CURR_LIM;

        break;

    case SMPC_TPDO_INPUT_AC:
        uint8_t raw_SMPC_INPUT_AC_INPUT_VOLTAGE;

        uint8_t raw_SMPC_INPUT_J1772_CONNECTED;

        uint8_t raw_SMPC_INPUT_J1772_DISCONNECTED;

        uint8_t raw_SMPC_INPUT_J1772_TRIGGERED;

        uint16_t raw_SMPC_INPUT_AC_INPUT_CURRENT_MA;

        uint8_t raw_SMPC_INPUT_AC_INPUT_FREQUENCY_HZ;

        uint8_t raw_SMPC_INPUT_MAX_CHARGER_TEMP_C;

        break;

    case SMPC_TPDO_SERIAL_PART_NUM:
        uint16_t raw_SMPC_SER_PART_NUMBER;

        uint32_t raw_SMPC_SER_SERIAL_NUMBER;

        uint16_t raw_SMPC_SER_FIRMWARE_VER;

        break;
    };
}

} // namespace fsdaq

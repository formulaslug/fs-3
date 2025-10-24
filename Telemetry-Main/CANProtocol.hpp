// CANProtocol.hpp
// Types for extracting CAN messages from the CAN bus
// (Refer to ../CANbus.dbc for the protocol)
// Written by: Njeri Gachoka

#ifndef CANPROTOCOL_HPP
#define CANPROTOCOL_HPP

#include "mbed.h"
#include <cstdint>

namespace CAN_ID {
    // ACC
    constexpr uint32_t ACC_STATUS          = 0x188;
    constexpr uint32_t ACC_SEG0_VOLTS      = 0x191;
    constexpr uint32_t ACC_SEG1_VOLTS      = 0x192;
    constexpr uint32_t ACC_SEG2_VOLTS      = 0x193;
    constexpr uint32_t ACC_SEG3_VOLTS      = 0x194;
    constexpr uint32_t ACC_SEG4_VOLTS      = 0x195;
    constexpr uint32_t ACC_POWER           = 0x288;
    constexpr uint32_t ACC_SEG0_TEMPS      = 0x291;
    constexpr uint32_t ACC_SEG1_TEMPS      = 0x292;
    constexpr uint32_t ACC_SEG2_TEMPS      = 0x293;
    constexpr uint32_t ACC_SEG3_TEMPS      = 0x294;
    constexpr uint32_t ACC_SEG4_TEMPS      = 0x295;
    // ETC
    constexpr uint32_t ETC_STATUS          = 0x189;
    // PDB
    constexpr uint32_t PDB_POWER_A         = 0x19A;
    constexpr uint32_t PDB_POWER_B         = 0x29B;
    // SME
    constexpr uint32_t SME_THROTTLE_DEMAND = 0x186;
    constexpr uint32_t SME_MAX_CURRENTS    = 0x286;
    constexpr uint32_t SME_TRQSPD          = 0x482;
    constexpr uint32_t SME_TEMP            = 0x682;
    // TMAIN
    constexpr uint32_t TMAIN_DATA          = 0x1A0;
    // TPERIPH
    constexpr uint32_t TPERIPH_BL_DATA     = 0x1A4;
    constexpr uint32_t TPERIPH_BL_TIRETEMP = 0x2A3;
    constexpr uint32_t TPERIPH_BR_DATA     = 0x1A5;
    constexpr uint32_t TPERIPH_BR_TIRETEMP = 0x2A4;
    constexpr uint32_t TPERIPH_FL_DATA     = 0x1A2;
    constexpr uint32_t TPERIPH_FL_TIRETEMP = 0x2A1;
    constexpr uint32_t TPERIPH_FR_DATA     = 0x1A3;
    constexpr uint32_t TPERIPH_FR_TIRETEMP = 0x2A2;
    // VDM
    constexpr uint32_t VDM_GPS_LAT_LONG    = 0x000A0000;
    constexpr uint32_t VDM_GPS_DATA        = 0x000A0001;
    constexpr uint32_t VDM_DATE_TIME       = 0x000A0002;
    constexpr uint32_t VDM_ACCELERATION    = 0x000A0003;
    constexpr uint32_t VDM_YAW_RATE        = 0x000A0004;
}

struct ACC_STATUS_t {
    uint8_t BMS_FAULT           : 1;   // bit 0
    uint8_t IMD_FAULT           : 1;   // bit 1
    uint8_t SHUTDOWN_STATE      : 1;   // bit 2
    uint8_t PRECHARGE_DONE      : 1;   // bit 3
    uint8_t PRECHARGING         : 1;   // bit 4
    uint8_t CHARGING            : 1;   // bit 5
    uint8_t padding_6_7         : 2;   // bits 6-7 (padding for gaps)
    uint8_t CELL_TOO_LOW        : 1;   // bit 8
    uint8_t CELL_TOO_HIGH       : 1;   // bit 9
    uint8_t TEMP_TOO_LOW        : 1;   // bit 10
    uint8_t TEMP_TOO_HIGH       : 1;   // bit 11
    uint8_t TEMP_TOO_HIGH_CRG   : 1;   // bit 12
    uint8_t padding_13_15       : 3;   // bits 13-15 (padding to complete byte)
    uint16_t GLV_VOLTAGE;              // bits 16-31
    uint32_t CELL_FAULT_INDEX;         // bits 32-63
};

struct ACC_SEG_VOLTS_t {
    uint8_t VOLTS_CELL0;               // bits 0-7
    uint8_t VOLTS_CELL1;               // bits 8-15
    uint8_t VOLTS_CELL2;               // bits 16-23
    uint8_t VOLTS_CELL3;               // bits 24-31
    uint8_t VOLTS_CELL4;               // byte 32-39
    uint8_t VOLTS_CELL5;               // byte 40-47
};

struct ACC_POWER_t {
    uint16_t PACK_VOLTAGE;             // bits 0-15
    uint8_t  SOC;                      // bits 16-23
    uint16_t CURRENT;                  // bits 24-39
};

struct ACC_SEG_TEMPS_t {
    uint8_t TEMPS_CELL0;               // bits 0-7
    uint8_t TEMPS_CELL1;               // bits 8-15
    uint8_t TEMPS_CELL2;               // bits 16-23
    uint8_t TEMPS_CELL3;               // bits 24-31
    uint8_t TEMPS_CELL4;               // bits 32-39
    uint8_t TEMPS_CELL5;               // bits 40-47
};

struct ETC_STATUS_t {
    uint16_t HE1;                      // bits 0-15
    uint16_t HE2;                      // bits 16-31
    uint16_t BRAKE_SENSE_VOLTAGE;      // bits 32-47
    uint8_t PEDAL_TRAVEL;              // bits 48-55
    uint8_t RTD_BUTTON     : 1;        // bit 56
    uint8_t RTDS           : 1;        // bit 57
    uint8_t REVERSE        : 1;        // bit 58
    uint8_t BRAKELIGHT     : 1;        // bit 59
    uint8_t RTD            : 1;        // bit 60
    uint8_t IMPLAUSIBILITY : 1;        // bit 61
    uint8_t TS_ACTIVE      : 1;        // bit 62
};

struct PDB_POWER_A_t {
    uint16_t GLV_VOLTAGE;              // bits 0-15
    uint8_t  CURRENT_SHUTDOWN;         // bits 16-23
    uint8_t  CURRENT_ACC;              // bits 24-31
    uint8_t  CURRENT_ETC;              // bits 32-39
    uint8_t  CURRENT_BPS;              // bits 40-47
    uint8_t  CURRENT_TRACTIVE;         // bits 48-55
    uint8_t  CURRENT_BSPD;             // bits 56-63
};

struct PDB_POWER_B_t {
    uint8_t  CURRENT_TELEMETRY;        // bits 0-7
    uint8_t  CURRENT_PDB;              // bits 8-15
    uint8_t  CURRENT_DASH;             // bits 16-23
    uint8_t  CURRENT_RTML;             // bits 24-31
    uint8_t  CURRENT_EXTRA_1;          // bits 32-39
    uint8_t  CURRENT_EXTRA_2;          // bits 40-47
};

struct SME_THROTTLE_DEMAND_t {
    uint16_t TORQUE_DEMAND;            // bits 0-15
    uint16_t MAX_SPEED;                // bits 16-31
    uint8_t FORWARD : 1;               // bit 32
    uint8_t REVERSE : 1;               // bit 33
    uint8_t padding_34 : 1;            // bit 34 (used for alignment)
    uint8_t POWER_READY : 1;           // bit 35
    uint8_t padding2 : 4;              // bits 36-39
    uint8_t MBB_ALIVE: 4;              // bits 40-43
};

struct SME_TRQSPD_t {
    uint16_t SPEED;                    // bits 0-15
    uint16_t TORQUE;                   // bits 16-31
    uint8_t  SOC_LOW_TRACTION    : 1;  // bit 32
    uint8_t  SOC_LOW_HYDRAULIC   : 1;  // bit 33
    uint8_t  REVERSE             : 1;  // bit 34
    uint8_t  FORWARD             : 1;  // bit 35
    uint8_t  PARK_BRAKE          : 1;  // bit 36
    uint8_t  PEDAL_BRAKE         : 1;  // bit 37
    uint8_t  CONTROLLER_OVERTEMP : 1;  // bit 38
    uint8_t  KEY_SWITCH_OVERVOLT : 1;  // bit 39
    uint8_t  KEY_SWITCH_UNDERVOLT: 1;  // bit 40
    uint8_t  RUNNING             : 1;  // bit 41
    uint8_t  TRACTION            : 1;  // bit 42
    uint8_t  HYDRAULIC           : 1;  // bit 43
    uint8_t  POWERING_ENABLED    : 1;  // bit 44
    uint8_t  POWERING_READY      : 1;  // bit 45
    uint8_t  PRECHARGING         : 1;  // bit 46
    uint8_t  CONTACTOR_CLOSED    : 1;  // bit 47
    uint16_t MOTOR_FLAGS;              // bits 48-63
};

struct SME_TEMP_t {
    uint8_t  MOTOR_TEMP;               // bits 0-7
    uint8_t  CONTROLLER_TEMP;          // bits 8-15
    uint16_t DC_BUS_V;                 // bits 16-31
    uint8_t  FAULT_CODE;               // bits 32-39
    uint8_t  FAULT_LEVEL;              // bits 40-47
    uint16_t BUS_CURRENT;              // bits 48-63
};

struct SME_MAX_CURRENTS_t {
    uint16_t CHARGE_CURRENT;           // bits 0-15
    uint16_t DISCHARGE_CURRENT;        // bits 16-31        
};

struct TMAIN_DATA_t {
    uint16_t BRAKES_F;                 // bits 0-15
    uint16_t BRAKES_R;                 // bits 16-31
};

struct TPERIPH_DATA_t {
    uint16_t WHEELSPEED;               // bits 0-15
    uint16_t SUSTRAVEL;                // bits 16-31
    uint16_t STRAIN;                   // bits 32-47
    uint8_t  SIDE_TIRE_TEMP;           // bits 48-55
};

struct TPERIPH_TIRETEMP_t {
    uint8_t TIRETEMP_1;                // bits 0-7
    uint8_t TIRETEMP_2;                // bits 8-15
    uint8_t TIRETEMP_3;                // bits 16-23
    uint8_t TIRETEMP_4;                // bits 24-31
    uint8_t TIRETEMP_5;                // bits 32-39
    uint8_t TIRETEMP_6;                // bits 40-47
    uint8_t TIRETEMP_7;                // bits 48-55
    uint8_t TIRETEMP_8;                // bits 56-63

};

struct VDM_GPS_LAT_LONG_t {
    float LATITUDE;                  // bits 7-38
    float LONGITUDE;                 // bits 39-50
};

struct VDM_GPS_DATA_t {
    uint16_t SPEED;                    // bits 7-22
    int16_t ALTITUDE;                  // bits 23-38
    uint16_t TRUE_COURSE;              // bits 39-54
    uint8_t  SATELLITES_IN_USE;        // bits 55-62
    uint8_t  VALID1;                   // bits 63-70
};

struct VDM_DATE_TIME_t {
    uint8_t VALID2;                    // bits 7-14
    uint8_t UTC_DATE_YEAR;             // bits 15-22
    uint8_t UTC_DATE_MONTH;            // bits 23-30
    uint8_t UTC_DATE_DAY;              // bits 31-38
    uint8_t padding;
    uint8_t UTC_TIME_HOURS;            // bits 47-54
    uint8_t UTC_TIME_MINUTES;          // bits 55-62
    uint8_t UTC_TIME_SECONDS;          // bits 63-70
};

struct VDM_ACCELERATION_t {
    int16_t X;                         // bits 7-22
    int16_t Y;                         // bits 23-38
    int16_t Z;                         // bits 39-54
};

struct VDM_YAW_RATE_t {
    int16_t X;                         // bits 7-22
    int16_t Y;                         // bits 23-38
    int16_t Z;                         // bits 39-54
};

#endif // CANPROTOCOL_HPP

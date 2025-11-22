#pragma once

#include "mbed.h"
#include "encoder_generated.hpp"

namespace fsdaq {

enum MessageID : uint32_t {
    SYNC = 0x80,
    SME_RPDO_Throttle_Demand = 0x186,
    SME_RPDO_Max_Currents = 0x286,
    SME_TPDO_Torque_speed = 0x482,
    SME_TPDO_Temperature = 0x682,
    ACC_TPDO_STATUS = 0x188,
    ACC_TPDO_POWER = 0x288,
    ACC_TPDO_SEG0_VOLTS = 0x191,
    ACC_TPDO_SEG0_TEMPS = 0x291,
    ACC_TPDO_SEG1_VOLTS = 0x192,
    ACC_TPDO_SEG1_TEMPS = 0x292,
    ACC_TPDO_SEG2_VOLTS = 0x193,
    ACC_TPDO_SEG2_TEMPS = 0x293,
    ACC_TPDO_SEG3_VOLTS = 0x194,
    ACC_TPDO_SEG3_TEMPS = 0x294,
    ACC_TPDO_SEG4_VOLTS = 0x195,
    ACC_TPDO_SEG4_TEMPS = 0x295,
    VDM_GPS_LAT_LONG = 0xa0000,
    VDM_GPS_DATA = 0xa0001,
    VDM_DATE_TIME = 0xa0002,
    VDM_ACCELERATION = 0xa0003,
    VDM_YAW_RATE = 0xa0004,
    ETC_TPDO_STATUS = 0x189,
    PDB_TPDO_POWER_A = 0x19a,
    PDB_TPDO_POWER_B = 0x29a,
    TMAIN_TPDO_DATA = 0x1a0,
    TPERIPH_FL_TPDO_DATA = 0x1a2,
    TPERIPH_FR_TPDO_DATA = 0x1a3,
    TPERIPH_BL_TPDO_DATA = 0x1a4,
    TPERIPH_BR_TPDO_DATA = 0x1a5,
    TPERIPH_FL_TPDO_TIRETEMP = 0x2a1,
    TPERIPH_FR_TPDO_TIRETEMP = 0x2a2,
    TPERIPH_BL_TPDO_TIRETEMP = 0x2a3,
    TPERIPH_BR_TPDO_TIRETEMP = 0x2a4,
    LSS_COMMAND = 0x7e5,
    LSS_RESPONSE = 0x7e4,
    SMPC_RPDO_CONTROL = 0x206,
    SMPC_RPDO_MAX_VOLTAGE_CURRENT = 0x306,
    SMPC_TPDO_STATUS = 0x190,
    SMPC_TPDO_MAX_VOLTAGE_CURRENT = 0x290,
    SMPC_TPDO_INPUT_AC = 0x390,
    SMPC_TPDO_SERIAL_PART_NUM = 0x490,
};

class CANProcessor {
public:
    std::unique_ptr<DataRow> current_row;

    CANProcessor(CAN &can);

    void process_message();

private:
    CAN &can;
};

} // namespace fsdaq

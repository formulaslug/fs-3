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
        uint16_t SME_THROTL_TorqueDemand;
        uint16_t SME_THROTL_MaxSpeed;

        SME_THROTL_TorqueDemand = (uint16_t)((uint16_t)(msg.data[0] & 0xffu) >> 0u); // int16_t *1 +0
        SME_THROTL_TorqueDemand |= (uint16_t)((uint16_t)(msg.data[1] & 0xffu) << 8u); // int16_t *1 +0
        current_row->SME_THROTL_TorqueDemand = (int16_t)SME_THROTL_TorqueDemand;

        SME_THROTL_MaxSpeed = (uint16_t)((uint16_t)(msg.data[2] & 0xffu) >> 0u); // int16_t *1 +0
        SME_THROTL_MaxSpeed |= (uint16_t)((uint16_t)(msg.data[3] & 0xffu) << 8u); // int16_t *1 +0
        current_row->SME_THROTL_MaxSpeed = (int16_t)SME_THROTL_MaxSpeed;

        current_row->SME_THROTL_Forward = (uint8_t)((uint8_t)(msg.data[4] & 0x01u) >> 0u); // bool *1 +0

        current_row->SME_THROTL_Reverse = (uint8_t)((uint8_t)(msg.data[4] & 0x02u) >> 1u); // bool *1 +0

        current_row->SME_THROTL_PowerReady = (uint8_t)((uint8_t)(msg.data[4] & 0x08u) >> 3u); // bool *1 +0

        current_row->SME_THROTL_MBB_Alive = (uint8_t)((uint8_t)(msg.data[5] & 0x0fu) >> 0u); // uint8_t *1 +0

        break;

    case SME_RPDO_Max_Currents:
        uint16_t SME_CURRLIM_ChargeCurrentLim;
        uint16_t SME_CURRLIM_DischargeCurrentLim;

        SME_CURRLIM_ChargeCurrentLim = (uint16_t)((uint16_t)(msg.data[0] & 0xffu) >> 0u); // uint16_t *1 +0
        SME_CURRLIM_ChargeCurrentLim |= (uint16_t)((uint16_t)(msg.data[1] & 0xffu) << 8u); // uint16_t *1 +0

        SME_CURRLIM_DischargeCurrentLim = (uint16_t)((uint16_t)(msg.data[2] & 0xffu) >> 0u); // uint16_t *1 +0
        SME_CURRLIM_DischargeCurrentLim |= (uint16_t)((uint16_t)(msg.data[3] & 0xffu) << 8u); // uint16_t *1 +0

        break;

    case SME_TPDO_Torque_speed:
        uint16_t SME_TRQSPD_Speed;
        uint16_t SME_TRQSPD_Torque;
        uint16_t SME_TRQSPD_MotorFlags;

        SME_TRQSPD_Speed = (uint16_t)((uint16_t)(msg.data[0] & 0xffu) >> 0u); // uint16_t *1 +0
        SME_TRQSPD_Speed |= (uint16_t)((uint16_t)(msg.data[1] & 0xffu) << 8u); // uint16_t *1 +0

        SME_TRQSPD_Torque = (uint16_t)((uint16_t)(msg.data[2] & 0xffu) >> 0u); // int16_t *1 +0
        SME_TRQSPD_Torque |= (uint16_t)((uint16_t)(msg.data[3] & 0xffu) << 8u); // int16_t *1 +0
        current_row->SME_TRQSPD_Torque = (int16_t)SME_TRQSPD_Torque;

        current_row->SME_TRQSPD_SOC_Low_Traction = (uint8_t)((uint8_t)(msg.data[4] & 0x01u) >> 0u); // bool *1 +0

        current_row->SME_TRQSPD_SOC_Low_Hydraulic = (uint8_t)((uint8_t)(msg.data[4] & 0x02u) >> 1u); // bool *1 +0

        current_row->SME_TRQSPD_Reverse = (uint8_t)((uint8_t)(msg.data[4] & 0x04u) >> 2u); // bool *1 +0

        current_row->SME_TRQSPD_Forward = (uint8_t)((uint8_t)(msg.data[4] & 0x08u) >> 3u); // bool *1 +0

        current_row->SME_TRQSPD_Park_Brake = (uint8_t)((uint8_t)(msg.data[4] & 0x10u) >> 4u); // bool *1 +0

        current_row->SME_TRQSPD_Pedal_Brake = (uint8_t)((uint8_t)(msg.data[4] & 0x20u) >> 5u); // bool *1 +0

        current_row->SME_TRQSPD_Controller_Overtermp = (uint8_t)((uint8_t)(msg.data[4] & 0x40u) >> 6u); // bool *1 +0

        current_row->SME_TRQSPD_Key_switch_overvolt = (uint8_t)((uint8_t)(msg.data[4] & 0x80u) >> 7u); // bool *1 +0

        current_row->SME_TRQSPD_Key_switch_undervolt = (uint8_t)((uint8_t)(msg.data[5] & 0x01u) >> 0u); // bool *1 +0

        current_row->SME_TRQSPD_Running = (uint8_t)((uint8_t)(msg.data[5] & 0x02u) >> 1u); // bool *1 +0

        current_row->SME_TRQSPD_Traction = (uint8_t)((uint8_t)(msg.data[5] & 0x04u) >> 2u); // bool *1 +0

        current_row->SME_TRQSPD_Hydraulic = (uint8_t)((uint8_t)(msg.data[5] & 0x08u) >> 3u); // bool *1 +0

        current_row->SME_TRQSPD_Powering_Enabled = (uint8_t)((uint8_t)(msg.data[5] & 0x10u) >> 4u); // bool *1 +0

        current_row->SME_TRQSPD_Powering_Ready = (uint8_t)((uint8_t)(msg.data[5] & 0x20u) >> 5u); // bool *1 +0

        current_row->SME_TRQSPD_Precharging = (uint8_t)((uint8_t)(msg.data[5] & 0x40u) >> 6u); // bool *1 +0

        current_row->SME_TRQSPD_contactor_closed = (uint8_t)((uint8_t)(msg.data[5] & 0x80u) >> 7u); // bool *1 +0

        SME_TRQSPD_MotorFlags = (uint16_t)((uint16_t)(msg.data[6] & 0xffu) >> 0u); // uint16_t *1 +0
        SME_TRQSPD_MotorFlags |= (uint16_t)((uint16_t)(msg.data[7] & 0xffu) << 8u); // uint16_t *1 +0

        break;

    case SME_TPDO_Temperature:
        uint16_t SME_TEMP_DC_Bus_V;
        uint16_t SME_TEMP_BusCurrent;

        current_row->SME_TEMP_MotorTemperature = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u) + -40; // uint8_t *1 +-40

        current_row->SME_TEMP_ControllerTemperature = (uint8_t)((uint8_t)(msg.data[1] & 0xffu) >> 0u) + -40; // uint8_t *1 +-40

        SME_TEMP_DC_Bus_V = (uint16_t)((uint16_t)(msg.data[2] & 0xffu) >> 0u); // float *0.1 +0
        SME_TEMP_DC_Bus_V |= (uint16_t)((uint16_t)(msg.data[3] & 0xffu) << 8u); // float *0.1 +0

        current_row->SME_TEMP_FaultCode = (uint8_t)((uint8_t)(msg.data[4] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->SME_TEMP_FaultLevel = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u); // uint8_t *1 +0

        SME_TEMP_BusCurrent = (uint16_t)((uint16_t)(msg.data[6] & 0xffu) >> 0u); // float *0.1 +0
        SME_TEMP_BusCurrent |= (uint16_t)((uint16_t)(msg.data[7] & 0xffu) << 8u); // float *0.1 +0
        current_row->SME_TEMP_BusCurrent = (int16_t)SME_TEMP_BusCurrent;

        break;

    case ACC_TPDO_STATUS:
        uint16_t ACC_STATUS_GLV_VOLTAGE;
        uint32_t ACC_STATUS_CELL_FAULT_INDEX;

        current_row->ACC_STATUS_BMS_FAULT = (uint8_t)((uint8_t)(msg.data[0] & 0x01u) >> 0u); // bool *1 +0

        current_row->ACC_STATUS_IMD_FAULT = (uint8_t)((uint8_t)(msg.data[0] & 0x02u) >> 1u); // bool *1 +0

        current_row->ACC_STATUS_SHUTDOWN_STATE = (uint8_t)((uint8_t)(msg.data[0] & 0x04u) >> 2u); // bool *1 +0

        current_row->ACC_STATUS_PRECHARGE_DONE = (uint8_t)((uint8_t)(msg.data[0] & 0x08u) >> 3u); // bool *1 +0

        current_row->ACC_STATUS_PRECHARGING = (uint8_t)((uint8_t)(msg.data[0] & 0x10u) >> 4u); // bool *1 +0

        current_row->ACC_STATUS_CHARGING = (uint8_t)((uint8_t)(msg.data[0] & 0x20u) >> 5u); // bool *1 +0

        current_row->ACC_STATUS_CELL_TOO_LOW = (uint8_t)((uint8_t)(msg.data[1] & 0x01u) >> 0u); // bool *1 +0

        current_row->ACC_STATUS_CELL_TOO_HIGH = (uint8_t)((uint8_t)(msg.data[1] & 0x02u) >> 1u); // bool *1 +0

        current_row->ACC_STATUS_TEMP_TOO_LOW = (uint8_t)((uint8_t)(msg.data[1] & 0x04u) >> 2u); // bool *1 +0

        current_row->ACC_STATUS_TEMP_TOO_HIGH = (uint8_t)((uint8_t)(msg.data[1] & 0x08u) >> 3u); // bool *1 +0

        current_row->ACC_STATUS_TEMP_TOO_HIGH_CRG = (uint8_t)((uint8_t)(msg.data[1] & 0x10u) >> 4u); // bool *1 +0

        current_row->ACC_STATUS_BALANCING = (uint8_t)((uint8_t)(msg.data[1] & 0x20u) >> 5u); // bool *1 +0

        ACC_STATUS_GLV_VOLTAGE = (uint16_t)((uint16_t)(msg.data[2] & 0xffu) >> 0u); // uint16_t *1 +0
        ACC_STATUS_GLV_VOLTAGE |= (uint16_t)((uint16_t)(msg.data[3] & 0xffu) << 8u); // uint16_t *1 +0

        ACC_STATUS_CELL_FAULT_INDEX = (uint32_t)((uint32_t)(msg.data[4] & 0xffu) >> 0u); // uint32_t *1 +0
        ACC_STATUS_CELL_FAULT_INDEX |= (uint32_t)((uint32_t)(msg.data[5] & 0xffu) << 8u); // uint32_t *1 +0
        ACC_STATUS_CELL_FAULT_INDEX |= (uint32_t)((uint32_t)(msg.data[6] & 0xffu) << 16u); // uint32_t *1 +0
        ACC_STATUS_CELL_FAULT_INDEX |= (uint32_t)((uint32_t)(msg.data[7] & 0xffu) << 24u); // uint32_t *1 +0

        break;

    case ACC_TPDO_POWER:
        uint16_t ACC_POWER_PACK_VOLTAGE;
        uint16_t ACC_POWER_CURRENT;

        ACC_POWER_PACK_VOLTAGE = (uint16_t)((uint16_t)(msg.data[0] & 0xffu) >> 0u); // float *0.01 +0
        ACC_POWER_PACK_VOLTAGE |= (uint16_t)((uint16_t)(msg.data[1] & 0xffu) << 8u); // float *0.01 +0

        current_row->ACC_POWER_SOC = (uint8_t)((uint8_t)(msg.data[2] & 0xffu) >> 0u); // uint8_t *1 +0

        ACC_POWER_CURRENT = (uint16_t)((uint16_t)(msg.data[3] & 0xffu) >> 0u); // float *0.1 +0
        ACC_POWER_CURRENT |= (uint16_t)((uint16_t)(msg.data[4] & 0xffu) << 8u); // float *0.1 +0
        current_row->ACC_POWER_CURRENT = (int16_t)ACC_POWER_CURRENT;

        break;

    case ACC_TPDO_SEG0_VOLTS:
        current_row->ACC_SEG0_VOLTS_CELL0 = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG0_VOLTS_CELL1 = (uint8_t)((uint8_t)(msg.data[1] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG0_VOLTS_CELL2 = (uint8_t)((uint8_t)(msg.data[2] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG0_VOLTS_CELL3 = (uint8_t)((uint8_t)(msg.data[3] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG0_VOLTS_CELL4 = (uint8_t)((uint8_t)(msg.data[4] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG0_VOLTS_CELL5 = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        break;

    case ACC_TPDO_SEG0_TEMPS:
        uint8_t ACC_SEG0_TEMPS_CELL0;
        uint8_t ACC_SEG0_TEMPS_CELL1;
        uint8_t ACC_SEG0_TEMPS_CELL2;
        uint8_t ACC_SEG0_TEMPS_CELL3;
        uint8_t ACC_SEG0_TEMPS_CELL4;
        uint8_t ACC_SEG0_TEMPS_CELL5;

        ACC_SEG0_TEMPS_CELL0 = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG0_TEMPS_CELL0 = (int8_t)ACC_SEG0_TEMPS_CELL0;

        ACC_SEG0_TEMPS_CELL1 = (uint8_t)((uint8_t)(msg.data[1] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG0_TEMPS_CELL1 = (int8_t)ACC_SEG0_TEMPS_CELL1;

        ACC_SEG0_TEMPS_CELL2 = (uint8_t)((uint8_t)(msg.data[2] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG0_TEMPS_CELL2 = (int8_t)ACC_SEG0_TEMPS_CELL2;

        ACC_SEG0_TEMPS_CELL3 = (uint8_t)((uint8_t)(msg.data[3] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG0_TEMPS_CELL3 = (int8_t)ACC_SEG0_TEMPS_CELL3;

        ACC_SEG0_TEMPS_CELL4 = (uint8_t)((uint8_t)(msg.data[4] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG0_TEMPS_CELL4 = (int8_t)ACC_SEG0_TEMPS_CELL4;

        ACC_SEG0_TEMPS_CELL5 = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG0_TEMPS_CELL5 = (int8_t)ACC_SEG0_TEMPS_CELL5;

        break;

    case ACC_TPDO_SEG1_VOLTS:
        current_row->ACC_SEG1_VOLTS_CELL0 = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG1_VOLTS_CELL1 = (uint8_t)((uint8_t)(msg.data[1] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG1_VOLTS_CELL2 = (uint8_t)((uint8_t)(msg.data[2] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG1_VOLTS_CELL3 = (uint8_t)((uint8_t)(msg.data[3] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG1_VOLTS_CELL4 = (uint8_t)((uint8_t)(msg.data[4] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG1_VOLTS_CELL5 = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        break;

    case ACC_TPDO_SEG1_TEMPS:
        uint8_t ACC_SEG1_TEMPS_CELL0;
        uint8_t ACC_SEG1_TEMPS_CELL1;
        uint8_t ACC_SEG1_TEMPS_CELL2;
        uint8_t ACC_SEG1_TEMPS_CELL3;
        uint8_t ACC_SEG1_TEMPS_CELL4;
        uint8_t ACC_SEG1_TEMPS_CELL5;

        ACC_SEG1_TEMPS_CELL0 = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG1_TEMPS_CELL0 = (int8_t)ACC_SEG1_TEMPS_CELL0;

        ACC_SEG1_TEMPS_CELL1 = (uint8_t)((uint8_t)(msg.data[1] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG1_TEMPS_CELL1 = (int8_t)ACC_SEG1_TEMPS_CELL1;

        ACC_SEG1_TEMPS_CELL2 = (uint8_t)((uint8_t)(msg.data[2] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG1_TEMPS_CELL2 = (int8_t)ACC_SEG1_TEMPS_CELL2;

        ACC_SEG1_TEMPS_CELL3 = (uint8_t)((uint8_t)(msg.data[3] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG1_TEMPS_CELL3 = (int8_t)ACC_SEG1_TEMPS_CELL3;

        ACC_SEG1_TEMPS_CELL4 = (uint8_t)((uint8_t)(msg.data[4] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG1_TEMPS_CELL4 = (int8_t)ACC_SEG1_TEMPS_CELL4;

        ACC_SEG1_TEMPS_CELL5 = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG1_TEMPS_CELL5 = (int8_t)ACC_SEG1_TEMPS_CELL5;

        break;

    case ACC_TPDO_SEG2_VOLTS:
        current_row->ACC_SEG2_VOLTS_CELL0 = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG2_VOLTS_CELL1 = (uint8_t)((uint8_t)(msg.data[1] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG2_VOLTS_CELL2 = (uint8_t)((uint8_t)(msg.data[2] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG2_VOLTS_CELL3 = (uint8_t)((uint8_t)(msg.data[3] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG2_VOLTS_CELL4 = (uint8_t)((uint8_t)(msg.data[4] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG2_VOLTS_CELL5 = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        break;

    case ACC_TPDO_SEG2_TEMPS:
        uint8_t ACC_SEG2_TEMPS_CELL0;
        uint8_t ACC_SEG2_TEMPS_CELL1;
        uint8_t ACC_SEG2_TEMPS_CELL2;
        uint8_t ACC_SEG2_TEMPS_CELL3;
        uint8_t ACC_SEG2_TEMPS_CELL4;
        uint8_t ACC_SEG2_TEMPS_CELL5;

        ACC_SEG2_TEMPS_CELL0 = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG2_TEMPS_CELL0 = (int8_t)ACC_SEG2_TEMPS_CELL0;

        ACC_SEG2_TEMPS_CELL1 = (uint8_t)((uint8_t)(msg.data[1] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG2_TEMPS_CELL1 = (int8_t)ACC_SEG2_TEMPS_CELL1;

        ACC_SEG2_TEMPS_CELL2 = (uint8_t)((uint8_t)(msg.data[2] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG2_TEMPS_CELL2 = (int8_t)ACC_SEG2_TEMPS_CELL2;

        ACC_SEG2_TEMPS_CELL3 = (uint8_t)((uint8_t)(msg.data[3] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG2_TEMPS_CELL3 = (int8_t)ACC_SEG2_TEMPS_CELL3;

        ACC_SEG2_TEMPS_CELL4 = (uint8_t)((uint8_t)(msg.data[4] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG2_TEMPS_CELL4 = (int8_t)ACC_SEG2_TEMPS_CELL4;

        ACC_SEG2_TEMPS_CELL5 = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG2_TEMPS_CELL5 = (int8_t)ACC_SEG2_TEMPS_CELL5;

        break;

    case ACC_TPDO_SEG3_VOLTS:
        current_row->ACC_SEG3_VOLTS_CELL0 = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG3_VOLTS_CELL1 = (uint8_t)((uint8_t)(msg.data[1] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG3_VOLTS_CELL2 = (uint8_t)((uint8_t)(msg.data[2] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG3_VOLTS_CELL3 = (uint8_t)((uint8_t)(msg.data[3] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG3_VOLTS_CELL4 = (uint8_t)((uint8_t)(msg.data[4] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG3_VOLTS_CELL5 = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        break;

    case ACC_TPDO_SEG3_TEMPS:
        uint8_t ACC_SEG3_TEMPS_CELL0;
        uint8_t ACC_SEG3_TEMPS_CELL1;
        uint8_t ACC_SEG3_TEMPS_CELL2;
        uint8_t ACC_SEG3_TEMPS_CELL3;
        uint8_t ACC_SEG3_TEMPS_CELL4;
        uint8_t ACC_SEG3_TEMPS_CELL5;

        ACC_SEG3_TEMPS_CELL0 = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG3_TEMPS_CELL0 = (int8_t)ACC_SEG3_TEMPS_CELL0;

        ACC_SEG3_TEMPS_CELL1 = (uint8_t)((uint8_t)(msg.data[1] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG3_TEMPS_CELL1 = (int8_t)ACC_SEG3_TEMPS_CELL1;

        ACC_SEG3_TEMPS_CELL2 = (uint8_t)((uint8_t)(msg.data[2] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG3_TEMPS_CELL2 = (int8_t)ACC_SEG3_TEMPS_CELL2;

        ACC_SEG3_TEMPS_CELL3 = (uint8_t)((uint8_t)(msg.data[3] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG3_TEMPS_CELL3 = (int8_t)ACC_SEG3_TEMPS_CELL3;

        ACC_SEG3_TEMPS_CELL4 = (uint8_t)((uint8_t)(msg.data[4] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG3_TEMPS_CELL4 = (int8_t)ACC_SEG3_TEMPS_CELL4;

        ACC_SEG3_TEMPS_CELL5 = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG3_TEMPS_CELL5 = (int8_t)ACC_SEG3_TEMPS_CELL5;

        break;

    case ACC_TPDO_SEG4_VOLTS:
        current_row->ACC_SEG4_VOLTS_CELL0 = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG4_VOLTS_CELL1 = (uint8_t)((uint8_t)(msg.data[1] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG4_VOLTS_CELL2 = (uint8_t)((uint8_t)(msg.data[2] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG4_VOLTS_CELL3 = (uint8_t)((uint8_t)(msg.data[3] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG4_VOLTS_CELL4 = (uint8_t)((uint8_t)(msg.data[4] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        current_row->ACC_SEG4_VOLTS_CELL5 = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u) * 0.01 + 2; // float *0.01 +2

        break;

    case ACC_TPDO_SEG4_TEMPS:
        uint8_t ACC_SEG4_TEMPS_CELL0;
        uint8_t ACC_SEG4_TEMPS_CELL1;
        uint8_t ACC_SEG4_TEMPS_CELL2;
        uint8_t ACC_SEG4_TEMPS_CELL3;
        uint8_t ACC_SEG4_TEMPS_CELL4;
        uint8_t ACC_SEG4_TEMPS_CELL5;

        ACC_SEG4_TEMPS_CELL0 = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG4_TEMPS_CELL0 = (int8_t)ACC_SEG4_TEMPS_CELL0;

        ACC_SEG4_TEMPS_CELL1 = (uint8_t)((uint8_t)(msg.data[1] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG4_TEMPS_CELL1 = (int8_t)ACC_SEG4_TEMPS_CELL1;

        ACC_SEG4_TEMPS_CELL2 = (uint8_t)((uint8_t)(msg.data[2] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG4_TEMPS_CELL2 = (int8_t)ACC_SEG4_TEMPS_CELL2;

        ACC_SEG4_TEMPS_CELL3 = (uint8_t)((uint8_t)(msg.data[3] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG4_TEMPS_CELL3 = (int8_t)ACC_SEG4_TEMPS_CELL3;

        ACC_SEG4_TEMPS_CELL4 = (uint8_t)((uint8_t)(msg.data[4] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG4_TEMPS_CELL4 = (int8_t)ACC_SEG4_TEMPS_CELL4;

        ACC_SEG4_TEMPS_CELL5 = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u); // int8_t *1 +0
        current_row->ACC_SEG4_TEMPS_CELL5 = (int8_t)ACC_SEG4_TEMPS_CELL5;

        break;

    case VDM_GPS_LAT_LONG:
        uint32_t VDM_GPS_Latitude;
        uint32_t VDM_GPS_Longitude;

        VDM_GPS_Latitude = (uint32_t)((uint32_t)(msg.data[0] & 0xffu) << 24u); // int32_t *1 +0
        VDM_GPS_Latitude |= (uint32_t)((uint32_t)(msg.data[1] & 0xffu) << 16u); // int32_t *1 +0
        VDM_GPS_Latitude |= (uint32_t)((uint32_t)(msg.data[2] & 0xffu) << 8u); // int32_t *1 +0
        VDM_GPS_Latitude |= (uint32_t)((uint32_t)(msg.data[3] & 0xffu) >> 0u); // int32_t *1 +0
        current_row->VDM_GPS_Latitude = (int32_t)VDM_GPS_Latitude;

        VDM_GPS_Longitude = (uint32_t)((uint32_t)(msg.data[4] & 0xffu) << 24u); // int32_t *1 +0
        VDM_GPS_Longitude |= (uint32_t)((uint32_t)(msg.data[5] & 0xffu) << 16u); // int32_t *1 +0
        VDM_GPS_Longitude |= (uint32_t)((uint32_t)(msg.data[6] & 0xffu) << 8u); // int32_t *1 +0
        VDM_GPS_Longitude |= (uint32_t)((uint32_t)(msg.data[7] & 0xffu) >> 0u); // int32_t *1 +0
        current_row->VDM_GPS_Longitude = (int32_t)VDM_GPS_Longitude;

        break;

    case VDM_GPS_DATA:
        uint16_t VDM_GPS_SPEED;
        uint16_t VDM_GPS_ALTITUDE;
        uint16_t VDM_GPS_TRUE_COURSE;

        VDM_GPS_SPEED = (uint16_t)((uint16_t)(msg.data[0] & 0xffu) << 8u); // float *0.01 +0
        VDM_GPS_SPEED |= (uint16_t)((uint16_t)(msg.data[1] & 0xffu) >> 0u); // float *0.01 +0

        VDM_GPS_ALTITUDE = (uint16_t)((uint16_t)(msg.data[2] & 0xffu) << 8u); // int16_t *1 +0
        VDM_GPS_ALTITUDE |= (uint16_t)((uint16_t)(msg.data[3] & 0xffu) >> 0u); // int16_t *1 +0
        current_row->VDM_GPS_ALTITUDE = (int16_t)VDM_GPS_ALTITUDE;

        VDM_GPS_TRUE_COURSE = (uint16_t)((uint16_t)(msg.data[4] & 0xffu) << 8u); // float *0.01 +0
        VDM_GPS_TRUE_COURSE |= (uint16_t)((uint16_t)(msg.data[5] & 0xffu) >> 0u); // float *0.01 +0

        current_row->VDM_GPS_SATELLITES_IN_USE = (uint8_t)((uint8_t)(msg.data[6] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->VDM_GPS_VALID1 = (uint8_t)((uint8_t)(msg.data[7] & 0xffu) >> 0u); // uint8_t *1 +0

        break;

    case VDM_DATE_TIME:
        current_row->VDM_GPS_VALID2 = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->VDM_UTC_DATE_YEAR = (uint8_t)((uint8_t)(msg.data[1] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->VDM_UTC_DATE_MONTH = (uint8_t)((uint8_t)(msg.data[2] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->VDM_UTC_DATE_DAY = (uint8_t)((uint8_t)(msg.data[3] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->VDM_UTC_TIME_HOURS = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->VDM_UTC_TIME_MINUTES = (uint8_t)((uint8_t)(msg.data[6] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->VDM_UTC_TIME_SECONDS = (uint8_t)((uint8_t)(msg.data[7] & 0xffu) >> 0u); // uint8_t *1 +0

        break;

    case VDM_ACCELERATION:
        uint16_t VDM_X_AXIS_ACCELERATION;
        uint16_t VDM_Y_AXIS_ACCELERATION;
        uint16_t VDM_Z_AXIS_ACCELERATION;

        VDM_X_AXIS_ACCELERATION = (uint16_t)((uint16_t)(msg.data[0] & 0xffu) << 8u); // float *0.000244141 +0
        VDM_X_AXIS_ACCELERATION |= (uint16_t)((uint16_t)(msg.data[1] & 0xffu) >> 0u); // float *0.000244141 +0
        current_row->VDM_X_AXIS_ACCELERATION = (int16_t)VDM_X_AXIS_ACCELERATION;

        VDM_Y_AXIS_ACCELERATION = (uint16_t)((uint16_t)(msg.data[2] & 0xffu) << 8u); // float *0.000244141 +0
        VDM_Y_AXIS_ACCELERATION |= (uint16_t)((uint16_t)(msg.data[3] & 0xffu) >> 0u); // float *0.000244141 +0
        current_row->VDM_Y_AXIS_ACCELERATION = (int16_t)VDM_Y_AXIS_ACCELERATION;

        VDM_Z_AXIS_ACCELERATION = (uint16_t)((uint16_t)(msg.data[4] & 0xffu) << 8u); // float *0.000244141 +0
        VDM_Z_AXIS_ACCELERATION |= (uint16_t)((uint16_t)(msg.data[5] & 0xffu) >> 0u); // float *0.000244141 +0
        current_row->VDM_Z_AXIS_ACCELERATION = (int16_t)VDM_Z_AXIS_ACCELERATION;

        break;

    case VDM_YAW_RATE:
        uint16_t VDM_X_AXIS_YAW_RATE;
        uint16_t VDM_Y_AXIS_YAW_RATE;
        uint16_t VDM_Z_AXIS_YAW_RATE;

        VDM_X_AXIS_YAW_RATE = (uint16_t)((uint16_t)(msg.data[0] & 0xffu) << 8u); // float *0.0152588 +0
        VDM_X_AXIS_YAW_RATE |= (uint16_t)((uint16_t)(msg.data[1] & 0xffu) >> 0u); // float *0.0152588 +0
        current_row->VDM_X_AXIS_YAW_RATE = (int16_t)VDM_X_AXIS_YAW_RATE;

        VDM_Y_AXIS_YAW_RATE = (uint16_t)((uint16_t)(msg.data[2] & 0xffu) << 8u); // float *0.0152588 +0
        VDM_Y_AXIS_YAW_RATE |= (uint16_t)((uint16_t)(msg.data[3] & 0xffu) >> 0u); // float *0.0152588 +0
        current_row->VDM_Y_AXIS_YAW_RATE = (int16_t)VDM_Y_AXIS_YAW_RATE;

        VDM_Z_AXIS_YAW_RATE = (uint16_t)((uint16_t)(msg.data[4] & 0xffu) << 8u); // float *0.0152588 +0
        VDM_Z_AXIS_YAW_RATE |= (uint16_t)((uint16_t)(msg.data[5] & 0xffu) >> 0u); // float *0.0152588 +0
        current_row->VDM_Z_AXIS_YAW_RATE = (int16_t)VDM_Z_AXIS_YAW_RATE;

        break;

    case ETC_TPDO_STATUS:
        uint16_t ETC_STATUS_HE1;
        uint16_t ETC_STATUS_HE2;
        uint16_t ETC_STATUS_BRAKE_SENSE_VOLTAGE;

        ETC_STATUS_HE1 = (uint16_t)((uint16_t)(msg.data[0] & 0xffu) >> 0u); // uint16_t *1 +0
        ETC_STATUS_HE1 |= (uint16_t)((uint16_t)(msg.data[1] & 0xffu) << 8u); // uint16_t *1 +0

        ETC_STATUS_HE2 = (uint16_t)((uint16_t)(msg.data[2] & 0xffu) >> 0u); // uint16_t *1 +0
        ETC_STATUS_HE2 |= (uint16_t)((uint16_t)(msg.data[3] & 0xffu) << 8u); // uint16_t *1 +0

        ETC_STATUS_BRAKE_SENSE_VOLTAGE = (uint16_t)((uint16_t)(msg.data[4] & 0xffu) >> 0u); // uint16_t *1 +0
        ETC_STATUS_BRAKE_SENSE_VOLTAGE |= (uint16_t)((uint16_t)(msg.data[5] & 0xffu) << 8u); // uint16_t *1 +0

        current_row->ETC_STATUS_PEDAL_TRAVEL = (uint8_t)((uint8_t)(msg.data[6] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->ETC_STATUS_RTD_BUTTON = (uint8_t)((uint8_t)(msg.data[7] & 0x01u) >> 0u); // bool *1 +0

        current_row->ETC_STATUS_RTDS = (uint8_t)((uint8_t)(msg.data[7] & 0x02u) >> 1u); // bool *1 +0

        current_row->ETC_STATUS_REVERSE = (uint8_t)((uint8_t)(msg.data[7] & 0x04u) >> 2u); // bool *1 +0

        current_row->ETC_STATUS_BRAKELIGHT = (uint8_t)((uint8_t)(msg.data[7] & 0x08u) >> 3u); // bool *1 +0

        current_row->ETC_STATUS_RTD = (uint8_t)((uint8_t)(msg.data[7] & 0x10u) >> 4u); // bool *1 +0

        current_row->ETC_STATUS_IMPLAUSIBILITY = (uint8_t)((uint8_t)(msg.data[7] & 0x20u) >> 5u); // bool *1 +0

        current_row->ETC_STATUS_TS_ACTIVE = (uint8_t)((uint8_t)(msg.data[7] & 0x40u) >> 6u); // bool *1 +0

        break;

    case PDB_TPDO_POWER_A:
        uint16_t PDB_POWER_A_GLV_VOLTAGE;

        PDB_POWER_A_GLV_VOLTAGE = (uint16_t)((uint16_t)(msg.data[0] & 0xffu) >> 0u); // uint16_t *1 +0
        PDB_POWER_A_GLV_VOLTAGE |= (uint16_t)((uint16_t)(msg.data[1] & 0xffu) << 8u); // uint16_t *1 +0

        current_row->PDB_POWER_A_CURRENT_SHUTDOWN = (uint8_t)((uint8_t)(msg.data[2] & 0xffu) >> 0u) * 0.1; // float *0.1 +0

        current_row->PDB_POWER_A_CURRENT_ACC = (uint8_t)((uint8_t)(msg.data[3] & 0xffu) >> 0u) * 0.1; // float *0.1 +0

        current_row->PDB_POWER_A_CURRENT_ETC = (uint8_t)((uint8_t)(msg.data[4] & 0xffu) >> 0u) * 0.1; // float *0.1 +0

        current_row->PDB_POWER_A_CURRENT_BPS = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u) * 0.1; // float *0.1 +0

        current_row->PDB_POWER_A_CURRENT_TRACTIVE = (uint8_t)((uint8_t)(msg.data[6] & 0xffu) >> 0u) * 0.1; // float *0.1 +0

        current_row->PDB_POWER_A_CURRENT_BSPD = (uint8_t)((uint8_t)(msg.data[7] & 0xffu) >> 0u) * 0.1; // float *0.1 +0

        break;

    case PDB_TPDO_POWER_B:
        current_row->PDB_POWER_B_CURRENT_TELEMETRY = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u) * 0.1; // float *0.1 +0

        current_row->PDB_POWER_B_CURRENT_PDB = (uint8_t)((uint8_t)(msg.data[1] & 0xffu) >> 0u) * 0.1; // float *0.1 +0

        current_row->PDB_POWER_B_CURRENT_DASH = (uint8_t)((uint8_t)(msg.data[2] & 0xffu) >> 0u) * 0.1; // float *0.1 +0

        current_row->PDB_POWER_B_CURRENT_RTML = (uint8_t)((uint8_t)(msg.data[3] & 0xffu) >> 0u) * 0.1; // float *0.1 +0

        current_row->PDB_POWER_B_CURRENT_EXTRA_1 = (uint8_t)((uint8_t)(msg.data[4] & 0xffu) >> 0u) * 0.1; // float *0.1 +0

        current_row->PDB_POWER_B_CURRENT_EXTRA_2 = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u) * 0.1; // float *0.1 +0

        break;

    case TMAIN_TPDO_DATA:
        uint16_t TMAIN_DATA_BRAKES_F;
        uint16_t TMAIN_DATA_BRAKES_R;

        TMAIN_DATA_BRAKES_F = (uint16_t)((uint16_t)(msg.data[0] & 0xffu) >> 0u); // float *0.1 +0
        TMAIN_DATA_BRAKES_F |= (uint16_t)((uint16_t)(msg.data[1] & 0xffu) << 8u); // float *0.1 +0

        TMAIN_DATA_BRAKES_R = (uint16_t)((uint16_t)(msg.data[2] & 0xffu) >> 0u); // float *0.1 +0
        TMAIN_DATA_BRAKES_R |= (uint16_t)((uint16_t)(msg.data[3] & 0xffu) << 8u); // float *0.1 +0

        break;

    case TPERIPH_FL_TPDO_DATA:
        uint16_t TPERIPH_FL_DATA_WHEELSPEED;
        uint16_t TPERIPH_FL_DATA_SUSTRAVEL;
        uint16_t TPERIPH_FL_DATA_STRAIN;

        TPERIPH_FL_DATA_WHEELSPEED = (uint16_t)((uint16_t)(msg.data[0] & 0xffu) >> 0u); // float *0.1 +0
        TPERIPH_FL_DATA_WHEELSPEED |= (uint16_t)((uint16_t)(msg.data[1] & 0xffu) << 8u); // float *0.1 +0
        current_row->TPERIPH_FL_DATA_WHEELSPEED = (int16_t)TPERIPH_FL_DATA_WHEELSPEED;

        TPERIPH_FL_DATA_SUSTRAVEL = (uint16_t)((uint16_t)(msg.data[2] & 0xffu) >> 0u); // float *0.01 +0
        TPERIPH_FL_DATA_SUSTRAVEL |= (uint16_t)((uint16_t)(msg.data[3] & 0xffu) << 8u); // float *0.01 +0

        TPERIPH_FL_DATA_STRAIN = (uint16_t)((uint16_t)(msg.data[4] & 0xffu) >> 0u); // float *1e-07 +0
        TPERIPH_FL_DATA_STRAIN |= (uint16_t)((uint16_t)(msg.data[5] & 0xffu) << 8u); // float *1e-07 +0
        current_row->TPERIPH_FL_DATA_STRAIN = (int16_t)TPERIPH_FL_DATA_STRAIN;

        current_row->TPERIPH_FL_DATA_SIDE_TIRE_TEMP = (uint8_t)((uint8_t)(msg.data[6] & 0xffu) >> 0u); // uint8_t *1 +0

        break;

    case TPERIPH_FR_TPDO_DATA:
        uint16_t TPERIPH_FR_DATA_WHEELSPEED;
        uint16_t TPERIPH_FR_DATA_SUSTRAVEL;
        uint16_t TPERIPH_FR_DATA_STRAIN;

        TPERIPH_FR_DATA_WHEELSPEED = (uint16_t)((uint16_t)(msg.data[0] & 0xffu) >> 0u); // float *0.1 +0
        TPERIPH_FR_DATA_WHEELSPEED |= (uint16_t)((uint16_t)(msg.data[1] & 0xffu) << 8u); // float *0.1 +0
        current_row->TPERIPH_FR_DATA_WHEELSPEED = (int16_t)TPERIPH_FR_DATA_WHEELSPEED;

        TPERIPH_FR_DATA_SUSTRAVEL = (uint16_t)((uint16_t)(msg.data[2] & 0xffu) >> 0u); // float *0.01 +0
        TPERIPH_FR_DATA_SUSTRAVEL |= (uint16_t)((uint16_t)(msg.data[3] & 0xffu) << 8u); // float *0.01 +0

        TPERIPH_FR_DATA_STRAIN = (uint16_t)((uint16_t)(msg.data[4] & 0xffu) >> 0u); // float *1e-07 +0
        TPERIPH_FR_DATA_STRAIN |= (uint16_t)((uint16_t)(msg.data[5] & 0xffu) << 8u); // float *1e-07 +0
        current_row->TPERIPH_FR_DATA_STRAIN = (int16_t)TPERIPH_FR_DATA_STRAIN;

        current_row->TPERIPH_FR_DATA_SIDE_TIRE_TEMP = (uint8_t)((uint8_t)(msg.data[6] & 0xffu) >> 0u); // uint8_t *1 +0

        break;

    case TPERIPH_BL_TPDO_DATA:
        uint16_t TPERIPH_BL_DATA_WHEELSPEED;
        uint16_t TPERIPH_BL_DATA_SUSTRAVEL;
        uint16_t TPERIPH_BL_DATA_STRAIN;

        TPERIPH_BL_DATA_WHEELSPEED = (uint16_t)((uint16_t)(msg.data[0] & 0xffu) >> 0u); // float *0.1 +0
        TPERIPH_BL_DATA_WHEELSPEED |= (uint16_t)((uint16_t)(msg.data[1] & 0xffu) << 8u); // float *0.1 +0
        current_row->TPERIPH_BL_DATA_WHEELSPEED = (int16_t)TPERIPH_BL_DATA_WHEELSPEED;

        TPERIPH_BL_DATA_SUSTRAVEL = (uint16_t)((uint16_t)(msg.data[2] & 0xffu) >> 0u); // float *0.01 +0
        TPERIPH_BL_DATA_SUSTRAVEL |= (uint16_t)((uint16_t)(msg.data[3] & 0xffu) << 8u); // float *0.01 +0

        TPERIPH_BL_DATA_STRAIN = (uint16_t)((uint16_t)(msg.data[4] & 0xffu) >> 0u); // float *1e-07 +0
        TPERIPH_BL_DATA_STRAIN |= (uint16_t)((uint16_t)(msg.data[5] & 0xffu) << 8u); // float *1e-07 +0
        current_row->TPERIPH_BL_DATA_STRAIN = (int16_t)TPERIPH_BL_DATA_STRAIN;

        current_row->TPERIPH_BL_DATA_SIDE_TIRE_TEMP = (uint8_t)((uint8_t)(msg.data[6] & 0xffu) >> 0u); // uint8_t *1 +0

        break;

    case TPERIPH_BR_TPDO_DATA:
        uint16_t TPERIPH_BR_DATA_WHEELSPEED;
        uint16_t TPERIPH_BR_DATA_SUSTRAVEL;
        uint16_t TPERIPH_BR_DATA_STRAIN;

        TPERIPH_BR_DATA_WHEELSPEED = (uint16_t)((uint16_t)(msg.data[0] & 0xffu) >> 0u); // float *0.1 +0
        TPERIPH_BR_DATA_WHEELSPEED |= (uint16_t)((uint16_t)(msg.data[1] & 0xffu) << 8u); // float *0.1 +0
        current_row->TPERIPH_BR_DATA_WHEELSPEED = (int16_t)TPERIPH_BR_DATA_WHEELSPEED;

        TPERIPH_BR_DATA_SUSTRAVEL = (uint16_t)((uint16_t)(msg.data[2] & 0xffu) >> 0u); // float *0.01 +0
        TPERIPH_BR_DATA_SUSTRAVEL |= (uint16_t)((uint16_t)(msg.data[3] & 0xffu) << 8u); // float *0.01 +0

        TPERIPH_BR_DATA_STRAIN = (uint16_t)((uint16_t)(msg.data[4] & 0xffu) >> 0u); // float *1e-07 +0
        TPERIPH_BR_DATA_STRAIN |= (uint16_t)((uint16_t)(msg.data[5] & 0xffu) << 8u); // float *1e-07 +0
        current_row->TPERIPH_BR_DATA_STRAIN = (int16_t)TPERIPH_BR_DATA_STRAIN;

        current_row->TPERIPH_BR_DATA_SIDE_TIRE_TEMP = (uint8_t)((uint8_t)(msg.data[6] & 0xffu) >> 0u); // uint8_t *1 +0

        break;

    case TPERIPH_FL_TPDO_TIRETEMP:
        current_row->TPERIPH_FL_TIRETEMP_1 = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_FL_TIRETEMP_2 = (uint8_t)((uint8_t)(msg.data[1] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_FL_TIRETEMP_3 = (uint8_t)((uint8_t)(msg.data[2] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_FL_TIRETEMP_4 = (uint8_t)((uint8_t)(msg.data[3] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_FL_TIRETEMP_5 = (uint8_t)((uint8_t)(msg.data[4] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_FL_TIRETEMP_6 = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_FL_TIRETEMP_7 = (uint8_t)((uint8_t)(msg.data[6] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_FL_TIRETEMP_8 = (uint8_t)((uint8_t)(msg.data[7] & 0xffu) >> 0u); // uint8_t *1 +0

        break;

    case TPERIPH_FR_TPDO_TIRETEMP:
        current_row->TPERIPH_FR_TIRETEMP_1 = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_FR_TIRETEMP_2 = (uint8_t)((uint8_t)(msg.data[1] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_FR_TIRETEMP_3 = (uint8_t)((uint8_t)(msg.data[2] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_FR_TIRETEMP_4 = (uint8_t)((uint8_t)(msg.data[3] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_FR_TIRETEMP_5 = (uint8_t)((uint8_t)(msg.data[4] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_FR_TIRETEMP_6 = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_FR_TIRETEMP_7 = (uint8_t)((uint8_t)(msg.data[6] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_FR_TIRETEMP_8 = (uint8_t)((uint8_t)(msg.data[7] & 0xffu) >> 0u); // uint8_t *1 +0

        break;

    case TPERIPH_BL_TPDO_TIRETEMP:
        current_row->TPERIPH_BL_TIRETEMP_1 = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_BL_TIRETEMP_2 = (uint8_t)((uint8_t)(msg.data[1] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_BL_TIRETEMP_3 = (uint8_t)((uint8_t)(msg.data[2] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_BL_TIRETEMP_4 = (uint8_t)((uint8_t)(msg.data[3] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_BL_TIRETEMP_5 = (uint8_t)((uint8_t)(msg.data[4] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_BL_TIRETEMP_6 = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_BL_TIRETEMP_7 = (uint8_t)((uint8_t)(msg.data[6] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_BL_TIRETEMP_8 = (uint8_t)((uint8_t)(msg.data[7] & 0xffu) >> 0u); // uint8_t *1 +0

        break;

    case TPERIPH_BR_TPDO_TIRETEMP:
        current_row->TPERIPH_BR_TIRETEMP_1 = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_BR_TIRETEMP_2 = (uint8_t)((uint8_t)(msg.data[1] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_BR_TIRETEMP_3 = (uint8_t)((uint8_t)(msg.data[2] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_BR_TIRETEMP_4 = (uint8_t)((uint8_t)(msg.data[3] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_BR_TIRETEMP_5 = (uint8_t)((uint8_t)(msg.data[4] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_BR_TIRETEMP_6 = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_BR_TIRETEMP_7 = (uint8_t)((uint8_t)(msg.data[6] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->TPERIPH_BR_TIRETEMP_8 = (uint8_t)((uint8_t)(msg.data[7] & 0xffu) >> 0u); // uint8_t *1 +0

        break;

    case LSS_COMMAND:
        current_row->COMMAND_COMMAND_SPECIFIER = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->COMMAND_MODE = (uint8_t)((uint8_t)(msg.data[1] & 0xffu) >> 0u); // uint8_t *1 +0

        break;

    case LSS_RESPONSE:
        current_row->RESPONSE_COMMAND_SPECIFIER = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u); // uint8_t *1 +0

        break;

    case SMPC_RPDO_CONTROL:
        uint32_t SMPC_CONTROL_PACK_VOLTAGE;

        current_row->SMPC_CONTROL_DEST_NODE_ID = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->SMPC_CONTROL_ENABLE = (uint8_t)((uint8_t)(msg.data[1] & 0x02u) >> 1u); // bool *1 +0

        current_row->SMPC_CONTROL_CURRENT_10X_MULT = (uint8_t)((uint8_t)(msg.data[1] & 0x08u) >> 3u); // bool *1 +0

        current_row->SMPC_CONTROL_EVSE_OVERRIDE = (uint8_t)((uint8_t)(msg.data[1] & 0x20u) >> 5u); // bool *1 +0

        SMPC_CONTROL_PACK_VOLTAGE = (uint32_t)((uint32_t)(msg.data[2] & 0xffu) >> 0u); // uint32_t *1 +0
        SMPC_CONTROL_PACK_VOLTAGE |= (uint32_t)((uint32_t)(msg.data[3] & 0xffu) << 8u); // uint32_t *1 +0
        SMPC_CONTROL_PACK_VOLTAGE |= (uint32_t)((uint32_t)(msg.data[4] & 0xffu) << 16u); // uint32_t *1 +0
        SMPC_CONTROL_PACK_VOLTAGE |= (uint32_t)((uint32_t)(msg.data[5] & 0xffu) << 24u); // uint32_t *1 +0

        break;

    case SMPC_RPDO_MAX_VOLTAGE_CURRENT:
        uint32_t SMPC_MAX_CHRG_VOLTAGE;
        uint16_t SMPC_MAX_CHRG_CURRENT;

        current_row->SMPC_MAX_DEST_NODE_ID = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u); // uint8_t *1 +0

        SMPC_MAX_CHRG_VOLTAGE = (uint32_t)((uint32_t)(msg.data[1] & 0xffu) >> 0u); // uint32_t *1 +0
        SMPC_MAX_CHRG_VOLTAGE |= (uint32_t)((uint32_t)(msg.data[2] & 0xffu) << 8u); // uint32_t *1 +0
        SMPC_MAX_CHRG_VOLTAGE |= (uint32_t)((uint32_t)(msg.data[3] & 0xffu) << 16u); // uint32_t *1 +0
        SMPC_MAX_CHRG_VOLTAGE |= (uint32_t)((uint32_t)(msg.data[4] & 0xffu) << 24u); // uint32_t *1 +0

        SMPC_MAX_CHRG_CURRENT = (uint16_t)((uint16_t)(msg.data[5] & 0xffu) >> 0u); // uint16_t *1 +0
        SMPC_MAX_CHRG_CURRENT |= (uint16_t)((uint16_t)(msg.data[6] & 0xffu) << 8u); // uint16_t *1 +0

        current_row->SMPC_MAX_INPUT_EVSE_OVERRIDE = (uint8_t)((uint8_t)(msg.data[7] & 0xffu) >> 0u); // uint8_t *1 +0

        break;

    case SMPC_TPDO_STATUS:
        uint32_t SMPC_STATUS_VOLTAGE_MV;
        uint16_t SMPC_STATUS_CURRENT_MA;

        current_row->SMPC_STATUS_READY = (uint8_t)((uint8_t)(msg.data[0] & 0x01u) >> 0u); // bool *1 +0

        current_row->SMPC_STATUS_CHARGE_COMPLETE = (uint8_t)((uint8_t)(msg.data[0] & 0x02u) >> 1u); // bool *1 +0

        current_row->SMPC_STATUS_OVERTEMP_FAULT = (uint8_t)((uint8_t)(msg.data[0] & 0x04u) >> 2u); // bool *1 +0

        current_row->SMPC_STATUS_OVERCURRENT_FAULT = (uint8_t)((uint8_t)(msg.data[0] & 0x08u) >> 3u); // bool *1 +0

        current_row->SMPC_STATUS_OVERVOLTAGE_FAULT = (uint8_t)((uint8_t)(msg.data[0] & 0x10u) >> 4u); // bool *1 +0

        current_row->SMPC_STATUS_UNDERVOLTAGE_FAULT = (uint8_t)((uint8_t)(msg.data[0] & 0x20u) >> 5u); // bool *1 +0

        current_row->SMPC_STATUS_INTERNAL_FAULT = (uint8_t)((uint8_t)(msg.data[0] & 0x40u) >> 6u); // bool *1 +0

        current_row->SMPC_STATUS_CHARGING = (uint8_t)((uint8_t)(msg.data[0] & 0x80u) >> 7u); // bool *1 +0

        current_row->SMPC_STATUS_CHARGER_UNPLUGGED = (uint8_t)((uint8_t)(msg.data[1] & 0x01u) >> 0u); // bool *1 +0

        current_row->SMPC_STATUS_REQUEST_EXCLUSIVE = (uint8_t)((uint8_t)(msg.data[1] & 0x10u) >> 4u); // bool *1 +0

        current_row->SMPC_STATUS_J1772_DISCONN = (uint8_t)((uint8_t)(msg.data[1] & 0x20u) >> 5u); // bool *1 +0

        SMPC_STATUS_VOLTAGE_MV = (uint32_t)((uint32_t)(msg.data[2] & 0xffu) >> 0u); // uint32_t *1 +0
        SMPC_STATUS_VOLTAGE_MV |= (uint32_t)((uint32_t)(msg.data[3] & 0xffu) << 8u); // uint32_t *1 +0
        SMPC_STATUS_VOLTAGE_MV |= (uint32_t)((uint32_t)(msg.data[4] & 0xffu) << 16u); // uint32_t *1 +0
        SMPC_STATUS_VOLTAGE_MV |= (uint32_t)((uint32_t)(msg.data[5] & 0xffu) << 24u); // uint32_t *1 +0

        SMPC_STATUS_CURRENT_MA = (uint16_t)((uint16_t)(msg.data[6] & 0xffu) >> 0u); // uint16_t *1 +0
        SMPC_STATUS_CURRENT_MA |= (uint16_t)((uint16_t)(msg.data[7] & 0xffu) << 8u); // uint16_t *1 +0

        break;

    case SMPC_TPDO_MAX_VOLTAGE_CURRENT:
        uint32_t SMPC_MAX2_MAX_VOLTAGE_MV;
        uint16_t SMPC_MAX2_MAX_CURRENT_MA;

        SMPC_MAX2_MAX_VOLTAGE_MV = (uint32_t)((uint32_t)(msg.data[0] & 0xffu) >> 0u); // uint32_t *1 +0
        SMPC_MAX2_MAX_VOLTAGE_MV |= (uint32_t)((uint32_t)(msg.data[1] & 0xffu) << 8u); // uint32_t *1 +0
        SMPC_MAX2_MAX_VOLTAGE_MV |= (uint32_t)((uint32_t)(msg.data[2] & 0xffu) << 16u); // uint32_t *1 +0
        SMPC_MAX2_MAX_VOLTAGE_MV |= (uint32_t)((uint32_t)(msg.data[3] & 0xffu) << 24u); // uint32_t *1 +0

        SMPC_MAX2_MAX_CURRENT_MA = (uint16_t)((uint16_t)(msg.data[4] & 0xffu) >> 0u); // uint16_t *1 +0
        SMPC_MAX2_MAX_CURRENT_MA |= (uint16_t)((uint16_t)(msg.data[5] & 0xffu) << 8u); // uint16_t *1 +0

        current_row->SMPC_MAX2_EVSE_CURRENT = (uint8_t)((uint8_t)(msg.data[6] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->SMPC_MAX2_INPUT_CURR_LIM = (uint8_t)((uint8_t)(msg.data[7] & 0xffu) >> 0u); // uint8_t *1 +0

        break;

    case SMPC_TPDO_INPUT_AC:
        uint16_t SMPC_INPUT_AC_INPUT_CURRENT_MA;

        current_row->SMPC_INPUT_AC_INPUT_VOLTAGE = (uint8_t)((uint8_t)(msg.data[0] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->SMPC_INPUT_J1772_CONNECTED = (uint8_t)((uint8_t)(msg.data[1] & 0x01u) >> 0u); // bool *1 +0

        current_row->SMPC_INPUT_J1772_DISCONNECTED = (uint8_t)((uint8_t)(msg.data[1] & 0x02u) >> 1u); // bool *1 +0

        current_row->SMPC_INPUT_J1772_TRIGGERED = (uint8_t)((uint8_t)(msg.data[1] & 0x04u) >> 2u); // bool *1 +0

        SMPC_INPUT_AC_INPUT_CURRENT_MA = (uint16_t)((uint16_t)(msg.data[2] & 0xffu) >> 0u); // uint16_t *1 +0
        SMPC_INPUT_AC_INPUT_CURRENT_MA |= (uint16_t)((uint16_t)(msg.data[3] & 0xffu) << 8u); // uint16_t *1 +0

        current_row->SMPC_INPUT_AC_INPUT_FREQUENCY_HZ = (uint8_t)((uint8_t)(msg.data[4] & 0xffu) >> 0u); // uint8_t *1 +0

        current_row->SMPC_INPUT_MAX_CHARGER_TEMP_C = (uint8_t)((uint8_t)(msg.data[5] & 0xffu) >> 0u) + -50; // uint8_t *1 +-50

        break;

    case SMPC_TPDO_SERIAL_PART_NUM:
        uint16_t SMPC_SER_PART_NUMBER;
        uint32_t SMPC_SER_SERIAL_NUMBER;
        uint16_t SMPC_SER_FIRMWARE_VER;

        SMPC_SER_PART_NUMBER = (uint16_t)((uint16_t)(msg.data[0] & 0xffu) >> 0u); // uint16_t *1 +0
        SMPC_SER_PART_NUMBER |= (uint16_t)((uint16_t)(msg.data[1] & 0xffu) << 8u); // uint16_t *1 +0

        SMPC_SER_SERIAL_NUMBER = (uint32_t)((uint32_t)(msg.data[2] & 0xffu) >> 0u); // uint32_t *1 +0
        SMPC_SER_SERIAL_NUMBER |= (uint32_t)((uint32_t)(msg.data[3] & 0xffu) << 8u); // uint32_t *1 +0
        SMPC_SER_SERIAL_NUMBER |= (uint32_t)((uint32_t)(msg.data[4] & 0xffu) << 16u); // uint32_t *1 +0
        SMPC_SER_SERIAL_NUMBER |= (uint32_t)((uint32_t)(msg.data[5] & 0xffu) << 24u); // uint32_t *1 +0

        SMPC_SER_FIRMWARE_VER = (uint16_t)((uint16_t)(msg.data[6] & 0xffu) >> 0u); // uint16_t *1 +0
        SMPC_SER_FIRMWARE_VER |= (uint16_t)((uint16_t)(msg.data[7] & 0xffu) << 8u); // uint16_t *1 +0

        break;

    };
}

} // namespace fsdaq

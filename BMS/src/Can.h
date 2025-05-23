// Copyright (c) 2018 Formula Slug. All Rights Reserved.

#ifndef _FS_BMS_SRC_CAN_H_
#define _FS_BMS_SRC_CAN_H_

#include <cstdint>
#include <stdint.h>

#include "mbed.h"
#include "BmsConfig.h" //I added this inclusion

// SIDs From Accumulator TODO: DOUBLE CHECK THESE
constexpr uint32_t kACC_TPDO_STATUS = 0x188;
constexpr uint32_t kACC_TPDO_POWER = 0x288;

constexpr uint32_t kACC_TPDO_SEG0_TEMPS = 0x291;
constexpr uint32_t kACC_TPDO_SEG1_TEMPS = 0x292;
constexpr uint32_t kACC_TPDO_SEG2_TEMPS = 0x293;
constexpr uint32_t kACC_TPDO_SEG3_TEMPS = 0x294;
constexpr uint32_t kACC_TPDO_SEG4_TEMPS = 0x295;

constexpr uint32_t kACC_TPDO_SEG0_VOLTS = 0x191;
constexpr uint32_t kACC_TPDO_SEG1_VOLTS = 0x192;
constexpr uint32_t kACC_TPDO_SEG2_VOLTS = 0x193;
constexpr uint32_t kACC_TPDO_SEG3_VOLTS = 0x194;
constexpr uint32_t kACC_TPDO_SEG4_VOLTS = 0x195;

constexpr uint32_t kNMT_ACC_HEARTBEAT = 0x703;
constexpr uint32_t kRPDO_MAX_CURRENTS = 0x286;

constexpr uint32_t kRPDO_ChargeControl = 0x206;
constexpr uint32_t kRPDO_ChargeLimits = 0x306;

CANMessage ACC_TPDO_STATUS(bool bmsFault, bool imdFault, bool shutdownState, bool prechargeDone,
                         bool precharging, bool charging, bool isBalancing, bool cell_too_low, bool cell_too_high, bool temp_too_low,
                         bool temp_too_high, bool temp_too_high_charging, uint16_t glv_voltage, uint32_t cell_fault_index);

//--------------------------------------------------------------------------------
// Accumulator Temperatures

CANMessage ACC_TPDO_SEG0_TEMPS(int8_t *temps);

CANMessage ACC_TPDO_SEG1_TEMPS(int8_t *temps);

CANMessage ACC_TPDO_SEG2_TEMPS(int8_t *temps);

CANMessage ACC_TPDO_SEG3_TEMPS(int8_t *temps);

CANMessage ACC_TPDO_SEG4_TEMPS(int8_t *temps);

//--------------------------------------------------------------------------------
// Accumulator Voltages

CANMessage ACC_TPDO_SEG0_VOLTS(uint16_t *volts);

CANMessage ACC_TPDO_SEG1_VOLTS(uint16_t *volts);

CANMessage ACC_TPDO_SEG2_VOLTS(uint16_t *volts);

CANMessage ACC_TPDO_SEG3_VOLTS(uint16_t *volts);

CANMessage ACC_TPDO_SEG4_VOLTS(uint16_t *volts);
//--------------------------------------------------------------------------------
// ACC Power

CANMessage ACC_TPDO_POWER(uint16_t packVoltage, uint8_t state_of_charge, uint8_t fan_pwm_duty_cycle, int16_t current);



void canSend(status_msg *status_message, uint16_t packVolt, uint8_t soc, int16_t curr, uint8_t fan_pwm_duty_cycle, uint16_t (&allVoltages)[BMS_BANK_COUNT * BMS_BANK_CELL_COUNT],
            int8_t (&allTemps)[BMS_BANK_COUNT * BMS_BANK_CELL_COUNT] );



void writeCAN(string name, CANMessage message);

#endif
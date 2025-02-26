// Copyright (c) 2018 Formula Slug. All Rights Reserved.

#ifndef _FS_BMS_SRC_CAN_H_
#define _FS_BMS_SRC_CAN_H_

#include <cstdint>
#include <stdint.h>

#include "mbed.h"
#include "BmsConfig.h" //I added this inclusion

// SIDs From Accumulator
constexpr uint32_t kTPDO_ACC_BOARD_State = 0x183;

constexpr uint32_t kTPDO_ACC_BOARD_Temp_0 = 0x189;
constexpr uint32_t kTPDO_ACC_BOARD_Temp_1 = 0x289;
constexpr uint32_t kTPDO_ACC_BOARD_Temp_2 = 0x389;
constexpr uint32_t kTPDO_ACC_BOARD_Temp_3 = 0x489;
constexpr uint32_t kTPDO_ACC_BOARD_Temp_4 = 0x589;

constexpr uint32_t kTPDO_ACC_BOARD_Volt_0 = 0x188;
constexpr uint32_t kTPDO_ACC_BOARD_Volt_1 = 0x288;
constexpr uint32_t kTPDO_ACC_BOARD_Volt_2 = 0x388;
constexpr uint32_t kTPDO_ACC_BOARD_Volt_3 = 0x488;
constexpr uint32_t kTPDO_ACC_BOARD_Volt_4 = 0x588;

constexpr uint32_t kNMT_ACC_HEARTBEAT = 0x703;
constexpr uint32_t kRPDO_MAX_CURRENTS = 0x286;

constexpr uint32_t kRPDO_ChargeControl = 0x206;
constexpr uint32_t kRPDO_ChargeLimits = 0x306;

/* Bootup message */
CANMessage accBoardBootup();

/* TPDO that sends various states and information about the accumulator */
CANMessage accBoardState(uint8_t glvVoltage, uint16_t tsVoltage, bool bmsFault,
                         bool bmsBalancing, bool prechargeDone, bool charging,
                         bool fansOn, bool shutdownClosed, bool unused_A,
                         bool unused_B, uint8_t minCellVoltage,
                         uint8_t maxCellVoltage, int16_t tsCurrent);

/* TPDO that sends all temperatures for one segment */
CANMessage accBoardTemp(uint8_t segment, const int8_t *temps);

/* TPDO that sends all voltages for one segment */
CANMessage accBoardVolt(uint8_t segment, uint16_t *voltages);

/* RPDO for limiting the current to the Motor Controller (AC-X1) */
CANMessage motorControllerCurrentLim(uint16_t chargeCurLim,
                                     uint16_t dischargeCurLim);

CANMessage chargerChargeControlRPDO(uint8_t destinationNodeID,
                                    uint32_t packVoltage, bool evseOverride,
                                    bool current10xMultiplier, bool enable);

CANMessage chargerMaxAllowedVoltageCurrentRPDO(uint8_t destinationNodeID,
                                    uint32_t maxAllowedChargeVoltage,
                                    uint16_t maxAllowedChargeCurrent,
                                    uint8_t maxAllowedInputCurrentEVSEoverride);

void initDrivingCAN(const EventQueue &queue, const CANMessage &msg,
                    const uint32_t &tsVoltage, uint16_t (&allVoltages)[BMS_BANK_COUNT * BMS_BANK_CELL_COUNT],
                    int8_t (&allTemps)[BMS_BANK_COUNT * BMS_BANK_CELL_COUNT]);

void initChargingCAN(const EventQueue &queue, const CANMessage &msg,
                    const uint32_t &tsVoltage, uint16_t (&allVoltages)[BMS_BANK_COUNT * BMS_BANK_CELL_COUNT],
                    int8_t (&allTemps)[BMS_BANK_COUNT * BMS_BANK_CELL_COUNT], const bool &enablecharge);

void canBootupTX();

void canBoardStateTX();

void canTempTX(uint8_t segment, int8_t allTemps[]);

void canVoltTX(uint8_t segment, uint16_t allVoltages[]);

void canCurrentLimTX();

void canLSS_SwitchStateGlobal();

void canLSS_SetNodeIDGlobal();

void can_ChargerSync();

void can_ChargerChargeControl();

void can_ChargerMaxCurrentVoltage();

void writeCAN(string name, CANMessage message);

#endif _FS_BMS_SRC_CAN_H_

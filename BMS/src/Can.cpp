 #include "Can.h"
#include "BmsConfig.h"
#include <cstdint>
#include <cstdio>

//-------------------------------------------------------------------------------
// Bootup CAN Messages

CANMessage accBoardBootup() {
  uint8_t startupMessage[8];
  return CANMessage{kNMT_ACC_HEARTBEAT, startupMessage};
}

void canBootupTX() {
    canBus->write(accBoardBootup());
}

//--------------------------------------------------------------------------------
//General Data Showing State Of Accumulator
CANMessage accBoardState(uint8_t glvVoltage, uint16_t tsVoltage, bool bmsFault,
                         bool bmsBalancing, bool prechargeDone, bool charging,
                         bool fansOn, bool shutdownClosed, bool unused_A,
                         bool unused_B, uint8_t maxCellTemp,
                         uint8_t avgCellTemp, int16_t tsCurrent) {
  uint8_t data[8];
  data[0] = glvVoltage;
  data[1] = tsVoltage;
  data[2] = tsVoltage >> 8;
  data[3] = bmsFault + (bmsBalancing << 1) + (prechargeDone << 2) +
            (charging << 3) + (fansOn << 4) + (shutdownClosed << 5) +
            (unused_A << 6) + (unused_B << 7);
  data[4] = maxCellTemp;
  data[5] = avgCellTemp;
  data[6] = tsCurrent;
  data[7] = tsCurrent >> 8;
  return CANMessage{kTPDO_ACC_BOARD_State, data};
}

void canBoardStateTX(const CANMessage &BoardState) {
    canBus->write(BoardState);
    ThisThread::sleep_for(1ms);
}

//--------------------------------------------------------------------------------
// Accumulator Temperatures
CANMessage accBoardTemp(uint8_t segment, int8_t  *temps) {
  uint8_t data[BMS_BANK_CELL_COUNT];
  unsigned int id;
  switch (segment) {
      case 0:
          id = kTPDO_ACC_BOARD_Temp_0;
        break;
      case 1:
          id = kTPDO_ACC_BOARD_Temp_1;
        break;
      case 2:
          id = kTPDO_ACC_BOARD_Temp_2;
        break;
      case 3:
          id = kTPDO_ACC_BOARD_Temp_3;
        break;
      case 4:
          id = kTPDO_ACC_BOARD_Temp_4;
        break;
  }
  for (int i = 0; i < BMS_BANK_CELL_COUNT; i++) {
    data[i] = static_cast<uint8_t>(temps[i]);
  }

  return CANMessage{id, data};
}

void canTempTX(uint8_t segment, const int8_t allTemps[]) {
    int8_t temps[BMS_BANK_COUNT * BMS_BANK_CELL_COUNT];
    for( uint8_t i = 0; i < BMS_BANK_COUNT; i++) {
        temps[i] = segment * BMS_BANK_CELL_COUNT + i;
    }
    canBus->write(accBoardTemp(segment, temps));
    ThisThread::sleep_for(1ms);
}

//--------------------------------------------------------------------------------
// Accumulator Voltages
  CANMessage accBoardVolt(uint8_t segment, uint16_t *volts) {
  uint8_t data[BMS_BANK_CELL_COUNT];
  uint32_t id;
  switch (segment) {
  case 0:
    id = kTPDO_ACC_BOARD_Volt_0;
    break;
  case 1:
    id = kTPDO_ACC_BOARD_Volt_1;
    break;
  case 2:
    id = kTPDO_ACC_BOARD_Volt_2;
    break;
  case 3:
    id = kTPDO_ACC_BOARD_Volt_3;
    break;
  case 4:
    id = kTPDO_ACC_BOARD_Volt_4;
    break;
  }
  for (int i = 0; i < BMS_BANK_CELL_COUNT; i++) {
    data[i] = (uint8_t)(50.0 * volts[i] / 1000.0);
  }
  return CANMessage(id, data);
}

void canVoltTX(uint8_t segment, const uint16_t allVoltages[]) {
    uint16_t volts[BMS_BANK_COUNT];
    for( uint8_t i = 0; i < BMS_BANK_COUNT; i++) {
        volts[i] = segment * BMS_BANK_CELL_COUNT + i;
    }
    canBus->write(accBoardVolt(segment, volts));
    ThisThread::sleep_for(1ms);
}

//--------------------------------------------------------------------------------
// Motor Controller Current Limit

CANMessage motorControllerCurrentLim(uint16_t chargeCurLim,
                                     uint16_t dischargeCurLim) {
  uint8_t data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  data[0] = chargeCurLim;
  data[1] = chargeCurLim >> 8;
  data[2] = dischargeCurLim;
  data[3] = dischargeCurLim >> 8;
  return CANMessage(kRPDO_MAX_CURRENTS, data);
}

void canCurrentLimTX(const uint32_t &tsVoltage) {
    uint16_t chargeCurrentLimit = 0x0000;
    auto dischargeCurrentLimit = (uint16_t)(((CAR_MAX_POWER/(tsVoltage/1000.0))*CAR_POWER_PERCENT < CAR_CURRENT_MAX) ? (CAR_MAX_POWER/(tsVoltage/1000.0)*CAR_POWER_PERCENT) : CAR_CURRENT_MAX);
    canBus->write(motorControllerCurrentLim(chargeCurrentLimit, dischargeCurrentLimit));
    ThisThread::sleep_for(1ms);
}

//--------------------------------------------------------------------------------
// Charger Charge Control

CANMessage chargerChargeControlRPDO(uint8_t destinationNodeID,
                                    uint32_t packVoltage, bool evseOverride,
                                    bool current10xMultiplier, bool enable) {
  uint8_t data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  data[0] = destinationNodeID;
  data[1] = (enable << 1) | (current10xMultiplier << 3) | (evseOverride << 5);
  data[2] = packVoltage;
  data[3] = packVoltage >> 8;
  data[4] = packVoltage >> 16;
  data[5] = packVoltage >> 24;
  return CANMessage(kRPDO_ChargeControl, data);
}

void can_ChargerChargeControl(const bool chargeEnable) {
    canBus->write(chargerChargeControlRPDO(
        0x10, // destination node ID
        0x00000000, // pack voltage; doesn't matter as only for internal charger logging
        true, // evse override, tells the charger to respect the max AC input current sent in the other message
        false, // current x10 multipler, only used for certain zero chargers
        chargeEnable // enable
    ));
}

//--------------------------------------------------------------------------------
// Max Allowed Voltage Of Charger

CANMessage
chargerMaxAllowedVoltageCurrentRPDO(uint8_t destinationNodeID,
                                    uint32_t maxAllowedChargeVoltage,
                                    uint16_t maxAllowedChargeCurrent,
                                    uint8_t maxAllowedInputCurrentEVSEoverride) {
    uint8_t data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    data[0] = destinationNodeID;
    data[1] = maxAllowedChargeVoltage;
    data[2] = maxAllowedChargeVoltage >> 8;
    data[3] = maxAllowedChargeVoltage >> 16;
    data[4] = maxAllowedChargeVoltage >> 24;
    data[5] = maxAllowedChargeCurrent;
    data[6] = maxAllowedChargeCurrent >> 8;
    data[7] = maxAllowedInputCurrentEVSEoverride;
    return CANMessage(kRPDO_ChargeLimits, data);
}

void can_ChargerMaxCurrentVoltage() {
    canBus->write(chargerMaxAllowedVoltageCurrentRPDO(
        0x10, // destination node ID
        CHARGE_VOLTAGE*1000, // desired voltage, mV
        CHARGE_DC_LIMIT, // charge current limit, mA
        CHARGE_AC_LIMIT // input AC current, can change to 20 if plugged into nema 5-20, nema 5-15 is standard
    ));
}

//--------------------------------------------------------------------------------
// MISC - idk what these do exactly

void canLSS_SwitchStateGlobal() { // Switch state global protocal, switch to LSS configuration state
    uint8_t data[8] = {0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    CANMessage msg(0x7E5, data);
    canBus->write(msg);
}

void canLSS_SetNodeIDGlobal() { // Configurate node ID protocal, set node ID to 0x10
    uint8_t data[8] = {0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    CANMessage msg(0x7E5, data);
    canBus->write(msg);
}

void can_ChargerSync() {
    uint8_t data[0] = {};
    CANMessage msg(0x80, data, 0);
    canBus->write(msg);
}

//-------------------------------------------------------------------------------------
// Driving/Charging CAN Messages To Regularly Display Car Data During Charging/Driving

// Since queue.call_every only allows functions with no parameters, we get around it by using lambda functions
void initDrivingCAN(EventQueue &queue, const CANMessage &boardstate,
            const uint32_t &tsVoltage, uint16_t (&allVoltages)[BMS_BANK_COUNT * BMS_BANK_CELL_COUNT],
            int8_t (&allTemps)[BMS_BANK_COUNT * BMS_BANK_CELL_COUNT]) {

    queue.call_every(100ms, [boardstate]() {
        canBoardStateTX(boardstate);
    });

    queue.call_every( 20ms, [tsVoltage]() {
        canCurrentLimTX(tsVoltage);
    });

    uint8_t num = 0;
    queue.call_every( 200ms, [num, allVoltages]() {
        canVoltTX(num, allVoltages);
    });

    num = 1;
    queue.call_every(200ms, [num, allVoltages]() {
        canVoltTX(num, allVoltages);
    });

    num = 2;
    queue.call_every(200ms, [num, allVoltages]() {
        canVoltTX(num, allVoltages);
    });

    num = 3;
    queue.call_every(200ms, [num, allVoltages]() {
        canVoltTX(num, allVoltages);
    });

    num = 4;
    queue.call_every(200ms, [num, allVoltages]() {
        canVoltTX(num, allVoltages);
    });

    num = 0;
    queue.call_every(200ms, [num, allTemps]() {
        canTempTX(num, allTemps);
    });

    num = 1;
    queue.call_every(200ms, [num, allTemps]() {
        canTempTX(num, allTemps);
    });

    num = 2;
    queue.call_every(200ms, [num, allTemps]() {
        canTempTX(num, allTemps);
    });

    num = 3;
    queue.call_every(200ms, [num, allTemps]() {
        canTempTX(num, allTemps);
    });

    num = 4;
    queue.call_every(200ms, [num, allTemps]() {
        canTempTX(num, allTemps);
    });
}

// Since queue.call_every and queue.call only allow functions with no parameters, we get around it by using lambda functions
void initChargingCAN(EventQueue &queue, const CANMessage &boardstate,
            const uint32_t &tsVoltage, uint16_t (&allVoltages)[BMS_BANK_COUNT * BMS_BANK_CELL_COUNT],
            int8_t (&allTemps)[BMS_BANK_COUNT * BMS_BANK_CELL_COUNT], const bool &enablecharge) {
    ThisThread::sleep_for(100ms);
    queue.call(&canLSS_SwitchStateGlobal);
    queue.dispatch_once();
    ThisThread::sleep_for(5ms);
    queue.call(&canLSS_SetNodeIDGlobal);
    queue.dispatch_once();
    ThisThread::sleep_for(5ms);
    queue.call(&can_ChargerSync);
    queue.call(&can_ChargerMaxCurrentVoltage);

    queue.call( [enablecharge]() {
        can_ChargerChargeControl(enablecharge);
    });

    queue.dispatch_once();
    queue.call_every(100ms, &can_ChargerSync);
    queue.call_every(100ms, &can_ChargerMaxCurrentVoltage);

    queue.call_every(100ms, [enablecharge]() {
        can_ChargerChargeControl(enablecharge);
    });

    queue.call_every(100ms, [boardstate]() {
        canBoardStateTX(boardstate);
    });
    queue.call_every( 20ms, [tsVoltage]() {
        canCurrentLimTX(tsVoltage);
    });

    uint8_t num = 0;
    queue.call_every( 200ms, [num, allVoltages]() {
        canVoltTX(num, allVoltages);
    });

    num = 1;
    queue.call_every(200ms, [num, allVoltages]() {
        canVoltTX(num, allVoltages);
    });

    num = 2;
    queue.call_every(200ms, [num, allVoltages]() {
        canVoltTX(num, allVoltages);
    });

    num = 3;
    queue.call_every(200ms, [num, allVoltages]() {
        canVoltTX(num, allVoltages);
    });

    num = 4;
    queue.call_every(200ms, [num, allVoltages]() {
        canVoltTX(num, allVoltages);
    });

    num = 0;
    queue.call_every(200ms, [num, allTemps]() {
        canTempTX(num, allTemps);
    });

    num = 1;
    queue.call_every(200ms, [num, allTemps]() {
        canTempTX(num, allTemps);
    });

    num = 2;
    queue.call_every(200ms, [num, allTemps]() {
        canTempTX(num, allTemps);
    });

    num = 3;
    queue.call_every(200ms, [num, allTemps]() {
        canTempTX(num, allTemps);
    });

    num = 4;
    queue.call_every(200ms, [num, allTemps]() {
        canTempTX(num, allTemps);
    });
}

// void canRX() {
//     CANMessage msg;

//     if (canBus->read(msg)) {
//         canqueue.push(msg);
//     }
// }
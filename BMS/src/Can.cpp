#include "Can.h"
#include "BmsConfig.h"
#include <cstdint>
#include <cstdio>

//--------------------------------------------------------------------------------
//General Data Showing State Of Accumulator
// TODO: add the proper casts to uint8_t for everything that is not that
CANMessage ACC_TPDO_STATUS(bool bmsFault, bool imdFault, bool shutdownState, bool prechargeDone,
                         bool precharging, bool charging, bool isBalancing, bool cell_too_low, bool cell_too_high, bool temp_too_low,
                         bool temp_too_high, bool temp_too_high_charging, uint16_t glv_voltage, uint32_t cell_fault_index) {
  uint8_t data[8];
  data[0] = bmsFault + (imdFault << 1) + (shutdownState << 2) +
            (prechargeDone << 3) + (precharging << 4) + (charging << 5) ;
  data[1] = cell_too_low + (cell_too_high << 1) + (temp_too_low << 2) + (temp_too_high << 3) + (temp_too_high_charging << 4) + (isBalancing << 5);
  data[2] = (uint8_t)glv_voltage;
  data[3] = (uint8_t)glv_voltage >> 8;
  data[4] = (uint8_t)cell_fault_index;
  data[5] = (uint8_t)(cell_fault_index >> 8);
  data[6] = (uint8_t)cell_fault_index >> 16;
  data[7] = (uint8_t)cell_fault_index >> 24;
  return CANMessage{kACC_TPDO_STATUS, data};
}

//--------------------------------------------------------------------------------
// Accumulator Temperatures

CANMessage ACC_TPDO_SEG0_TEMPS(int8_t *temps) {
    int8_t data[BMS_BANK_CELL_COUNT];
    for (int i = 0; i < BMS_BANK_CELL_COUNT; i++) {
        data[i] = temps[i];
    }

    return CANMessage{kACC_TPDO_SEG0_TEMPS, data};
}

CANMessage ACC_TPDO_SEG1_TEMPS(int8_t *temps) {
    int8_t data[BMS_BANK_CELL_COUNT];
    for (int i = 0; i < BMS_BANK_CELL_COUNT; i++) {
        data[i] = temps[i];
    }

    return CANMessage{kACC_TPDO_SEG1_TEMPS, data};
}

CANMessage ACC_TPDO_SEG2_TEMPS(int8_t *temps) {
    int8_t data[BMS_BANK_CELL_COUNT];
    for (int i = 0; i < BMS_BANK_CELL_COUNT; i++) {
        data[i] = temps[i];
    }

    return CANMessage{kACC_TPDO_SEG2_TEMPS, data};
}

CANMessage ACC_TPDO_SEG3_TEMPS(int8_t *temps) {
    int8_t data[BMS_BANK_CELL_COUNT];
    for (int i = 0; i < BMS_BANK_CELL_COUNT; i++) {
        data[i] = temps[i];
    }

    return CANMessage{kACC_TPDO_SEG3_TEMPS, data};
}

CANMessage ACC_TPDO_SEG4_TEMPS(int8_t *temps) {
    int8_t data[BMS_BANK_CELL_COUNT];
    for (int i = 0; i < BMS_BANK_CELL_COUNT; i++) {
        data[i] = temps[i];
    }

    return CANMessage{kACC_TPDO_SEG4_TEMPS, data};
}


//--------------------------------------------------------------------------------
// Accumulator Voltages

CANMessage ACC_TPDO_SEG0_VOLTS(uint16_t *volts) {
    uint8_t data[BMS_BANK_CELL_COUNT];
    for (int i = 0; i < BMS_BANK_CELL_COUNT; i++) {
        data[i] = (uint8_t)(volts[i] / 10) - 2;
    }

    return CANMessage{kACC_TPDO_SEG0_VOLTS, data};
}

// TODO: make sure that 200 is within the cast
CANMessage ACC_TPDO_SEG1_VOLTS(uint16_t *volts) {
    uint8_t data[BMS_BANK_CELL_COUNT];
    for (int i = 0; i < BMS_BANK_CELL_COUNT; i++) {
        data[i] = (uint8_t)(volts[BMS_BANK_CELL_COUNT + i] / 10 - 2);
    }

    return CANMessage{kACC_TPDO_SEG1_VOLTS, data};
}

CANMessage ACC_TPDO_SEG2_VOLTS(uint16_t *volts) {
    uint8_t data[BMS_BANK_CELL_COUNT];
    for (int i = 0; i < BMS_BANK_CELL_COUNT; i++) {
        data[i] = (uint8_t)(volts[(2 * BMS_BANK_CELL_COUNT) + i] / 10) - 2;
    }

    return CANMessage{kACC_TPDO_SEG2_VOLTS, data};
}

CANMessage ACC_TPDO_SEG3_VOLTS(uint16_t *volts) {
    uint8_t data[BMS_BANK_CELL_COUNT];
    for (int i = 0; i < BMS_BANK_CELL_COUNT; i++) {
        data[i] = (uint8_t)(volts[(3 * BMS_BANK_CELL_COUNT) + i] / 10) - 2;
    }

    return CANMessage{kACC_TPDO_SEG3_VOLTS, data};
}

CANMessage ACC_TPDO_SEG4_VOLTS(uint16_t *volts) {
    uint8_t data[BMS_BANK_CELL_COUNT];
    for (int i = 0; i < BMS_BANK_CELL_COUNT; i++) {
        data[i] = (uint8_t)(volts[(4 * BMS_BANK_CELL_COUNT) + i] / 10) - 2;
    }

    return CANMessage{kACC_TPDO_SEG4_VOLTS, data};
}
//--------------------------------------------------------------------------------
// ACC Power

// TODO: CAST THESE PROPERLY!! cast the signed stuff to char ... this is so cooked
CANMessage ACC_TPDO_POWER(uint16_t packVoltage, uint8_t state_of_charge, int16_t current, uint8_t fan_pwm_duty_cycle) {
    data[0] = (uint8_t)packVoltage;
    data[1] = (uint8_t)packVoltage >> 8;
    data[2] = state_of_charge;
    data[3] = current;
    data[4] = current >> 8;
    data[5] = fan_pwm_duty_cycle;

    return CANMessage{kACC_TPDO_POWER, data};
}

// FULL CAN send message, sends all the possible can messages for the ACC in one go
// status is precalled for this to work
void canSend(status_msg* status_message, uint16_t packVolt, uint8_t soc, int16_t curr, uint8_t fan_pwm_duty_cycle, uint16_t (&allVoltages)[BMS_BANK_COUNT * BMS_BANK_CELL_COUNT],
            int8_t (&allTemps)[BMS_BANK_COUNT * BMS_BANK_CELL_COUNT] ) {

    // status
    canBus->write(ACC_TPDO_STATUS(status_message->hasBmsFault, status_message->imdFault, status_message->checkingShutdownStatus,
        status_message->prechargeDone, status_message->checkingPrechargeStatus, status_message->isCharging, status_message->isBalancing,
        status_message->cell_too_low, status_message->cell_too_high, status_message->temp_too_low, status_message->temp_too_high, status_message->temp_too_high_charging,
        status_message->glv_voltage, status_message->cell_fault_index));
    ThisThread::sleep_for(1ms);

    //power
    canBus->write(ACC_TPDO_POWER(packVolt, soc, curr, fan_pwm_duty_cycle));
    ThisThread::sleep_for(1ms);

    // all the segment volts
    canBus->write(ACC_TPDO_SEG0_VOLTS(allVoltages));
    ThisThread::sleep_for(1ms);
    canBus->write(ACC_TPDO_SEG1_VOLTS(allVoltages));
    ThisThread::sleep_for(1ms);
    canBus->write(ACC_TPDO_SEG2_VOLTS(allVoltages));
    ThisThread::sleep_for(1ms);
    canBus->write(ACC_TPDO_SEG3_VOLTS(allVoltages));
    ThisThread::sleep_for(1ms);
    canBus->write(ACC_TPDO_SEG4_VOLTS(allVoltages));
    ThisThread::sleep_for(1ms);

    // all the segment temps
    canBus->write(ACC_TPDO_SEG0_TEMPS(allTemps));
    ThisThread::sleep_for(1ms);
    canBus->write(ACC_TPDO_SEG1_TEMPS(allTemps + BMS_BANK_CELL_COUNT));
    ThisThread::sleep_for(1ms);
    canBus->write(ACC_TPDO_SEG2_TEMPS(allTemps + (2 * BMS_BANK_CELL_COUNT)));
    ThisThread::sleep_for(1ms);
    canBus->write(ACC_TPDO_SEG3_TEMPS(allTemps + (3* BMS_BANK_CELL_COUNT)));
    ThisThread::sleep_for(1ms);
    canBus->write(ACC_TPDO_SEG4_TEMPS(allTemps + (4*BMS_BANK_CELL_COUNT)));
    ThisThread::sleep_for(1ms);
}
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <string>
#include <memory>
#include <vector>
#include <iostream>

#include "mbed.h"

#include "LTC681xParallelBus.h"
#include "BmsThread.h"

#include "Can.h"


CAN* canBus;

// void canRX();

EventQueue queue(32*EVENTS_EVENT_SIZE);// creates an eventqueue which is thread and ISR safe. EVENTS_EVENT_SIZE is the size of the buffer allocated

CircularBuffer<CANMessage, 32> canqueue;

uint8_t canCount;

void initIO(const EventQueue &equeue);
void checkPrechargeVoltage();
void checkShutdownStatus();

DigitalIn shutdown_measure_pin(ACC_SHUTDOWN_MEASURE);
DigitalIn imd_status_pin(ACC_IMD_STATUS);
DigitalIn charge_state_pin(ACC_CHARGE_STATE);


DigitalOut fan_control_pin(ACC_FAN_CONTROL);
DigitalOut charge_enable_pin(ACC_CHARGE_ENABLE);
DigitalOut bms_fault_pin(ACC_BMS_FAULT);
DigitalOut precharge_control_pin(ACC_PRECHARGE_CONTROL);


AnalogIn current_vref_pin(ACC_BUFFERED_C_VREF);
AnalogIn current_sense_pin(ACC_BUFFERED_C_OUT);
AnalogIn glv_voltage_pin(ACC_GLV_VOLTAGE);

bool checkingPrechargeStatus = false;
bool checkingShutdownStatus = false;

bool prechargeDone = false;
bool hasBmsFault = true;
bool isCharging = false;
bool hasFansOn = false;
bool isBalancing = false;

bool chargeEnable = false;

uint16_t dcBusVoltage; // in tenths of volts
uint32_t tsVoltagemV;
//uint16_t tsVoltage;
uint8_t glvVoltage;
uint16_t tsCurrent;

uint16_t allVoltages[BMS_BANK_COUNT*BMS_BANK_CELL_COUNT];
int8_t allTemps[BMS_BANK_COUNT*BMS_BANK_TEMP_COUNT];

int8_t avgCellTemp; // in c
int8_t maxCellTemp; // in c

int main() {
  osThreadSetPriority(osThreadGetId(), osPriorityHigh7);

  printf("main\n");
  initIO(queue);
  printf("initIO\n");


  SPI* spiDriver = new SPI(BMS_PIN_SPI_MOSI,
                           BMS_PIN_SPI_MISO,
                           BMS_PIN_SPI_SCLK,
                           BMS_PIN_SPI_SSEL,
                           use_gpio_ssel);
  spiDriver->format(8, 0);
  auto ltcBus = LTC681xParallelBus(spiDriver);

  BmsEventMailbox* bmsMailbox = new BmsEventMailbox();
  MainToBMSMailbox* mainToBMSMailbox = new MainToBMSMailbox();

  Thread bmsThreadThread;
  BMSThread bmsThread(ltcBus, 1, bmsMailbox, mainToBMSMailbox);
  bmsThreadThread.start(callback(&BMSThread::startThread, &bmsThread));
  printf("BMS thread started\n");

  Timer t;
  t.start();
  while (1) {
    glvVoltage = (uint8_t)(glv_voltage_pin * 185.3); // in mV
    //printf("GLV voltage: %d mV\n", glvVoltage * 100);

    while (!bmsMailbox->empty()) {
        BmsEvent *bmsEvent;

        osEvent evt = bmsMailbox->get();
        if (evt.status == osEventMessage) {
            bmsEvent = (BmsEvent*)evt.value.p;
        } else {
            continue;
        }

        switch (bmsEvent->bmsState) {
            case BMSThreadState::BMSStartup:
                printf("BMS Fault Startup State\n");
                break;
            case BMSThreadState::BMSIdle:
                // printf("BMS Fault Idle State\n");
                hasBmsFault = false;

                maxCellTemp = bmsEvent->maxTemp;
                avgCellTemp = bmsEvent->avgTemp;
                isBalancing = bmsEvent->isBalancing;

                tsVoltagemV = 0;

                for (int i = 0; i < BMS_BANK_COUNT*BMS_BANK_CELL_COUNT; i++) {
                    allVoltages[i] = bmsEvent->voltageValues[i];
                    tsVoltagemV += allVoltages[i];
                    //printf("%d, V: %d\n", i, allVoltages[i]);
                }
                for (int i = 0; i < BMS_BANK_COUNT*BMS_BANK_TEMP_COUNT; i++) {
                    allTemps[i] = bmsEvent->temperatureValues[i];
                    // printf("%d, T: %d\n", i, allTemps[i]);
                }

                break;
            case BMSThreadState::BMSFaultRecover:
                printf("BMS Fault Recovery State\n");
                hasBmsFault = false;
                break;
            case BMSThreadState::BMSFault:
                printf("*** BMS FAULT ***\n");
                hasBmsFault = true;
                break;
            default:
                printf("FUBAR\n");
                break;
        }
        delete bmsEvent;
    }

    //Take In And Process Incoming CAN Messages
    CANMessage readmsg;
    if (canBus->read(readmsg)) {
        canqueue.push(readmsg);
    }

    while (!canqueue.empty()) {
        CANMessage msg;
        canqueue.pop(msg);

        uint32_t id = msg.id;
        unsigned char* data = msg.data;

        switch(id) {
          case 0x682: // temperature message from MC
            dcBusVoltage = (data[2] | (data[3] << 8)); // TODO: check if this is correct
            break;
          case 0x190: // charge status from charger, 180 + node ID (10)
            dcBusVoltage = (data[2] | (data[3] << 8) | (data[4] << 16) | (data[5] << 24)) / 100;
          default:
            break;
        }
    }

    if (!mainToBMSMailbox->full()) {
        MainToBMSEvent* mainToBMSEvent = new MainToBMSEvent();
        mainToBMSEvent->balanceAllowed = shutdown_measure_pin;
        mainToBMSEvent->charging = isCharging;
        mainToBMSMailbox->put(mainToBMSEvent);
    }

    if (!shutdown_measure_pin && !checkingShutdownStatus) {
        checkingShutdownStatus = false;
        queue.call_in(100ms, &checkShutdownStatus);
    }

    if (dcBusVoltage >= (uint16_t)(tsVoltagemV/100.0) * PRECHARGE_PERCENT && tsVoltagemV >= 60000) {
        prechargeDone = true;
    } else if (dcBusVoltage < 20000 && !checkingPrechargeStatus) {
        checkingPrechargeStatus = true;
        queue.call_in(500ms, &checkPrechargeVoltage);
        // prechargeDone = false;
    }

    bms_fault_pin = !hasBmsFault;

    isCharging = charge_state_pin;
    // printf("charge state: %x\n", isCharging);

    precharge_control_pin = prechargeDone /*false*/;

    hasFansOn = prechargeDone || isCharging;

    chargeEnable = isCharging && !hasBmsFault && shutdown_measure_pin && prechargeDone;
    charge_enable_pin = chargeEnable;

    fan_control_pin = hasFansOn;
    // printf("charge state: %x, hasBmsFault: %x, shutdown_measure: %x\n", isCharging, hasBmsFault, true && shutdown_measure_pin);


    float cSense = (current_sense_pin) * 5; // x5 for 5 volts, pin is number from 0-1
    float cVref = (current_vref_pin) * 5; // x5 for 5 volts, pin is number from 0-1

    // divided by 0.625 for how the current sensor works :/
    // times by 300 because that's the nominal current reading of the sensor (ie baseline)
    // multiplied by 10 and cast to a uint16 for 1 decimal place
    tsCurrent = (uint16_t)((cSense-cVref)*4800);


    // printf("cSense: %d, cVref: %d, Ts current: %d\n", (uint32_t)(cSense*10000), (uint32_t)(cVref*10000), tsCurrent);

    // printf("Error Rx %d - tx %d\n", canBus->rderror(),canBus->tderror());

    queue.dispatch_once();
    ThisThread::sleep_for(5 - (t.read_ms()%5));
  }
}

void initIO(const EventQueue &equeue) {
    fan_control_pin = 0; // turn fans off at start
    charge_enable_pin = 0; // charge not allowed at start
    bms_fault_pin = 0; // assume fault at start, low means fault
    precharge_control_pin = 0; // positive AIR open at start

    canBus = new CAN(BMS_PIN_CAN_RX, BMS_PIN_CAN_TX, BMS_CAN_FREQUENCY);
    // canBus->frequency(BMS_CAN_FREQUENCY);
    // canBus->reset();
    // canBus->attach(canRX);

    queue.call(&canBootupTX);
    queue.dispatch_once();

    CANMessage acc_board_state = accBoardState(
        glvVoltage,
        (uint16_t)(tsVoltagemV/100.0),
        hasBmsFault,
        isBalancing,
        prechargeDone,
        isCharging,
        hasFansOn,
        shutdown_measure_pin,
        false,
        false,
        maxCellTemp,
        avgCellTemp,
        tsCurrent
    );

    ThisThread::sleep_for(1ms);
    isCharging = charge_state_pin;
    if (isCharging) {
        initChargingCAN(equeue, acc_board_state, tsVoltagemV, allVoltages, allTemps, chargeEnable);
    } else {
        initDrivingCAN(equeue, acc_board_state, tsVoltagemV, allVoltages, allTemps);
    }
}

void checkPrechargeVoltage() {
    if (dcBusVoltage < 20000) {
        prechargeDone = false;
    }
    checkingPrechargeStatus = false;
}

void checkShutdownStatus() {
    if (!shutdown_measure_pin) {
        prechargeDone = false;
    }
    checkingShutdownStatus = false;
}
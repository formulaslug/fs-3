#pragma once

#include "mbed.h"

// Global pointer to can bus object
//
// This allows for all files to access the can bus output
extern CAN* canBus;

//Global pointer to DigitalOut objects for BMS_FAULT and CHARGER_CONTROL pins
//
//This allows for all files to access BMS_FAULT and CHARGER_CONTROL pins
extern DigitalOut* bmsFault;
extern DigitalOut* chargerControl;

//
// BMS Master Configuration
//

// Number of LTC6811 battery banks to communicate with
#ifndef BMS_BANK_COUNT
#define BMS_BANK_COUNT 5
#endif

// Number of cell voltage readings per LTC6811
#ifndef BMS_BANK_CELL_COUNT
#define BMS_BANK_CELL_COUNT 6
#endif

// Number of cell temperature readings per LTC6811
#ifndef BMS_BANK_TEMP_COUNT
#define BMS_BANK_TEMP_COUNT BMS_BANK_CELL_COUNT
#endif

// Upper threshold when fault will be thrown for cell temperature
//
// Units: degrees celcius
#ifndef BMS_FAULT_TEMP_THRESHOLD_HIGH
#define BMS_FAULT_TEMP_THRESHOLD_HIGH 60
#endif

// Upper threshold when fault will be thrown for cell temperature when charging
//
// Units: degrees celcius
#ifndef BMS_FAULT_TEMP_THRESHOLD_CHARGING_HIGH
#define BMS_FAULT_TEMP_THRESHOLD_CHARGING_HIGH 45
#endif

// Lower threshold when fault will be thrown for cell temperature
//
// Units: degrees celcius
#ifndef BMS_FAULT_TEMP_THRESHOLD_LOW
#define BMS_FAULT_TEMP_THRESHOLD_LOW 0
#endif

// Upper threshold when fault will be thrown for cell voltage
//
// Units: millivolts
#ifndef BMS_FAULT_VOLTAGE_THRESHOLD_HIGH
#define BMS_FAULT_VOLTAGE_THRESHOLD_HIGH 4150
#endif

// Lower threshold when fault will be thrown for cell voltage
//
// Units: millivolts
#ifndef BMS_FAULT_VOLTAGE_THRESHOLD_LOW
#define BMS_FAULT_VOLTAGE_THRESHOLD_LOW 2650
#endif

// Threshold when cells will be discharged when discharging is enabled.
//
// Units: millivolts
#ifndef BMS_DISCHARGE_THRESHOLD
#define BMS_DISCHARGE_THRESHOLD 5
#endif


// Threshold when cells are allowed to balance. should be in upper 3/4 or so of charging (defualt 3900)
//
// Units: millivolts
#ifndef BMS_BALANCE_THRESHOLD
#define BMS_BALANCE_THRESHOLD 3900
#endif

// highest power allowed
#ifndef CAR_MAX_POWER
#define CAR_MAX_POWER 80000
#endif


// percent of precharge needed to consider precharging done (and close the +AIR)
#ifndef PRECHARGE_PERCENT
#define PRECHARGE_PERCENT 0.95
#endif

// Current threshold for lookuptable, in mA
#ifndef CURR_SOC_LIMIT
#define CURR_SOC_LIMIT 10000
#endif

// Temperature limit for lookup, in C
#ifndef TEMP_SOC_LIMIT
#define TEMP_SOC_LIMIT 35
#endif

// state of charge time threshold - still dont know what it is (maybe in millisec?)
#ifndef SOC_TIME_THRESHOLD
#define SOC_TIME_THRESHOLD 30  // 30 sec
#endif

// cell capacity rated
#ifndef CELL_CAPACITY_RATED
#define CELL_CAPACITY_RATED 50000 // in mAh
#endif


// BMS Cell lookup
//
// This defines the mapping from LTC6811 pins to cell indicies.
// Values of -1 indicate the pin is not connected.
const int BMS_CELL_MAP[12] = {0, 1, 2, -1, -1, -1, 3, 4, 5, -1, -1, -1};


//
// IO Configuration
//

// BMS fault line
//
// To be set high and held high when software enters fault state
#ifndef BMS_PIN_BMS_FLT

#ifdef TARGET_LPC1768
  #define BMS_PIN_BMS_FLT p10
#elif TARGET_NUCLEO_F303K8
  #define BMS_PIN_BMS_FLT PB_0
#elif TARGET_NUCLEO_L432KC
  #define BMS_PIN_BMS_FLT PB_7
#else
  #error "Unknown board for BMS_PIN_BMS_FLT"
#endif

#endif


// Acc board shutdown measurement
//
// To measure the state of the shutdown circuit
#ifndef ACC_SHUTDOWN_MEASURE

#ifdef TARGET_NUCLEO_L432KC
  #define ACC_SHUTDOWN_MEASURE PA_9
#else
  #error "Unknown board for ACC_SHUTDOWN_MEASURE"
#endif

#endif


// Acc board current sensor output read
//
// To measure the output on the current sensor
#ifndef ACC_AMP_CURR_OUT

#ifdef TARGET_NUCLEO_L432KC
  #define ACC_AMP_CURR_OUT PA_3
#else
  #error "Unknown board for ACC_AMP_CURR_OUT"
#endif

#endif


// Acc board GLV voltage read
//
// To measure the voltage of the GLV system
//TODO: Can someone confirm if glvVoltage is in mV? If not we need to convert it in main for SOC Logic.
#ifndef ACC_GLV_VOLTAGE

#ifdef TARGET_NUCLEO_L432KC
  #define ACC_GLV_VOLTAGE PA_0
#else
  #error "Unknown board for ACC_GLV_VOLTAGE"
#endif

#endif


// Acc charge state input
//
// To measure if the acc is connected to the charger
#ifndef ACC_CHARGE_STATE

#ifdef TARGET_NUCLEO_L432KC
  #define ACC_CHARGE_STATE PA_10
#else
  #error "Unknown board for ACC_CHARGE_STATE"
#endif

#endif


// Acc IMD status input
//
// To measure the state of the IMD status pin
#ifndef ACC_IMD_STATUS

#ifdef TARGET_NUCLEO_L432KC
  #define ACC_IMD_STATUS PB_0
#else
  #error "Unknown board for ACC_IMD_STATUS"
#endif

#endif


// Acc BMS Fault Output
//
// To be set low when there is a BMS fault
#ifndef ACC_BMS_FAULT

#ifdef TARGET_NUCLEO_L432KC
  #define ACC_BMS_FAULT PB_7
#else
  #error "Unknown board for ACC_BMS_FAULT"
#endif

#endif

// Acc BMS Fault Output Inverse
//
// To be set to the opposite of the BMS Fault pin
#ifndef ACC_BMS_FAULT_INVERSE

#ifdef TARGET_NUCLEO_L432KC
#define ACC_BMS_FAULT_INVERSE PA_7
#else
#error "Unknown board for ACC_BMS_FAULT_INVERSE"
#endif

#endif


// Acc Precharge Control
//
// To be set high when precharging is done to allow the positive AIR to close
#ifndef ACC_PRECHARGE_CONTROL

#ifdef TARGET_NUCLEO_L432KC
  #define ACC_PRECHARGE_CONTROL PB_1
#else
  #error "Unknown board for ACC_PRECHARGE_CONTROL"
#endif

#endif


// Acc Fans On
//
// To be set high when precharging is done to allow the positive AIR to close
#ifndef ACC_FANS_ON

#ifdef TARGET_NUCLEO_L432KC
#define ACC_FANS_ON PA_8
#else
#error "Unknown board for ACC_FANS_ON"
#endif

#endif

//
// SPI Configuration
//

// SPI master out slave in
#ifndef BMS_PIN_SPI_MOSI
#define BMS_PIN_SPI_MOSI PB_5
#endif


// SPI master in slave out
#ifndef BMS_PIN_SPI_MISO
#define BMS_PIN_SPI_MISO PB_4
#endif


// SPI clock
#ifndef BMS_PIN_SPI_SCLK
#define BMS_PIN_SPI_SCLK PB_3
#endif


// SPI chip select
#ifndef BMS_PIN_SPI_SSEL

#ifdef TARGET_LPC1768
  #define BMS_PIN_SPI_SSEL p8
#elif TARGET_NUCLEO_F303K8
  #define BMS_PIN_SPI_SSEL PA_4
#elif TARGET_NUCLEO_L432KC
  #define BMS_PIN_SPI_SSEL PA_4
#else
  #error "Unknown board for BMS_PIN_SPI_SSEL"
#endif

#endif


//
// CAN Configuration
//

// CAN TX pin to transceiver
#ifndef BMS_PIN_CAN_TX

#ifdef TARGET_LPC1768
  #define BMS_PIN_CAN_TX p29
#elif TARGET_NUCLEO_F303K8
  #define BMS_PIN_CAN_TX PA_12
#elif TARGET_NUCLEO_L432KC
  #define BMS_PIN_CAN_TX PA_12
#else
  #error "Unknown board for BMS_PIN_CAN_TX"
#endif

#endif

// CAN RX pin from transceiver
#ifndef BMS_PIN_CAN_RX

#ifdef TARGET_LPC1768
  #define BMS_PIN_CAN_RX p30
#elif TARGET_NUCLEO_F303K8
  #define BMS_PIN_CAN_RX PA_11
#elif TARGET_NUCLEO_L432KC
  #define BMS_PIN_CAN_RX PA_11
#else
  #error "Unknown board for BMS_PIN_CAN_RX"
#endif

#endif

// CAN frequency to used
// default: 500k
#ifndef BMS_CAN_FREQUENCY
#define BMS_CAN_FREQUENCY 500000
#endif


enum class BMSThreadState {
    // BMS startup and self test
    //   Run self test on all banks
    //   If OK, go to BMSIdle
    BMSStartup,

    // BMS in idle mode
    //   no faults and cells are being actively monitored
    //
    //   wait for either faults or a signal to move in to charging state
    BMSIdle,

    // potential BMS fault, move to BMS fault if persists for another BMS thread cycle
    BMSFaultRecover,

    // BMS in failure mode
    BMSFault
};

struct status_msg {
  bool bmsFault;
  bool imdFault;
  bool shutdownState;
  bool prechargeDone;
  bool precharging;
  bool charging;
  bool isBalancing;
  bool cell_too_low;
  bool cell_too_high;
  bool temp_too_low;
  bool temp_too_high;
  bool temp_too_high_charging;
  uint16_t glv_voltage;
  uint32_t cell_fault_index;
};
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <string>
#include <memory>
#include <numeric>
#include <vector>
#include <iostream>

#include "mbed.h"

#include "LTC681xParallelBus.h"
#include "BmsThread.h"
#include "Can.h"
#include "StateOfCharge.h"

#include "Event.h"

// CAN* canBus;

void initIO();

void initDrivingCAN();
int linearFans_percent(int temp);



struct power_msg {
    uint16_t packVoltage;
    uint8_t state_of_charge;
    int16_t current;
    uint8_t fan_percent;
};

EventQueue queue(32*EVENTS_EVENT_SIZE);// creates an eventqueue which is thread and ISR safe. EVENTS_EVENT_SIZE is the size of the buffer allocated


CircularBuffer<CANMessage, 32> canqueue;

// uint8_t canCount;


DigitalIn shutdown_measure_pin(ACC_SHUTDOWN_MEASURE);
DigitalIn imd_status_pin(ACC_IMD_STATUS);
// DigitalIn charge_state_pin(ACC_CHARGE_STATE);

DigitalOut bms_fault_pin(ACC_BMS_FAULT);
DigitalOut precharge_control_pin(ACC_PRECHARGE_CONTROL);

// AnalogIn current_vref_pin(ACC_BUFFERED_C_VREF);
AnalogIn current_sense_pin(ACC_AMP_CURR_OUT);
AnalogIn glv_voltage_pin(ACC_GLV_VOLTAGE);


PwmOut fan_pwm(ACC_FANS_ON);
uint8_t fan_percent;

bool checkingPrechargeStatus = false;
bool checkingShutdownStatus = false;

bool prechargeDone = false;
bool hasBmsFault = true;
bool isCharging = false;
bool hasFansOn = false;
bool isBalancing = false;

// bool chargeEnable = false;

uint16_t dcBusVoltage; // in tenths of volts
uint32_t packVoltagemV;
//uint16_t tsVoltage;
uint8_t glvVoltage;
float rawTsCurrent;
uint16_t filteredTsCurrent;

uint16_t allVoltages[BMS_BANK_COUNT * BMS_BANK_CELL_COUNT];
int8_t allTemps[BMS_BANK_COUNT * BMS_BANK_TEMP_COUNT];

int8_t avgCellTemp; // in c
int8_t maxCellTemp; // in c

int8_t state_of_charge;

vector<uint16_t> lastCurrentReadings;

status_msg status_message;

int main() {
    osThreadSetPriority(osThreadGetId(), osPriorityHigh7);

    printf("main\n");
    initIO();
    printf("initIO\n");


    SPI *spiDriver = new SPI(BMS_PIN_SPI_MOSI,
                             BMS_PIN_SPI_MISO,
                             BMS_PIN_SPI_SCLK,
                             BMS_PIN_SPI_SSEL,
                             use_gpio_ssel);
    spiDriver->format(8, 0);
    auto ltcBus = LTC681xParallelBus(spiDriver);

    BmsEventMailbox *bmsMailbox = new BmsEventMailbox(); // define bms event mailbox
    MainToBMSMailbox *mainToBMSMailbox = new MainToBMSMailbox();
    // define main to bms mailbox (sends info from main to bms)

    Thread bmsThreadThread;
    BMSThread bmsThread(ltcBus, 1, bmsMailbox, mainToBMSMailbox); // define bmsThread object...?
    bmsThreadThread.start(callback(&BMSThread::startThread, &bmsThread));





    printf("BMS thread started\n");

    Timer t; // create timer obj
    Timer volt_timer;
    Timer soc_timer;
    t.start(); // start timer
    soc_timer.start();
    while (1) {
        // infinite loop
        // glvVoltage = (uint8_t)(glv_voltage_pin * 185.3); // Read voltage from glv_voltage_pin and convert it to mV
        //printf("GLV voltage: %d mV\n", glvVoltage * 100);

        while (!bmsMailbox->empty()) {
            // while the bmsMailbox is not empty
            BmsEvent *bmsEvent; // create bms event pointer

            osEvent evt = bmsMailbox->get(); // Fetch a message (instance of BmsEvent) from the bmsMailbox
            // fetch a message (instance of BmsEvent) from the bmsmailbox
            if (evt.status == osEventMessage) {
                // if status is equal to event message
                bmsEvent = (BmsEvent *) evt.value.p; // set bmsEvent to the value of the received message
            } else {
                continue; // If not an osEventMessage, continue
            }

            switch (bmsEvent->bmsState) {
                // Process the bmsState value in bmsEvent
                case BMSThreadState::BMSStartup:
                    printf("BMS Fault Startup State\n");
                    break;
                case BMSThreadState::BMSIdle:
                    // printf("BMS Fault Idle State\n");
                    // hasBmsFault = false;


                    maxCellTemp = bmsEvent->maxTemp; // Assign the maxTemp from bmsEvent
                    avgCellTemp = bmsEvent->avgTemp; // Assign the avgTemp from bmsEvent
                    isBalancing = bmsEvent->isBalancing; // Assign the isBalancing value from bmsEvent

                    status_message.cell_too_high = bmsEvent->cell_volt_high;
                    status_message.cell_too_low = bmsEvent->cell_volt_low;
                    status_message.temp_too_high = bmsEvent->cell_temp_high;
                    status_message.temp_too_low = bmsEvent->cell_temp_low;
                    status_message.temp_too_high_charging = bmsEvent->cell_temp_high_charging;



                    packVoltagemV = 0;

                    for (int i = 0; i < BMS_BANK_COUNT * BMS_BANK_CELL_COUNT; i++) {
                        // Loop through all bank cells
                        allVoltages[i] = bmsEvent->voltageValues[i]; // Assign voltage values from bmsEvent
                        packVoltagemV += allVoltages[i]; // Sum of voltage values
                        //printf("%d, V: %d\n", i, allVoltages[i]);
                    }
                    for (int i = 0; i < BMS_BANK_COUNT * BMS_BANK_TEMP_COUNT; i++) {
                        // Loop through all bank temperatures
                        allTemps[i] = bmsEvent->temperatureValues[i]; // Assign temperature values from bmsEvent
                        // printf("%d, T: %d\n", i, allTemps[i]);
                    }
                    for (int i = 0; i < BMS_BANK_COUNT * BMS_BANK_TEMP_COUNT; i++) {
                        if (bmsEvent->cell_fault_index[i]) {
                            status_message.cell_fault_index |= (1U << i);  // Set bit i
                        }
                    }

                // #### I CHANGED THIS #### REPLACED THE FOR LOOPS WITH ACCUMULATE AND COPY...
                    packVoltagemV = std::accumulate(bmsEvent->voltageValues,
                                                  bmsEvent->voltageValues + BMS_BANK_COUNT * BMS_BANK_CELL_COUNT, 0);
                // Sum of voltage values
                    std::copy(bmsEvent->temperatureValues,
                              bmsEvent->temperatureValues + BMS_BANK_COUNT * BMS_BANK_TEMP_COUNT, allTemps);
                // Copy temperature values
                // #### THIS IS SOMETHING I ADDED IN ####
                    break;
                case BMSThreadState::BMSFaultRecover:
                    printf("BMS Fault Recovery State\n");
                    hasBmsFault = false;
                    break;
                case BMSThreadState::BMSFault:
                    printf("*** BMS FAULT ***\n");
                // hasBmsFault = true;
                    break;
                default:
                    printf("FUBAR\n");
                    break;
            }
            delete bmsEvent; //// deallocate memory that was previously allocated dynamically to BMSEvent
        }

        // CANMessage readmsg;
        // if (canBus->read(readmsg)) {
        //     canqueue.push(readmsg);
        // }
        //
        // while (!canqueue.empty()) {
        //     CANMessage msg;
        //     canqueue.pop(msg);
        //
        //     uint32_t id = msg.id;
        //     unsigned char* data = msg.data;
        //
        //     switch(id) {
        //       case 0x682: // temperature message from MC
        //         dcBusVoltage = (data[2] | (data[3] << 8));
        //         break;
        //       case 0x190: // charge status from charger, 180 + node ID (10)
        //         dcBusVoltage = (data[2] | (data[3] << 8) | (data[4] << 16) | (data[5] << 24)) / 100;
        //       default:
        //         break;
        //     }
        // }


        if (!mainToBMSMailbox->full()) {
            //if mailbox is not full
            MainToBMSEvent *mainToBMSEvent = new MainToBMSEvent();
            // Create a new MainToBMSEvent object and assign it to the pointer mainToBMSEvent
            // mainToBMSEvent->balanceAllowed = shutdown_measure_pin; // Assign the shutdown_measure_pin value to mainToBMSEvent's balanceAllowed
            // mainToBMSEvent->charging = isCharging; // Assign the isCharging value to mainToBMSEvent's charging
            mainToBMSMailbox->put(mainToBMSEvent);
            // Enqueue the newly allocated MainToBMSEvent object into the mainToBMSMailbox for later processing
        }


        if (!shutdown_measure_pin && !checkingShutdownStatus) {
            checkingShutdownStatus = false;
            queue.call_in(100ms, &checkShutdownStatus);
        }
        //
        if (dcBusVoltage >= (uint16_t)(packVoltagemV/100.0) * PRECHARGE_PERCENT && packVoltagemV >= 60000) {
            prechargeDone = true;
        } else if (dcBusVoltage < 20000 && !checkingPrechargeStatus) {
            checkingPrechargeStatus = true;
            queue.call_in(500ms, &checkPrechargeVoltage);
                    prechargeDone = false;

        }

        bms_fault_pin = !hasBmsFault;


        // isCharging = charge_state_pin;
        // printf("charge state: %x\n", isCharging);

        precharge_control_pin = prechargeDone /*false*/;

        hasFansOn = prechargeDone || isCharging;
        // TODO: FIX FOR THE FANS
        if (prechargeDone) {
            fan_percent = 0.2;
            fan_pwm.write(fan_percent);
        }

        if (hasFansOn)
        {
            fan_percent = linearFans_percent(maxCellTemp) / 100;
            fan_pwm.write(fan_percent);
        }

        status_message.hasBmsFault =hasBmsFault;
        status_message.checkingPrechargeStatus =checkingPrechargeStatus;
        status_message.checkingShutdownStatus =checkingShutdownStatus;
        status_message.isBalancing =isBalancing;
        status_message.isCharging =isCharging;


        // chargeEnable = isCharging && !hasBmsFault && shutdown_measure_pin && prechargeDone;
        // charge_enable_pin = chargeEnable;

        // fan_control_pin = hasFansOn;

        // printf("charge state: %x, hasBmsFault: %x, shutdown_measure: %x\n", isCharging, hasBmsFault, true && shutdown_measure_pin);


        // Current sensor math, look at ACC board (AMP_Curr_Sensor) and datasheet

        // divided by 0.625, according to datasheet
        // multiplied by 300 because that's the nominal current reading of the sensor (ie baseline)
        // divided by 4 (400k / 100k) because of the differential amplifier
        // divided by 3/8 because of voltage divider
        rawTsCurrent = (float) (current_sense_pin.read_u16() * 0.1611); // in 100 mAs
        filteredTsCurrent = (uint16_t) (-138.178 + rawTsCurrent * 1.120198); // in 100 mAs
        // printf("Raw Current: %f\n", rawTsCurrent);
        // printf("Current: %d\n", filteredTsCurrent);

        lastCurrentReadings.push_back(filteredTsCurrent);
        if (lastCurrentReadings.size() > 10) {
            lastCurrentReadings.erase(lastCurrentReadings.begin());
        }

        int avgCurrent = 0;
        for (int i = 0; i < lastCurrentReadings.size(); i++) {
            avgCurrent += lastCurrentReadings[i];
        }
        avgCurrent = avgCurrent / (uint16_t) lastCurrentReadings.size();
        printf("Avg Current: %d\n", avgCurrent);

        if (filteredTsCurrent < CURR_SOC_LIMIT && avgCellTemp < TEMP_SOC_LIMIT) {
            if (volt_timer.read_ms() == 0) {
                volt_timer.start();
            } else if (volt_timer.read_ms() > SOC_TIME_THRESHOLD) {
                // LOOKUP table
                int16_t capacity = convertLowVoltage(glvVoltage);
                state_of_charge = state_of_charge + (capacity / CELL_CAPACITY_RATED) * 100;

            }
        } else {
            volt_timer.reset();
            soc_timer.stop();
            if (lastCurrentReadings.size() >= 2) {
                state_of_charge = currStateOfCharge(state_of_charge, soc_timer.read_ms(),
                    lastCurrentReadings[-1], lastCurrentReadings[-2]);
                soc_timer.reset();
                soc_timer.start();
            }
        }



        // printf("cSense: %d, cVref: %d, Ts current: %d\n", (uint32_t)(cSense*10000), (uint32_t)(cVref*10000), tsCurrent);
        //
        // printf("Error Rx %d - tx %d\n", canBus->rderror(),canBus->tderror());

        // queue.dispatch_once();
        ThisThread::sleep_for(5 - (t.read_ms() % 5));
    }
}
// todo: manage the fan percent; when precharge is still going-> 0; as soon as precharge is done->20; then (linearly) scale to highest temp cell
// todo: when temp is 50, fan percent is 100
void initIO() {
    fan_pwm.period_us(40);
    fan_pwm.write(0);// TODO: range is 0-1, use write to change the fan percent
    fan_percent = 0;
    // charge_enable_pin = 0; // charge not allowed at start
    bms_fault_pin = 0; // assume fault at start, low means fault
    precharge_control_pin = 0; // positive AIR open at start
    state_of_charge = 100; // TODO: CHANGE TO ACCOUNT FOR POSSIBLE DISCHARGE

    // canBus = new CAN(BMS_PIN_CAN_RX, BMS_PIN_CAN_TX, BMS_CAN_FREQUENCY);
    // canBus->frequency(BMS_CAN_FREQUENCY);
    // canBus->reset();
    // canBus->attach(canRX);

    // queue.call(&canBootupTX);
    // queue.dispatch_once();

    ThisThread::sleep_for(1ms);
    // isCharging = charge_state_pin;
    if (isCharging) {
        // initChargingCAN();
    } else {
        // initDrivingCAN();
    }
}




// move to main!!!
void canSendmain() {

    canSend(&status_message, packVoltagemV, state_of_charge, filteredTsCurrent, allVoltages, allTemps);
}

// move to main!!!
void initDrivingCAN() {
    // const CANMessage &boardstate, uint16_t packVolt, uint8_t soc, int16_t curr, uint16_t (&allVoltages)[BMS_BANK_COUNT * BMS_BANK_CELL_COUNT],
    // int8_t(&allTemps)[BMS_BANK_COUNT * BMS_BANK_CELL_COUNT]
    queue.call_every(100ms, &canSendmain);
}

//
void checkPrechargeVoltage() {
    if (dcBusVoltage < 20000) {
        prechargeDone = false;
    }
    checkingPrechargeStatus = false;
}
//
void checkShutdownStatus() {
    if (!shutdown_measure_pin) {
        prechargeDone = false;
    }
    checkingShutdownStatus = false;
}

int linearFans_percent(int temp)
{
    int tmp = ((8/3) * temp) + (100/3);
    return tmp;
}

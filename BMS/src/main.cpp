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

CAN* canBus;
SPI* spiDriver;

void initIO();

void initDrivingCAN();
uint8_t linearFans_percent(int8_t temp);
void checkShutdownStatus();
void checkPrechargeVoltage();

void canSendMain();
void sendSync();

struct power_msg {
    uint16_t packVoltage;
    uint8_t state_of_charge;
    int16_t current;
    uint8_t fan_percent;
};

EventQueue queue(32*EVENTS_EVENT_SIZE);// creates an eventqueue which is thread and ISR safe. EVENTS_EVENT_SIZE is the size of the buffer allocated


DigitalIn shutdown_measure_pin(ACC_SHUTDOWN_MEASURE);
DigitalIn imd_status_pin(ACC_IMD_STATUS);
DigitalIn charge_state_pin(ACC_CHARGE_STATE);

DigitalOut bms_fault_pin(ACC_BMS_FAULT);
DigitalOut bms_fault_inverse_pin(ACC_BMS_FAULT_INVERSE);
DigitalOut precharge_control_pin(ACC_PRECHARGE_CONTROL);

AnalogIn current_sense_pin(ACC_AMP_CURR_OUT);
AnalogIn glv_voltage_pin(ACC_GLV_VOLTAGE);


PwmOut fan_pwm(ACC_FANS_ON);
uint8_t fan_percent;

bool checkingPrechargeStatus = false;
bool checkingShutdownStatus = false;

bool prechargeDone = false;
bool hasBmsFault = false; // assume no fault at start
bool isCharging = false;
bool hasFansOn = false;
bool isBalancing = false;

uint16_t dcBusVoltage; // in tenths of volts
uint32_t packVoltagemV;
uint16_t glvVoltage;
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

    LTC681xParallelBus ltcBus = LTC681xParallelBus(spiDriver);

    BmsEventMailbox* bmsMailbox = new BmsEventMailbox(); // define bms event mailbox
    MainToBMSMailbox* mainToBMSMailbox = new MainToBMSMailbox();
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
    int32_t capacityDischarged = -1;
    while (1) {
        // infinite loop
        glvVoltage = (uint16_t)(glv_voltage_pin * 185.3); // Read voltage from glv_voltage_pin and convert it to mV
        status_message.glv_voltage = glvVoltage;
        //printf("GLV voltage: %d mV\n", glvVoltage * 100);
         // capacity initialization using the voltage lookup table
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
            // Status Message unpacking from bmsEvent


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

            if (capacityDischarged == -1)
            {
                capacityDischarged = convertLowVoltage(packVoltagemV);
            }

            for (int i = 0; i < BMS_BANK_COUNT * BMS_BANK_TEMP_COUNT; i++) {
                // Loop through all bank temperatures
                allTemps[i] = bmsEvent->temperatureValues[i]; // Assign temperature values from bmsEvent
                // printf("%d, T: %d\n", i, allTemps[i]);
            }

            status_message.cell_fault_index = bmsEvent->cell_fault_index;

            switch (bmsEvent->bmsState) {
                // Process the bmsState value in bmsEvent
                case BMSThreadState::BMSStartup:
                    printf("BMS Fault Startup State\n");
                    hasBmsFault = false;
                    break;
                case BMSThreadState::BMSIdle:
                    // printf("BMS Fault Idle State\n");

                // i don't want to delete this because might be better for copying temps and volts
                // // #### I CHANGED THIS #### REPLACED THE FOR LOOPS WITH ACCUMULATE AND COPY...
                //     packVoltagemV = std::accumulate(bmsEvent->voltageValues,
                //                                   bmsEvent->voltageValues + BMS_BANK_COUNT * BMS_BANK_CELL_COUNT, 0);
                // // Sum of voltage values
                //     std::copy(bmsEvent->temperatureValues,
                //               bmsEvent->temperatureValues + BMS_BANK_COUNT * BMS_BANK_TEMP_COUNT, allTemps);
                // Copy temperature values
                // #### THIS IS SOMETHING I ADDED IN ####
                    break;
                case BMSThreadState::BMSFaultRecover:
                    printf("BMS Fault Recovery State\n");
                    break;
                case BMSThreadState::BMSFault:
                    printf("*** BMS FAULT ***\n");
                    hasBmsFault = true;
                    break;
                default:
                    printf("FUBAR\n");
                    break;
            }
            delete bmsEvent; //// deallocate memory that was previously allocated dynamically to BMSEvent
        }


        CANMessage msg;
        while (canBus->read(msg)) {
            uint32_t id = msg.id;
            unsigned char* data = msg.data;

            if (!isCharging)
            {
                switch(id)
                {
                    case 0x682: // temperature message from MC
                        dcBusVoltage = (data[2] | (data[3] << 8));
                        break;
                    default:
                        break;
                }
            } else {
                switch(id)
                {
                    case 0x190: // charge status from charger, 180 + node ID (10)
                        dcBusVoltage = (data[2] | (data[3] << 8) | (data[4] << 16) | (data[5] << 24)) / 100;
                    default:
                        break;
                }
            }
        }


        if (!mainToBMSMailbox->full()) {
            //if mailbox is not full
            MainToBMSEvent *mainToBMSEvent = new MainToBMSEvent();
            // Create a new MainToBMSEvent object and assign it to the pointer mainToBMSEvent
            mainToBMSEvent->balanceAllowed = shutdown_measure_pin; // Assign the shutdown_measure_pin value to mainToBMSEvent's balanceAllowed
            mainToBMSEvent->charging = isCharging; // Assign the isCharging value to mainToBMSEvent's charging
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
                    // prechargeDone = false;

        }

        bms_fault_pin = !hasBmsFault;
        bms_fault_inverse_pin = hasBmsFault;


        precharge_control_pin = prechargeDone;

        hasFansOn = prechargeDone;
        if (hasFansOn) {
            fan_percent = linearFans_percent(maxCellTemp);
        } else {
            fan_percent = 0;
        }
        fan_pwm.write(fan_percent / 100.0);

        // printf("shutdown state: %x\n", shutdown_measure_pin.read());

        status_message.bmsFault = hasBmsFault;
        status_message.shutdownState = shutdown_measure_pin.read();
        status_message.isBalancing = isBalancing;
        status_message.charging = isCharging;
        status_message.precharging = !prechargeDone && shutdown_measure_pin.read();
        status_message.prechargeDone = prechargeDone;
        status_message.imdFault = !imd_status_pin.read();


        // Current sensor math, look at ACC board (AMP_Curr_Sensor) and datasheet

        // divided by 0.625, according to datasheet
        // multiplied by 300 because that's the nominal current reading of the sensor (ie baseline)
        // divided by 4 (400k / 100k) because of the differential amplifier
        // divided by 3/8 because of voltage divider
        rawTsCurrent = (float) (current_sense_pin.read_u16() * 0.1611); // in 100 mAs
        filteredTsCurrent = (uint16_t) (-138.178 + rawTsCurrent * 1.120198); // in 100 mAs

        lastCurrentReadings.push_back(filteredTsCurrent);
        if (lastCurrentReadings.size() > 10) {
            lastCurrentReadings.erase(lastCurrentReadings.begin());
        }
        // printf("Voltage before SoC: %d\n", packVoltagemV);
        // State of Charge calculations
        if (filteredTsCurrent < CURR_SOC_LIMIT && avgCellTemp < TEMP_SOC_LIMIT) { // if the current is low and temp is low
            if (volt_timer.read() == 0) {
                volt_timer.start();
            } else if (volt_timer.read() > SOC_TIME_THRESHOLD) {
                // LOOKUP table
                capacityDischarged = convertLowVoltage(packVoltagemV);
            } else {
                capacityDischarged += ( (soc_timer.read() / 3600) * (((lastCurrentReadings[lastCurrentReadings.size()-1] * 100) + (lastCurrentReadings[lastCurrentReadings.size()-2] * 100)) / 2));
                //Note: Multiplied lastCurrentReadings by 100 since filteredTsCurrent is in 100 mAs
                soc_timer.reset();
                soc_timer.start();
            }
        } else {
            volt_timer.reset();
            volt_timer.stop();
            soc_timer.stop();
            if (lastCurrentReadings.size() >= 2) {
                // soc_timer should be in hours here for mAh
                capacityDischarged += ( (int32_t)(soc_timer.read() / 3600.0) * (((lastCurrentReadings[lastCurrentReadings.size()-1] * 100) + (lastCurrentReadings[lastCurrentReadings.size()-2] * 100)) / 2));
                //Note: Multiplied lastCurrentReadings by 100 since filteredTsCurrent is in 100 mAs
                soc_timer.reset();
                soc_timer.start();
            }
        }
        // printf("capacity discharged: %d\n", capacityDischarged);
        state_of_charge = 100 - (100 * capacityDischarged / (CELL_CAPACITY_RATED));
        uint32_t cell_voltage = packVoltagemV / (BMS_BANK_COUNT * BMS_BANK_CELL_COUNT);

        int32_t new_capacity_discharged = soc_energy(cell_voltage, capacityDischarged);
        state_of_charge = 100 - (100 * new_capacity_discharged / (CELL_CAPACITY_RATED));
        // printf("state of charge: %d\n", state_of_charge);

        // printf("pack voltage: %d\n", packVoltagemV);

        if (canBus->rderror() > 250 || canBus->tderror() > 250)
        {
            printf("CanBus Error overflow! rderror: %d, txerror: %d\n", canBus->rderror(), canBus->tderror());
            canBus->reset();
        }

        queue.dispatch_once();
        ThisThread::sleep_for(5 - (t.read_ms() % 5));
    }
}
// todo: manage the fan percent; when precharge is still going-> 0; as soon as precharge is done->20; then (linearly) scale to highest temp cell
// todo: when temp is 50, fan percent is 100
void initIO() {
    fan_pwm.period_us(40);
    fan_pwm.write(0);
    fan_percent = 0;
    bms_fault_pin = !hasBmsFault; // assume no fault at start, 1 is no fault
    bms_fault_inverse_pin = hasBmsFault; // assume no fault at start, 0 is no fault
    precharge_control_pin = 0; // positive AIR open at start
    state_of_charge = 100; // TODO: CHANGE TO ACCOUNT FOR POSSIBLE DISCHARGE

    canBus = new CAN(BMS_PIN_CAN_RX, BMS_PIN_CAN_TX, BMS_CAN_FREQUENCY);
    canBus->frequency(BMS_CAN_FREQUENCY);
    canBus->reset();


    spiDriver = new SPI(BMS_PIN_SPI_MOSI,
                             BMS_PIN_SPI_MISO,
                             BMS_PIN_SPI_SCLK,
                             BMS_PIN_SPI_SSEL,
                             use_gpio_ssel);
    spiDriver->format(8, 0);


    ThisThread::sleep_for(1ms);
    isCharging = charge_state_pin;
    queue.call_every(100ms, &canSendMain);
    printf("Am I charging?: %x\n", isCharging);
    if (isCharging) {
        queue.call_every(100ms, &sendSync);
    }
}


void canSendMain() {

    canSend(&status_message, packVoltagemV / 10, state_of_charge, (int16_t)(filteredTsCurrent / 10), fan_percent, allVoltages, allTemps);
}

void sendSync()
{
    // printf("send sync!\n");
    CANMessage msg = CANMessage();
    msg.id = 0x80; // Sync ID
    msg.len = 0;
    canBus->write(msg);
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

uint8_t linearFans_percent(int8_t temp) {
    int raw_percent = static_cast<uint8_t>((2.6667 * temp) - 33.3333);
    return std::clamp(raw_percent, 20, 100);
}

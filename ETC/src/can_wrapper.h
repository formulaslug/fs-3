//
// Created by wangd on 1/14/2025.
//

#ifndef CAN_WRAPPER_H
#define CAN_WRAPPER_H

#include "etc_controller.h"
#include "module.h"
#include "../mbed-os/mbed.h"

class CANWrapper : public Module {

    CAN* mainBus; // to be initialized
    CAN* motorBus; // to be initialized
    EventFlags& Global_Events;
    ETCController& etc;
    Ticker throttleTicker;
    // Ticker syncTicker;
    // Ticker stateTicker;

    const int32_t CAN_FREQ = 500000;

    constexpr static PinName MAIN_BUS_RD = PB_5;
    constexpr static PinName MAIN_BUS_TD = PB_6;
    constexpr static PinName MOTOR_BUS_RD = PA_11;
    constexpr static PinName MOTOR_BUS_TD = PA_12;

public:

    const int32_t THROTTLE_FLAG = (1UL << 0);
    const int32_t SYNC_FLAG = (1UL << 1);
    const int32_t STATE_FLAG = (1UL << 2);
    const int32_t RX_FLAG = (1UL << 3);


    CANWrapper(ETCController& etcController, EventFlags& events) : Global_Events(events), etc(etcController) {
        /* Attempt CAN connections */
        mainBus = new CAN(MAIN_BUS_RD, MAIN_BUS_TD, CAN_FREQ);
        motorBus = new CAN(MOTOR_BUS_RD, MOTOR_BUS_TD, CAN_FREQ);

        /* start regular ISR routine for sending*/
        throttleTicker.attach(callback([this]() {
            Global_Events.set(THROTTLE_FLAG);
            }), 1s);
        //
        // syncTicker.attach(callback([this]() {
        //     Global_Events.set(THROTTLE_FLAG);
        //     }), 100ms);
        //
        // stateTicker.attach(callback([this]() {
        //     Global_Events.set(THROTTLE_FLAG);
        //     }), 100ms);

        /* Set up CAN RX ISR */
        mainBus->attach(callback([this]() {
            Global_Events.set(RX_FLAG);
            }));
        motorBus->attach(callback([this]() {
            Global_Events.set(RX_FLAG);
            }));

    }


    /**
    * Sends throttle data to the CANBus
    *
    * NOTE* Sends 0 for torque demand until start conditions are met
    */
    void sendThrottle() {
        etc.updateMBBAlive();

        CANMessage throttleMessage;
        throttleMessage.id = 0x186;

        throttleMessage.data[0] = etc.getTorqueDemand();
        throttleMessage.data[1] = etc.getTorqueDemand() >> 8;
        throttleMessage.data[2] = etc.getMaxSpeed();
        throttleMessage.data[3] = etc.getMaxSpeed() >> 8;
        throttleMessage.data[4] = 0x00 | (0x01 & etc.isMotorForward()) | ((0x01 & !etc.isMotorForward()) << 1) | ((0x01 & etc.isMotorEnabled()) << 3);
        throttleMessage.data[5] = 0x00 | (0x0f & etc.getMBBAlive());
        throttleMessage.data[6] = 0x00;
        throttleMessage.data[7] = 0x00;

        //motorBus->write(throttleMessage);
        printf("Sending Throttle...");
    };

    void sendSync() {
        unsigned char data[0];
        CANMessage syncMessage(0x80, data, 0);
        //send syncMessage
    };

    //TODO add the proper values 
    void sendState() {
        CANMessage stateMessage;
        stateMessage.id = 0x1A1;

        stateMessage.data[0] = 0x00 | (0x01 & etc.isTSReady()) | ((0x01 & etc.isMotorEnabled()) << 1) | ((0x01 & CANFlag) << 2) | ((0x01 & RTDSqueued) << 3) | ((0x01 & cockpitSwitch << 4));
        stateMessage.data[1] = (int8_t)(brakes.read()*100);
        stateMessage.data[2] = (int8_t)(HE1_read*100);
        stateMessage.data[3] = (int8_t)(HE2_read*100);
        stateMessage.data[4] = (int8_t)(HE1_travel*100);
        stateMessage.data[5] = (int8_t)(HE2_travel*100);
        stateMessage.data[6] = (int8_t)(pedal_travel*100);
        stateMessage.data[7] = 0x00;
    };

    /**
     * Parse CAN msg and then run ETC updateStateFromCAN and provide parameters
     */
    void processCANRx();
};

#endif //CAN_WRAPPER_H

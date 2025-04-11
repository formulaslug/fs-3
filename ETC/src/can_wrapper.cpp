//
// Created by wangd on 1/14/2025.
//

#include "../mbed-os/mbed.h"

#include "can_wrapper.h"
#include "etc_controller.h"

#include "module.h"

CANWrapper::CANWrapper(ETCController& etcController, EventFlags& events)
    : Global_Events(events),
    etc(etcController) {
        /* Attempt CAN connections */

        // TODO add fail code for failed CAN instantiation
        mainBus = new CAN(MAIN_BUS_RD, MAIN_BUS_TD, CAN_FREQ);
        /* start regular ISR routine for sending*/
        throttleTicker.attach(callback([this]() {
                    // NOTE that we do 1 sec interval for testing it should rly be
                    // 100ms
                    Global_Events.set(THROTTLE_FLAG);
                    }),
                1s);
        //
        // syncTicker.attach(callback([this]() {
        //     Global_Events.set(THROTTLE_FLAG);
        //     }), 100ms);
        //
        // stateTicker.attach(callback([this]() {
        //     Global_Events.set(THROTTLE_FLAG);
        //     }), 100ms);

        /* Set up CAN RX ISR */
        mainBus->attach(callback([this]() { Global_Events.set(RX_FLAG); }));
    }

void CANWrapper::sendThrottle() {
    etc.updateMBBAlive();

    auto [mbb_alive, he1_read, he2_read, he1_travel, he2_travel, pedal_travel, brakes_read,
         ts_ready, motor_enabled, motor_forward, cockpit, torque_demand] = etc.getState();

    CANMessage throttleMessage;
    throttleMessage.id = 0x186;

    throttleMessage.data[0] = torque_demand;
    throttleMessage.data[1] = torque_demand >> 8;
    throttleMessage.data[2] = etc.getMaxSpeed();
    throttleMessage.data[3] = etc.getMaxSpeed() >> 8;
    throttleMessage.data[4] = 0x00 | (0x01 & motor_forward) | ((0x01 & !motor_forward) << 1) |
        ((0x01 & motor_enabled) << 3);
    throttleMessage.data[5] = 0x00 | (0x0f & mbb_alive);
    throttleMessage.data[6] = 0x00;
    throttleMessage.data[7] = 0x00;

    // mainBus->write(throttleMessage);
    printf("Sending Throttle...");
}

void CANWrapper::sendSync() {
    unsigned char data[0];
    CANMessage syncMessage(0x80, data, 0);
    // send syncMessage
}

void CANWrapper::sendState() {
    ETCState state = etc.getState();

    CANMessage stateMessage;
    stateMessage.id = 0x1A1;

    stateMessage.data[0] = 0x00 | (0x01 & etc.isTSReady()) |
        ((0x01 & etc.isMotorEnabled()) << 1) | ((0x01 & state.cockpit << 4));
    stateMessage.data[1] = static_cast<int8_t>(state.brakes_read * 100);
    stateMessage.data[2] = static_cast<int8_t>(state.he1_read * 100);
    stateMessage.data[3] = static_cast<int8_t>(state.he2_read * 100);
    stateMessage.data[4] = static_cast<int8_t>(state.he1_travel * 100);
    stateMessage.data[5] = static_cast<int8_t>(state.he2_travel * 100);
    stateMessage.data[6] = static_cast<int8_t>(state.pedal_travel * 100);
    stateMessage.data[7] = 0x00;
}

void CANWrapper::processCANRx() {
    if (CANMessage rx; mainBus->read(rx)) {
    }
}

#include "can_wrapper.h"
#include "etc_controller.h"
#include "mbed.h"
#include <cstdint>


CANWrapper::CANWrapper(ETCController& etcController, EventFlags& events)
    : globalEvents(events),
      etc(etcController)
{
    this->bus = new CAN(CANWrapper::CAN_RX_PIN, CANWrapper::CAN_TX_PIN, CANWrapper::CAN_FREQUENCY);

    // Start regular ISR routine for sending
    this->throttleTicker.attach(callback([this]() {
        this->globalEvents.set(CANWrapper::THROTTLE_FLAG);
    }), 40ms);

    this->syncTicker.attach(callback([this]() {
        this->globalEvents.set(CANWrapper::SYNC_FLAG);
    }), 100ms);

    this->stateTicker.attach(callback([this]() {
        this->globalEvents.set(CANWrapper::STATE_FLAG);
    }), 100ms);

    // Set up CAN receive ISR
    this->bus->attach(callback([this]() {
        this->globalEvents.set(CANWrapper::RX_FLAG);
    }));
}

void CANWrapper::sendThrottle() {
    this->etc.updateMBBAlive();

    ETCState state = this->etc.getState();

    CANMessage throttleMessage;
    throttleMessage.id = 0x186;

    throttleMessage.data[0] = state.torque_demand;
    throttleMessage.data[1] = state.torque_demand >> 8;
    throttleMessage.data[2] = static_cast<uint8_t>(ETCController::MAX_SPEED);
    throttleMessage.data[3] = static_cast<uint8_t>(ETCController::MAX_SPEED >> 8);
    throttleMessage.data[4] =
        (0x01 & state.motor_forward) |
        ((0x01 & !state.motor_forward) << 1) |
        ((0x01 & state.motor_enabled) << 3);
    throttleMessage.data[5] = 0x0f & state.mbb_alive;
    throttleMessage.data[6] = 0x00;
    throttleMessage.data[7] = 0x00;

    this->bus->write(throttleMessage);
}


void CANWrapper::sendSync() {
    uint8_t data[0];
    CANMessage syncMessage(0x80, data, 0);

    this->bus->write(syncMessage);
}


void CANWrapper::sendState() {
    ETCState state = this->etc.getState();

    CANMessage stateMessage;
    stateMessage.id = 0x1A1;

    stateMessage.data[0] =
        (0x01 & state.ts_ready) |
        ((0x01 & state.motor_enabled) << 1) |
        ((0x01 & state.cockpit << 4));
    stateMessage.data[1] = static_cast<int8_t>(state.brakes_read * 100);
    stateMessage.data[2] = static_cast<int8_t>(state.he1_travel * 100);
    stateMessage.data[3] = static_cast<int8_t>(state.he2_travel * 100);
    stateMessage.data[4] = static_cast<int8_t>(state.pedal_travel * 100);
    stateMessage.data[5] = 0x00;
    stateMessage.data[6] = 0x00;
    stateMessage.data[7] = 0x00;

    this->bus->write(stateMessage);
}

void CANWrapper::processCANRx() {
    CANMessage rx;
    if (this->bus->read(rx)) {
        /** TODO: process the received message... */
    }
}

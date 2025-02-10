//
// Created by wangd on 1/14/2025.
//

#include "can_wrapper.h"

/**
 * Reads off CAN msg and then composes ETCState struct to updateStateFromCAN(ETCState& state)
 */
void CANWrapper::processCANRx() {
    if (CANMessage rx; mainBus->read(rx)) {
    }
    if (CANMessage rx; motorBus->read(rx)) {
    }
}

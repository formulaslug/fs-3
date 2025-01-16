//
// Created by wangd on 1/14/2025.
//

#include "etc_controller.h"

void ETCController::updatePedalTravel() {
    
}

void ETCController::updateMBBAlive() {
    state.mbb_alive++;
    state.mbb_alive %= 16;
}

void ETCController::updateStateFromCAN(const ETCState& new_state) {
    state = new_state;
}

void ETCController::resetState() {
    state.mbb_alive = 0;
    state.he1_read = 0.f;
    state.he2_read = 0.f;
    state.brakes_read = 0.f;
    state.ts_ready = false;
    state.motor_enabled = false;
    state.motor_forward = true;
    state.torque_demand = 0;
}

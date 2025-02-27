//
// Created by wangd on 1/14/2025.
//

#include "etc_controller.h"

// TODO make the function :)))
void ETCController::updatePedalTravel() {
    /* read HE1 and HE2 sensors */

    /* calculate pedal travel using voltage divider ratio */

    /* Implausability check here*/

    /* update relevant values */
}

void ETCController::updateMBBAlive() {
    state.mbb_alive++;
    state.mbb_alive %= 16;
}

void ETCController::updateStateFromCAN(const ETCState& new_state) {
    state = new_state;
}

void ETCController::updateBrakeSignal() {
    bool brake_light_on = this->state.brakes_read > this->BRAKE_TOL;
    this->BrakesOut.write(brake_light_on);
}

// TODO make function : )
void ETCController::checkStartConditions() {
    /* this function is already set up to run when the cockpit is switched on */

    /* Check that the brake is pressed and also that TS_rdy is true */

    /* if all that is happening switch motor_on to true */
}

// TODO make function :)
void ETCController::runRTDS() { /* Run RTDS for 3 seconds */ }

void ETCController::resetState() {
    state.mbb_alive = 0;
    state.he1_read = 0.f;
    state.he2_read = 0.f;
    state.he1_travel = 0.f;
    state.he2_travel = 0.f;
    state.pedal_travel = 0.f;
    state.brakes_read = 0.f;
    state.ts_ready = false;
    state.motor_enabled = false;
    state.motor_forward = true;
    state.cockpit = false;
    state.torque_demand = 0;
}

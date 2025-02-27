//
// Created by wangd on 1/14/2025.
//

#include "etc_controller.h"

// TODO make the function :)))
void ETCController::updateState() {

   float he1_voltage = (HE1.read() * MAX_V)/VOLT_SCALE_he1;
   float he2_voltage = (HE2.read() * MAX_V)/VOLT_SCALE_he2;

    /* convert sensor voltages into travel percentages*/
    // voltage - 0.25/ 2 * range (0.20) * 100.0 to turn to percentage
    constexpr float he1_lowerbound = 0.344f;
    constexpr float he1_upperbound = 3.094f;
    constexpr float he2_lowerbound = 0.25f;
    constexpr float he2_upperbound = 2.25f;
    const float he1_travel_percent = ((he1_voltage - he1_lowerbound) / (he1_upperbound - he1_lowerbound));
    const float he2_travel_percent = ((he2_voltage - he2_lowerbound) / (he2_upperbound - he2_lowerbound));


    /* Implausibility check here*/
    float abs_difference = fabs(he1_travel_percent - he2_travel_percent);


   /* calculate pedal travel using voltage divider ratio */
   if (he1_voltage == 0.0f || he1_voltage == 3.3f ||
       he2_voltage == 0.0f || he2_voltage == 3.3f){
       if (!voltage_timer_running) {
           // we now start our timer, if it's not already running
           VoltageTimer.start();
           voltage_timer_running = true;
       }
       else if (VoltageTimer.elapsed_time() > 100ms) {
           VoltageTimer.stop();
           VoltageTimer.reset();
           voltage_timer_running = false;
           state.motor_enabled = false;
           return;
           }
       else {
               // if everything is good and timer is running, we reset
               VoltageTimer.stop();
               VoltageTimer.reset();
               voltage_timer_running = false;
        }
    }

   if (abs_difference > 0.1f) {
       if (!out_of_range_timer_running){
           // we now start our timer, if it's not already running
           OutOfRangeTimer.start();
           out_of_range_timer_running = true;
           }
       else if (OutOfRangeTimer.elapsed_time() > 100ms){
           OutOfRangeTimer.stop();
           OutOfRangeTimer.reset();
           out_of_range_timer_running = false;
           state.motor_enabled = false;
           return;
       }
       else if (voltage_timer_running) {
               OutOfRangeTimer.stop();
               OutOfRangeTimer.reset();
               out_of_range_timer_running = false;
           }
       }


   /* update relevant values */
   float pedal_travel = (he1_travel_percent + he2_travel_percent) / 2.0f;


   state.he1_read = HE1.read() * MAX_V;
   state.he2_read = HE2.read() * MAX_V;
   state.he1_travel = he1_travel_percent;
   state.he2_travel = he2_travel_percent;
   state.pedal_travel = pedal_travel;
   state.torque_demand = static_cast<int16_t>(pedal_travel * static_cast<float>(MAX_TORQUE));
   state.brakes_read = Brakes.read() * MAX_V; //TO-DO ask about brake sensor voltage output

}

void ETCController::updateMBBAlive() {
    state.mbb_alive++;
    state.mbb_alive %= 16;
}

void ETCController::updateStateFromCAN(const ETCState& new_state) { state = new_state; }

// TODO make function : )
void ETCController::checkStartConditions() {
    /* this function is already set up to run when the cockpit is switched on */

    /* Check that the brake is pressed and also that TS_rdy is true */

    /* if all that is happening switch motor_on to true */

    if(state.ts_ready && state.brakes_read >= BRAKE_TOL) {
        state.motor_enabled = true;
    }
}

// TODO make function :)
void ETCController::runRTDS() {
    RTDS.write(true);
    RTDS_Ticker.attach(callback([this] {stopRTDS();}), 1s);
}

void ETCController::stopRTDS() {
    RTDS.write(false);
    RTDS_Ticker.detach();
}

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

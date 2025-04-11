#include "etc_controller.h"


ETCController::ETCController()
    : HE1(PA_0),
      HE2(PB_0),
      Brakes(PC_0),
      Cockpit(PH_1),
      Reverse(PC_15),
      RTDS(PC_13),
      BrakeLight(PB_6)
{
    // Initialize state variables to their default conditions
    this->resetState();
    this->voltage_timer_running = false;
    this->out_of_range_timer_running = false;

    // Add ISRs for cockpit and reverse switches
    Cockpit.rise(callback([this]() { this->turnOffMotor(); }));
    Cockpit.fall(callback([this]() { this->checkStartConditions(); }));
    Reverse.rise(callback([this]() { this->switchForwardMotor(); }));
    Reverse.fall(callback([this]() { this->switchReverseMotor(); }));
}


void ETCController::updateState() {
   float he1_voltage = (this->HE1.read() * ETCController::MAX_V) / ETCController::VOLT_SCALE_he1;
   float he2_voltage = (this->HE2.read() * ETCController::MAX_V) / ETCController::VOLT_SCALE_he2;

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
    if (abs_difference > 0.1f){
        if (!this->voltage_timer_running) {
            // we now start our timer, if it's not already running
            this->VoltageTimer.start();
            this->voltage_timer_running = true;
        }
        else if (this->VoltageTimer.elapsed_time() > 100ms) {
            this->VoltageTimer.stop();
            this->VoltageTimer.reset();
            this->voltage_timer_running = false;
            this->state.motor_enabled = false;
            return;
        }
        else {
            // if everything is good and timer is running, we reset
            this->VoltageTimer.stop();
            this->VoltageTimer.reset();
            this->voltage_timer_running = false;
        }
    }

    if (he1_voltage <= 0.0f || he1_voltage >= 3.3f ||
        he2_voltage <= 0.0f || he2_voltage >= 3.3f) {
        if (!this->out_of_range_timer_running){
            // we now start our timer, if it's not already running
            this->OutOfRangeTimer.start();
            this->out_of_range_timer_running = true;
        }
        else if (this->OutOfRangeTimer.elapsed_time() > 100ms){
            this->OutOfRangeTimer.stop();
            this->OutOfRangeTimer.reset();
            this->out_of_range_timer_running = false;
            this->state.motor_enabled = false;
            return;
        }
        else if (this->voltage_timer_running) {
            this->OutOfRangeTimer.stop();
            this->OutOfRangeTimer.reset();
            this->out_of_range_timer_running = false;
        }
    }


   /* update relevant values */
   float pedal_travel = (he1_travel_percent + he2_travel_percent) / 2.0f;

   this->state.he1_read = this->HE1.read() * ETCController::MAX_V;
   this->state.he2_read = this->HE2.read() * ETCController::MAX_V;
   this->state.he1_travel = he1_travel_percent;
   this->state.he2_travel = he2_travel_percent;
   this->state.pedal_travel = pedal_travel;
   this->state.torque_demand = static_cast<int16_t>(pedal_travel * ETCController::MAX_TORQUE);
   /** TODO: ask about brake sensor voltage output */
   this->state.brakes_read = this->Brakes.read() * ETCController::MAX_V; 
}


void ETCController::updateMBBAlive() {
    this->state.mbb_alive++;
    this->state.mbb_alive %= 16;
}


void ETCController::updateStateFromCAN(const ETCState& new_state) {
    this->state = new_state;
}


void ETCController::checkStartConditions() {
    // If the brake is pressed past the tolerance threshold and the tractive system is ready
    // then the motor can be enabled. The last condition for motor start is the cockpit switch
    // being set to the ON position, which is what calls this method.
    if(this->state.ts_ready && this->state.brakes_read >= ETCController::BRAKE_TOL) {
        this->state.motor_enabled = true;
    }
}


void ETCController::runRTDS() {
    this->RTDS.write(true);
    this->RTDS_Ticker.attach(callback([this] {this->stopRTDS();}), 1s);
}


void ETCController::stopRTDS() {
    this->RTDS.write(false);
    this->RTDS_Ticker.detach();
}


void ETCController::resetState() {
    this->state.mbb_alive = 0;
    this->state.he1_read = 0.f;
    this->state.he2_read = 0.f;
    this->state.he1_travel = 0.f;
    this->state.he2_travel = 0.f;
    this->state.pedal_travel = 0.f;
    this->state.brakes_read = 0.f;
    this->state.ts_ready = false;
    this->state.motor_enabled = false;
    this->state.motor_forward = true;
    this->state.cockpit = false;
    this->state.torque_demand = 0;
}


ETCState ETCController::getState() const {
    return this->state;
}


uint8_t ETCController::getMBBAlive() const {
    return this->state.mbb_alive;
}


float ETCController::getBrakes() const {
    return this->state.brakes_read;
}


float ETCController::getHE1Read() const {
    return this->state.he1_read;
}


float ETCController::getHE2Read() const {
    return this->state.he2_read;
}


float ETCController::getHE1Travel() const {
    return this->state.he1_travel;
}


float ETCController::getHE2Travel() const {
    return this->state.he2_travel;
}


float ETCController::getPedalTravel() const {
    return this->state.pedal_travel;
}


int16_t ETCController::getTorqueDemand() const {
    return this->state.motor_enabled ? this->state.torque_demand : 0;
}


bool ETCController::isMotorForward() const {
    return this->state.motor_forward;
}


bool ETCController::isMotorEnabled() const {
    return this->state.motor_enabled;
}


bool ETCController::isTSReady() const {
    return this->state.ts_ready;
}


bool ETCController::isCockpit() const {
    return this->state.cockpit;
}


void ETCController::switchReverseMotor() {
    this->state.motor_forward = false;
}


void ETCController::switchForwardMotor() {
    this->state.motor_forward = true;
}


void ETCController::turnOffMotor() {
    this->state.motor_enabled = false;
}

#include "etc_controller.h"
#include <cmath>
#include <cstdint>


ETCController::ETCController()
    : he1Input(PA_7),
      he2Input(PA_1),
      brakePedalInput(PA_0),
      cockpitSwitchInterrupt(PB_0),
      reverseSwitchInterrupt(PB_1),
      rtdsOutput(PB_5),
      brakeLightOutput(PB_4)
{
    // Initialize state variables to their default conditions
    this->resetState();
    this->implausTravelTimerRunning = false;
    this->implausBoundsTimerRunning = false;

    // Add ISRs for cockpit and reverse switches
    this->cockpitSwitchInterrupt.rise(callback([this]() {
        this->turnOffMotor();
        this->state.cockpit = false;
    }));
    this->cockpitSwitchInterrupt.fall(callback([this]() {
        this->checkStartConditions();
        this->state.cockpit = true;
    }));
    this->reverseSwitchInterrupt.rise(callback([this]() { this->switchForwardMotor(); }));
    this->reverseSwitchInterrupt.fall(callback([this]() { this->switchReverseMotor(); }));
}


void ETCController::updateState() {
    float he1Voltage = this->he1Input.read() * ETCController::MAX_VOLTAGE;
    float he2Voltage = this->he2Input.read() * ETCController::MAX_VOLTAGE;

    float he1Travel = (he1Voltage - ETCController::HE1_LOW_VOLTAGE) / ETCController::HE1_RANGE;
    float he2Travel = (he2Voltage - ETCController::HE2_LOW_VOLTAGE) / ETCController::HE2_RANGE;
    he1Travel = clamp(he1Travel, 0.0f, 1.0f);
    he2Travel = clamp(he2Travel, 0.0f, 1.0f);

    // Check for implausibility conditions: either the travel percent difference between the
    // two sensors differs by too much, or the voltages of either sensor is out of range (less
    // than 0% or more than 100% pedal travel).

    float travelDifference = std::fabs(he1Travel - he2Travel);
    if (travelDifference > 0.1f) {
        if (!this->implausTravelTimerRunning) {
            // we now start our timer, if it's not already running
            this->implausTravelTimer.start();
            this->implausTravelTimerRunning = true;
        }
        else if (this->implausTravelTimer.elapsed_time() > 100ms) {
            this->implausTravelTimer.stop();
            this->implausTravelTimer.reset();
            this->implausTravelTimerRunning = false;
            this->turnOffMotor();
        }
    }
    else {
        // if everything is good and timer is running, we reset
        this->implausTravelTimer.stop();
        this->implausTravelTimer.reset();
        this->implausTravelTimerRunning = false;
    }

    if (he1Voltage <= 0.05f || he1Voltage >= ETCController::MAX_VOLTAGE ||
        he2Voltage <= 0.05f || he2Voltage >= ETCController::MAX_VOLTAGE)
    {
        if (!this->implausBoundsTimerRunning) {
            // we now start our timer, if it's not already running
            this->implausBoundsTimer.start();
            this->implausBoundsTimerRunning = true;
        }
        else if (this->implausBoundsTimer.elapsed_time() > 100ms) {
            this->implausBoundsTimer.stop();
            this->implausBoundsTimer.reset();
            this->implausBoundsTimerRunning = false;
            this->turnOffMotor();
        }
    }
    else {
        this->implausBoundsTimer.stop();
        this->implausBoundsTimer.reset();
        this->implausBoundsTimerRunning = false;
    }

    // At this point, we have passed all the implausibility checks for the current loop. We
    // can then update the state information related to pedal travel.

    float pedalTravel = (he1Travel + he2Travel) / 2.0f;

    this->state.he1_read = this->he1Input.read() * ETCController::MAX_VOLTAGE;
    this->state.he2_read = this->he2Input.read() * ETCController::MAX_VOLTAGE;
    this->state.he1_travel = he1Travel;
    this->state.he2_travel = he2Travel;
    this->state.pedal_travel = pedalTravel;
    this->state.torque_demand =
        this->state.motor_enabled ?
        static_cast<int16_t>(pedalTravel * ETCController::MAX_TORQUE) :
        0;
    this->state.brakes_read = this->brakePedalInput.read() * ETCController::MAX_VOLTAGE;

    this->brakeLightOutput.write(this->state.brakes_read >= ETCController::BRAKE_TOLERANCE);
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
    if(this->state.ts_ready && this->state.brakes_read >= ETCController::BRAKE_TOLERANCE) {
        this->state.motor_enabled = true;
        this->runRTDS();
    }
}


void ETCController::runRTDS() {
    this->rtdsOutput.write(true);
    this->rtdsTicker.attach(callback([this] {this->stopRTDS();}), 1s);
}


void ETCController::stopRTDS() {
    this->rtdsOutput.write(false);
    this->rtdsTicker.detach();
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
    return this->state.torque_demand;
}


bool ETCController::isMotorForward() const {
    return this->state.motor_forward;
}


bool ETCController::isMotorEnabled() const {
    return this->state.motor_enabled;
}


bool ETCController::isTractiveSystemReady() const {
    return this->state.ts_ready;
}


bool ETCController::isCockpitSwitchSet() const {
    return this->state.cockpit;
}


void ETCController::switchReverseMotor() {
    // this->state.motor_forward = false;
}


void ETCController::switchForwardMotor() {
    this->state.motor_forward = true;
}


void ETCController::turnOffMotor() {
    this->state.motor_enabled = false;
}



bool ETCController::getRTDS() {
    return this->rtdsOutput.read();
}


bool ETCController::isBraking() {
    return this->brakeLightOutput.read();
}


bool ETCController::hasImplausibility() {
    return this->implausTravelTimerRunning || this->implausBoundsTimerRunning;
}

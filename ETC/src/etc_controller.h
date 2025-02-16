//
// Created by wangd on 1/14/2025.
//

#ifndef ETC_CONTROLLER_H
#define ETC_CONTROLLER_H

#include "../mbed-os/mbed.h"
#include "module.h"

struct ETCState {
    uint8_t mbb_alive;
    float he1_read;
    float he2_read;
    float he1_travel;
    float he2_travel;
    float pedal_travel;
    float brakes_read;
    bool ts_ready;
    bool motor_enabled;
    bool motor_forward;
    bool cockpit;
    int16_t torque_demand;
};

class ETCController {
    // Digital and Analog Inputs/Outputs
    AnalogIn HE1;
    AnalogIn HE2;
    AnalogIn Brakes;
    InterruptIn Cockpit;
    InterruptIn Reverse;
    DigitalOut BrakesOut;
    DigitalOut RTDS;

    // State Variables
    ETCState state{0};

    Ticker brakeSignalTicker;

public:
    // Constants
    const int16_t MAX_SPEED = 7500;
    const int16_t MAX_TORQUE = 30000;
    const float MAX_V = 3.3;
    const float BRAKE_TOL = 0.1;

    // Constructor
    ETCController()
        : HE1(PA_0),
          HE2(PB_0),
          Brakes(PC_0),
          Cockpit(PH_1),
          Reverse(PC_15),
          BrakesOut(PC_14),
          RTDS(PC_13) {
        resetState();

        // Update the status of the brake light periodically.
        this->brakeSignalTicker.attach(callback([this]() {
            this->updateBrakeSignal();
        }), 100ms);

        /* ADD ISR for Cockpit and Reverse */
        Cockpit.rise(callback([this]() { turnOffMotor(); }));
        Cockpit.fall(callback([this]() { checkStartConditions(); }));
        Reverse.rise(callback([this]() { switchForwardMotor(); }));
        Reverse.fall(callback([this]() { switchReverseMotor(); }));
    }

    // Member Functions

    /**
     * Read Hall Effect Sensors and then update ETC State. Checks implausibility also and starts
     * timer.
     */
    void updatePedalTravel();

    /**
     * Add to state.mbbalive and then %= 16
     */
    void updateMBBAlive();

    /**
     * Update state given ETCState struct
     * @param new_state
     */
    void updateStateFromCAN(const ETCState& new_state);

    /**
     * Updates the digital output signal on the brake light pin based on the current ETC state.
     */
    void updateBrakeSignal();

    /**
     * Reset state to default values
     */
    void resetState();

    /**
     *  Runs on falling cockpit switch, checks if brakes are pressed and TS is ready, then switches
     * motor_enabled to true
     */
    void checkStartConditions();

    /**
     *  Runs RTDS for 3 seconds
     */
    void runRTDS();

    // Accessors
    [[nodiscard]] uint8_t getMBBAlive() const { return state.mbb_alive; }
    [[nodiscard]] float getBrakes() const { return state.brakes_read; }
    [[nodiscard]] float getHE1Read() const { return state.he1_read; }
    [[nodiscard]] float getHE2Read() const { return state.he2_read; }
    [[nodiscard]] float getHE1Travel() const { return state.he1_travel; }
    [[nodiscard]] float getHE2Travel() const { return state.he2_travel; }
    [[nodiscard]] float getPedalTravel() const { return state.pedal_travel; }
    [[nodiscard]] int16_t getTorqueDemand() const {
        return state.motor_enabled ? state.torque_demand : 0;
    }
    [[nodiscard]] int16_t getMaxSpeed() const { return MAX_SPEED; }

    [[nodiscard]] bool isMotorForward() const { return state.motor_forward; }
    [[nodiscard]] bool isMotorEnabled() const { return state.motor_enabled; }
    [[nodiscard]] bool isTSReady() const { return state.ts_ready; }
    [[nodiscard]] bool isCockpit() const { return state.cockpit; }

    [[nodiscard]] ETCState getState() const { return state; }

    void switchReverseMotor() { state.motor_forward = false; }
    void switchForwardMotor() { state.motor_forward = true; }

    void turnOffMotor() { state.motor_enabled = false; }
};

#endif  // ETC_CONTROLLER_H

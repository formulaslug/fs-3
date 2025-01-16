//
// Created by wangd on 1/14/2025.
//

#ifndef ETC_CONTROLLER_H
#define ETC_CONTROLLER_H

#include "module.h"
#include "../mbed-os/mbed.h"



class ETCController {
private:

    struct ETCState {
        uint8_t mbb_alive;
        float he1_read;
        float he2_read;
        float brakes_read;
        bool ts_ready;
        bool motor_enabled;
        bool motor_forward;
        int16_t torque_demand;
    };

    // Digital and Analog Inputs/Outputs
    AnalogIn HE1;
    AnalogIn HE2;
    AnalogIn Brakes;
    DigitalIn Cockpit;
    DigitalIn Reverse;
    DigitalOut RTDS;

    // Constants
    const int16_t MAX_SPEED = 7500;
    const int16_t MAX_TORQUE = 30000;
    const float MAX_V = 3.3;
    const float BRAKE_TOL = 0.1;

    // State Variables
    ETCState state{0};

public:
    // Constructor
    ETCController()
        : HE1(PA_0), HE2(PB_0), Brakes(PC_0),
          Cockpit(PH_1), Reverse(PC_15), RTDS(PC_13) {
        resetState();
    }

    // Member Functions

    /**
     * Read Hall Effect Sensors and then update ETC State
     */
    void updatePedalTravel();

    /**
     * Add to state.mbbalive and then %= 16
     */
    void updateMBBAlive();

    /**
     * Update state from ETCState struct
     * @param new_state
     */
    void updateStateFromCAN(const ETCState& new_state);

    /**
     * Reset state
     */
    void resetState();

    // Accessors
    [[nodiscard]] uint8_t getMBBAlive() const { return state.mbb_alive; };
    [[nodiscard]] float getBrakes() const { return state.brakes_read; };
    [[nodiscard]] int16_t getTorqueDemand() const { return state.motor_enabled ? state.torque_demand : 0; };
    [[nodiscard]] int16_t getMaxSpeed() const { return MAX_SPEED; };

    [[nodiscard]] bool isMotorForward() const { return state.motor_forward; };
    [[nodiscard]] bool isMotorEnabled() const { return state.motor_enabled; };
    [[nodiscard]] bool isTSReady() const { return state.ts_ready; };

};

#endif //ETC_CONTROLLER_H

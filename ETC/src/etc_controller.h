/**
 * @file etc_controller.h
 *
 * Representation of the state of the ETC with methods to set, reset, and modify state attributes.
 */


#ifndef ETC_CONTROLLER_H
#define ETC_CONTROLLER_H


#include "mbed.h"
#include <cstdint>


/**
 * Represents the current state of the ETC.
 *
 * @field mbb_alive      A counter on [0, 15] that updates to indicate a "live" ping from the ETC.
 * @field he1_read       The hall-effect 1 sensor voltage scaled to the interval [0, 1].
 * @field he2_read       The hall-effect 2 sensor voltage scaled to the interval [0, 1].
 * @field he1_travel     The percent travel of the hall-effect 1 sensor toward its stop.
 * @field he2_travel     The percent travel of the hall-effect 2 sensor toward its stop.
 * @field pedal_travel   The pedal travel percentage, which is the average of the HE travels.
 * @field brakes_read    The brake pressure voltage scaled to the interval [0, 1].
 * @field ts_ready       Whether the tractive system is ready.
 * @field motor_enabled  Whether the motor is currently running.
 * @field motor_forward  Whether the motor is in forward drive mode (as opposed to reverse).
 * @field cockpit        Whether the cockpit switch is in the ON position.
 * @field torque_demand  The current torque demand in RPM.
 */
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
    bool brakes_implausibility;
};


/**
 * Controller for the parameters around an {@code ETCState} structure.
 */
class ETCController {
    /** The pin connected to the hall-effect 1 sensor. */
    AnalogIn he1Input;
    /** The pin connected to the hall-effect 2 sensor. */
    AnalogIn he2Input;
    /** The pin connected to the brake pedal sensor. */
    AnalogIn brakePedalInput;
    /** Interrupt to trigger when the cockpit switch is set. */
    InterruptIn cockpitSwitchInterrupt;
    /** Interrupt to trigger when the reverse switch is set. */
    InterruptIn reverseSwitchInterrupt;
    /** Output pin for the ready-to-drive buzzer. */
    DigitalOut rtdsOutput;
    /** Output pin to turn on the brake light. */
    DigitalOut brakeLightOutput;

    /** Timer for implausibility via HE sensor travel percent mismatch. */
    Timer implausTravelTimer;
   /** Whether the implausibility travel percent mismatch timer is active. */
    bool implausTravelTimerRunning;
    /** Timer for implausibility via HE sensor invalid voltages. */
    Timer implausBoundsTimer;
    /** Whether the implausibility invalid voltage timer is active. */
    bool implausBoundsTimerRunning;

    /** Timer for the RTDS activation. */
    Ticker rtdsTicker;

    /** Ticker for debouncing cockpit switch */
    Ticker cockpitSwitchTicker;

    /** State of the ETC. */
    ETCState state;

public:
    /** The maximum motor speed in RPM. */
    static constexpr int16_t MAX_SPEED = 7500;
    /** The maximum motor torque in Nm. */
    static constexpr int16_t MAX_TORQUE = 30000;
    /** The maximum microcontroller pin voltage in volts. */
    static constexpr float MAX_VOLTAGE = 3.3f;

    /** The percentage tolerance for the brake pedal to be considered pressed. */
    static constexpr float BRAKE_TOLERANCE_HIGH = 0.42f;
    static constexpr float BRAKE_TOLERANCE_LOW = 0.36f;

    /** The voltage divider slope for the hall-effect 1 sensor. */
    static constexpr float HE1_SCALE = (330.0f / 480.0f);
    /** The voltage for HE1 corresponding to 0% pedal travel. */
    static constexpr float HE1_LOW_VOLTAGE = 0.585f; // 0.425f
    /** The voltage for HE1 corresponding to 100% pedal travel. */
    static constexpr float HE1_HIGH_VOLTAGE = 2.650f; // 2.420f
    /** The difference between the maximum and minimum voltages (100% and 0% travel) for HE1. */
    static constexpr float HE1_RANGE =
        ETCController::HE1_HIGH_VOLTAGE - ETCController::HE1_LOW_VOLTAGE;
    /** The voltage divider slope for the hall-effect 2 sensor. */
    static constexpr float HE2_SCALE = (1.0f / 2.0f);
    /** The voltage for HE2 corresponding to 0% pedal travel. */
    static constexpr float HE2_LOW_VOLTAGE = 0.505f; // 0.417f
    /** The voltage for HE2 corresponding to 100% pedal travel. */
    static constexpr float HE2_HIGH_VOLTAGE = 2.130f; // 1.830f
    /** The difference between the maximum and minimum voltages (100% and 0% travel) for HE2. */
    static constexpr float HE2_RANGE =
        ETCController::HE2_HIGH_VOLTAGE - ETCController::HE2_LOW_VOLTAGE;


    /**
     * Constructs a new {@code ETCController} object with a reset state.
     *
     * ISRs are added for the cockpit and reverse switch at construction time.
     */
    ETCController();


    /**
     * Updates the state of the ETC from the current values read off the hall-effect sensors.
     *
     * After updating the state, this function checks the conditions for implausibility and
     * starts or resets the appropriate timers. If an implausibility is detected, the motor
     * is turned off.
     */
    void updateState();


    /**
     * Increments the {@code mmb_alive} field of the ETC state, wrapping after 15.
     */
    void updateMBBAlive();


    /**
     * Sets the current state to the provided state.
     *
     * @param new_state  The new state to set.
     */
    void updateStateFromCAN(const ETCState& new_state);


    /**
     * Checks the start conditions for the motor and sets {@code motor_enabled} if they are met.
     *
     * The conditions for start are:
     * - The brake pedal is depressed.
     * - The tractive system is ready.
     * - The cockpit switch is ON (this method is called on a falling cockpit switch).
     */
    void checkStartConditions();

    /**
     * Wait and check again before allowing starting
     */
    void debounceCockpitSwitch();

    /**
     * Runs the RTDS buzzer for 3 seconds.
     */
    void runRTDS();


    /**
     * Quiets the RTDS buzzer.
     */
    void stopRTDS();


    /**
     * Reset state to default values.
     */
    void resetState();


    /**
     * Returns the state of the ETC.
     */
    ETCState getState() const;

    /**
    * sets brakes implausibility
    */
    void set_brake_implausibility();

    /**
     * Returns {@code state.mbb_alive}.
     */
    uint8_t getMBBAlive() const;


    /**
     * Returns {@code state.brakes_read}.
     */
    float getBrakes() const;


    /**
     * Returns {@code state.he1_read}.
     */
    float getHE1Read() const;


    /**
     * Returns {@code state.he2_read}.
     */
    float getHE2Read() const;


    /**
     * Returns {@code state.he1_travel}.
     */
    float getHE1Travel() const;


    /**
     * Returns {@code state.he2_travel}.
     */
    float getHE2Travel() const;


    /**
     * Returns {@code state.pedal_travel}.
     */
    float getPedalTravel() const;


    /**
     * Returns {@code state.torque_demand} if the motor is enabled, otherwise {@code 0}.
     */
    int16_t getTorqueDemand() const;


    /**
     * Returns {@code state.motor_forward}.
     */
    bool isMotorForward() const;


    /**
     * Returns {@code state.motor_enabled}.
     */
    bool isMotorEnabled() const;


    /**
     * Returns {@code state.ts_ready}.
     */
    bool isTractiveSystemReady() const;


    /**
     * Returns {@code state.cockpit}.
     */
    bool isCockpitSwitchSet() const;


    /**
     * Sets the motor to reverse mode.
     */
    void switchReverseMotor();


    /**
     * Sets the motor to forward mode.
     */
    void switchForwardMotor();


    /**
     * Disables the motor.
     */
    void turnOffMotor();


    /**
     * Whether the RTDS buzzer is running.
     */
    bool getRTDS();


    /**
     * Whether the brake pedal is being pressed.
     */
    bool isBraking();


    /**
     * Whether an implausibility is currently detected.
     */
    bool hasImplausibility();
};


#endif  // ETC_CONTROLLER_H

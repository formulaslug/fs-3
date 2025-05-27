/**
 * @file can_wrapper.h
 *
 * Provides utility functions on top of the MbedOS CAN interface.
 */


#ifndef CAN_WRAPPER_H
#define CAN_WRAPPER_H

#include "etc_controller.h"
#include "mbed.h"
#include <cstdint>


/**
 * Manages reading and writing of CAN messages that concern the ETC.
 */
class CANWrapper {
    /** The main CAN bus on the car. */
    CAN* bus;
    /** MARK: what is this */
    EventFlags& globalEvents;
    /** The ETC controller and state to interface with when reading and writing messages. */
    ETCController& etc;
    /** Ticker to send regular throttle status messages. */
    Ticker throttleTicker;
    /** Ticker to send regular sync messages. */
    Ticker syncTicker;
    /** Ticker to send regular ETC state summary messages. */
    Ticker stateTicker;

public:
    /** The frequency of messages on the CAN bus, in Hz. */
    static constexpr int32_t CAN_FREQUENCY = 500000;
    /** The microcontroller pin to read messages on. */
    static constexpr PinName CAN_RX_PIN = PA_11;
    /** The microcontroller pin to write messages on. */
    static constexpr PinName CAN_TX_PIN = PA_12;

    /** The flag bit indicating a throttle message needs to be sent. */
    static constexpr int32_t THROTTLE_FLAG = (1UL << 0);
    /** The flag bit indicating a sync message needs to be sent. */
    static constexpr int32_t SYNC_FLAG = (1UL << 1);
    /** The flag bit indicating a state message needs to be sent. */
    static constexpr int32_t STATE_FLAG = (1UL << 2);
    /** The flag bit indicating received messages need to be processed. */
    static constexpr int32_t RX_FLAG = (1UL << 3);


    /**
     * Class constructor for CANWrapper
     *
     * Holds motor and main CAN bus, composes and handles routine CAN message, handles CAN RX as
     * well.
     *
     * @param etcController  The ETC controller object to interface with and collect data from.
     * @param events         The event flags to listen to (to use as markers to send messages).
     */
    CANWrapper(ETCController& etcController, EventFlags& events);

    /**
     * Sends throttle data to the CAN bus.
     *
     * NOTE: Sends 0 for torque demand until start conditions are met.
     */
    void sendThrottle();


    /**
     * Sends sync message via CAN.
     */
    void sendSync();


    /**
     * Sends ETC State via CAN.
     *
     * TODO: Update based on DBC...
     */
    void sendState();


    void sendCurrentLimits();


    /**
     * Reads a CAN message and updates the state of {@code tihs->etc}.
     *
     * TODO: implement
     */
    void processCANRx();
};

#endif  // CAN_WRAPPER_H

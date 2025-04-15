//
// Created by wangd on 1/14/2025.
//

#ifndef CAN_WRAPPER_H
#define CAN_WRAPPER_H

#include "../mbed-os/mbed.h"
#include "etc_controller.h"
#include "module.h"

class CANWrapper : public Module {
    CAN* mainBus;
    EventFlags& Global_Events;
    ETCController& etc;
    Ticker throttleTicker;
    Ticker syncTicker;

    const int32_t CAN_FREQ = 500000;

    constexpr static PinName MAIN_BUS_RD = PB_5;
    constexpr static PinName MAIN_BUS_TD = PB_6;

    public:
    const int32_t THROTTLE_FLAG = (1UL << 0);
    const int32_t SYNC_FLAG = (1UL << 1);
    const int32_t STATE_FLAG = (1UL << 2);
    const int32_t RX_FLAG = (1UL << 3);

    /**
     * Class constructor for CANWrapper
     * Holds motor and main CAN bus, composes and handles routine CAN message, handles CAN Rx as
     * well.
     *
     * @param etcController
     * @param events
     */
    CANWrapper(ETCController& etcController, EventFlags& events);

    /**
     * Sends throttle data to the CANBus
     *
     * NOTE* Sends 0 for torque demand until start conditions are met
     */
    void sendThrottle();

    /**
     * Sends sync message via CAN
     */
    void sendSync();

    /**
     * Sends ETC State via CAN
     *
     * TODO: Update based on DBC...
     */
    void sendState();


    /** TODO: Add sendCurrentLimits function... */

    /**
     * Reads off CAN msg and then composes ETCState struct to updateStateFromCAN(ETCState& state)
     * TODO: implement
     */
    void processCANRx();
};

#endif

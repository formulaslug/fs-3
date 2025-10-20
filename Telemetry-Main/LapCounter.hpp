/**
 * Lap Counter
 * Author: Nicholas Tang
 *
 * Made to separate lap counting from VSM. Uses VSM GPS and HEADING/TRUE_COURSE
 * data. Make sure to call updateLapCounter a lot.
 *
 * Note: "local coordinate plane" is defined as: nose towards positive x
 */

#ifndef LAP_COUNTER_HPP
#define LAP_COUNTER_HPP

#include "CANProtocol.hpp"
#include "VehicleStateManager.hpp"
#include "mbed.h"

struct LapCounterData {
    float lap_home_latitude_f;
    float lap_home_longitude_f;
    float lap_home_x_f;                 // relative to local coordinate plane
    float lap_home_y_f;                 // relative to local coordinate plane
    float lap_home_heading_f;

    float lap_prev_latitude_f;
    float lap_prev_longitude_f;
    float lap_prev_x_f;                 // relative to local coordinate plane
    float lap_prev_y_f;                 // relative to local coordinate plane
    float lap_prev_heading_f;

    float lap_curr_latitude_f;
    float lap_curr_longitude_f;
    float lap_curr_x_f;                 // relative to local coordinate plane
    float lap_curr_y_f;                 // relative to local coordinate plane
    float lap_curr_heading_f;

    uint8_t lap_counter;                // MARK: surely we don't do more than 255 laps(?)
    std::chrono::microseconds lap_time;
};

class LapCounter {
    public:
        LapCounterData data;
        Timer timer;

        /**
         * Constructor, initializes all fields to 0 but lat/long
         */
        LapCounter(VehicleState state);
        /**
         * Resets object field. GPS data is set to wherever is in vehicle state
         */
        void resetLapCounter(VehicleState state);
        /**
         * Checks if a lap has been completed. If so, increments lap counter
         */
        void updateLapCounter(VehicleState state);
        /**
         * Gets the lap time in microseconds. 
         */
        std::chrono::microseconds getTime();
};

#endif

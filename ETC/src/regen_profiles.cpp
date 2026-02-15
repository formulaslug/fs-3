#include "regen_profiles.h"

#include <cmath>

const float MIN_SPEED = 5.25f;                  //  regulation is 5.0 KPH (See EV.3.3.3) with 5% margin

float variable_profile(float accel_input, float brake_input, float linearity, float speed) {
    float accel = powf(accel_input, 1 + 4 * linearity);
    float brake = powf(brake_input, 1 + 4 * linearity);

    float torque_demand = accel - brake;
    
    if (speed < MIN_SPEED) 
        return torque_demand < 0.0f ? 0.0f : torque_demand;

    return torque_demand;
}

float template_profile(float driver_input, float speed) {
    // Do processing
    return 0.0f;
}

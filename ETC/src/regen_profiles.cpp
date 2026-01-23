#include "regen_profiles.h"

#include <cmath>

const float SIGN_THRESH = 0.35f;         //  when throttle transitions from
                                         //     negative to positive
const float MIN_SPEED = 5.25f;           //  regulation is 5.0 KPH (See EV.3.3.3)
                                         //      added 5% margin

float quartic_profile(float driver_input, float speed) {
    if (speed <= MIN_SPEED) {
        float out = driver_input - SIGN_THRESH;
        if (out <= 0.0f) return 0.0f;

        out = 0.25 * powf(fabsf(out), 4.0) + 
            0.50 * powf(fabsf(out), 3.0) + 
            0.25 * powf(fabsf(out), 2.0);             //  loses sign

        return out;
    }

    float sign = driver_input / fabsf(driver_input);

    float out = driver_input - SIGN_THRESH;     //  translation
    out = sign < 0 ? out / SIGN_THRESH : out / (1 - SIGN_THRESH);   //  scale to [-1, 1]
    out = 0.25 * powf(fabsf(out), 4.0) + 
        0.50 * powf(fabsf(out), 3.0) + 
        0.25 * powf(fabsf(out), 2.0);             //  loses sign
    out *= sign;

    return out;
}

float template_profile(float driver_input, float speed) {
    return 0.0f;
}

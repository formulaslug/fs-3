#include "regen_profiles.h"

#include <cmath>

const float SIGN_THRESH = 0.35f;         //  when throttle transitions from negative to positive
const float MIN_SPEED = 5.25f;           //  regulation is 5.0 KPH (See EV.3.3.3) with 5% margin

float generic_profile(float driver_input, float linearity, float speed) {
    float coeff[3]; //applied on x^4, x^3, x^2
    coeff[0] = (1.0f - linearity) / 2.0f;
    coeff[1] = (1.0f - linearity) / 2.0f;
    coeff[2] = linearity;
    
    if (speed <= MIN_SPEED) {
        float out = driver_input - SIGN_THRESH;
        if (out <= 0.0f) return 0.0f;

        out = coeff[0] * powf(fabsf(out), 4.0) + 
              coeff[1] * powf(fabsf(out), 3.0) + 
              coeff[2] * powf(fabsf(out), 2.0);             //  loses sign

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
    // Do processing
    return 0.0f;
}

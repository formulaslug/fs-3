// pd.h
#ifndef PD_H
#define PD_H
#include "mbed.h"

class PDController {
public:
    PDController(float kp, float kd);
    float update(float ws_fl, float ws_fr, float ws_rl, float ws_rr);
    void reset();

private:
    float Kp;
    float Kd;
    float prevError;
    float targetSlip;
    float minOutput;
    float maxOutput;
    Timer loopTimer;

    // TODO: replace with appropriate value
    // TC active when avg front wheel speed is greater than minimum_tc_rpm
    static constexpr float minimum_tc_rpm = 1.0f;
};

#endif // PD_H
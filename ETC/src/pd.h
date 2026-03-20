// pd.h
#ifndef PD_H
#define PD_H
#include "mbed.h"

class PDController {
public:
    PDController(float kp, float kd);
    float update(float ws_fl, float ws_fr, float ws_rl, float ws_rr);
    void reset();
    float getLastErr();

private:
    float Kp;
    float Kd;
    float prevError;
    float targetSlip;
    float minOutput;
    float maxOutput;
    Timer loopTimer;
    bool prevErrorStale;

    static constexpr float MIN_TC_RPM = 100.0f;
};

#endif // PD_H
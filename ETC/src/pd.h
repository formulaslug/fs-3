// pd.h
#ifndef PD_H
#define PD_H
#include "etc_controller.h"

class PDController {
public:
    PDController(float kp, float kd, float dt);

    float update(ETCController& etc);
    void reset();
    void setTargetSlip(float target);
    void setOutputLimits(float minVal, float maxVal);

private:
    float Kp;
    float Kd;
    float dt;
    float prevError;
    float targetSlip;
    float minOutput;
    float maxOutput;
};

#endif // PD_H
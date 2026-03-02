 #include "pD.h"
#include "etc_controller.h"

PDController::PDController(float kp, float kd, float dt)
    : Kp(kp), Kd(kd), dt(dt), prevError(0.0f), targetSlip(0.09f), minOutput(0.0f), maxOutput(1.0f) {}
    // Target Slip is set to 9% as a placeholder value, will be tuned after testing
    //Clamped

float PDController::update(ETCController& etc)
{
    auto state = etc.getState();

    float v_front = (state.wheel_speed_fl + state.wheel_speed_fr) / 2.0f;
    float v_rear  = (state.wheel_speed_rl + state.wheel_speed_rr) / 2.0f;

    float slip = 0.0f;
    if (v_front > 0.1f) {
        slip = (v_front - v_rear) / v_front;
    }

    float error = targetSlip - slip;
    float derivative = (error - prevError) / dt;
    prevError = error;

    float output = Kp * error + Kd * derivative;

    if (output > maxOutput) output = maxOutput;
    if (output < minOutput) output = minOutput;

    return output;
}
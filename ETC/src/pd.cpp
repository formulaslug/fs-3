#include "pd.h"

PDController::PDController(float kp, float kd)
    : Kp(kp), Kd(kd), prevError(0.0f), targetSlip(0.09f), minOutput(0.1f), maxOutput(1.0f), prevErrorStale(true)
    {
	this->loopTimer.start();
    }
    // Target Slip is set to 9% as a placeholder value, will be tuned after testing
    //Clamped

float PDController::update(float ws_fl, float ws_fr, float ws_rl, float ws_rr)
{
    float v_front = (ws_fr + ws_fl) / 2.0f;
    float v_rear  = (ws_rr + ws_rl) / 2.0f;

    float slip = 0.0f;
    if (v_front > this->MIN_TC_RPM) {
       slip = (v_rear - v_front) / v_front;
    } else {
	this->reset();
	return 1.0f; // don't reduce torque 
    }

    this->loopTimer.stop();
    std::chrono::microseconds loop_time = this->loopTimer.elapsed_time();
    float dt = loop_time.count() / 1000000.0f; // dt in seconds

    float error = slip - this->targetSlip;

    float derivative = 0.0f;
    if (!this->prevErrorStale && dt > 0.0f) derivative = (error - this->prevError) / dt;
    this->prevError = (error > 0.0f) ? error : 0.0f;
    this->prevErrorStale = false;

    float output = 1 - (Kp * error + Kd * derivative);
    if (output > maxOutput) output = maxOutput;
    if (output < minOutput) output = minOutput;

    this->loopTimer.reset();
    this->loopTimer.start();
    
    return output;
}

void PDController::reset()
{
    this->prevError = 0.0f;
    this->prevErrorStale = true;
    this->loopTimer.stop();
    this->loopTimer.reset();
    this->loopTimer.start();
}

float PDController::getLastErr()
{
    if (prevErrorStale) return 0.0f;
    else return this->prevError; 
}
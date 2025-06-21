//
// Created by Nova Mondal on 5/12/25.
//

#ifndef LAYOUTS_H
#define LAYOUTS_H

#include "BT817Q.hpp"

struct Faults {
  bool fans : 1;
  bool precharge : 1;
  bool shutdown : 1;
};

class Layouts : public BT817Q {
public:
  Layouts(PinName mosi,
          PinName miso,
          PinName sck,
          PinName cs,
          PinName pdn,
          PinName irq,
          EvePanel panel);

  struct StandardLayoutParams {
    Faults faults;
    uint8_t speed;
    uint8_t soc;
    uint8_t acc_temp;
    uint8_t ctrl_tmp;
    uint8_t mtr_tmp;
    float mtr_volt;
    float glv;
    float steering_angle;
    float brake_balance;
    float brake_f;
    float brake_r;
    float throttle_demand;
    float brake_val;
    float brake_psi;
    std::chrono::milliseconds time;
    double delta_time_seconds;
    bool rtds;
    uint16_t rpm;
  };

  void drawStandardLayout(Faults faults,
                          uint8_t speed,
                          uint8_t soc,
                          uint8_t acc_temp,
                          uint8_t ctrl_tmp,
                          uint8_t mtr_tmp,
                          float mtr_volt,
                          float glv,
                          float steering_angle,
                          float brake_balance,
                          float brake_f,
                          float brake_r,
                          float throttle_demand,
                          float brake_val,
                          float brake_psi,
                          std::chrono::milliseconds time,
                          double delta_time_seconds
                          );

  void drawStandardLayout(const StandardLayoutParams &params) {
    drawStandardLayout(params.faults,
                       params.speed,
                       params.soc,
                       params.acc_temp,
                       params.ctrl_tmp,
                       params.mtr_tmp,
                       params.mtr_volt,
                       params.glv,
                       params.steering_angle,
                       params.brake_balance,
                       params.brake_f,
                       params.brake_r,
                       params.throttle_demand,
                       params.brake_val,
                       params.brake_psi,
                       params.time,
                       params.delta_time_seconds);
  }
  void drawStandardLayout2(Faults faults,
                          uint8_t speed,
                          uint8_t soc,
                          uint8_t acc_temp,
                          uint8_t ctrl_tmp,
                          uint8_t mtr_tmp,
                          float mtr_volt,
                          float glv,
                          float steering_angle,
                          float brake_balance,
                          float brake_f,
                          float brake_r,
                          float throttle_demand,
                          float brake_val,
                          float brake_psi,
                          std::chrono::milliseconds time,
                          double delta_time_seconds,
                          bool rtds,
                          uint16_t rpm
                          );

  void drawStandardLayout2(const StandardLayoutParams &params) {
    drawStandardLayout2(params.faults,
                       params.speed,
                       params.soc,
                       params.acc_temp,
                       params.ctrl_tmp,
                       params.mtr_tmp,
                       params.mtr_volt,
                       params.glv,
                       params.steering_angle,
                       params.brake_balance,
                       params.brake_f,
                       params.brake_r,
                       params.throttle_demand,
                       params.brake_val,
                       params.brake_psi,
                       params.time,
                       params.delta_time_seconds,
                       params.rtds,
                       params.rpm);
  }

  void drawLayout3(Faults faults,
                    float mtr_volt,
                    uint8_t acc_temp,
                    uint8_t ctrl_temp,
                    uint8_t mtr_temp,
                    uint8_t soc,
                    float glv,
                    bool rtds,
                    int tick //pass in tick to allow flashing/blinking
                    );

  void drawTestLayout(int var);
};

#endif // LAYOUTS_H

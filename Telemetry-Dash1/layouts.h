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
    float brake_balance;
    float throttle_demand;
    float brake_demand;
    std::chrono::milliseconds time;
    double delta_time_seconds;
  };

  void drawStandardLayout(Faults faults,
                          uint8_t speed,
                          uint8_t soc,
                          uint8_t acc_temp,
                          uint8_t ctrl_tmp,
                          uint8_t mtr_tmp,
                          float mtr_volt,
                          float glv,
                          float brake_balance,
                          float throttle_demand,
                          float brake_demand,
                          std::chrono::milliseconds time,
                          double delta_time_seconds);

  void drawStandardLayout(const StandardLayoutParams &params) {
    drawStandardLayout(params.faults,
                       params.speed,
                       params.soc,
                       params.acc_temp,
                       params.ctrl_tmp,
                       params.mtr_tmp,
                       params.mtr_volt,
                       params.glv,
                       params.brake_balance,
                       params.throttle_demand,
                       params.brake_demand,
                       params.time,
                       params.delta_time_seconds);
  }

  void drawTestLayout(int var);
};

#endif // LAYOUTS_H

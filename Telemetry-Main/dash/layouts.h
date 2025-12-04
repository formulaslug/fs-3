//
// Created by Nova Mondal on 5/12/25.
//

#ifndef LAYOUTS_H
#define LAYOUTS_H

#include "BT817Q.hpp"
#include "CANProtocol.hpp"

struct Faults {
  bool fans : 1;
  bool precharge : 1;
  bool shutdown : 1;
};

class Layouts : public BT817Q {
private:
  // color intervals
  static const uint8_t CELL_WARNING_TEMP = 50;
  static const uint8_t CELL_NORMAL_TEMP = 45; 
  static const uint8_t CELL_LOW_TEMP = 15; 
  
  static const uint16_t CELL_HIGH_VOLT = 4000;
  static const uint16_t CELL_STD_VOLT = 3400; 
  static const uint16_t CELL_LOW_VOLT = 2800; 

  static const uint16_t PACK_STD_VOLT = 10300;  
  static const uint16_t PACK_WARNING_VOLT = 10000; 
  static const uint16_t PACK_FLASH_VOLT = 9400; 

  // debug grid cell dimensions
  static const uint16_t CELL_WIDTH = 100;
  static const uint16_t CELL_HEIGHT = 68;
  static const uint16_t CELL_FONT = 24;

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
                    float acc_volt,
                    uint8_t acc_temp,
                    uint8_t ctrl_temp,
                    uint8_t mtr_temp,
                    uint8_t soc,
                    float glv,
                    bool rtds,
                    int tick //pass in tick to allow flashing/blinking
                    );

  void drawLayout4(Faults faults,
                    float acc_volt,
                    uint8_t acc_temp,
                    float ctrl_v,
                    uint8_t mtr_temp,
                    uint8_t soc,
                    float glv,
                    bool rtds,
                    int tick, //pass in tick to allow flashing/blinking
                    float brake_r,
                    float brake_f
                    );

  void drawTestLayout(int var);

  void drawMainDisplay(bool shtd, bool mtr_ctrl, bool rtd, bool pchg, bool fans, 
	uint16_t acc_volt, uint8_t acc_temp, uint8_t soc, int tick, uint16_t speed, 
	const char* lap_time, uint16_t glv, uint8_t mtr_temp, uint8_t ctrl_temp, 
	uint16_t ctrl_volt);

  void debugCellTemps(const ACC_SEG_TEMPS_t seg_temps[5]);

  void debugCellVolts(const ACC_SEG_VOLTS_t seg_volts[5]);

  void drawTempCell(int16_t x, int16_t y, uint8_t temp);

  void drawVoltCell(int16_t x, int16_t y, uint16_t volts);

  Color cellTempToColor(uint8_t temp);

  Color cellVoltToColor(uint16_t volt);

  Color socToColor(uint8_t soc);

};

#endif // LAYOUTS_H

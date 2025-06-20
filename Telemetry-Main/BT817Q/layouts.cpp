//
// Created by Nova Mondal on 5/12/25.
//

#include "layouts.h"

Layouts::Layouts(PinName mosi, PinName miso, PinName sck, PinName cs,
                 PinName pdn, PinName irq, EvePanel panel)
    : BT817Q(mosi, miso, sck, cs, pdn, irq, panel) {}

void Layouts::drawTestLayout(int var) {
  uint8_t alpha = std::floor(double(var) / 800.0 * 255);

  if (failure == startFrame()) {
    return;
  }
  clear(255, 255, 255);
  uint8_t alpha2 = 255 - alpha;
  drawRect(Point{400, 200}, Point{600, 350}, Color{0, 255, 255, alpha2}, 64);
  setMainColor(Color{0, 0, 255});
  drawPoint(64 * 16, 200, 100);
  drawPoint(32 * 16, 500, 200, Color{0, 0, 255, alpha});
  setMainColor(Color{255, 0, 0});
  drawFormattedText(600, 300, "current value: %d", 30, BT817Q::OPT_CENTER, var);
  uint8_t tall = 195 + alpha;
  drawLine(Point{300, 0}, Point{300, tall}, Color{255, 0, 128}, 32);
  drawNumber(150, 200, (var - 250), 31, 9,
             BT817Q::OPT_CENTER | BT817Q::OPT_SIGNED);

  // eve.drawGauge(Point{650, 100}, 100, transform, 800);

  drawGauge(Point{650, 100}, 100, var, 800, Color{64, 64, 90}, Color{0, 255, 0},
            Color{255, 0, 0}, Color{0, 0, 255});
  drawProgressBar(Point{50, 380}, 100, 10, var, 800, Color{128, 0, 0},
                  Color{0, 0, 0});

  endFrame(); // automatically waits for cmd queue to clear out
}

void Layouts::drawStandardLayout(
    Faults faults, const uint8_t speed, const uint8_t soc, uint8_t acc_temp,
    uint8_t ctrl_tmp, uint8_t mtr_tmp, float mtr_volt, float glv,
    float steering_angle, float brake_balance, float brake_f, float brake_r,
    float throttle_demand, float brake_demand, std::chrono::milliseconds time,
    double delta_time_seconds) {
  if (failure == startFrame()) {
    return;
  }
  loadFonts();
  // printf("Drew!\n");
  clear(255, 255, 255); // black background for frame
  setMainColor(black);
  drawProgressBar(Point{220, 10}, 340, 35, speed, 110, mid_gray,
                  orange);       // mph progress bar
  for (int i = 0; i < 16; i++) { // to section off the progress bar
    uint16_t base_x = 220 + (20 * (i + 1));
    drawLine(Point{base_x, 0}, Point{base_x, 50}, white, 16 * 5);
  }
  drawRect(Point{200, 10}, Point{220, 45}, white);
  drawRect(Point{560, 10}, Point{580, 45}, white);
  drawProgressBar(Point{220, 180}, 340, 30, soc, 100, mid_gray,
                  Color{0, 200, 36}); // soc progress bar
  drawRect(Point{200, 180}, Point{220, 210}, white);
  drawRect(Point{560, 180}, Point{580, 210}, white);
  for (int i = 0; i < 16; i++) { // to section off the progress bar
    uint16_t base_x = 220 + (20 * (i + 1));
    drawLine(Point{base_x, 175}, Point{base_x, 210}, white, 16 * 5);
  }

  drawText(75, 25, "FS-3", 31);
  const Color fan_color = faults.fans ? red : green;
  const Color precharge_color = faults.precharge ? red : green;
  const Color shutdown_color = faults.shutdown ? red : green;
  drawText(600, 30, "F", fan_color, 24);
  drawText(675, 30, "PC", precharge_color, 24);
  drawText(750, 30, "SD", shutdown_color, 24);
  drawRect(Point{0, 60}, Point{190, 210}, black);    // outline box for clock
  drawRect(Point{2, 62}, Point{188, 208}, white);    // bg for clock box
  drawRect(Point{580, 60}, Point{800, 170}, black);  // outline box for temps
  drawRect(Point{582, 62}, Point{798, 168}, white);  // bg for temps box
  drawRect(Point{580, 175}, Point{800, 265}, black); // outline box for voltages
  drawRect(Point{582, 177}, Point{798, 263}, white); // black bg
  drawRect(Point{0, 220}, Point{190, 300}, black); // outline for brake balance
  drawRect(Point{2, 222}, Point{188, 298}, white); // black bg
  using namespace chrono;
  auto time_s = duration_cast<seconds>(time);
  const auto time_ms = time - duration_cast<milliseconds>(time_s);
  const auto time_min = duration_cast<minutes>(time_s);
  time_s -= duration_cast<seconds>(time_min);
  drawFormattedText(100, 100, "%02d:%02d:%02d", 24, OPT_CENTER,
                    time_min.count(), time_s.count(),
                    time_ms.count()); // lap time formatted in mm:ss::ms
  const char sign_char = (delta_time_seconds > 0) ? '+' : ' ';
  const Color delta_color = (delta_time_seconds > 0) ? red : green;
  drawFormattedText(100, 150, "%c %00.2f s  ", delta_color, 24, OPT_CENTER,
                    sign_char,
                    delta_time_seconds); // delta time formatted in ± s.ms
  // TODO! fix large font on boot up
  drawFormattedText(400, 110, "%d MPH", 1, OPT_CENTER,
                    speed); // speed display text
  drawFormattedText(400, 160, "SOC: %d", 24, OPT_CENTER, soc);
  drawFormattedText(100, 260, "BB: %02.1f %%  ", 24, OPT_CENTER,
                    brake_balance * 100.0f); // brake balance display
  drawFormattedText(680, 80, "ACC: %03d C", 24, OPT_CENTER, acc_temp); // temps
  drawFormattedText(680, 115, "CTRL: %03d C", 24, OPT_CENTER, ctrl_tmp);
  drawFormattedText(680, 150, "MTR: %03d C", 24, OPT_CENTER, mtr_tmp);
  drawFormattedText(680, 205, "MC: %03.1f V  ", 24, OPT_CENTER,
                    mtr_volt); // voltages
  drawFormattedText(680, 235, "GLV: %03.1f V  ", 24, OPT_CENTER, glv);

  // drawProgressBar(Point{700, 300},
  //                 32,
  //                 160,
  //                 (100 - uint16_t(throttle_demand * 100)),
  //                 100,
  //                 green,
  //                 mid_gray); // throttle demand bar
  drawRect(Point{700, 300}, Point{732, 480}, mid_gray);
  const uint16_t throttle_bar_h = (480 - floor(160 * throttle_demand));
  drawRect(Point{700, throttle_bar_h}, Point{732, 480}, green);
  // drawProgressBar(Point{750, 300},
  //                 32,
  //                 160,
  //                 (100 - uint16_t(brake_demand * 100)),
  //                 100,
  //                 red,
  //                 mid_gray); // brake demand bar
  drawRect(Point{750, 300}, Point{782, 480}, mid_gray);
  const uint16_t brake_bar_h = (480 - floor(160 * brake_demand));
  drawRect(Point{750, brake_bar_h}, Point{782, 480}, red);
  endFrame();
}

void Layouts::drawStandardLayout2(
    Faults faults, uint8_t speed, uint8_t soc, uint8_t acc_temp,
    uint8_t ctrl_tmp, uint8_t mtr_tmp, float mtr_volt, float glv,
    float steering_angle, float brake_balance, float brake_f, float brake_r,
    float throttle_demand, float brake_demand, std::chrono::milliseconds time,
    double delta_time_seconds, bool rtds, uint16_t rpm) {
  if (failure == startFrame()) {
    return;
  }
  clear(255, 255, 255); // black background for frame
  loadFonts();
  setMainColor(black);

  drawFormattedText(400, 100, "%03d", 1, OPT_CENTER, speed);

  drawText(185, 235, "TACH", 21);
  uint16_t tachPos = 211 + 426 * (rpm / 7500);
  drawRect(Point{211, 207}, Point{589, 260}, mid_gray);
  drawRect(Point{211, 207}, Point{tachPos, 260}, green);
  drawText(406, 270,
           "0        1        2        3        4        5        6        7 "
           "       ",
           23);

  drawText(638, 30, "SOC", 24);

  drawFormattedText(637, 79, "%02d", 2, OPT_CENTER, soc);

  setMainColor(green);
  bool pastPoint = false;
  for (int j = 1; j <= 25; j++) {
    // segmented soc
    uint16_t segTopLeftY = 490 - j * 19;
    uint16_t segBotRightY = segTopLeftY - 10;
    if (soc < j * 4 && !pastPoint) {
      setMainColor(mid_gray);
      pastPoint = true;
    }
    drawRect(Point{700, segTopLeftY}, {795, segBotRightY});
    // if (soc > j * 4) {
    //   drawRect(Point{700, segTopLeftY}, Point{795, segBotRightY}, orange);
    // } else if (soc > (j - 1) * 4) {
    //   drawRect(Point{700, segTopLeftY}, Point{795, segBotRightY}, red);
    // } else {
    //   drawRect(Point{700, segTopLeftY}, Point{795, segBotRightY}, mid_gray);
    // }
  }
  setMainColor(black);

  Color fan_color = faults.fans ? red : green;
  Color precharge_color = faults.precharge ? red : green;
  Color shutdown_color = faults.shutdown ? red : green;
  Color rtds_color = rtds ? green : red;

  drawRect(Point{1, 1}, Point{73, 73}, fan_color);
  drawRect(Point{75, 1}, Point{147, 73}, precharge_color);
  drawRect(Point{1, 75}, Point{73, 147}, shutdown_color);
  drawRect(Point{75, 75}, Point{147, 147}, rtds_color);

  drawText(36, 36, "FANS", 23);
  drawText(110, 36, "PCHG", 23);
  drawText(36, 110, "SHTD", 23);
  drawText(110, 110, "RTD", 23);

  drawFormattedText(70, 180, "FAN       %03d", 24, OPT_CENTER, acc_temp);
  drawFormattedText(70, 210, "ACC    %03dC", 24, OPT_CENTER, acc_temp);
  drawFormattedText(70, 240, "CTRL  %03dC", 24, OPT_CENTER, ctrl_tmp);
  drawFormattedText(70, 270, "MTR    %03dC", 24, OPT_CENTER, mtr_tmp);
  drawFormattedText(70, 300, "MC       %03.1fV  ", 24, OPT_CENTER,
                    mtr_volt); // voltages
  drawFormattedText(70, 330, "GLV     %03.1fV  ", 24, OPT_CENTER, glv);
  drawFormattedText(70, 390, "BRF     %03.2f  ", 24, OPT_CENTER, brake_f);
  drawFormattedText(70, 420, "BRR     %03.2f  ", 24, OPT_CENTER, brake_r);
  drawFormattedText(70, 450, "STEER   %03.2f  ", 24, OPT_CENTER, steering_angle);

  drawRect(Point{614, 127}, Point{637, 310}, mid_gray);
  uint16_t throttle_bar_h = 127 + 183*throttle_demand;
  drawRect(Point{614, throttle_bar_h}, Point{637, 310}, green);

  drawRect(Point{643, 127}, Point{666, 310}, mid_gray);
  uint16_t brake_bar_h = 127 + 183*brake_demand;
  drawRect(Point{643, brake_bar_h}, Point{666, 310}, red);
  // drawRect(Point{643, static_cast<uint16_t>(brake_demand)}, Point{666, 310},
  // red);
  endFrame();
}

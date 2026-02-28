//
// Created by Nova Mondal on 5/12/25.
//
#include "mbed.h"
#include "layouts.h"

Layouts::Layouts(PinName mosi, PinName miso, PinName sck, PinName cs,
                 PinName pdn, PinName irq, EvePanel panel)
    : BT817Q(mosi, miso, sck, cs, pdn, irq, panel) {}

// void Layouts::drawTestLayout(int var) {
//     uint8_t alpha = std::floor(double(var) / 800.0 * 255);

//     if (failure == startFrame()) {
//         return;
//     }
//     clear(255, 255, 255);
//     uint8_t alpha2 = 255 - alpha;
//     drawRect(Point{400, 200}, Point{600, 350}, Color{0, 255, 255, alpha2}, 64);
//     setMainColor(Color{0, 0, 255});
//     drawPoint(64 * 16, 200, 100);
//     drawPoint(32 * 16, 500, 200, Color{0, 0, 255, alpha});
//     setMainColor(Color{255, 0, 0});
//     drawFormattedText(600, 300, "current value: %d", 30, BT817Q::OPT_CENTER, var);
//     uint8_t tall = 195 + alpha;
//     drawLine(Point{300, 0}, Point{300, tall}, Color{255, 0, 128}, 32);
//     drawNumber(150, 200, (var - 250), 31, 9, BT817Q::OPT_CENTER | BT817Q::OPT_SIGNED);

//     // eve.drawGauge(Point{650, 100}, 100, transform, 800);

//     drawGauge(Point{650, 100}, 100, var, 800, Color{64, 64, 90}, Color{0, 255, 0}, Color{255, 0, 0}, Color{0, 0, 255});
//     drawProgressBar(Point{50, 380}, 100, 10, var, 800, Color{128, 0, 0}, Color{0, 0, 0});

//     endFrame(); // automatically waits for cmd queue to clear out
// }

void Layouts::drawStandardLayout(
    Faults faults, const uint8_t speed, const uint8_t soc, uint8_t acc_temp,
    uint8_t ctrl_tmp, uint8_t mtr_tmp, float mtr_volt, float glv,
    float steering_angle, float brake_balance, float brake_f, float brake_r,
    float throttle_demand, float brake_val, float brake_psi, std::chrono::milliseconds time,
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
  const uint16_t brake_bar_h = (480 - floor(160 * brake_val));
  drawRect(Point{750, brake_bar_h}, Point{782, 480}, red);
  endFrame();
}

void Layouts::drawStandardLayout2(
    Faults faults, uint8_t speed, uint8_t soc, uint8_t acc_temp,
    uint8_t ctrl_tmp, uint8_t mtr_tmp, float mtr_volt, float glv,
    float steering_angle, float brake_balance, float brake_f, float brake_r,
    float throttle_demand, float brake_val, float brake_psi, std::chrono::milliseconds time,
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
  drawFormattedText(70, 300, "MC      %03.1fV  ", 24, OPT_CENTER,
                    mtr_volt); // voltages
  drawFormattedText(70, 330, "GLV     %03.1fV  ", 24, OPT_CENTER, glv);
  drawFormattedText(70, 360, "BRF     %03.2fmV     ", 24, OPT_CENTER, brake_f);
  drawFormattedText(70, 390, "BRR     %03.2fmV  ", 24, OPT_CENTER, brake_r);
  drawFormattedText(70, 420, "BR      %03.2fpsi  ", 24, OPT_CENTER, brake_psi);
  drawFormattedText(70, 450, "STEER   %03.2f  ", 24, OPT_CENTER, steering_angle);

  drawRect(Point{614, 127}, Point{637, 310}, mid_gray);
  uint16_t throttle_bar_h = 310 - 183*throttle_demand;
  drawRect(Point{614, 127}, Point{637, throttle_bar_h}, green);

  drawRect(Point{643, 127}, Point{666, 310}, mid_gray);
  uint16_t brake_bar_h = 310 - 183*(brake_val);
  drawRect(Point{643, 310}, Point{666, brake_bar_h}, red);
  // drawRect(Point{643, static_cast<uint16_t>(brake_demand)}, Point{666, 310},
  // red);
  endFrame();
}

#define ACC_WARNING_VOLT 94

#define ACC_WARNING2 100

#define ACC_WARNING3 103

#define ACC_WARNING_TEMP 54

void Layouts::drawLayout3(Faults faults, float acc_volt, uint8_t acc_temp,
                          uint8_t ctrl_temp, uint8_t mtr_temp, uint8_t soc,
                          float glv, bool rtds, int tick) {
  //--------init-------------
  if (failure == startFrame()) {
    return;
  }
  clear(255, 255, 255); // white background for frame
  loadFonts();
  //-----------\init-------------

  //--------------SOC bars---------------
  setMainColor(green);
  bool pastPoint = false;
  for (int j = 1; j <= 10; j++) {
    // segmented soc
    uint16_t segTopLeftY = 480 - j * 45;
    uint16_t segBotRightY = segTopLeftY + 20;
    if (soc < j * 10 && !pastPoint) {
      setMainColor(mid_gray);
      pastPoint = true;
    }
    drawRect(Point{700, segTopLeftY}, {795, segBotRightY});
  }
  //----------\SOC bars----------------------

  //--------indicator lights------------------
  Color fan_color = faults.fans ? red : green;
  Color precharge_color = faults.precharge ? red : green;
  Color shutdown_color = faults.shutdown ? red : green;
  Color rtds_color = rtds ? green : red;
  drawRect(Point{1, 1}, Point{73, 73}, fan_color);
  drawRect(Point{75, 1}, Point{147, 73}, precharge_color);
  drawRect(Point{1, 75}, Point{73, 147}, shutdown_color);
  drawRect(Point{75, 75}, Point{147, 147}, rtds_color);
  //-----------\indicator lights--------------------

  //---------MC warning box----------
  Color mc_volts_bg = Color {18, 219, 255};
  if (acc_volt < ACC_WARNING_VOLT) { // flash the box to alarm driver if MC volts below warning point
    if (tick % 2 == 0) {
      mc_volts_bg = red;
    }
    else {
      mc_volts_bg = Color {255, 255, 0};
    }
  }
  else if (acc_volt < ACC_WARNING2) {
    mc_volts_bg = red;
  }
  else if (acc_volt < ACC_WARNING3) {
    mc_volts_bg = Color{255, 155, 0};
  }
  setMainColor(mc_volts_bg);
  drawRect(Point{155, 0}, {390, 205});
  //--------------\MC warning box-------

  //---------ACC TEMP warning box----------
  Color acc_temp_bg = Color {18, 219, 255};
  if (acc_temp > ACC_WARNING_TEMP) {//flash the box to alarm driver if MC volts below warning point
    if (tick % 2 == 0) {
      acc_temp_bg = red;
    }
    else {
      acc_temp_bg = Color {255, 255, 0};
    }
  }
  setMainColor(acc_temp_bg);
  drawRect(Point{395, 0}, {695, 205});
  //--------------\ACC TEMP warning box-------

  //--------------TEXT-----------------
  setMainColor(black);

  //___status indicators_____
  drawText(36, 36, "FANS", 23);
  drawText(110, 36, "PCHG", 23);
  drawText(36, 110, "SHTD", 23);
  drawText(110, 110, "RTD", 23);

  //___ACC Voltage and ACC Temp______
  drawFormattedText(275, 110,
    "%003.1f V", 1, OPT_CENTER, acc_volt);
  drawFormattedText(550, 110,
   "%03d'C", 1, OPT_CENTER, acc_temp);

  //___GLV, CTRL and MTR______
  // drawFormattedText(275, 225, "GLV\n%03d V     ", 24, OPT_CENTER, glv);
  // drawFormattedText(225, 225, "MC\n%003.1f'V     ", 24, OPT_CENTER, ctrl_v);
  // drawFormattedText(375, 225, "MTR\n%03d'C     ", 24, OPT_CENTER, mtr_temp);

  //____SOC number
  drawText(525,300, "SOC", 31);
  drawFormattedText(550, 250, "%03d    ", 2, OPT_CENTER, soc);
  endFrame();
}

void Layouts::drawLayout4(Faults faults, float acc_volt, uint8_t acc_temp,
                          float ctrl_v, uint8_t mtr_temp, uint8_t soc,
                          float glv, bool rtds, int tick, float brake_r, float brake_f) {
  //--------init-------------
  if (failure == startFrame()) {
    return;
  }
  clear(255, 255, 255); 
  loadFonts();
  //-----------\init-------------

  //--------------SOC bars---------------
  setMainColor(green);
  bool pastPoint = false;
  for (int j = 1; j <= 10; j++) {
    // segmented soc
    uint16_t segTopLeftY = 480 - j * 45;
    uint16_t segBotRightY = segTopLeftY + 20;
    if (soc < j * 10 && !pastPoint) {
      setMainColor(mid_gray);
      pastPoint = true;
    }
    drawRect(Point{700, segTopLeftY}, {795, segBotRightY});
  }
  //----------\SOC bars----------------------

  //--------indicator lights------------------
  Color fan_color = faults.fans ? red : green;
  Color precharge_color = faults.precharge ? red : green;
  Color shutdown_color = faults.shutdown ? red : green;
  Color rtds_color = rtds ? green : red;
  drawRect(Point{1, 1}, Point{73, 73}, fan_color);
  drawRect(Point{75, 1}, Point{147, 73}, precharge_color);
  drawRect(Point{1, 75}, Point{73, 147}, shutdown_color);
  drawRect(Point{75, 75}, Point{147, 147}, rtds_color);
  //-----------\indicator lights--------------------

  //---------MC warning box----------
  Color mc_volts_bg = Color {18, 219, 255};
  if (acc_volt < ACC_WARNING_VOLT) { // flash the box to alarm driver if MC volts below warning point
    if (tick % 2 == 0) {
      mc_volts_bg = red;
    }
    else {
      mc_volts_bg = Color {255, 255, 0};
    }
  }
  else if (acc_volt < ACC_WARNING2) {
    mc_volts_bg = red;
  }
  else if (acc_volt < ACC_WARNING3) {
    mc_volts_bg = Color{255, 155, 0};
  }
  setMainColor(mc_volts_bg);
  drawRect(Point{155, 0}, {390, 205});
  //--------------\MC warning box-------

  //---------ACC TEMP warning box----------
  Color acc_temp_bg = Color {18, 219, 255};
  if (acc_temp > ACC_WARNING_TEMP) {//flash the box to alarm driver if MC volts below warning point
    if (tick % 2 == 0) {
      acc_temp_bg = red;
    }
    else {
      acc_temp_bg = Color {255, 255, 0};
    }
  }
  setMainColor(acc_temp_bg);
  drawRect(Point{395, 0}, {695, 205});
  //--------------\ACC TEMP warning box-------

  //--------------TEXT-----------------
  setMainColor(black);

  //___status indicators_____
  drawText(36, 36, "FANS", 23);
  drawText(110, 36, "PCHG", 23);
  drawText(36, 110, "SHTD", 23);
  drawText(110, 110, "RTD", 23);

  //___Acc Temp n V______
  drawFormattedText(275, 110,
    "%003.1f V", 1, OPT_CENTER, acc_volt);
  drawFormattedText(550, 110,
   "%03d'C", 1, OPT_CENTER, acc_temp);

  //___GLV, CTRL and MTR______
  drawFormattedText(275, 250, "GLV %03.1f V     ", 31, OPT_CENTER, glv);
  drawFormattedText(275, 370, "CTRL %003.1f V     ", 31, OPT_CENTER, ctrl_v);


  //_____Brake Balance
    int bb_f = 100*( brake_f / (brake_f + brake_r));
    int bb_r = 100-bb_f;
  drawFormattedText(275, 320,"BB %02d/%02d", 2, OPT_CENTER, bb_f, bb_r);
  // drawFormattedText(375, 225, "MTR\n%03d'C     ", 24, OPT_CENTER, mtr_temp);

  //____SOC number
  drawText(525,300, "SOC", 31);
  drawFormattedText(550, 250, "%03d    ", 2, OPT_CENTER, soc);

  endFrame();
}

#define ACC_WARNING_TEMP   54

void Layouts::drawThermalScreen(
    uint8_t acc_temp, uint8_t mtr_temp, uint8_t ctrl_temp,
    float fl_surface, float fl_side,
    float fr_surface, float fr_side,
    float rl_surface, float rl_side,
    float rr_surface, float rr_side,
    float brake_fl, float brake_fr, float brake_rl, float brake_rr,
    float brake_f, float brake_r)
{
    if (failure == startFrame()) return;

    clear(255, 255, 255);
    loadFonts();

    auto setTempColor = [&](int temp, int warnTemp, int redTemp) {
        if (temp >= redTemp) setMainColor(red);
        else if (temp >= warnTemp) setMainColor(yellow);
        else setMainColor(green);
    };

    // top row
    const int topY = 15;
    const int topH = 70;
    const int topW = 230;
    const int topSpacing = 20;
    int currX = 35;

    auto drawTopBox = [&](const char* label, uint8_t val, int x, int warnTemp, int redTemp) {
        if (val >= redTemp) setMainColor(red);
        else if (val >= warnTemp) setMainColor(yellow);
        else setMainColor(green);

        drawRect({(uint16_t)x, (uint16_t)topY},
                 {(uint16_t)(x + topW), (uint16_t)(topY + topH)});
        setMainColor(black);
        drawFormattedText(x + topW / 2, topY + topH / 2, label, 28, OPT_CENTER, val);
    };

    setMainColor((acc_temp > ACC_WARNING_TEMP) ? red : green);
    drawRect({(uint16_t)currX, (uint16_t)topY},
             {(uint16_t)(currX + topW), (uint16_t)(topY + topH)});
    setMainColor(black);
    drawFormattedText(currX + topW / 2, topY + topH / 2, "ACC: %dC", 28, OPT_CENTER, acc_temp);
    currX += topW + topSpacing;

    drawTopBox("MTR: %dC",  mtr_temp,  currX, 80, 100);
    currX += topW + topSpacing;

    drawTopBox("CTRL: %dC", ctrl_temp, currX, 70, 85);

    // main area
    setMainColor(black);
    drawRect({30, 100}, {770, 470}, 3);

    const int boxW = 120;
    const int boxH = 135;
    const int horizontalGap = 140;
    const int verticalGap = 40;
    const int textLineHeight = 35;

    int leftBoxX = 400 - (horizontalGap / 2) - boxW;
    int rightBoxX = 400 + (horizontalGap / 2);

    auto drawWheelPod = [&](int x, int y, float surface, float side, float brk,
                            const char* label, bool isLeft) {
        int avgVal = (int)((surface + side) / 2.0f);
        int iSurface = (int)surface;
        int iSide = (int)side;
        int iBrk = (int)brk;

        setTempColor(avgVal, 80, 100);
        drawRect({(uint16_t)x, (uint16_t)y},
                 {(uint16_t)(x + boxW), (uint16_t)(y + boxH)});

        // inner boxes
        setMainColor(black);
        drawFormattedText(x + boxW / 2, y + boxH / 2 - 25, label, 30, OPT_CENTER);
        drawFormattedText(x + boxW / 2, y + boxH / 2 + 25, "%dC", 30, OPT_CENTER, avgVal);

        // side text
        int textX = isLeft ? (x - 25) : (x + boxW + 25);
        uint16_t align = isLeft ? OPT_RIGHTX : 0;
        int textStartY = y + 15;

        setTempColor(iSurface, 80, 100);
        drawFormattedText(textX, textStartY, "Surface: %dC", 24, align, iSurface);

        setTempColor(iSide, 80, 100);
        drawFormattedText(textX, textStartY + textLineHeight, "Side: %dC", 24, align, iSide);

        setTempColor(iBrk, 80, 100);
        drawFormattedText(textX, textStartY + (textLineHeight * 2), "Brk Disc: %dC", 24, align, iBrk);
    };

    int frontY = 130;
    int rearY = frontY + boxH + verticalGap;

    drawWheelPod(leftBoxX,  frontY, fl_surface, fl_side, brake_fl, "FL:", true);
    drawWheelPod(rightBoxX, frontY, fr_surface, fr_side, brake_fr, "FR:", false);
    drawWheelPod(leftBoxX,  rearY,  rl_surface, rl_side, brake_rl, "RL:", true);
    drawWheelPod(rightBoxX, rearY,  rr_surface, rr_side, brake_rr, "RR:", false);

    // brake balance
    int bb_f = 50;
    int bb_r = 50;

    if ((brake_f + brake_r) > 0.1f) {
        bb_f = (int)(100 * (brake_f / (brake_f + brake_r)));
        bb_r = 100 - bb_f;
    }

    setMainColor(black);
    drawFormattedText(400, 270, "BB:", 30, OPT_CENTER);
    drawFormattedText(400, 300, "%02d/%02d", 30, OPT_CENTER, bb_f, bb_r);

    endFrame();
}
void Layouts::drawDebugFaultLayout(                    
                    uint8_t bms, //BMS_FAULT
                    uint8_t imd, //IMD_FAULT
                    uint8_t sdwn, //SHUTDOWN
                    uint8_t pchgd, //PRECHARGED
                    uint8_t pchgi, //PRECHARGING
                    uint8_t chging, //CHARGING
                    uint16_t packv, //PACK_VOLTAGE
                    uint16_t glv, //GROUNDED LOW VOLTAGE
                    uint32_t cellfault, //CELL FAULT INDEX
                    uint8_t rtd, //READY TO DRIVE
                    uint8_t implausiblity, //BRAKE PEDAL SYSTEM IMPLAUSIBILITY
                    uint8_t tsactive, //TRACTICE SYSTEM ACTIVE
                    uint8_t pedaltravel, //PEDAL TRAVEL PERCENT
                    uint8_t brakesensev, //BRAKE SENSOR VOLTAGE
                    uint8_t ctrlovertemp, //CONTROLLER OVERTEMP
                    uint8_t running, //RUNNING
                    uint8_t poweron, //POWER ON
                    uint8_t powerrdy, //POWER READY
                    uint8_t motortemp, //MOTOR TEMP
                    uint8_t faultcode, //FAULT CODE
                    uint8_t faultlevel, //FAULT LEVEL
                    int tick
                  ){
        
  if (failure == startFrame()) {
    return;
  }
  //-----------\init-------------
  Color yellow = Color {255, 255, 0};
  Color orange = Color {200, 89, 41};
  clear(255, 255, 255); // white background for frame why doesnt clear take a color I don't know
  loadFonts();          //Not sure what possible fonts there are to use, above around 17?

  // --- Column layout ---
  const uint16_t col1_x = 50;
  const uint16_t col2_x = 310;
  const uint16_t col3_x = 575;
  const uint16_t row_start = 80;
  const uint16_t row_step = 40;
  const uint16_t text_offset = 130;          
  const uint16_t status_box_size = 16;
  const uint16_t status_box_offset = 190;   
  const uint16_t font_height_adjustment = 5;

  // --- Section titles ---
  setMainColor(black);
  drawText(col1_x, 40, "BAT", 24); //Battery
  drawText(col2_x, 40, "ETC", 24); //Eletronic throttle control
  drawText(col3_x, 40, "SME", 24); //The SME motor controller
  //Seperating lines between columns
  drawLine(Point{col1_x + status_box_offset + 24, 0}, Point{col1_x + status_box_offset + 24, 480}, 10);
  drawLine(Point{col2_x + status_box_offset + 24, 0}, Point{col2_x + status_box_offset + 24, 480}, 10);

  // --- Battery / Accumulator Column ---
  const char* batt_labels[] = {
      "BMS FLT", "IMD FLT", "SHTD", "PCHGD",
      "PCHGING", "PACK V", "GLV", "CELL FLT"
  };
  #define GLV_LOW 10800
  #define GLV_HIGH 13500
  #define MTR_TEMP_HIGH 80
  char batt_values[9][16];
  strcpy(batt_values[0], bms ? "[FAULT]" : "[NO]");
  strcpy(batt_values[1], imd ? "[FAULT]" : "[NO]");
  strcpy(batt_values[2], sdwn ? "[TRUE]" : "[FALSE]");
  strcpy(batt_values[3], pchgd ? "[TRUE]" : "[FALSE]");
  strcpy(batt_values[4], pchgi ? "[TRUE]" : "[FALSE]");
  // strcpy(batt_values[5], chging ? "[TRUE]" : "[FALSE]");
  snprintf(batt_values[5], sizeof(batt_values[5]),"%u.%02u V", packv / 100, packv % 100);
  snprintf(batt_values[6], sizeof(batt_values[6]),"%u mV", glv);
  snprintf(batt_values[7], sizeof(batt_values[7]),"%lu", (unsigned long)cellfault);
  for (uint16_t i = 0; i < 8; ++i) {
      uint16_t y = row_start + i * row_step;
      Color boxColor = green; //default color to green
      if(i == 5){
        //Flash the box to alarm driver if MC volts below warning point
        if (packv / 100 < ACC_WARNING_VOLT) { 
          if (tick % 2 == 0) boxColor = red;
          else boxColor = yellow;
        }
        else if (packv / 100 < ACC_WARNING2)boxColor = red;
        else if (packv / 100 < ACC_WARNING3)boxColor = orange;
      }
      else if(i==6){
        //GLV range checking
        if(glv > GLV_LOW && glv < GLV_HIGH) boxColor = green;
        else boxColor = red;
      }
      else if(i==7){
        //CELL FLT 
        if(cellfault != 0) boxColor = red;
        else boxColor = green;
      }
      else if (strstr(batt_values[i], "FAULT")) boxColor = red; 
      else if (strstr(batt_values[i], "FALSE")) boxColor = mid_gray;
      setMainColor(boxColor);
      drawText(col1_x-30, y, batt_labels[i], 23, OPT_CENTERY);
      drawText(col1_x + text_offset, y, batt_values[i], 23);
      drawRect(Point{col1_x + status_box_offset, static_cast<unsigned short>(y - font_height_adjustment)},
                Point{col1_x + status_box_offset + status_box_size, static_cast<unsigned short>(y + status_box_size - font_height_adjustment)},
                boxColor);
       setMainColor(mid_gray);
  }

  // --- ETC Column ---
  const char* etc_labels[] = {
      "RTD", "IMPL", "TS ACTIVE", "PED TRV", "BRK V"
  };
  char etc_values[5][16];
  strcpy(etc_values[0], rtd ? "[TRUE]" : "[FALSE]");
  strcpy(etc_values[1], implausiblity ? "[FAULT]" : "[NO]");
  strcpy(etc_values[2], tsactive ? "[TRUE]" : "[FALSE]");
  snprintf(etc_values[3], sizeof(etc_values[3]),"%u%%", pedaltravel);
  snprintf(etc_values[4], sizeof(etc_values[4]),"%u.%01u V", brakesensev / 10, brakesensev % 10);
  for (uint16_t i = 0; i < 5; ++i) {
      uint16_t y = row_start + i * row_step;
      Color boxColor = green;
      if(i==3){
        //Not sure about ranges for pedal travel
        boxColor = mid_gray;
      }
      else if(i==4){
        //Not sure about ranges for brake sensor voltage
        boxColor = mid_gray;
      }
      else if (strstr(etc_values[i], "FAULT")) boxColor = red;
      else if (strstr(etc_values[i], "FALSE")) boxColor = mid_gray;
      setMainColor(boxColor);
      drawText(col2_x-30, y, etc_labels[i], 23, OPT_CENTERY);
      drawText(col2_x + text_offset, y, etc_values[i], 23);
      drawRect(Point{col2_x + status_box_offset, static_cast<unsigned short>(y - font_height_adjustment)},
                Point{col2_x + status_box_offset + status_box_size, static_cast<unsigned short>(y + status_box_size - font_height_adjustment)},
                boxColor);
      setMainColor(mid_gray);
  }

  // --- SME Column ---
  const char* sme_labels[] = {
      "TMP OVR", "RUNNING", "PWR ON", "PWR RDY",
      "MTR TMP", "FLT CODE", "FLT LEVEL"
  };
  char sme_values[7][16];
  strcpy(sme_values[0], ctrlovertemp ? "[TRUE]" : "[FALSE]");
  strcpy(sme_values[1], running ? "[TRUE]" : "[FALSE]");
  strcpy(sme_values[2], poweron ? "[TRUE]" : "[FALSE]");
  strcpy(sme_values[3], powerrdy ? "[TRUE]" : "[FALSE]");
  snprintf(sme_values[4], sizeof(sme_values[4]),
            "%u.%01u 'C", motortemp / 10, motortemp % 10);
  snprintf(sme_values[5], sizeof(sme_values[5]), "%u", faultcode);
  snprintf(sme_values[6], sizeof(sme_values[6]), "%u", faultlevel);
  for (uint16_t i = 0; i < 7; ++i) {
      uint16_t y = row_start + i * row_step;
      Color boxColor = green;
      if(i==4){
        //motor temp
        if(motortemp < MTR_TEMP_HIGH) boxColor = green;
        else boxColor = red;
      }
      else if(i==5){
        //fault code
        if(faultcode <= 0) boxColor = green;
        else boxColor = red;
      }
      else if (i==6){
        //fault level
        if(faultlevel != 0) boxColor = red;
        else boxColor = green;
      }
      else if (strstr(sme_values[i], "FAULT")) boxColor = red;
      else if (strstr(sme_values[i], "FALSE")) boxColor = mid_gray;
      setMainColor(boxColor);
      drawText(col3_x-30, y, sme_labels[i], 23, OPT_CENTERY);
      drawText(col3_x + text_offset, y, sme_values[i], 23);
      drawRect(Point{col3_x + status_box_offset, static_cast<unsigned short>(y - font_height_adjustment)},
                Point{col3_x + status_box_offset + status_box_size, static_cast<unsigned short>(y + status_box_size - font_height_adjustment)},
                boxColor);
       setMainColor(mid_gray);
  }
                
  // String interpretations of common fault codes
  const uint16_t xPos = 530;
  const uint16_t yPos = 430;
  drawText(xPos,400, "MTR CTL: ",23, OPT_CENTERY);
  if(faultcode <= 0) drawText(xPos,yPos, "No faults",23, OPT_CENTERY);
  else if(faultcode == 1) drawText(xPos,yPos, "Over Voltage", 23, OPT_CENTERY);
  else if(faultcode == 2) drawText(xPos,yPos, "Under Voltage", 23, OPT_CENTERY);
  else if(faultcode == 3) drawText(xPos,yPos, "User Over Voltage", 23, OPT_CENTERY);
  else if(faultcode == 26) drawText(xPos,yPos, "High Voltage",23, OPT_CENTERY);
  else if(faultcode == 25) drawText(xPos,yPos, "Low Voltage",23, OPT_CENTERY);
  else if(faultcode == 62) drawText(xPos,yPos, "Heartbeat Timeout",23, OPT_CENTERY);
  else if(faultcode == 63) drawText(xPos,yPos, "RPDO Timeout",23, OPT_CENTERY);
  else drawText(530,470, "String interpretation not added",23, OPT_CENTERY);
  endFrame();
}

void Layouts::drawMainDisplay(bool shtd, bool mtr_ctrl, bool rtd, bool pchg, bool fans, 
	uint16_t acc_volt, uint8_t acc_temp, uint8_t soc, int tick, uint16_t speed, 
	const char* lap_time, uint16_t glv, uint8_t mtr_temp, uint8_t ctrl_temp, uint16_t dc_bus)
{
	// init
	if (!startFrame()) return;
	clear(white.red, white.green, white.blue);
	loadFonts();
	
	// soc
	if (soc > 100) soc = 100;
	drawProgressBar(Point{100, 25}, 600, 20, soc, 100, Color{50,50,50}, socToColor(soc), true);

	// acc temp box
	drawRect(Point{100, 70}, Point{400, 170}, cellTempToColor(acc_temp), 32);
	setMainColor(black);
	drawFormattedText(250, 120, "%d'C", 31, OPT_CENTER, acc_temp);

	// acc pack volt box
	// 	choose color
	Color volt_box_color = blue;
	if (acc_volt < PACK_FLASH_VOLT)
	{
		if (tick % 2 == 0) volt_box_color = red;
		else volt_box_color = yellow;
	}
	else if (acc_volt < PACK_WARNING_VOLT) volt_box_color = red;
	else if (acc_volt < PACK_STD_VOLT) volt_box_color = yellow;
	//	draw box
	drawRect(Point{400, 70}, Point{700, 170}, volt_box_color, 32);
	drawFormattedText(550, 120, "%d.%dV", 31, OPT_CENTER, acc_volt / 100, acc_volt % 100);

	// speedometer
	drawFormattedText(400, 315, "%u", 1, OPT_CENTER, speed);

	// Non-fault bools
	// 	RTD
	if (rtd) drawRect(Point{100, 200}, Point{300, 250}, green, 32);
	drawText(200, 225, "RTD", 31, OPT_CENTER);
	// 	fans
	if (fans) drawRect(Point{100, 250}, Point{300, 300}, green, 32);
	drawText(200, 275, "FANS", 31, OPT_CENTER);
	// 	precharge
	if (pchg) drawRect(Point{100, 300}, Point{300, 350}, green, 32);
	drawText(200, 325, "PCHG", 31, OPT_CENTER);
	
	// Faults
	// 	shutdown circuit
	if (shtd)
	{
		Color shtd_c = yellow;
		if (tick % 2 == 0) shtd_c = red;
		drawRect(Point{100, 350}, Point{300, 400}, shtd_c, 32);
	}
	drawText(200, 375, "SHTD", 31, OPT_CENTER);
	// 	motor controller
	if (mtr_ctrl)
	{
		Color mctrl_c = yellow;
		if (tick % 2 == 0) mctrl_c = red;
		drawRect(Point{100, 400}, Point{300, 450}, mctrl_c, 32);
	}
	drawText(200, 425, "MCTRL", 31, OPT_CENTER);

	// data
	setMainColor(black);
	// LAP
	drawText(500, 225, "LAP", 31, OPT_CENTERY);
	drawText(600, 225, lap_time, 31, OPT_CENTERY);
	// GLV
	drawText(500, 275, "GLV", 31, OPT_CENTERY);
	drawFormattedText(600, 275, "%d.%dV", 31, OPT_CENTERY, glv / 1000, glv % 1000);
	// DC BUS
	drawText(500, 325, "DC BUS", 31, OPT_CENTERY);
	drawFormattedText(675, 325, "%d.%dV", 31, OPT_CENTERY, dc_bus / 10, dc_bus % 10);
	// MTR
	drawText(500, 375, "MTR", 31, OPT_CENTERY);
	drawFormattedText(650, 375, "%d'C", 31, OPT_CENTERY, mtr_temp);
	// CTRL
	drawText(500, 425, "CTRL", 31, OPT_CENTERY);
	drawFormattedText(650, 425, "%d'C", 31, OPT_CENTERY, ctrl_temp);

	endFrame();
}

void Layouts::debugCellTemps(const ACC_SEG_TEMPS_t seg_temps[5])
{
  // initialize frame
  if (!startFrame()) return;
  clear(white.red, white.green, white.blue);

  // screen label
  setMainColor(black);
  drawFormattedText(400, 34, "BATTERY CELL TEMPERATURES", 25, OPT_CENTER);

  // cell labels
  setMainColor(black);
  int16_t x = 150;
  for (uint8_t cell = 0; cell < 6; cell++)
  {
    drawFormattedText(x, 84, "CELL%u", CELL_FONT, OPT_CENTER, cell);
    x += CELL_WIDTH;
  }

  // draw grid
  uint16_t y = 118;
  for (uint8_t seg = 0; seg < 5; seg++)
  {
    // segment label
    drawFormattedText(50, y + (CELL_HEIGHT / 2), "SEG%u", CELL_FONT, OPT_CENTER, seg);
    // cells
    drawTempCell(100, y, seg_temps[seg].TEMPS_CELL0);
    drawTempCell(200, y, seg_temps[seg].TEMPS_CELL1);
    drawTempCell(300, y, seg_temps[seg].TEMPS_CELL2);
    drawTempCell(400, y, seg_temps[seg].TEMPS_CELL3);
    drawTempCell(500, y, seg_temps[seg].TEMPS_CELL4);
    drawTempCell(600, y, seg_temps[seg].TEMPS_CELL5);
    y += CELL_HEIGHT;
  }

  endFrame();
}

void Layouts::debugCellVolts(const ACC_SEG_VOLTS_t seg_volts[5])
{
  // initialize frame
  if (!startFrame()) return;
  clear(white.red, white.green, white.blue);

  // screen label
  setMainColor(black);
  drawFormattedText(400, 34, "BATTERY CELL VOLTAGES", 25, OPT_CENTER);

  // cell labels
  setMainColor(black);
  int16_t x = 150;
  for (uint8_t cell = 0; cell < 6; cell++)
  {
    drawFormattedText(x, 84, "CELL%u", CELL_FONT, OPT_CENTER, cell);
    x += CELL_WIDTH;
  }

  // draw grid
  uint16_t y = 118;
  for (uint8_t seg = 0; seg < 5; seg++)
  {
    // segment label
    drawFormattedText(50, y + (CELL_HEIGHT / 2), "SEG%u", CELL_FONT, OPT_CENTER, seg);
    // cells
    drawVoltCell(100, y, (seg_volts[seg].VOLTS_CELL0 * 2550 / 255) + 2000);
    drawVoltCell(200, y, (seg_volts[seg].VOLTS_CELL1 * 2550 / 255) + 2000);
    drawVoltCell(300, y, (seg_volts[seg].VOLTS_CELL2 * 2550 / 255) + 2000);
    drawVoltCell(400, y, (seg_volts[seg].VOLTS_CELL3 * 2550 / 255) + 2000);
    drawVoltCell(500, y, (seg_volts[seg].VOLTS_CELL4 * 2550 / 255) + 2000);
    drawVoltCell(600, y, (seg_volts[seg].VOLTS_CELL5 * 2550 / 255) + 2000);
    y += CELL_HEIGHT;
  }

  endFrame();
}

void Layouts::drawTempCell(uint16_t x, uint16_t y, uint8_t temp)
{
  uint16_t br_x = x + CELL_WIDTH, br_y = y + CELL_HEIGHT;
  // draw cell
  drawRect(Point{x, y}, Point{br_x, br_y}, cellTempToColor(temp), 32);
  // draw cell text
  setMainColor(black);
  drawFormattedText(x + (CELL_WIDTH / 2), y + (CELL_HEIGHT / 2), "%u", CELL_FONT, OPT_CENTER, temp);
}

void Layouts::drawVoltCell(uint16_t x, uint16_t y, uint16_t volt)
{
  uint16_t br_x = x + CELL_WIDTH, br_y = y + CELL_HEIGHT;
  // draw cell
  drawRect(Point{x, y}, Point{br_x, br_y}, cellVoltToColor(volt), 32);
  // draw cell text
  setMainColor(black);
  drawFormattedText(x + (CELL_WIDTH / 2), y + (CELL_HEIGHT / 2), "%u.%uV", CELL_FONT, OPT_CENTER, volt / 1000, volt % 1000);
}

Color Layouts::cellTempToColor(uint8_t temp)
{
  if (temp > CELL_WARNING_TEMP) return red;
  else if (temp > CELL_NORMAL_TEMP) return yellow;
  else if (temp > CELL_LOW_TEMP) return green;
  else return blue;
}

Color Layouts::cellVoltToColor(uint16_t volt)
{
  if (volt > CELL_HIGH_VOLT) return blue;
  else if (volt > CELL_STD_VOLT) return green;
  else if (volt > CELL_LOW_VOLT) return yellow;
  else return red;
}

Color Layouts::socToColor(uint8_t soc)
{
  if (soc > 25) return green;
  else if (soc > 10) return yellow;
  else return red;
}

#include "BT817Q.hpp"
#include "mbed.h"
#include "ui.h"

BT817Q eve(PC_12, PC_11, PC_10, PD_2, PB_7, PC_13, EvePresets::CFA800480E3);

int main() {
  printf("v1.07\n");
  eve.init(EvePresets::CFA800480E3);
  printf("Finished init!\n");
  // eve.setBacklight(true);
  // printf("Finished setting backlight!\n");
  // eve.startFrame();
  // eve.clear(0, 0, 0);                        // black background
  // eve.drawText(400, 240, 31, "Hello World", BT817Q::OPT_CENTER);
  // eve.endFrame();

  ThisThread::sleep_for(10ms);
  eve.startFrame();
  eve.clear(0, 255, 0);
  eve.endFrame();
  ThisThread::sleep_for(1000ms);

  int transform = 0;
  while (true) {
    transform = (transform + 1) % 800;

    uint8_t alpha = std::floor(double(transform) / 800.0 * 255);

    eve.startFrame();
    eve.clear(255, 255, 255);
    uint8_t alpha2 = 255 - alpha;
    eve.drawRect(
        Point{400, 200}, Point{600, 350}, Color{0, 255, 255, alpha2}, 64);
    eve.setMainColor(Color{0, 0, 255});
    eve.drawPoint(64 * 16, 200, 100);
    eve.drawPoint(32 * 16, 500, 200, Color{0, 0, 255, alpha});
    eve.setMainColor(Color{255, 0, 0});
    eve.drawFormattedText(
        600, 300, "current value: %d", 30, BT817Q::OPT_CENTER, transform);
    uint8_t tall = 195 + alpha;
    eve.drawLine(Point{300, 0}, Point{300, tall}, Color{255, 0, 128}, 32);
    eve.drawNumber(150,
                   200,
                   (transform - 250),
                   31,
                   9,
                   BT817Q::OPT_CENTER | BT817Q::OPT_SIGNED);

    // eve.drawGauge(Point{650, 100}, 100, transform, 800);

    eve.drawGauge(Point{650, 100},
                  100,
                  transform,
                  800,
                  Color{64, 64, 90},
                  Color{0, 255, 0},
                  Color{255, 0, 0},
                  Color{0, 0, 255});
    eve.drawProgressBar(Point{50, 380},
                        100,
                        10,
                        transform,
                        800,
                        Color{128, 0, 0},
                        Color{0, 0, 0});

    eve.endFrame(); // automatically waits for cmd queue to clear out

    // ThisThread::sleep_for(25ms);
  }
}

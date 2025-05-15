#include "BT817Q.hpp"
#include "layouts.h"
#include "mbed.h"

Layouts eve(PC_12, PC_11, PC_10, PD_2, PB_7, PC_13, EvePresets::CFA800480E3);



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

  Layouts::StandardLayoutParams p{
    .faults = Faults{0, 0, 1},
  };
  ThisThread::sleep_for(10ms);
  eve.startFrame();
  eve.clear(0, 255, 0);
  eve.endFrame();
  ThisThread::sleep_for(1000ms);

  while (true) {
    eve.drawStandardLayout(p);
    // eve.drawTestLayout(var);
    // eve.drawStandardLayout(Layouts::StandardLayoutParams{.faults =
    // Faults{}});
    // eve.drawStandardLayout(
    //     Faults{0, 1, 0},
    //     50,
    //     75,
    //     98,
    //     120,
    //     100,
    //     105.6,
    //     0.23,
    //     0.23,
    //     0.95,
    //     0.05,
    //     std::chrono::milliseconds(1000 * 60 * 2 + 1000 * 23 + 456),
    //     23.3);

    // ThisThread::sleep_for(25ms);
  }
}

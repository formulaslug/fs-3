#include "mbed.h"
#include "BT817Q.hpp"

BT817Q eve(PC_12, PC_11, PC_10, PD_2, PB_7, PC_13);

SPI spi(PC_12, PC_11, PC_10);

int main() {
   eve.init(EvePresets::CFA800480E3);
   eve.setBacklight(true);
   eve.startFrame();
   eve.clear(0, 0, 0);                        // black background
   eve.drawText(400, 240, 31, "Hello World", BT817Q::OPT_CENTER);
   eve.endFrame();
   while (true) { ThisThread::sleep_for(1s); }
}
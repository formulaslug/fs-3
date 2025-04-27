#include "mbed.h"
#include "BT817Q.hpp"

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

    eve.startFrame();
    eve.clear(0, 0, 255);
    eve.endFrame();
    ThisThread::sleep_for(1000ms);
    eve.startFrame();
    eve.clear(0, 255, 0);
    eve.drawText(10, 10, 25, "alskdjalsdj");
    eve.endFrame();

    while (true) { ThisThread::sleep_for(1s); }
}

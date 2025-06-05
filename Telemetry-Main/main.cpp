#include "BT817Q.hpp"
#include "layouts.h"

Layouts eve(PC_12, PC_11, PC_10, PD_2, PB_7, PC_13, EvePresets::CFA800480E3);

int main() {
    eve.init(EvePresets::CFA800480E3);

    ThisThread::sleep_for(10ms);
    eve.startFrame();
    eve.clear(0, 0, 0);
    eve.endFrame();
    ThisThread::sleep_for(10ms);

    while (true) {
        Layouts::StandardLayoutParams p{
            .faults = Faults{0, 0, 1}, .soc = 60, .acc_temp = 80};
        eve.drawStandardLayout2(p);
    }
}

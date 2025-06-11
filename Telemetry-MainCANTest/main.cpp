#include "Ticker.h"
#include "mbed.h"

[[noreturn]] int main() {
    printf("Hello world\n");
    auto can = CAN(PA_11, PA_12, 500000);
    uint8_t i = 0;
    while (true) {
        if (i>100) {
            i = 0;
        }
        constexpr uint16_t idA = 393;
        // constexpr uint16_t idB = 0x191;
        // constexpr uint16_t idC = 0x192;
        const uint8_t dataA[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, i, 0x00};
        // const uint8_t dataB[] = {0xAB, 0xCD, 0xAB, 0xCD, 0xAB, 0xCD, 0xAB, 0xCD};
        // const uint8_t dataC[] = {0xAB, 0xCD, 0xAB, 0xCD, 0xAB, 0xCD, 0xAB, 0xCD};
        const auto messageA = CANMessage(idA, dataA);
        // const auto messageB = CANMessage(idB, dataB);
        // const auto messageC = CANMessage(idC, dataC);
        can.write(messageA);
        // can.write(messageB);
        // can.write(messageC);
        i++;
    }
}

#include "mbed.h"

CAN *can;

DigitalOut led(LED1);

int main() {
  printf("v1.0\n");

  // can = new CAN(PB_8, PB_9, 500000);
  can = new CAN(PA_11, PA_12, 500000);
  CANMessage msg;

  // READ
  while (true) {
    if (can->read(msg)) {
      printf("%x: ", msg.id);
      for (int i = 0; i < msg.len; i++) {
        printf("%x ", msg.data[i]);
      }
      printf("\n");
      led.write(!led.read());
    }
  }

  // // WRITE
  // while (true) {
  //   CANMessage msg;
  //   msg.id = 0x74;
  //   msg.len = 0;
  //   can->write(msg);
  //   ThisThread::sleep_for(50ms);
  //   led.write(!led.read());
  // }

  // main() is expected to loop forever.
  // If main() actually returns the processor will halt
  return 0;
}

#include "mbed.h"

CAN *can;

DigitalOut led(LED1);

int main() {
  // while (1) {
  //   led.write(!led.read());
  //   ThisThread::sleep_for(100ms);
  //   printf("hi\n");
  // }
  printf("v1.0\n");
  // can = new CAN(PB_8, PB_9, 500000);
  can = new CAN(PA_11, PA_12, 500000);
  CANMessage msg;
  while (true) {
    if (can->read(msg)) {
      printf("%x: %x %x %x %x %x %x %x %x\n", msg.id, msg.data[0], msg.data[1],
             msg.data[2], msg.data[3], msg.data[4], msg.data[5], msg.data[6],
             msg.data[7]);
      led.write(!led.read());
    }
  }

  // main() is expected to loop forever.
  // If main() actually returns the processor will halt
  return 0;
}

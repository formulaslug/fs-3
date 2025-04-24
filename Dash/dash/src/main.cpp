#include "DigitalOut.h"
#include "ThisThread.h"
#include "mbed.h"

// main() runs in its own thread in the OS

int main() {
    DigitalOut led(LED1);

    while (true) {
	led = !led;
	ThisThread::sleep_for(100ms);
    }
    return 0;
}

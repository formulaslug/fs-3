/* mbed Microcontroller Library
 * Copyright (c) 2025 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"

// -----------------------------------------------
// Basic Serial Bridge (manual typing)
// -----------------------------------------------
BufferedSerial radio_serial(PA_9, PA_10, 9600);
BufferedSerial usb_serial(USBTX, USBRX);
DigitalOut led(LED1);

int main() {
  usb_serial.write("Main() - v1.0 - LiveTelemetryXBeeConfigurator\r\n", 48);

  uint8_t buf[100] = {0};
  while (true) {
        if (usb_serial.readable()) {
            uint32_t num = usb_serial.read(buf, sizeof(buf));
            if (num) {
                led = !led;
                // usb_serial.write(buf, num);
                radio_serial.write(buf, num);
            }
        }

        if (radio_serial.readable()) {
            uint32_t num = radio_serial.read(buf, sizeof(buf));
            if (num) {
                usb_serial.write(buf, num);
                usb_serial.write("\n", 2);
            }
        }
    }

  return 0;
}

// -----------------------------------------------
// Failed Attempt at Automating Configuration (not worth the time)
// -----------------------------------------------
// BufferedSerial radio_serial(PA_9, PA_10, 9600);
// BufferedSerial usb_serial(USBTX, USBRX);
// DigitalOut led(LED1);
//
// void enter_command_mode() {
//   uint8_t resp_buf[10] = {0};
//
//   radio_serial.write("+++", 3);
//
//   for (int i = 0; i < 10 && radio_serial.readable(); i++) {
//     while (true) {
//       uint32_t num = radio_serial.read(resp_buf + i, 1);
//       if (num) {
//         continue;
//       }
//     }
//   }
//
//   usb_serial.write(resp_buf, strlen((char*)resp_buf));
//   usb_serial.write("\r\n", 2);
//
//         // // TODO: the radio_serial.read() returns just the first 'O' so this
//         // // never passes
//         // if (resp_buf[0] == 'O' && resp_buf[1] == 'K') {
//         //   usb_serial.write(resp_buf, num);
//         //   usb_serial.write("\n", 2);
//         //   break;
//         // } else {
//         //   usb_serial.write("WARN: radio response was not OK. It was: ", 41);
//         //   usb_serial.write(resp_buf, sizeof(resp_buf));
//         //   usb_serial.write("\r\n", 2);
//         // }
//
//   usb_serial.write("Entered command mode!\r\n", 24);
// }
//
// int main() {
//   usb_serial.write("Main() - v1.0 - LiveTelemetryXBeeConfigurator\r\n", 48);
//
//   ThisThread::sleep_for(100ms);
//
//   enter_command_mode();
//   ThisThread::sleep_for(10ms);
//   radio_serial.write("ATD11\n", 6);
//   ThisThread::sleep_for(10ms);
//
//   enter_command_mode();
//   ThisThread::sleep_for(10ms);
//   radio_serial.write("ATD21\n", 6);
//   ThisThread::sleep_for(10ms);
//
//   enter_command_mode();
//   ThisThread::sleep_for(10ms);
//   radio_serial.write("ATD31\n", 6);
//   ThisThread::sleep_for(10ms);
//
//   enter_command_mode();
//   ThisThread::sleep_for(10ms);
//   radio_serial.write("ATD41\n", 6);
//   ThisThread::sleep_for(10ms);
//
//   enter_command_mode();
//   ThisThread::sleep_for(10ms);
//   radio_serial.write("ATP21\n", 6);
//   ThisThread::sleep_for(10ms);
//
//   usb_serial.write("\r\nFinished configuration!\r\n\r\n", 30);
//
//   while (true) {
//     uint8_t buf[100] = {0};
//     if (usb_serial.readable()) {
//       uint32_t num = usb_serial.read(buf, sizeof(buf));
//       if (num) {
//         led = !led;
//         // usb_serial.write(buf, num);
//         radio_serial.write(buf, num);
//       }
//     }
//
//     if (radio_serial.readable()) {
//       uint32_t num = radio_serial.read(buf, sizeof(buf));
//       if (num) {
//         usb_serial.write(buf, num);
//         usb_serial.write("\n", 2);
//       }
//     }
//   }
//
//   return 0;
// }

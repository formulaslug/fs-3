#include <thread>

#include "mbed.h"
#include "config.h"

void initIO();
void initChargerCAN();
void sendCAN();

CAN* can;

int main()
{

   printf("main()\n");
   initIO();

   printf("Starting main loop\n");
   while(true) {
      CANMessage msg;
      while (can->read(msg)) {
         switch (msg.id) {
            case 0x80: // sync
               sendCAN();
               break;
         }
      }

   }

   // main() is expected to loop forever.
   // If main() actually returns the processor will halt
   return 0;
}

void initIO() {
   printf("initIO()\n");
   can = new CAN(PIN_CAN1_RD, PIN_CAN1_TD, CAN_FREQUENCY);

   // LSS assign charger
   initChargerCAN();
}

void initChargerCAN() {
   printf("initChargerCAN()\n");

   this_thread::sleep_for(100ms);

   // Switch state global protocal, switch to LSS configuration state
   uint8_t data[8] = {0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
   CANMessage msg(0x7E5, data);
   can->write(msg);

   this_thread::sleep_for(5ms);

   // Configurate node ID protocal, set node ID to 0x10
   uint8_t data[8] = {0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
   CANMessage msg(0x7E5, data);
   can->write(msg);

   this_thread::sleep_for(5ms);

}


void sendCAN() {
   // send charge limits
   can->write(chargerMaxAllowedVoltageCurrentRPDO(
       0x10, // destination node ID
       CHARGE_VOLTAGE*1000, // desired voltage, mV
       CHARGE_DC_LIMIT, // charge current limit, mA
       CHARGE_AC_LIMIT // input AC current, can change to 20 if plugged into nema 5-20, nema 5-15 is standard
   ));
   this_thread::sleep_for(1ms);

   // send charge control
   can->write(chargerChargeControlRPDO(
       0x10, // destination node ID
       0x00000000, // pack voltage; doesn't matter as only for internal charger logging
       true, // evse override, tells the charger to respect the max AC input current sent in the other message
       false, // current x10 multipler, only used for certain zero chargers
       chargeEnable // enable
   ));
   this_thread::sleep_for(1ms);
}
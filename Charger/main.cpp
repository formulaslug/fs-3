#include <thread>

#include "mbed.h"
#include "config.h"

void initIO();
void initChargerCAN();
void sendCAN();

CAN* can;

uint32_t max_voltage_mV = 0;
uint16_t max_dc_current_mA = 0;
uint8_t max_ac_current_A = 0;

bool enable = false;


AnalogIn control_pilot(PIN_CONTROL_PILOT);
AnalogIn proximity_pilot(PIN_PROXIMITY_PILOT);


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


      // if proximity pilot is about


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
   uint8_t lss0_data[8] = {0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
   CANMessage lss0_msg(0x7E5, lss0_data);
   can->write(lss0_msg);

   this_thread::sleep_for(5ms);

   // Configurate node ID protocal, set node ID to 0x10
   uint8_t lss1_data[8] = {0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
   CANMessage lss1_msg(0x7E5, lss1_data);
   can->write(lss1_msg);

   this_thread::sleep_for(5ms);

}


void sendCAN() {
   // send charge limits
   uint8_t charge_limits_data[8] = {
      0x10,
      static_cast<uint8_t>(max_voltage_mV),
      static_cast<uint8_t>(max_voltage_mV >> 8),
      static_cast<uint8_t>(max_voltage_mV >> 16),
      static_cast<uint8_t>(max_voltage_mV >> 24),
      static_cast<uint8_t>(max_dc_current_mA),
      static_cast<uint8_t>(max_dc_current_mA >> 8),
      max_ac_current_A
   };
   CANMessage charge_limits_msg(0x306,  charge_limits_data);
   can->write(charge_limits_msg);

   this_thread::sleep_for(1ms);

   // send charge control
   uint8_t charge_control_data[8] = {
      0x10,
      static_cast<uint8_t>(enable << 1 + 0b00100000),
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00
   };
   CANMessage charge_control_msg(0x206,  charge_control_data);
   can->write(charge_control_msg);
   this_thread::sleep_for(1ms);
}
#include <thread>

#include "mbed.h"
#include "config.h"

void initIO();
void initChargerCAN();
void sendCAN();

CAN* can;

uint32_t max_voltage_mV = 0;
uint16_t max_dc_current_cA = 0; // in centiamps (0.01 amps)
uint8_t max_ac_current_A = 0;

uint16_t pack_voltage = 0;
uint16_t soc = 0;

bool enable = false;


AnalogIn control_pilot(PIN_CONTROL_PILOT);
AnalogIn proximity_pilot(PIN_PROXIMITY_PILOT);

EventQueue queue = EventQueue(EVENTS_EVENT_SIZE * 32);


int main()
{

   printf("main()\n");
   initIO();

   ThisThread::sleep_for(500ms);

   printf("Starting main loop\n");

   bool prechargeDone = false;
   bool fault = false;
   bool shutdown_closed = false;
   bool cell_temps_fine = false;

   while(true) {
      CANMessage msg;


      while (can->read(msg)) {
         switch (msg.id) {
         case 0x188: // ACC_TPDO_STATUS
            prechargeDone = msg.data[0] & 0b00001000;
            fault = msg.data[0] & 0b00000011;
            shutdown_closed = msg.data[0] & 0b00000100;
            cell_temps_fine = !(msg.data[1] & 0b00010000);
            break;
         case 0x288: // ACC_TPDO_POWER
            pack_voltage = msg.data[0] + (msg.data[1] << 8);
            soc = msg.data[2];
            break;
         default:
            break;
         }
      }


      // if proximity pilot is about 2.7v then no EVSE connected
      // if proximity pilot is about 1.7v then EVSE connected and button pressed
      // if proximity pilot is about 0.9v then EVSE connected and button not pressed
      bool proximity_pilot_ready = (proximity_pilot.read() * 3.3 < 1.2);

      // printf("pp: %f\n",proximity_pilot.read());

      // Duty cycle is 31 times voltage on control pilot
      // Duty cycle times 0.6 is max allowed continuous current draw
      // control pilot voltage times 18.5 is the max allowed current
      int max_ac_current_CP = std::floor(control_pilot.read() * 3.3 * 19);

      // printf ("proximity pilot: %x\n", proximity_pilot_ready);

      // printf("cp: %f\n", control_pilot.read());
      //
      // printf("max ac current: %d\n", max_ac_current_CP);

      max_ac_current_A = std::min(max_ac_current_CP, MAX_AC_CURRENT);



      max_voltage_mV = VOLTAGE_TARGET_MV;

      printf("pp_ready: %x, precharge done: %x, fault: %x, sh closed: %x, cell temps fine: %x\n", proximity_pilot_ready, prechargeDone, fault, shutdown_closed, cell_temps_fine);
      enable = proximity_pilot_ready && prechargeDone && !fault && shutdown_closed && cell_temps_fine;
      printf("Enable: %x\nVoltage: %f\nSOC: %d\n\n", enable, pack_voltage / 100.0, soc);

      if (enable) {
         max_dc_current_cA = CURRENT_MAX_CA;
      } else {
         max_dc_current_cA = 0;
      }

      queue.dispatch_once();
   }

   // main() is expected to loop forever.
   // If main() actually returns the processor will halt
   return 0;
}

void initIO() {
   printf("initIO()\n");
   can = new CAN(PIN_CAN1_RD, PIN_CAN1_TD, CAN_FREQUENCY);
   can->filter(0x088, 0x00FF, CANAny); // accept any TPDOs from ACC (0x188, 0x288)

   ThisThread::sleep_for(2500ms);

   // LSS assign charger
   initChargerCAN();

   ThisThread::sleep_for(100ms);
   queue.call_every(100ms, &sendCAN);
}

void initChargerCAN() {
   printf("initChargerCAN()\n");

   // Switch state global protocol, switch to LSS configuration state
   uint8_t lss0_data[8] = {0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
   CANMessage lss0_msg(0x7E5, lss0_data);
   can->write(lss0_msg);

   ThisThread::sleep_for(500ms);

   // Configurate node ID protocol, set node ID to 0x10
   uint8_t lss1_data[8] = {0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
   CANMessage lss1_msg(0x7E5, lss1_data);
   can->write(lss1_msg);

   ThisThread::sleep_for(100ms);

}


void sendCAN() {
   // send charge limits
   uint8_t charge_limits_data[8] = {
      0x10,
      static_cast<uint8_t>(max_voltage_mV),
      static_cast<uint8_t>(max_voltage_mV >> 8),
      static_cast<uint8_t>(max_voltage_mV >> 16),
      static_cast<uint8_t>(max_voltage_mV >> 24),
      static_cast<uint8_t>(max_dc_current_cA),
      static_cast<uint8_t>(max_dc_current_cA >> 8),
      max_ac_current_A
   };
   CANMessage charge_limits_msg(0x306,  charge_limits_data);
   can->write(charge_limits_msg);

   // ThisThread::sleep_for(1ms);

   // send charge control
   uint8_t charge_control_data[8] = {
      0x10,
      static_cast<uint8_t>((enable << 1) + 0b00100000),
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00
   };
   CANMessage charge_control_msg(0x206,  charge_control_data);
   can->write(charge_control_msg);
   ThisThread::sleep_for(1ms);
}

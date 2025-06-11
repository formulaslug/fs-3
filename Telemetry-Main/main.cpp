#include "BT817Q.hpp"
#include "Ticker.h"
#include "VehicleStateManager.hpp"
#include "layouts.h"
#include "radio.hpp"
#include <stdbool.h>
#include <stdlib.h>

#include <bits/this_thread_sleep.h>

#define ENABLE_RADIO false
#define ENABLE_SD false
#define ENABLE_DASH true

#define TICKS_PER_SECOND 100

#define SD_UPDATE_HZ 5
#define DASH_UPDATE_HZ 5
#define RADIO_UPDATE_HZ 1

struct TelemetrySystemState {
    int tick;
    bool radio_on;
    bool sd_on;
    bool dash_on;
    bool radio_event;
    bool sd_event;
    bool dash_event;
};

static TelemetrySystemState state = {0, ENABLE_RADIO, ENABLE_SD, ENABLE_DASH};

DigitalIn spi_attn(PA_9);
DigitalOut cs(PC_8);
SPI spi(PA_7, PA_6, PA_5);
XBeeRadio radio(spi, cs, spi_attn);
auto mbed_can = CAN(PB_8, PB_9, 500000);
auto can = MbedCAN(mbed_can);
auto vsm = VehicleStateManager(&can);

// Default DASH parameters
Layouts::StandardLayoutParams params = {
    .faults = Faults{false, false, false},
    .speed = 0,
    .soc = 100,
    .acc_temp = 20,
    .ctrl_tmp = 20,
    .mtr_tmp = 20,
    .mtr_volt = 110,
    .glv = 12,
    .brake_balance = 50,
    .throttle_demand = 0,
    .brake_demand = 0,
    .time = chrono::milliseconds(0),
    .delta_time_seconds = 0.01,
    .rtds = false,
    .rpm = 0
};



Layouts eve(PC_12, PC_11, PC_10, PD_2, PB_7, PC_13, EvePresets::CFA800480E3);

// Ticker ticker;

void update_radio() {
    // printf("RADIO!\n");
}

void update_sd() {
  // printf("SD!\n");
}
void update_dash() {
  // printf("DASH!\n");
  const VehicleState vsm_state = vsm.getState();
  int8_t total_temp = 0;
  uint8_t max_temp = 0;
  for (auto [TEMPS_CELL0, TEMPS_CELL1, TEMPS_CELL2, TEMPS_CELL3, TEMPS_CELL4,TEMPS_CELL5] : vsm_state.accSegTemps) {
    total_temp += (TEMPS_CELL0 + TEMPS_CELL1 + TEMPS_CELL2 + TEMPS_CELL3 + TEMPS_CELL4 + TEMPS_CELL5);
    max_temp = max( TEMPS_CELL0, max(TEMPS_CELL1, max(TEMPS_CELL2, max(TEMPS_CELL3, max(TEMPS_CELL4, TEMPS_CELL5)))));
  }
  params = {
      .faults =
          Faults{false, static_cast<bool>(vsm_state.accStatus.PRECHARGE_DONE),
                 static_cast<bool>(vsm_state.accStatus.SHUTDOWN_STATE)},
      // .speed = static_cast<uint8_t>(vsm_state.vdmGpsData.SPEED / 100),
      .speed = static_cast<uint8_t>(vsm_state.etcStatus.PEDAL_TRAVEL),
      .soc = vsm_state.accPower.SOC,
      .acc_temp = max_temp,
      .ctrl_tmp = vsm_state.smeTemp.CONTROLLER_TEMP,
      .mtr_tmp = vsm_state.smeTemp.MOTOR_TEMP,
      .mtr_volt = static_cast<float>(vsm_state.accPower.PACK_VOLTAGE / 100.0),
      .glv = static_cast<float>(vsm_state.pdbPowerA.GLV_VOLTAGE),
      .brake_balance = 50,
      // .throttle_demand =
      // static_cast<float>(static_cast<float>(vsm_state.smeThrottleDemand.TORQUE_DEMAND)/30000.0),
      .throttle_demand = static_cast<float>(vsm_state.etcStatus.PEDAL_TRAVEL),
      .brake_demand = static_cast<float>(
          ((static_cast<float>(vsm_state.etcStatus.BRAKE_SENSE_VOLTAGE) /
            1000.0) -
           0.5) /
          4),
      .time = chrono::milliseconds(0),
      .delta_time_seconds = 0.01,
      .rtds = false,
      .rpm = vsm_state.smeTrqSpd.SPEED};
  // params.speed++;
  eve.drawStandardLayout(params);
}

int main() {
    printf("Hello world\n");
    ThisThread::sleep_for(100ms);

    // Initializing CAN and vehicle state
    // ticker.attach([&]() {
    //     state.tick++;
    //     if (state.tick % (TICKS_PER_SECOND / RADIO_UPDATE_HZ) == 0) {
    //         state.radio_event = true;
    //     }
    //     if (state.tick % (TICKS_PER_SECOND / SD_UPDATE_HZ) == 0) {
    //         state.sd_event = true;
    //     }
    //     if (state.tick % (TICKS_PER_SECOND / DASH_UPDATE_HZ) == 0) {
    //         state.dash_event = true;
    //     }
    // }, 1000ms / TICKS_PER_SECOND);

    
    // Initializing Dash
    if (state.dash_on) {
        eve.init(EvePresets::CFA800480E3);
        ThisThread::sleep_for(10ms);
        eve.startFrame();
        eve.clear(0, 0, 0);
        eve.endFrame();
        ThisThread::sleep_for(10ms);
    }

    // int radio_temp = radio.get_temp();
    // if (radio_temp >= 70 || radio_temp <= 10) {
    //     printf("Radio temperature returned an unrealistic value. Disabling.\n");
    //     state.radio_on = false;
    // }
    int x = 0;
    // Timer t;
    // t.start();
    while (true) {
        // t.reset();
        vsm.update();

        x++;
        // Remember to read f and r brake pressure


        // if (state.radio_event) {
        //     state.radio_event = false;
        //     if (state.radio_on) {
        //         update_radio();
        //     }
        // }
        //
        // if (state.sd_event) {
        //     state.sd_event = false;
        //     if (state.sd_on) {
        //         update_sd();
        //     }
        // }

        // if (state.dash_event) {
        //     printf("Update Dash -- ");
        //     update_dash();
        //     state.dash_event = false;
        // }
        // printf("Time: %f\n", t.elapsed_time().count()/1.0);
        // t.reset();
        // update_dash();
        // printf("\tDash: %f\n", t.elapsed_time().count()/1.0);
        if (x>7000) {
            // printf("Time: %f\n", t.elapsed_time().count()/1.0);
            // t.reset();
            update_dash();
            // printf("\tDash: %f\n", t.elapsed_time().count()/1.0);
            x = 0;
            // printf("Updating Dash\n");
        }

        // printf("CAN!\n");

    }
}

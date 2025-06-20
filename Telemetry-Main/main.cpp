#include "BT817Q.hpp"
#include "Ticker.h"
#include "VehicleStateManager.hpp"
#include "layouts.h"
#include "radio.hpp"
#include <stdbool.h>
#include <stdlib.h>
#include "fsdaq_encoder_generated_from_dbc.hpp"
#include "FATFileSystem.h"
#include "SDBlockDevice.h"

#define ENABLE_RADIO false
#define ENABLE_SD false
#define ENABLE_DASH true

// #define TICKS_PER_SECOND 1000

#define SD_UPDATE_HZ 100ms
#define DASH_UPDATE_HZ 100ms
#define RADIO_UPDATE_HZ 1ms

// struct TelemetrySystemState {
//   int tick;
//   bool radio_on;
//   bool sd_on;
//   bool dash_on;
//   bool radio_event;
//   bool sd_event;
//   bool dash_event;
// };
//
// static TelemetrySystemState state = {ENABLE_RADIO, ENABLE_SD, ENABLE_DASH};

DigitalIn spi_attn(PA_9);
DigitalOut cs(PC_8);
SPI spi(PA_7, PA_6, PA_5);
XBeeRadio radio(spi, cs, spi_attn);
auto mbed_can = CAN(PB_8, PB_9, 500000);
auto can = MbedCAN(mbed_can);
auto vsm = VehicleStateManager(&can, PC_5, PC_1, PC_0);

// For SD card
// SDBlockDevice - lowest-level interfaces with the SD card.
// Pin configurations and transfer speeds are set in mbed_app.json.
SDBlockDevice sd{ PB_15, PB_14, PB_13, PB_12, 25000000 };
// FATFileSystem - Creates a FAT filesystem on the SDBlockDevice.
// "sd" is the name of the filesystem; i.e. filepaths are /sd/...
FATFileSystem fs{"sd"};

FILE *sd_fp;

Values vals{};
ValuesRow current_row{};
int row_idx = 0;


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
  .steering_angle = 11,
  .brake_balance = 50,
  .brake_f = 0,
  .brake_r = 0,
  .throttle_demand = 0,
  .brake_demand = 0,
  .time = chrono::milliseconds(0),
  .delta_time_seconds = 0.01,
  .rtds = false,
  .rpm = 0
};



Layouts eve(PC_12, PC_11, PC_10, PD_2, PB_7, PC_13, EvePresets::CFA800480E3);

// Ticker ticker;
EventQueue queue{EVENTS_QUEUE_SIZE};

void update_radio() {
    // printf("RADIO!\n");
}

void update_sd() {
  // printf("SD!\n");

    if (row_idx == ROWS) {
        write_fsdaq_batch(&vals, sd_fp);
    } else {
        vals.setRow(current_row, row_idx);

        row_idx++;
    }
}
void update_dash() {
  const VehicleState vsm_state = vsm.getState();
  // int8_t total_temp = 0;
  uint8_t max_temp = 0;
  for (auto [TEMPS_CELL0, TEMPS_CELL1, TEMPS_CELL2, TEMPS_CELL3, TEMPS_CELL4,TEMPS_CELL5] : vsm_state.accSegTemps) {
    // total_temp += (TEMPS_CELL0 + TEMPS_CELL1 + TEMPS_CELL2 + TEMPS_CELL3 + TEMPS_CELL4 + TEMPS_CELL5);
    max_temp = max( TEMPS_CELL0, max(TEMPS_CELL1, max(TEMPS_CELL2, max(TEMPS_CELL3, max(TEMPS_CELL4, TEMPS_CELL5)))));
  }
  params = {
      .faults =
          Faults{false, static_cast<bool>(!vsm_state.accStatus.PRECHARGE_DONE),
                 static_cast<bool>(!vsm_state.accStatus.SHUTDOWN_STATE)},
      .speed = static_cast<uint8_t>(vsm_state.smeTrqSpd.SPEED * 112 / 7500),
      .soc = vsm_state.accPower.SOC,
      .acc_temp = max_temp,
      .ctrl_tmp = vsm_state.smeTemp.CONTROLLER_TEMP,
      .mtr_tmp = vsm_state.smeTemp.MOTOR_TEMP,
      .mtr_volt = static_cast<float>(vsm_state.accPower.PACK_VOLTAGE / 100.0),
      .glv = static_cast<float>(vsm_state.pdbPowerA.GLV_VOLTAGE),
      .steering_angle = vsm_state.steering_sensor,
      .brake_balance = vsm_state.brake_sensor_f / (vsm_state.brake_sensor_f + vsm_state.brake_sensor_r),
      .brake_f = vsm_state.brake_sensor_f,
      .brake_r = vsm_state.brake_sensor_r,
      .throttle_demand = vsm_state.smeThrottleDemand.TORQUE_DEMAND / 32768.0f, // 0-1
      // 0 - 1
      .brake_val = static_cast<float>(((vsm_state.etcStatus.BRAKE_SENSE_VOLTAGE / 32768.0)*3.3 - 0.33) / (1.65 - 0.33)),
      // psi
      .brake_psi = static_cast<float>((vsm_state.etcStatus.BRAKE_SENSE_VOLTAGE / 32768.0) - 0.1) * 2500,
      .time = chrono::milliseconds(0),
      .delta_time_seconds = 0.01,
      .rtds = static_cast<bool>(vsm_state.etcStatus.RTD),
      .rpm = vsm_state.smeTrqSpd.SPEED,
  };
  printf("%f %f\n", params.brake_f, params.brake_r);
  // params.speed++;
  eve.drawStandardLayout2(params);
}

void error_quit(std::string msg) {
    printf("%s\n", msg.c_str());
    while (1) {
    };
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
    if (ENABLE_DASH) {
        eve.init(EvePresets::CFA800480E3);
        ThisThread::sleep_for(10ms);
        // eve.startFrame();
        // eve.clear(255, 255, 0);
        // eve.endFrame();
    }

    // Initializing SD card
    if (ENABLE_SD) {
        printf("hi\n");
        sd.debug(true);
        int error = fs.mount(&sd);
        if (error) {
            // Reformat if we can't mount the filesystem.
            // This should only happen on the first boot
            printf("No filesystem found, formatting...\n");
            error = fs.reformat(&sd);
            if (error) error_quit("Error: could not reformat SD card! Is the SD card plugged in?\n");
        }

        mkdir("/sd/fsdaq", 0777);

        DIR *dp;
        struct dirent *ep;

        dp = opendir("/sd/fsdaq");

        vector<string> existing_filenames{};

        if (dp != NULL) {
          while ((ep = readdir(dp))) {
            existing_filenames.push_back(ep->d_name);
          }
          closedir(dp);
        } else {
          error_quit("Couldn't open fsdaq directory!\n");
        }
    
        int max_num = 0;
        for (auto it = existing_filenames.begin(); it != existing_filenames.end(); ++it) {
          max_num = std::max(max_num, std::stoi(*it));
        }

        sd_fp = fopen(std::to_string(max_num + 1).c_str(), "w+");
        if (sd_fp == NULL) {
            error_quit("Error opening file!");
        }

        write_fsdaq_schema(sd_fp);
    }

    // int radio_temp = radio.get_temp();
    // if (radio_temp >= 70 || radio_temp <= 10) {
    //     printf("Radio temperature returned an unrealistic value. Disabling.\n");
    //     state.radio_on = false;
    // }

    // int x = 0;
    // Timer t;
    // t.start();
    // auto p = Layouts::StandardLayoutParams{.faults= Faults{}, .soc = 3}; //for testing

    // event_queue.call_every(RADIO_UPDATE_HZ, &update_radio())
    if (ENABLE_SD) {
        queue.call_every(SD_UPDATE_HZ, &update_sd);
    }
    if (ENABLE_DASH) {
        queue.call_every(DASH_UPDATE_HZ, &update_dash);
    }
    queue.call_every(1ms, [](){vsm.update();});
    queue.dispatch_forever();

    while (true) {
        // t.reset();
        // vsm.update();
        // if (x % 7000 == 0) {
        //     printf("\tVSM: %.3fms\n", t.elapsed_time().count()/1000.0);
        // }
        //
        // x++;

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

        // if (x>5000) {
        //     printf("Time: %f\n", t.elapsed_time().count()/1.0);
        //     t.reset();
        //
        //     update_dash();
        //
        //     // eve.drawStandardLayout2(p);
        //     printf("\tDash: %.3fms\n", t.elapsed_time().count()/1000.0);
        //
        //     x = 0;
            // printf("Updating Dash\n");
        // }

        // printf("CAN!\n");
    }
}

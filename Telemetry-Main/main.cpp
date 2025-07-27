#include "BT817Q.hpp"
#include "FATFileSystem.h"
#include "SDBlockDevice.h"
#include "Ticker.h"
#include "VehicleStateManager.hpp"
#include "fsdaq_encoder_generated_from_dbc.hpp"
#include "layouts.h"
#include "radio.hpp"
#include <stdbool.h>
#include <string>

#define ENABLE_RADIO false
#define ENABLE_SD true
#define ENABLE_DASH true

#define SD_UPDATE_HZ 100ms
#define DASH_UPDATE_HZ 100ms
#define RADIO_UPDATE_HZ 1ms

DigitalIn xbee_spi_attn(PA_9);
DigitalOut xbee_spi_cs(PC_8);
SPI xbee_spi(PA_7, PA_6, PA_5);
XBeeRadio radio(xbee_spi, xbee_spi_cs, xbee_spi_attn);

auto mbed_can = CAN(PB_8, PB_9, 500000);
auto can = MbedCAN(mbed_can);
auto vsm = VehicleStateManager(&can, PC_5, PC_1, PC_0);

// SDBlockDevice - lowest-level interfaces with the SD card.
// Pin configurations and transfer speeds are set in mbed_app.json.
SDBlockDevice sd{
    // clang-format off
    MBED_CONF_SD_SPI_MOSI,
    MBED_CONF_SD_SPI_MISO,
    MBED_CONF_SD_SPI_CLK,
    MBED_CONF_SD_SPI_CS,
    MBED_CONF_SD_TRX_FREQUENCY,
    // clang-format on
};
// FATFileSystem - Creates a FAT filesystem on the SDBlockDevice.
// "sd" is the name of the filesystem; i.e. filepaths are /sd/...
FATFileSystem fs{"sd"};

FILE *sd_fp;

Values vals{};
ValuesRow current_row{};

Layouts eve(PC_12, PC_11, PC_10, PD_2, PB_7, PC_13, EvePresets::CFA800480E3);

EventQueue queue{EVENTS_QUEUE_SIZE};

void error_quit(std::string msg) {
    printf("%s\n", msg.c_str());
    while (1) {};
}

void init_sd() {
    int error = fs.mount(&sd);
    if (error) {
        // Reformat if we can't mount the filesystem.
        // This should only happen on the first boot
        printf("No filesystem found, attempting to reformat...\n");
        error = fs.reformat(&sd);
        if (error)
            error_quit("Error: could not reformat SD card! Is the SD card plugged in?\n");
    }

    mkdir("/sd/fsdaq", 0777);

    DIR *dp;
    struct dirent *ep;

    dp = opendir("/sd/fsdaq");

    vector<string> existing_filenames{};

    if (dp == NULL) {
        error_quit("Couldn't open fsdaq directory!\n");
    }
    while ((ep = readdir(dp))) {
        existing_filenames.push_back(ep->d_name);
    }
    closedir(dp);

    int max_num = 0;
    for (auto it = existing_filenames.begin(); it != existing_filenames.end(); ++it) {
        bool is_valid = true;
        for (char c : *it) {
            if (c == '.' && (*it).ends_with(".fsdaq")) break;
            if (!isdigit(c)) is_valid = false;
        }
        if (!is_valid) continue;

        int fnum = std::stoi(*it);
        max_num = max(max_num, fnum);
    }

    sd_fp = fopen(("/sd/fsdaq/" + std::to_string(max_num + 1) + ".fsdaq").c_str(), "w+");
    if (sd_fp == NULL) {
        error_quit("Error opening file!");
    }

    printf("Initialized SD card: writing to %d.fsdaq!\n", max_num + 1);

    fwrite("FSDAQ001", 8, 1, sd_fp);
    write_fsdaq_schema(sd_fp);
}

void init_dash() {
    eve.init(EvePresets::CFA800480E3);
    ThisThread::sleep_for(10ms);
}

void update_radio() {
    // printf("RADIO!\n");
}

int row_idx = 0;
void update_sd() {
    if (row_idx == ROWS) {
        write_fsdaq_batch(&vals, sd_fp);
    } else {
        vals.setRow(current_row, row_idx);

        row_idx++;
    }
}

int n = 0;
void update_dash() {
    const VehicleState vsm_state = vsm.getState();
    uint8_t max_temp = 0;

    for (auto [TEMPS_CELL0, TEMPS_CELL1, TEMPS_CELL2, TEMPS_CELL3, TEMPS_CELL4,
               TEMPS_CELL5] : vsm_state.accSegTemps) {
        max_temp =
            max(TEMPS_CELL0,
                max(TEMPS_CELL1,
                    max(TEMPS_CELL2,
                        max(TEMPS_CELL3, max(TEMPS_CELL4, TEMPS_CELL5)))));
    }
    // params = {
    //     .faults =
    //         Faults{false,
    //                static_cast<bool>(!vsm_state.accStatus.PRECHARGE_DONE),
    //                static_cast<bool>(!vsm_state.accStatus.SHUTDOWN_STATE)},
    //     .speed = static_cast<uint8_t>(vsm_state.smeTrqSpd.SPEED * 112 /
    //     7500), .soc = vsm_state.accPower.SOC, .acc_temp = max_temp, .ctrl_tmp
    //     = vsm_state.smeTemp.CONTROLLER_TEMP, .mtr_tmp =
    //     vsm_state.smeTemp.MOTOR_TEMP, .mtr_volt =
    //     static_cast<float>(vsm_state.accPower.PACK_VOLTAGE / 100.0), .glv =
    //     static_cast<float>(vsm_state.pdbPowerA.GLV_VOLTAGE), .steering_angle
    //     = vsm_state.steering_sensor, .brake_balance =
    //     vsm_state.brake_sensor_f /
    //                      (vsm_state.brake_sensor_f +
    //                      vsm_state.brake_sensor_r),
    //     .brake_f = vsm_state.brake_sensor_f,
    //     .brake_r = vsm_state.brake_sensor_r,
    //     .throttle_demand =
    //         vsm_state.smeThrottleDemand.TORQUE_DEMAND / 32768.0f, // 0-1
    //     // 0 - 1
    //     .brake_val = static_cast<float>(
    //         ((vsm_state.etcStatus.BRAKE_SENSE_VOLTAGE / 32768.0) * 3.3 -
    //         0.33) / (1.65 - 0.33)),
    //     // psi
    //     .brake_psi =
    //         static_cast<float>(
    //             (vsm_state.etcStatus.BRAKE_SENSE_VOLTAGE / 32768.0) - 0.1) *
    //         2500,
    //     .time = chrono::milliseconds(0),
    //     .delta_time_seconds = 0.01,
    //     .rtds = static_cast<bool>(vsm_state.etcStatus.RTD),
    //     .rpm = vsm_state.smeTrqSpd.SPEED,
    // };
    // printf("%f %f\n", params.brake_f, params.brake_r);
    // // params.speed++;
    // eve.drawStandardLayout2(params);

    eve.drawLayout3(
        Faults{false,
               static_cast<bool>(!vsm_state.accStatus.PRECHARGE_DONE),
               static_cast<bool>(!vsm_state.accStatus.SHUTDOWN_STATE)},
        static_cast<float>(vsm_state.accPower.PACK_VOLTAGE / 100.0), max_temp,
        vsm_state.smeTemp.CONTROLLER_TEMP, vsm_state.smeTemp.MOTOR_TEMP,
        vsm_state.accPower.SOC,
        static_cast<float>(vsm_state.pdbPowerA.GLV_VOLTAGE),
        static_cast<bool>(vsm_state.etcStatus.RTD), n);
}

int main() {
    printf("Hello world\n");

    // Initializing SD card
    if (ENABLE_SD) init_sd();

    // Initializing Dash
    if (ENABLE_DASH) init_dash();

    // int radio_temp = radio.get_temp();
    // if (radio_temp >= 70 || radio_temp <= 10) {
    //     printf("Radio temperature returned an unrealistic value.
    //     Disabling.\n"); state.radio_on = false;
    // }

    // int x = 0;
    // Timer t;
    // t.start();
    // auto p = Layouts::StandardLayoutParams{.faults= Faults{}, .soc = 3};
    // //for testing

    // event_queue.call_every(RADIO_UPDATE_HZ, &update_radio())
    if (ENABLE_SD) {
        queue.call_every(SD_UPDATE_HZ, &update_sd);
    }
    if (ENABLE_DASH) {
        queue.call_every(DASH_UPDATE_HZ, &update_dash);
    }
    queue.call_every(1ms, []() {
        vsm.update();
        const VehicleState state = vsm.getState();
        current_row.ACC_SEG0_TEMPS_CELL0 = state.accSegTemps[0].TEMPS_CELL0;
        current_row.VDM_X_AXIS_YAW_RATE = state.vdmYawRate.X;
        current_row.VDM_Y_AXIS_YAW_RATE = state.vdmYawRate.Y;
        current_row.VDM_Z_AXIS_YAW_RATE = state.vdmYawRate.Z;
        current_row.VDM_X_AXIS_ACCELERATION = state.vdmAcceleration.X;
        current_row.VDM_Y_AXIS_ACCELERATION = state.vdmAcceleration.Y;
        current_row.VDM_Z_AXIS_ACCELERATION = state.vdmAcceleration.Z;
    });
    queue.call_every(100ms, []() { n++; });
    queue.dispatch_forever();
}

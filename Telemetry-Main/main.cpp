#include "BT817Q.hpp"
#include "Ticker.h"
#include "layouts.h"
#include "radio.hpp"

#define ENABLE_RADIO true
#define ENABLE_SD true
#define ENABLE_DASH false

#define TICKS_PER_SECOND 200

#define SD_UPDATE_HZ 5
#define RADIO_UPDATE_HZ 1

struct TelemetrySystemState {
    int tick;
    bool radio_on;
    bool sd_on;
    bool dash_on;
    bool radio_event;
    bool sd_event;
};

static TelemetrySystemState state = {0, ENABLE_RADIO, ENABLE_SD, ENABLE_DASH};

DigitalIn spi_attn(PA_9);
DigitalOut cs(PC_8);
SPI spi(PA_7, PA_6, PA_5);
XBeeRadio radio(spi, cs, spi_attn);

Layouts eve(PC_12, PC_11, PC_10, PD_2, PB_7, PC_13, EvePresets::CFA800480E3);

Ticker ticker;

void update_radio(void) {
    printf("RADIO!\n");
}

void update_sd(void) {
    printf("SD!\n");
}

void update_dash(void) {
    printf("DASH!\n");
    Layouts::StandardLayoutParams p{
        .faults = Faults{0, 0, 1}, .soc = 60, .acc_temp = 80};
    eve.drawStandardLayout2(p);
}

int main() {
    printf("Hello world\n");

    ticker.attach([&]() {
        state.tick++;
        if (state.tick % (TICKS_PER_SECOND / RADIO_UPDATE_HZ) == 0) {
            state.radio_event = true;
        }
        if (state.tick % (TICKS_PER_SECOND / SD_UPDATE_HZ) == 0) {
            state.sd_event = true;
        }
    }, 1000ms / TICKS_PER_SECOND);

    if (state.dash_on) {
        eve.init(EvePresets::CFA800480E3);
        ThisThread::sleep_for(10ms);
        eve.startFrame();
        eve.clear(0, 0, 0);
        eve.endFrame();
        ThisThread::sleep_for(10ms);
    }

    int radio_temp = radio.get_temp();
    if (radio_temp >= 50 || radio_temp <= 20) {
        printf("Radio temperature returned an unrealistic value. Disabling.\n");
        state.radio_on = false;
    }

    while (true) {

        if (state.radio_event) {
            state.radio_event = false;
            if (state.radio_on) {
                update_radio();
            }
        }

        if (state.sd_event) {
            state.sd_event = false;
            if (state.sd_on) {
                update_sd();
            }
        }

        if (state.dash_on) {
            update_dash();
        }

        // printf("CAN!\n");

    }
}

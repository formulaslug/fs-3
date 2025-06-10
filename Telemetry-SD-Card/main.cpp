#include "SDFileSystem.h"
#include "mbed.h"
#include "./fsdaq_encoder_generated_from_dbc.hpp"

void error_quit(std::string msg) {
    printf("%s\n", msg.c_str());
    while (1) {
    };
}

int main(int argc, char *argv[]) {
    printf("Hello World!\n");

    // SDFileSystem sd{D2, A5, A4, D3, "sd"}; // Mosi, miso, sclk, cs // L423KC
    SDFileSystem sd{D11, D12, D13, D10, "sd"}; // Mosi, miso, sclk, cs // F446RE
    if (sd.disk_initialize() != 0) {
        error_quit("Failed to initialize SD card!");
    }

    mkdir("/sd/fsdaq", 0777);

    FILE *file = fopen("/sd/fsdaq/test.fsdaq", "w");
    if (file == NULL) {
        error_quit("Error opening file!");
    }

    fwrite("FSDAQ001", 8, 1, file);

    write_fsdaq_schema(file);

    Values vals{};
    // memset(&vals, INT_MAX, sizeof(vals));

    write_fsdaq_batch(&vals, file);

    fwrite("FSDAQ001", 8, 1, file);

    fclose(file);

    printf("Goodbye World!\n");
    while (1) {
    };
}

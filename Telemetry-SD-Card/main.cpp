#include "FATFileSystem.h"
#include "SDBlockDevice.h"
#include "mbed.h"

// SDBlockDevice - lowest-level interfaces with the SD card.
// Pin configurations and transfer speeds are set in mbed_app.json.
SDBlockDevice sd{
    MBED_CONF_SD_SPI_MOSI,
    MBED_CONF_SD_SPI_MISO,
    MBED_CONF_SD_SPI_CLK, 
    MBED_CONF_SD_SPI_CS,
    MBED_CONF_SD_TRX_FREQUENCY,
};
// FATFileSystem - Creates a FAT filesystem on the SDBlockDevice.
// "sd" is the name of the filesystem; i.e. filepaths are /sd/...
FATFileSystem fs{"sd"};

void error_quit(std::string msg) {
    printf("%s\n", msg.c_str());
    while (1) {
    };
}

int main(int argc, char *argv[]) {
    printf("Hello World!\n");

    // Enable debug logging (to be turned off)
    sd.debug(true);

    // TODO: experiment with async SPI and/or DMA
    // sd.set_async_spi_mode(true, DMAUsage::DMA_USAGE_ALWAYS);

    // Mount the FATFileSystem (so we can use regular C file IO like fopen/read)
    int error = fs.mount(&sd);
    if (error) {
        // Reformat if we can't mount the filesystem.
        // This should only happen on the first boot
        printf("No filesystem found, formatting...\n");
        error = fs.reformat(&sd);
        if (error) error_quit("Error: could not reformat SD card! Is the SD card plugged in?\n");
    }

    mkdir("/sd/testing", 0777);

    FILE *file = fopen("/sd/testing/test.txt", "w+");
    if (file == NULL) {
        error_quit("Error opening file!");
    }

    fwrite("alksjdlkasjdklasjflka Hellow  World!!!", 100, 1, file);

    fclose(file);

    printf("Goodbye World!\n\n");
    while (1) {
    };
}

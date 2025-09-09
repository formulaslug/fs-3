#include "./fsdaq_encoder_generated_from_dbc.hpp"
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

// Our data
Values vals{};
ValuesRow current_row{};

// Debug
mbed_stats_heap_t heap;
mbed_stats_stack_t stack;

void print_mem_usage() {
    mbed_stats_heap_get(&heap);
    printf("\nHeap: %u/%u (used/reserved), max usage: %u, allocs: %u\n",
           heap.current_size, heap.reserved_size, heap.max_size,
           heap.alloc_cnt);
    mbed_stats_stack_get(&stack);
    printf("Stack: max usage: %u, bytes reserved: %u\n\n", stack.max_size,
           stack.reserved_size);
}

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

    print_mem_usage();

    // Mount the FATFileSystem (so we can use regular C file IO like fopen/read)
    int error = fs.mount(&sd);
    if (error) {
        // Reformat if we can't mount the filesystem.
        // This should only happen on the first boot
        printf("No filesystem found, formatting...\n");
        error = fs.reformat(&sd);
        if (error) error_quit("Error: could not reformat SD card! Is the SD card plugged in?\n");
    }

    mkdir("/sd/fsdaqqqq", 0777);

    print_mem_usage();

    FILE *file = fopen("/sd/fsdaqqqq/test.fsdaq", "w+");
    if (file == NULL) {
        error_quit("Error opening file!");
    }

    fwrite("FSDAQ001", 8, 1, file);

    write_fsdaq_schema(file);

    print_mem_usage();


    for (int i=0; i<5; i++) {
        memset(&current_row, INT_MAX, sizeof(current_row));
        vals.setRow(current_row, i);
    }
    write_fsdaq_batch(&vals, file);
    write_fsdaq_batch(&vals, file);
    write_fsdaq_batch(&vals, file);
    write_fsdaq_batch(&vals, file);
    write_fsdaq_batch(&vals, file);

    print_mem_usage();

    fwrite("FSDAQ001", 8, 1, file);

    fclose(file);

    printf("Goodbye World!\n\n");
    while (1) {
    };
}

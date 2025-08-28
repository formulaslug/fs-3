#pragma once

#include "FATFileSystem.h"
#include "SDBlockDevice.h"
#include "encoder_generated.hpp"
#include "mbed.h"
#include "radio.hpp"
#include <cstdio>

namespace fsdaq {

class DataLogger {
public:
    DataLogger();
    ~DataLogger();
    bool init_logging();
    /*
     * Adds a DataRow to the current fsdaq batch. Once row_idx reachers
     * ROWS_PER_BATCH, it writes the batch to the SD card and resets the current
     * batch.
     */
    void append_row(fsdaq::DataRow& next_row);

    bool radio_enabled = false;
private:
    /* 
     * Initializes the SD Card block device, mounts the FATFileSystem, and opens
     * a new fsdaq file.
     */
    bool init_sd();
    void deinit_sd();

    // Main timer used for the Time_ms data column.
    Timer timer;

    // SDBlockDevice - lowest-level interfaces with the SD card.
    // Pin configurations and transfer speeds are set in mbed_app.json.
    SDBlockDevice sd;
    // FATFileSystem - Creates a FAT filesystem on the SDBlockDevice.
    // "sd" is the name of the filesystem; i.e. filepaths are /sd/...
    FATFileSystem fatfs;
    // File pointer to the file opened on the SD card
    FILE *sd_fp;

    // The current batch of data that we're appending rows to.
    fsdaq::DataBatch current_batch{};
    int row_idx;

    // Everything pertaining to XBee radio (TODO: clean)
    DigitalIn xbee_spi_attn{PA_9};
    DigitalOut xbee_spi_cs{PC_8};
    SPI xbee_spi{PA_7, PA_6, PA_5};
    XBeeRadio radio{xbee_spi, xbee_spi_cs, xbee_spi_attn};

};

}

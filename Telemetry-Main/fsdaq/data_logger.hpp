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
    static constexpr chrono::duration SD_UPDATE_HZ = 10ms;
    static constexpr chrono::duration RADIO_UPDATE_HZ = 100ms;

    static constexpr uint ROWS_PER_BATCH_SDCARD = 80;
    static constexpr uint ROWS_PER_BATCH_RADIO = 8;
    using DataBatchSD = DataBatch<ROWS_PER_BATCH_SDCARD>;
    using DataBatchRadio = DataBatch<ROWS_PER_BATCH_RADIO>;

    DataLogger();
    ~DataLogger();
    /*
     * Initializes SD card logging, and Radio live telemetry if the parameter
     * enables it.
     */
    bool init_logging(bool use_radio);
    /*
     * Adds a DataRow to the current fsdaq batches for SD card and radio.
     * Flushes to SD Card / radio once the row index meets the respective
     * batch's ROWS_PER_BATCH.
     *
     * init_logging() must have been called first!
     */
    void append_row(fsdaq::DataRow& next_row);

private:
    /*
     * Initializes the SD Card block device, mounts the FATFileSystem, and opens
     * a new fsdaq file.
     */
    bool init_sd();

    void deinit_sd();

    // Main timer used for the Time_ms data column.
    Timer timer;

    /* --- SD Card --- */
    // Lowest-level interface with the SD card. Pin configurations and transfer
    // speeds are set in mbed_app.json.
    SDBlockDevice sd;
    // Creates a FAT filesystem on the SDBlockDevice. "sd" is the name of the
    // filesystem; i.e. filepaths are /sd/...
    FATFileSystem fatfs;
    // Standard libc file handle to the DAQ file on the SD card filesystem
    FILE* sd_fp;

    /* --- XBee Radio --- */
    DigitalIn xbee_spi_attn{PA_9};
    DigitalOut xbee_spi_cs{PC_8};
    SPI xbee_spi{PA_7, PA_6, PA_5};
    XBeeRadio radio{xbee_spi, xbee_spi_cs, xbee_spi_attn};

    // The current batches of data that we're appending rows to. We store
    // separate batches for the SD card and for the radio, as radio bandwidth is
    // much smaller.
    fsdaq::DataBatch<80> sd_current_batch{};
    int sd_row_idx;
    fsdaq::DataBatch<8> radio_current_batch{};
    int radio_row_idx;
};

} // namespace fsdaq

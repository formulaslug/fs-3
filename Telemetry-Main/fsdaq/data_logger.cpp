// TODO: USE UPDATED FILES (9/12/25)

#include "data_logger.hpp"
#include "encoder_generated.hpp"
#include "mbed.h"

namespace fsdaq {

DataLogger::DataLogger() : sd{}, fatfs{"sd"} {}

DataLogger::~DataLogger() {
    fclose(sd_fp);
    fatfs.unmount();
}

bool DataLogger::init_logging() {
    bool success = init_sd();
    if (!success) return false;

    timer.start();

    fsdaq::write_fsdaq_header(sd_fp);
    fsdaq::write_fsdaq_schema<DataLogger::ROWS_PER_BATCH_SDCARD>(sd_fp);

    return true;
}

void DataLogger::append_row(fsdaq::DataRow &next_row) {
    // TODO: Do we care that this will insert the time into the row when it's
    // finished/added rather than in the middle or beginning of its duration?
    // Should we move ownership of the timer logic from main to data_logger?
    const uint32_t elapsed_time_ms = duration_cast<chrono::milliseconds>(timer.elapsed_time()).count();
    sd_current_batch.set_row(next_row, sd_row_idx, elapsed_time_ms);

    sd_row_idx++;
    if (sd_row_idx == fsdaq::DataLogger::ROWS_PER_BATCH_SDCARD) {
        write_fsdaq_batch(&sd_current_batch, sd_fp);
        sd_row_idx = 0;
    }
}

bool DataLogger::init_sd() {
    printf("mounting sdfilesystem...\n");
    int error = fatfs.mount(&sd);
    if (error) {
        // Reformat if we can't mount the filesystem.
        // This should only happen on the first boot
        printf("No filesystem found, attempting to reformat...\n");
        error = fatfs.reformat(&sd);
        if (error) {
            printf("Error: could not reformat SD card! Is the SD card plugged in?\n");
            return false;
        }
    }

    printf("mkdir...\n");
    mkdir("/sd/fsdaq", 0777);

    DIR *dp;
    struct dirent *ep;

    printf("opendir...\n");
    dp = opendir("/sd/fsdaq");

    vector<string> existing_filenames{};

    if (dp == NULL) {
        printf("Couldn't open fsdaq directory!\n");
        return false;
    }
    while ((ep = readdir(dp))) {
        existing_filenames.push_back(ep->d_name);
    }
    closedir(dp);

    printf("finding max_num...\n");
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

    printf("fopen...\n");
    sd_fp = fopen(("/sd/fsdaq/" + std::to_string(max_num + 1) + ".fsdaq").c_str(), "w+");
    if (sd_fp == NULL) {
        printf("Error opening file!");
        return false;
    }

    printf("Initialized SD card: writing to %d.fsdaq!\n", max_num + 1);
    return true;
}

} // namespace fsdaq

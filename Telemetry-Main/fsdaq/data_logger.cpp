#include "data_logger.hpp"
#include "encoder_generated.hpp"
#include "mbed.h"

namespace fsdaq {

DataLogger::DataLogger() : sd{}, fatfs{"sd"} { }

DataLogger::~DataLogger() {
    fclose(sd_fp);
    fatfs.unmount();
}

bool DataLogger::init_logging() {
    bool success = init_sd();
    if (!success) return false;

    fsdaq::write_fsdaq_header(sd_fp);
    fsdaq::write_fsdaq_schema(sd_fp);

    return true;
}

/* 
 * Append a row to the current fsdaq data batch. 
 * init() must have been called first!
 */
void DataLogger::append_row(fsdaq::DataRow &next_row) {
    current_batch.setRow(next_row, row_idx);

    row_idx++;
    if (row_idx == fsdaq::ROWS_PER_BATCH) {
        write_fsdaq_batch(&current_batch, sd_fp);
        row_idx = 0;
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

}

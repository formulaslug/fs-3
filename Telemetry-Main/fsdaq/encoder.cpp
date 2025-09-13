#include "encoder.hpp"
#include "mbed.h"

void fsdaq::fwrite_schema(FILE* file) {
    uint32_t m = COLS;
    uint32_t n = SDCARD_ROWS_PER_BATCH;
    fwrite(&m, sizeof(m), 1, file);
    fwrite(&n, sizeof(n), 1, file);

    for (int i = 0; i < COLS; i++) {
        const uint8_t col_len = col_name_sizes[i];
        const char* col = col_names[i];

        fwrite(&col_len, sizeof(col_len), 1, file);
        fwrite(col, col_len, 1, file);
    }

    for (int i = 0; i < COLS; i++) {
        const char* col_type = col_name_types[i];
        fwrite(col_type, 2, 1, file);
    }
}
void fsdaq::fwrite_data_batch(FILE* file, DataBatchSD* batch) {
    fwrite(batch, sizeof(*batch), 1, file);
}
void fsdaq::fwrite_header(FILE* file) {
    fwrite("FSDAQ001", 8, 1, file);
}


size_t fsdaq::create_fsdaq_schema(uint8_t* dest) {
    size_t offset = 0;
    uint32_t m = COLS;
    uint32_t n = RADIO_ROWS_PER_BATCH;
    memcpy(dest, &m, sizeof(m));
    offset += sizeof(m);
    memcpy(dest + offset, &n, sizeof(n));
    offset += sizeof(n);

    for (int i = 0; i < COLS; i++) {
        const uint8_t col_len = col_name_sizes[i];
        const char* col = col_names[i];

        memcpy(dest + offset, &col_len, sizeof(col_len));
        offset += sizeof(col_len);
        memcpy(dest + offset, &col, col_len);
        offset += col_len;
    }

    for (int i = 0; i < COLS; i++) {
        const char* col_type = col_name_types[i];
        memcpy(dest, &col_type, 2);
        offset += 2;
    }

    return offset;
};

// TODO: FINISH COPYING FROM https://github.com/formulaslug/fs-3/compare/main...radio-tranceiver-terrible-bodged-code

#pragma once

#include "encoder_generated_from_dbc.hpp"

namespace fsdaq {

// Takes in data rows, handles the batching and file writing.
// Does not own underlying file pointer (never calls fopen/fclose).
class BatchFileWriter {
public:
    BatchFileWriter(FILE *file);

    void append_row(BatchRow next_row);

private:
    Batch curr_batch;

    int row_idx;

    FILE *fp;
};

} // namespace fsdaq

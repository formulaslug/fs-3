#include "encoder_generated_from_dbc.hpp"

namespace fsdaq {

class BatchFileWriter {
public:
    BatchFileWriter(FILE *file) : fp(file){};

    void append_row(BatchRow next_row) {
        curr_batch.setRow(next_row, row_idx);

        row_idx++;
        if (row_idx == ROWS_PER_BATCH) {
            write_fsdaq_batch(&curr_batch, fp);
            row_idx = 0;
        }
    }

private:
    Batch curr_batch{};

    int row_idx = 0;

    FILE *fp;
};


} // namespace fsdaq

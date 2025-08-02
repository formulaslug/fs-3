#include "encoder_generated.hpp"

namespace fsdaq {

class FileBatchWriter {
public:
    FileBatchWriter(FILE *file)
        : curr_batch(std::make_unique<DataBatch>()), fp(file) {}

    void append_row(DataRow next_row) {
        curr_batch->setRow(next_row, row_idx);

        row_idx++;
        if (row_idx == ROWS_PER_BATCH) {
            write_fsdaq_batch(curr_batch.get(), fp);
            row_idx = 0;
        }
    }

private:
    std::unique_ptr<DataBatch> curr_batch;

    int row_idx = 0;

    FILE *fp;
};

} // namespace fsdaq

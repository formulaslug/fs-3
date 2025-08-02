#include "encoder_generated.hpp"
#include "file_batch_writer.hpp"

namespace fsdaq {

    FileBatchWriter::FileBatchWriter(FILE *file)
        : curr_batch(std::make_unique<DataBatch>()), fp(file) {}

    void FileBatchWriter::append_row(DataRow& next_row) {
        // printf("setRow()\n");
        curr_batch->setRow(next_row, row_idx);

        row_idx++;
        if (row_idx == ROWS_PER_BATCH) {
            write_fsdaq_batch(curr_batch.get(), this->fp);
            row_idx = 0;
        }
    }

} // namespace fsdaq

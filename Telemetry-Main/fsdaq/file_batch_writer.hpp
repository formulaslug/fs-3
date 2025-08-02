#pragma once

#include "encoder_generated.hpp"

namespace fsdaq {

class FileBatchWriter {
public:
    FileBatchWriter(FILE *file);

    void append_row(DataRow next_row);

private:
    std::unique_ptr<DataBatch> curr_batch;

    int row_idx;

    FILE *fp;
};

} // namespace fsdaq

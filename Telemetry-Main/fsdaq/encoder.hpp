#include "generated_types.hpp"
#include "mbed.h"

namespace fsdaq {

constexpr uint SDCARD_ROWS_PER_BATCH = 80;
constexpr uint RADIO_ROWS_PER_BATCH = 8;

using DataBatchSD = DataBatch<SDCARD_ROWS_PER_BATCH>;
using DataBatchRadio = DataBatch<RADIO_ROWS_PER_BATCH>;

void fwrite_schema(FILE* file);
void fwrite_data_batch(FILE* file, DataBatchSD* batch);
void fwrite_header(FILE* file);

// TODO: REPLACE WITH OWNED DYNAMIC BYTEBUFS
/*std::vector<uint8_t>*/ size_t create_fsdaq_schema(uint8_t* dest);

} // namespace fsdaq

#include "SDFileSystem.h"
#include "flatcc_generated.h"
#include <string>

#define ns(x) FLATBUFFERS_WRAP_NAMESPACE(org_apache_arrow_flatbuf, x)
#define FLATCC_PRINT_UNLESS_0_NO_NS(x)                               \
  if ((x) != 0) {                                                     \
    printf("%s:%d: %s failed", __FILE__, __LINE__, #x); \
  }
#define FLATCC_PRINT_UNLESS_0(x, error) FLATCC_RETURN_UNLESS_0_NO_NS(ns(x), error)

void error_quit(std::string msg) {
    printf("%s\n", msg.c_str());
    while (1) {
    };
}

static inline int64_t round_up_to_multiple_of_8(int64_t value) {
  return (value + 7) & ~((int64_t)7);
}
static void maybe_pad_to_next_8_bytes(FILE *f, size_t written_bytes) {
    size_t padding = (8 - (written_bytes % 8)) % 8;
    if (padding) {
        uint8_t zeroes[8] = {0};
        fwrite(zeroes, padding, 1, f);
    }
}


const char *cols[] = {"aaaa", "bbbb"};
#define COLS 2
#define ROWS 9

// static int batches_written = 0;
// static int64_t batch_offsets[100];
// static int32_t batch_metaDataLengths[100];
// static int64_t batch_bodyLengths[100];

void arrow_flatcc_build_schema(flatcc_builder_t *b) {
    org_apache_arrow_flatbuf_Schema_endianness_add(b, org_apache_arrow_flatbuf_Endianness_Little);
    org_apache_arrow_flatbuf_Schema_fields_start(b);

    for (const char* col : cols) {
        org_apache_arrow_flatbuf_Schema_fields_push_start(b);

        org_apache_arrow_flatbuf_Field_name_create_str(b, col);
        org_apache_arrow_flatbuf_Field_nullable_add(b, false);
        org_apache_arrow_flatbuf_Field_type_Int_create(b, 32, true);

        org_apache_arrow_flatbuf_Field_children_start(b);
        // Our columns are super simple (primitives only); no children
        org_apache_arrow_flatbuf_Field_children_end(b);
        org_apache_arrow_flatbuf_Field_custom_metadata_start(b);
        // Our fields don't have any custom metadata afaik
        org_apache_arrow_flatbuf_Field_custom_metadata_end(b);

        org_apache_arrow_flatbuf_Schema_fields_push_end(b);
    }

    org_apache_arrow_flatbuf_Schema_fields_end(b);
    org_apache_arrow_flatbuf_Schema_custom_metadata_start(b);
    // we don't have any custom metadata afaik
    org_apache_arrow_flatbuf_Schema_custom_metadata_end(b);
    org_apache_arrow_flatbuf_Schema_features_start(b);
    org_apache_arrow_flatbuf_Schema_features_end(b);
}

void arrow_flatcc_encode_schema_message(flatcc_builder_t *b) {
    org_apache_arrow_flatbuf_Message_start_as_root(b);
    org_apache_arrow_flatbuf_Message_version_add(b, org_apache_arrow_flatbuf_MetadataVersion_V5);
    org_apache_arrow_flatbuf_Message_header_Schema_start(b);

    arrow_flatcc_build_schema(b);

    org_apache_arrow_flatbuf_Message_header_Schema_end(b);
    org_apache_arrow_flatbuf_Message_bodyLength_add(b, 0);
    org_apache_arrow_flatbuf_Message_end_as_root(b);
}

// Note that the actual data buffers come after this flatbuffer message, in the
// messageBody. This is contains only the metadata (lenghts, offsets, etc)
void arrow_flatcc_encode_record_batch_message(flatcc_builder_t *b) {
    org_apache_arrow_flatbuf_Message_start_as_root(b);
    org_apache_arrow_flatbuf_Message_version_add(b, org_apache_arrow_flatbuf_MetadataVersion_V5);
    org_apache_arrow_flatbuf_Message_header_RecordBatch_start(b);

    // each column defined in the schema is represented by one Node struct here
    org_apache_arrow_flatbuf_FieldNode nodes[COLS] = {};
    // every column has 1-3 vectors depending on its type; eg. data, validity, offsets, etc
    org_apache_arrow_flatbuf_Buffer buffers[COLS * 2] = {};

    int64_t offset = 0;
    for (uint i = 0; i < COLS; i++) {
        // We can assume each node has no null's for now. In that case the
        // validity buffer can either be all 1s, or be left out entirely
        nodes[i] = {.length = ROWS, .null_count = 0};
    }
    for (uint i = 0; i < COLS*2; i+=2) {
        // Every time we add a buffer, the next buffer's offset increases by the
        // number of bytes in the previous buffer
        const uint length_validity = (int)(ceil(ROWS / 8.0f));
        buffers[i] = {.offset = offset, .length = length_validity};
        offset += round_up_to_multiple_of_8(length_validity);

        const uint length_values = ROWS * sizeof(int32_t);
        buffers[i+1] = {.offset = offset, .length = length_values};
        offset += round_up_to_multiple_of_8(length_values);
    }

    org_apache_arrow_flatbuf_RecordBatch_nodes_create(b, nodes, COLS);
    org_apache_arrow_flatbuf_RecordBatch_buffers_create(b, buffers, COLS * 2);

    org_apache_arrow_flatbuf_Message_header_RecordBatch_end(b);
    // Here, bodyLength refers to the # of bytes after the Flatbuffer message
    // metadata that contain the actual data buffers.
    org_apache_arrow_flatbuf_Message_bodyLength_add(b, offset);
    org_apache_arrow_flatbuf_Message_end_as_root(b);
}

// void arrow_flatcc_encode_footer_message(flatcc_builder_t *b) {
//     org_apache_arrow_flatbuf_Footer_start_as_root(b);
//     org_apache_arrow_flatbuf_Footer_version_add(b, org_apache_arrow_flatbuf_MetadataVersion_V5);
//     org_apache_arrow_flatbuf_Footer_schema_start(b);
//
//     arrow_flatcc_build_schema(b);
//
//     org_apache_arrow_flatbuf_Footer_schema_end(b);
//
//     org_apache_arrow_flatbuf_Footer_recordBatches_start(b);
//     org_apache_arrow_flatbuf_Block *blocks = org_apache_arrow_flatbuf_Footer_recordBatches_extend(b, batches_written);
//
//
//     org_apache_arrow_flatbuf_Footer_recordBatches_end(b);
//     org_apache_arrow_flatbuf_Footer_end_as_root(b);
// }

// void arrow_start_file(FILE *f) {
//     fwrite("ARROW1\0\0", 8, 1, f);
// }
// void arrow_end_file(FILE *f) {
//     fwrite("ARROW1", 6, 1, f);
// }

static constexpr uint8_t continuation[] = {0xFF, 0xFF, 0xFF, 0xFF};

int main(int argc, char *argv[]) {
    printf("Hello World!\n");

    SDFileSystem sd{D2, A5, A4, D3, "sd"}; // Mosi, miso, sclk, cs // L423KC
    // SDFileSystem sd{D11, D12, D13, D10, "sd"}; // Mosi, miso, sclk, cs // F446RE
    if (sd.disk_initialize() != 0)
        error_quit("Failed to initialize SD card!");

    mkdir("/sd/arrow", 0777);

    FILE *file = fopen("/sd/arrow/test.arrows", "w");
    if (file == NULL)
        error_quit("Error opening file!");

    flatcc_builder_t b;
    flatcc_builder_init(&b);

    // arrow_start_file(file);

    arrow_flatcc_encode_schema_message(&b);
    size_t schema_size;
    void *schema = flatcc_builder_finalize_buffer(&b, &schema_size);
    MBED_ASSERT(schema_size != 0 && schema);

    flatcc_builder_reset(&b);

    fwrite(continuation, 4, 1, file);
    const int32_t schema_size_padded = round_up_to_multiple_of_8(schema_size);
    fwrite(&schema_size_padded, 4, 1, file);
    fwrite(schema, schema_size, 1, file);
    // Pad schema to be 8-byte aligned
    maybe_pad_to_next_8_bytes(file, schema_size);


    arrow_flatcc_encode_record_batch_message(&b);
    size_t record_batch_size;
    void *record_batch = flatcc_builder_finalize_buffer(&b, &record_batch_size);
    MBED_ASSERT(record_batch_size != 0 && record_batch);

    flatcc_builder_reset(&b);

    fwrite(continuation, 4, 1, file);
    const int32_t record_batch_size_padded = round_up_to_multiple_of_8(record_batch_size);
    fwrite(&record_batch_size_padded, 4, 1, file);
    fwrite(record_batch, record_batch_size, 1, file);

    // Pad record batch Message flatbuffer to 8-byte alignment
    maybe_pad_to_next_8_bytes(file, record_batch_size);

    // Write actual record batch data buffers (message_body)
    uint8_t col1validity[] = {0xFF}; // all non-null
    int32_t col1values[ROWS] = {0,1,2,3,4,5,6,7,8};
    uint8_t col2validity[] = {0xFF}; // all non-null
    int32_t col2values[ROWS] = {1,2,3,4,5,6,7,8,9};
    fwrite(col1validity, sizeof(col1validity), 1, file);
    maybe_pad_to_next_8_bytes(file, sizeof(col1validity));
    fwrite(col1values, sizeof(col1values), 1, file);
    maybe_pad_to_next_8_bytes(file, sizeof(col1values));
    fwrite(col2validity, sizeof(col2validity), 1, file);
    maybe_pad_to_next_8_bytes(file, sizeof(col2validity));
    fwrite(col2values, sizeof(col2values), 1, file);
    maybe_pad_to_next_8_bytes(file, sizeof(col2values));
    const int message_body_size = round_up_to_multiple_of_8(sizeof(col1values)) + round_up_to_multiple_of_8(sizeof(col2values)) + round_up_to_multiple_of_8(sizeof(col1validity)) + round_up_to_multiple_of_8(sizeof(col2validity));

    // Pad record batch data buffers to 8-byte alignment
    maybe_pad_to_next_8_bytes(file, message_body_size);


    // arrow_flatcc_encode_footer_message(&b);
    // size_t footer_size;
    // void *footer = flatcc_builder_finalize_buffer(&b, &footer_size);
    // MBED_ASSERT(footer_size != 0 && footer);
    //
    // flatcc_builder_reset(&b);
    //
    // fwrite(footer, footer_size, 1, file);
    //
    //
    // printf("%d\n", footer_size);
    // fwrite(&footer_size, sizeof(footer_size), 1, file);

    // arrow_end_file(file);

    const int32_t eos[] = {-1, 0};
    fwrite(eos, 8, 1, file);

    fclose(file);

    printf("Goodbye World!\n");
    while (1) {};
}

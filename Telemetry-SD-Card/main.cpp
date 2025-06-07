#include "SDFileSystem.h"
#include "flatcc_generated.h"
#include <string>

void error_quit(std::string msg) {
    printf("%s\n", msg.c_str());
    while (1) {
    };
}

const char *cols[] = {"aaaa", "bbbb"};

void arrow_flatcc_encode_schema(flatcc_builder_t *b) {
    org_apache_arrow_flatbuf_Message_start_as_root(b);
    org_apache_arrow_flatbuf_Message_version_add(b, org_apache_arrow_flatbuf_MetadataVersion_V5);
    org_apache_arrow_flatbuf_Message_header_Schema_start(b);
    // start encode
    org_apache_arrow_flatbuf_Schema_endianness_add(b, org_apache_arrow_flatbuf_Endianness_Little);
    org_apache_arrow_flatbuf_Schema_fields_start(b);
    // start encode fields
    for (const char* col : cols) {
        org_apache_arrow_flatbuf_Schema_fields_push_start(b);
        org_apache_arrow_flatbuf_Field_name_create_str(b, col);
        org_apache_arrow_flatbuf_Field_nullable_add(b, false);
        org_apache_arrow_flatbuf_Field_type_Int_create(b, 32, true);
        org_apache_arrow_flatbuf_Schema_fields_push_end(b);
    }
    // end encode fields
    org_apache_arrow_flatbuf_Schema_fields_end(b);
    // org_apache_arrow_flatbuf_Schema_custom_metadata_start(b);
    // org_apache_arrow_flatbuf_Schema_custom_metadata_end(b);
    org_apache_arrow_flatbuf_Schema_features_start(b);
    org_apache_arrow_flatbuf_Schema_features_end(b);
    // end encode
    org_apache_arrow_flatbuf_Message_header_Schema_end(b);
    org_apache_arrow_flatbuf_Message_bodyLength_add(b, 0);
    org_apache_arrow_flatbuf_Message_end_as_root(b);
}

void arrow_flatcc_encode_footer(flatcc_builder_t *b) {
    org_apache_arrow_flatbuf_Footer_start_as_root(b);
    org_apache_arrow_flatbuf_Footer_version_add(b, org_apache_arrow_flatbuf_MetadataVersion_V5);
    org_apache_arrow_flatbuf_Footer_schema_start(b);
    arrow_flatcc_encode_schema(b);
    org_apache_arrow_flatbuf_Footer_schema_end(b);

    org_apache_arrow_flatbuf_Footer_recordBatches_start(b);
    org_apache_arrow_flatbuf_Footer_recordBatches_extend(b, 0);
    // TODO: add record batch blocks
    org_apache_arrow_flatbuf_Footer_recordBatches_end(b);
    org_apache_arrow_flatbuf_Footer_end_as_root(b);
}

#define ARROW_MAGIC_PADDED "ARROW1\0"
#define ARROW_MAGIC "ARROW1"

void arrow_start_file(FILE *f) {
    fwrite(ARROW_MAGIC, sizeof(ARROW_MAGIC), 1, f);
}
void arrow_end_file(FILE *f) {
    fwrite(ARROW_MAGIC, sizeof(ARROW_MAGIC), 1, f);
}

int main (int argc, char *argv[]) {
    // SDFileSystem sd{D2, A5, A4, D3, "sd"}; // Mosi, miso, sclk, cs // L423KC
    SDFileSystem sd{D11, D12, D13, D10, "sd"}; // Mosi, miso, sclk, cs // F446RE
    if (sd.disk_initialize() != 0)
        error_quit("Failed to initialize SD card!");

    mkdir("/sd/arrow", 0777);

    FILE *file = fopen("/sd/arrow/test.arrows", "w");
    if (file == NULL)
        error_quit("Error opening file!");

    flatcc_builder_t b;
    flatcc_builder_init(&b);

    arrow_start_file(file);
    arrow_flatcc_encode_schema(&b);

    arrow_end_file(file);


    while (1) {};
}

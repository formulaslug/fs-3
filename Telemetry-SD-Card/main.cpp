#include "FATFileSystem.h"
#include "SDBlockDevice.h"
#include "flatcc_generated.h"
#include "mbed_stats.h"
#include <string>

// SDBlockDevice - lowest-level interfaces with the SD card.
// Pin configurations and transfer speeds are set in mbed_app.json.
SDBlockDevice sd{
    MBED_CONF_SD_SPI_MOSI,
    MBED_CONF_SD_SPI_MISO,
    MBED_CONF_SD_SPI_CLK, 
    MBED_CONF_SD_SPI_CS,
    MBED_CONF_SD_TRX_FREQUENCY,
};
// FATFileSystem - Creates a FAT filesystem on the SDBlockDevice.
// "sd" is the name of the filesystem; i.e. filepaths are /sd/...
FATFileSystem fs{"sd"};

// Debug
mbed_stats_heap_t heap;
mbed_stats_stack_t stack;

void print_mem_usage() {
    mbed_stats_heap_get(&heap);
    printf("\nHeap: %u/%u (used/reserved), max usage: %u, allocs: %u\n",
           heap.current_size, heap.reserved_size, heap.max_size,
           heap.alloc_cnt);
    mbed_stats_stack_get(&stack);
    printf("Stack: max usage: %u, bytes reserved: %u\n\n", stack.max_size,
           stack.reserved_size);
}

void error_quit(std::string msg) {
    printf("%s\n", msg.c_str());
    while (1) {
    };
}

#define ns(x) FLATBUFFERS_WRAP_NAMESPACE(org_apache_arrow_flatbuf, x)
#define FLATCC_PRINT_UNLESS_0_NO_NS(x)                               \
  if ((x) != 0) {                                                     \
    printf("%s:%d: %s failed", __FILE__, __LINE__, #x); \
  }
#define FLATCC_PRINT_UNLESS_0(x, error) FLATCC_RETURN_UNLESS_0_NO_NS(ns(x), error)

static inline int64_t round_up_to_multiple_of_8(int64_t value) {
  return (value + 7) & ~((int64_t)7);
}
static void maybe_pad_to_8_bytes(FILE *f, size_t written_bytes) {
    size_t padding = (8 - (written_bytes % 8)) % 8;
    if (padding) {
        uint8_t zeroes[8] = {0};
        fwrite(zeroes, padding, 1, f);
    }
}

const char *col_names[] = {
    "0",   "1",   "2",   "3",   "4",   "5",   "6",   "7",   "8",   "9",   "10",
    "11",  "12",  "13",  "14",  "15",  "16",  "17",  "18",  "19",
    "20",  "21",
    "22",  "23",  "24",  "25",  "26",  "27",  "28",  "29",  "30",  "31",  "32",
    "33",  "34",  "35",  "36",  "37",  "38",  "39",  "40",  "41",  "42",  "43",
    "44",  "45",  "46",  "47",  "48",  "49",
    "50",  "51",  "52",  "53",  "54",
    "55",  "56",  "57",  "58",  "59",  "60",  "61",  "62",  "63",  "64",  "65",
    "66",  "67",  "68",  "69",  "70",  "71",  "72",  "73",  "74",  "75",  "76",
    "77",  "78",  "79",  "80",  "81",  "82",  "83",  "84",  "85",  "86",  "87",
    "88",  "89",  "90",  "91",  "92",  "93",  "94",  "95",  "96",  "97",  "98",
    "99",  
    "100", "101", "102", "103", "104", "105", "106", "107", "108", "109",
    "110", "111", "112", "113", "114", "115", "116", "117", "118", "119", "120",
    "121", "122", "123", "124", "125", "126", "127", "128", "129", "130", "131",
    "132", "133", "134", "135", "136", "137", "138", "139", "140", "141", "142",
    "143", "144", "145", "146", "147", "148", "149", "150", "151", "152", "153",
    "154", "155", "156", "157", "158", "159", "160", "161", "162", "163", "164",
    "165", "166", "167", "168", "169", "170", "171", "172", "173", "174", "175",
    "176", "177", "178", "179", "180", "181", "182", "183", "184", "185", "186",
    "187", "188", "189", "190", "191", "192", "193", "194", "195", "196", "197",
    "198", "199",
};
#define COLS 200
#define ROWS 5

// We use alignas(8) because Arrow mandates all buffers to be 8-byte aligned
struct RecordBatchBody {
    // Note: if we every want to enable element nullability, we would simply
    // interweave validity buffers here in between each column
    alignas(8) int32_t col0[ROWS];
    alignas(8) int32_t col1[ROWS];
    alignas(8) int32_t col2[ROWS];
    alignas(8) int32_t col3[ROWS];
    alignas(8) int32_t col4[ROWS];
    alignas(8) int32_t col5[ROWS];
    alignas(8) int32_t col6[ROWS];
    alignas(8) int32_t col7[ROWS];
    alignas(8) int32_t col8[ROWS];
    alignas(8) int32_t col9[ROWS];
    alignas(8) int32_t col10[ROWS];
    alignas(8) int32_t col11[ROWS];
    alignas(8) int32_t col12[ROWS];
    alignas(8) int32_t col13[ROWS];
    alignas(8) int32_t col14[ROWS];
    alignas(8) int32_t col15[ROWS];
    alignas(8) int32_t col16[ROWS];
    alignas(8) int32_t col17[ROWS];
    alignas(8) int32_t col18[ROWS];
    alignas(8) int32_t col19[ROWS];
    alignas(8) int32_t col20[ROWS];
    alignas(8) int32_t col21[ROWS];
    alignas(8) int32_t col22[ROWS];
    alignas(8) int32_t col23[ROWS];
    alignas(8) int32_t col24[ROWS];
    alignas(8) int32_t col25[ROWS];
    alignas(8) int32_t col26[ROWS];
    alignas(8) int32_t col27[ROWS];
    alignas(8) int32_t col28[ROWS];
    alignas(8) int32_t col29[ROWS];
    alignas(8) int32_t col30[ROWS];
    alignas(8) int32_t col31[ROWS];
    alignas(8) int32_t col32[ROWS];
    alignas(8) int32_t col33[ROWS];
    alignas(8) int32_t col34[ROWS];
    alignas(8) int32_t col35[ROWS];
    alignas(8) int32_t col36[ROWS];
    alignas(8) int32_t col37[ROWS];
    alignas(8) int32_t col38[ROWS];
    alignas(8) int32_t col39[ROWS];
    alignas(8) int32_t col40[ROWS];
    alignas(8) int32_t col41[ROWS];
    alignas(8) int32_t col42[ROWS];
    alignas(8) int32_t col43[ROWS];
    alignas(8) int32_t col44[ROWS];
    alignas(8) int32_t col45[ROWS];
    alignas(8) int32_t col46[ROWS];
    alignas(8) int32_t col47[ROWS];
    alignas(8) int32_t col48[ROWS];
    alignas(8) int32_t col49[ROWS];
    alignas(8) int32_t col50[ROWS];
    alignas(8) int32_t col51[ROWS];
    alignas(8) int32_t col52[ROWS];
    alignas(8) int32_t col53[ROWS];
    alignas(8) int32_t col54[ROWS];
    alignas(8) int32_t col55[ROWS];
    alignas(8) int32_t col56[ROWS];
    alignas(8) int32_t col57[ROWS];
    alignas(8) int32_t col58[ROWS];
    alignas(8) int32_t col59[ROWS];
    alignas(8) int32_t col60[ROWS];
    alignas(8) int32_t col61[ROWS];
    alignas(8) int32_t col62[ROWS];
    alignas(8) int32_t col63[ROWS];
    alignas(8) int32_t col64[ROWS];
    alignas(8) int32_t col65[ROWS];
    alignas(8) int32_t col66[ROWS];
    alignas(8) int32_t col67[ROWS];
    alignas(8) int32_t col68[ROWS];
    alignas(8) int32_t col69[ROWS];
    alignas(8) int32_t col70[ROWS];
    alignas(8) int32_t col71[ROWS];
    alignas(8) int32_t col72[ROWS];
    alignas(8) int32_t col73[ROWS];
    alignas(8) int32_t col74[ROWS];
    alignas(8) int32_t col75[ROWS];
    alignas(8) int32_t col76[ROWS];
    alignas(8) int32_t col77[ROWS];
    alignas(8) int32_t col78[ROWS];
    alignas(8) int32_t col79[ROWS];
    alignas(8) int32_t col80[ROWS];
    alignas(8) int32_t col81[ROWS];
    alignas(8) int32_t col82[ROWS];
    alignas(8) int32_t col83[ROWS];
    alignas(8) int32_t col84[ROWS];
    alignas(8) int32_t col85[ROWS];
    alignas(8) int32_t col86[ROWS];
    alignas(8) int32_t col87[ROWS];
    alignas(8) int32_t col88[ROWS];
    alignas(8) int32_t col89[ROWS];
    alignas(8) int32_t col90[ROWS];
    alignas(8) int32_t col91[ROWS];
    alignas(8) int32_t col92[ROWS];
    alignas(8) int32_t col93[ROWS];
    alignas(8) int32_t col94[ROWS];
    alignas(8) int32_t col95[ROWS];
    alignas(8) int32_t col96[ROWS];
    alignas(8) int32_t col97[ROWS];
    alignas(8) int32_t col98[ROWS];
    alignas(8) int32_t col99[ROWS];
    alignas(8) int32_t col100[ROWS];
    alignas(8) int32_t col101[ROWS];
    alignas(8) int32_t col102[ROWS];
    alignas(8) int32_t col103[ROWS];
    alignas(8) int32_t col104[ROWS];
    alignas(8) int32_t col105[ROWS];
    alignas(8) int32_t col106[ROWS];
    alignas(8) int32_t col107[ROWS];
    alignas(8) int32_t col108[ROWS];
    alignas(8) int32_t col109[ROWS];
    alignas(8) int32_t col110[ROWS];
    alignas(8) int32_t col111[ROWS];
    alignas(8) int32_t col112[ROWS];
    alignas(8) int32_t col113[ROWS];
    alignas(8) int32_t col114[ROWS];
    alignas(8) int32_t col115[ROWS];
    alignas(8) int32_t col116[ROWS];
    alignas(8) int32_t col117[ROWS];
    alignas(8) int32_t col118[ROWS];
    alignas(8) int32_t col119[ROWS];
    alignas(8) int32_t col120[ROWS];
    alignas(8) int32_t col121[ROWS];
    alignas(8) int32_t col122[ROWS];
    alignas(8) int32_t col123[ROWS];
    alignas(8) int32_t col124[ROWS];
    alignas(8) int32_t col125[ROWS];
    alignas(8) int32_t col126[ROWS];
    alignas(8) int32_t col127[ROWS];
    alignas(8) int32_t col128[ROWS];
    alignas(8) int32_t col129[ROWS];
    alignas(8) int32_t col130[ROWS];
    alignas(8) int32_t col131[ROWS];
    alignas(8) int32_t col132[ROWS];
    alignas(8) int32_t col133[ROWS];
    alignas(8) int32_t col134[ROWS];
    alignas(8) int32_t col135[ROWS];
    alignas(8) int32_t col136[ROWS];
    alignas(8) int32_t col137[ROWS];
    alignas(8) int32_t col138[ROWS];
    alignas(8) int32_t col139[ROWS];
    alignas(8) int32_t col140[ROWS];
    alignas(8) int32_t col141[ROWS];
    alignas(8) int32_t col142[ROWS];
    alignas(8) int32_t col143[ROWS];
    alignas(8) int32_t col144[ROWS];
    alignas(8) int32_t col145[ROWS];
    alignas(8) int32_t col146[ROWS];
    alignas(8) int32_t col147[ROWS];
    alignas(8) int32_t col148[ROWS];
    alignas(8) int32_t col149[ROWS];
    alignas(8) int32_t col150[ROWS];
    alignas(8) int32_t col151[ROWS];
    alignas(8) int32_t col152[ROWS];
    alignas(8) int32_t col153[ROWS];
    alignas(8) int32_t col154[ROWS];
    alignas(8) int32_t col155[ROWS];
    alignas(8) int32_t col156[ROWS];
    alignas(8) int32_t col157[ROWS];
    alignas(8) int32_t col158[ROWS];
    alignas(8) int32_t col159[ROWS];
    alignas(8) int32_t col160[ROWS];
    alignas(8) int32_t col161[ROWS];
    alignas(8) int32_t col162[ROWS];
    alignas(8) int32_t col163[ROWS];
    alignas(8) int32_t col164[ROWS];
    alignas(8) int32_t col165[ROWS];
    alignas(8) int32_t col166[ROWS];
    alignas(8) int32_t col167[ROWS];
    alignas(8) int32_t col168[ROWS];
    alignas(8) int32_t col169[ROWS];
    alignas(8) int32_t col170[ROWS];
    alignas(8) int32_t col171[ROWS];
    alignas(8) int32_t col172[ROWS];
    alignas(8) int32_t col173[ROWS];
    alignas(8) int32_t col174[ROWS];
    alignas(8) int32_t col175[ROWS];
    alignas(8) int32_t col176[ROWS];
    alignas(8) int32_t col177[ROWS];
    alignas(8) int32_t col178[ROWS];
    alignas(8) int32_t col179[ROWS];
    alignas(8) int32_t col180[ROWS];
    alignas(8) int32_t col181[ROWS];
    alignas(8) int32_t col182[ROWS];
    alignas(8) int32_t col183[ROWS];
    alignas(8) int32_t col184[ROWS];
    alignas(8) int32_t col185[ROWS];
    alignas(8) int32_t col186[ROWS];
    alignas(8) int32_t col187[ROWS];
    alignas(8) int32_t col188[ROWS];
    alignas(8) int32_t col189[ROWS];
    alignas(8) int32_t col190[ROWS];
    alignas(8) int32_t col191[ROWS];
    alignas(8) int32_t col192[ROWS];
    alignas(8) int32_t col193[ROWS];
    alignas(8) int32_t col194[ROWS];
    alignas(8) int32_t col195[ROWS];
    alignas(8) int32_t col196[ROWS];
    alignas(8) int32_t col197[ROWS];
    alignas(8) int32_t col198[ROWS];
    alignas(8) int32_t col199[ROWS];
};

void arrow_flatcc_build_schema(flatcc_builder_t *b) {
    org_apache_arrow_flatbuf_Schema_endianness_add(b, org_apache_arrow_flatbuf_Endianness_Little);
    org_apache_arrow_flatbuf_Schema_fields_start(b);

    for (const char* col : col_names) {
        org_apache_arrow_flatbuf_Schema_fields_push_start(b);

        // FIXME: code generation needs to handle each case for field types
        org_apache_arrow_flatbuf_Field_name_create_str(b, col);
        org_apache_arrow_flatbuf_Field_nullable_add(b, false);
        org_apache_arrow_flatbuf_Field_type_Int_create(b, 32, true);

        // org_apache_arrow_flatbuf_Field_children_start(b);
        // // Our columns are super simple (primitives only); no children
        // org_apache_arrow_flatbuf_Field_children_end(b);
        // org_apache_arrow_flatbuf_Field_custom_metadata_start(b);
        // // Our fields don't have any custom metadata afaik
        // org_apache_arrow_flatbuf_Field_custom_metadata_end(b);

        org_apache_arrow_flatbuf_Schema_fields_push_end(b);
    }

    org_apache_arrow_flatbuf_Schema_fields_end(b);
    // org_apache_arrow_flatbuf_Schema_custom_metadata_start(b);
    // // we don't have any custom metadata afaik
    // org_apache_arrow_flatbuf_Schema_custom_metadata_end(b);
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

// Each column defined in the schema is represented by one Node struct here,
// giving it's length and null count
org_apache_arrow_flatbuf_FieldNode rb_nodes[COLS] = {};
// Each column has 1-3+ buffers depending on its type; eg. data, validity,
// offsets, etc. For primitives (which we're using exclusively), those are
// validity followed by data. See buffer orders for different types here:
// https://arrow.apache.org/docs/format/Columnar.html#buffer-listing-for-each-layout
org_apache_arrow_flatbuf_Buffer rb_buffers[COLS * 2] = {};

// Note that the actual data buffers come after this flatbuffer message, in the
// messageBody. This is contains only the metadata (lenghts, offsets, etc)
void arrow_flatcc_encode_record_batch_message(flatcc_builder_t *b) {
    org_apache_arrow_flatbuf_Message_start_as_root(b);
    org_apache_arrow_flatbuf_Message_version_add(b, org_apache_arrow_flatbuf_MetadataVersion_V5);
    org_apache_arrow_flatbuf_Message_header_RecordBatch_start(b);

    int64_t offset = 0;
    int buffer_index = 0;
    for (uint i = 0; i < COLS; i++) {
        // We can assume each node has no null's for now. In that case we can
        // chose to omit the validity buffer entirely (saves >=8 bytes per column)
        rb_nodes[i] = {.length = ROWS, .null_count = 0};

        // Every time we add a buffer, the next buffer's offset increases by the
        // number of bytes in the previous buffer.

        // Since we're omitting validity buffers (enforcing non-null values)
        // entirely for now, we set each validity buffer metadata to say
        // length=0 so Arrow decoders know to not look for them
        const uint validity_buf_size = 0; // (int)(ceil(ROWS / 8.0f));
        rb_buffers[buffer_index++] = {.offset = offset, .length = 0};
        if (rb_nodes[i].null_count > 0) // NOTE: ATM this will never happen
            offset += round_up_to_multiple_of_8(validity_buf_size);

        // FIXME: datatypes (bool)
        const uint values_buf_size = ROWS * sizeof(int32_t);
        rb_buffers[buffer_index++] = {.offset = offset, .length = values_buf_size};
        offset += round_up_to_multiple_of_8(values_buf_size);
    }

    org_apache_arrow_flatbuf_RecordBatch_nodes_create(b, rb_nodes, COLS);
    org_apache_arrow_flatbuf_RecordBatch_buffers_create(b, rb_buffers, COLS * 2);

    org_apache_arrow_flatbuf_Message_header_RecordBatch_end(b);
    // Here, bodyLength refers to the # of bytes after the Flatbuffer message
    // metadata that contain the actual data buffers.
    org_apache_arrow_flatbuf_Message_bodyLength_add(b, offset);
    org_apache_arrow_flatbuf_Message_end_as_root(b);
}


// The structure of encapsulated Arrow Stream messages is defined here:
// https://arrow.apache.org/docs/format/Columnar.html#encapsulated-message-format
// It consists of:
// - continuation sign = 0xFFFFFFFF (int32)
// - flatbuffer_size (int32)
// - flatbuffer (bytes)
// - padding to 8-byte boundary
// - message body (optional, depends on message type)
//
// Note that flatbuffer_size must include the padding as well.
void arrow_stream_write_message_to_file(void *flatbuf, size_t flatbuf_size, void *message_body, size_t message_body_size, FILE *file) {
    static constexpr uint32_t continuation = 0xFFFFFFFF;
    fwrite(&continuation, sizeof(uint32_t), 1, file);

    // Ensure the flatbuf size we write includes the 8-byte alignment padding
    const int32_t flatbuf_size_padded = round_up_to_multiple_of_8(flatbuf_size);
    fwrite(&flatbuf_size_padded, sizeof(int32_t), 1, file);

    // Write the flatbuffer, then write any padding necessary
    fwrite(flatbuf, flatbuf_size, 1, file);
    maybe_pad_to_8_bytes(file, flatbuf_size);

    // If the message has a message body associated with it, write it
    if (message_body && message_body_size > 0) {
        fwrite(message_body, message_body_size, 1, file);
        maybe_pad_to_8_bytes(file, message_body_size);
    }
}

flatcc_builder_t b;
RecordBatchBody values;
int32_t *cols[] = {
    values.col0,   values.col1,   values.col2,   values.col3,
    values.col4,   values.col5,   values.col6,   values.col7,
    values.col8,   values.col9,   values.col10,  values.col11,
    values.col12,  values.col13,  values.col14,  values.col15,
    values.col16,  values.col17,  values.col18,  values.col19,
    values.col20,  values.col21,  values.col22,  values.col23,
    values.col24,  values.col25,  values.col26,  values.col27,
    values.col28,  values.col29,  values.col30,  values.col31,
    values.col32,  values.col33,  values.col34,  values.col35,
    values.col36,  values.col37,  values.col38,  values.col39,
    values.col40,  values.col41,  values.col42,  values.col43,
    values.col44,  values.col45,  values.col46,  values.col47,
    values.col48,  values.col49,

    values.col50,  values.col51,
    values.col52,  values.col53,  values.col54,  values.col55,
    values.col56,  values.col57,  values.col58,  values.col59,
    values.col60,  values.col61,  values.col62,  values.col63,
    values.col64,  values.col65,  values.col66,  values.col67,
    values.col68,  values.col69,  values.col70,  values.col71,
    values.col72,  values.col73,  values.col74,  values.col75,
    values.col76,  values.col77,  values.col78,  values.col79,
    values.col80,  values.col81,  values.col82,  values.col83,
    values.col84,  values.col85,  values.col86,  values.col87,
    values.col88,  values.col89,  values.col90,  values.col91,
    values.col92,  values.col93,  values.col94,  values.col95,
    values.col96,  values.col97,  values.col98,  values.col99,

    values.col100, values.col101, values.col102, values.col103,
    values.col104, values.col105, values.col106, values.col107,
    values.col108, values.col109, values.col110, values.col111,
    values.col112, values.col113, values.col114, values.col115,
    values.col116, values.col117, values.col118, values.col119,
    values.col120, values.col121, values.col122, values.col123,
    values.col124, values.col125, values.col126, values.col127,
    values.col128, values.col129, values.col130, values.col131,
    values.col132, values.col133, values.col134, values.col135,
    values.col136, values.col137, values.col138, values.col139,
    values.col140, values.col141, values.col142, values.col143,
    values.col144, values.col145, values.col146, values.col147,
    values.col148, values.col149, values.col150, values.col151,
    values.col152, values.col153, values.col154, values.col155,
    values.col156, values.col157, values.col158, values.col159,
    values.col160, values.col161, values.col162, values.col163,
    values.col164, values.col165, values.col166, values.col167,
    values.col168, values.col169, values.col170, values.col171,
    values.col172, values.col173, values.col174, values.col175,
    values.col176, values.col177, values.col178, values.col179,
    values.col180, values.col181, values.col182, values.col183,
    values.col184, values.col185, values.col186, values.col187,
    values.col188, values.col189, values.col190, values.col191,
    values.col192, values.col193, values.col194, values.col195,
    values.col196, values.col197, values.col198, values.col199,
};

int main(int argc, char *argv[]) {
    printf("Hello World!\n");

    // Enable debug logging (to be turned off)
    sd.debug(true);

    // TODO: experiment with async SPI and/or DMA
    // sd.set_async_spi_mode(true, DMAUsage::DMA_USAGE_ALWAYS);

    print_mem_usage();

    // Mount the FATFileSystem (so we can use regular C file IO like fopen/read)
    int error = fs.mount(&sd);
    if (error) {
        // Reformat if we can't mount the filesystem.
        // This should only happen on the first boot
        printf("No filesystem found, formatting...\n");
        error = fs.reformat(&sd);
        if (error) error_quit("Error: could not reformat SD card! Is the SD card plugged in?\n");
    }

    mkdir("/sd/arrow", 0777);

    FILE *file = fopen("/sd/arrow/test.arrows", "w");
    if (file == NULL)
        error_quit("Error opening file!");

    flatcc_builder_init(&b);

    print_mem_usage();

    arrow_flatcc_encode_schema_message(&b);
    size_t schema_size;
    void *schema = flatcc_builder_finalize_buffer(&b, &schema_size);
    MBED_ASSERT(schema_size != 0 && schema);

    print_mem_usage();

    flatcc_builder_reset(&b);

    print_mem_usage();

    arrow_stream_write_message_to_file(schema, schema_size, nullptr, 0, file);

    print_mem_usage();

    arrow_flatcc_encode_record_batch_message(&b);

    print_mem_usage();

    size_t record_batch_flatbuf_size;
    // void *record_batch_flatbuf = flatcc_builder_finalize_buffer(&b, &record_batch_flatbuf_size);
    void *record_batch_flatbuf = flatcc_builder_get_direct_buffer(&b, &record_batch_flatbuf_size);
    MBED_ASSERT(record_batch_flatbuf_size != 0 && record_batch_flatbuf);

    print_mem_usage();

    flatcc_builder_reset(&b);

    int val = 0;
    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < ROWS; j++) {
            cols[i][j] = val;
            val++;
        }
    }

    print_mem_usage();

    arrow_stream_write_message_to_file(record_batch_flatbuf, record_batch_flatbuf_size, &values, sizeof(values), file);

    const int32_t eos[] = {-1, 0};
    fwrite(eos, 8, 1, file);

    print_mem_usage();

    fclose(file);

    printf("Goodbye World!\n");
    while (1) {};
}




// ------ OLD CODE FOR WRITING EACH VALUE BUFFER SEPARATELY ------

// // Write actual record batch data buffers (message_body)
// int32_t col1values[ROWS] = {0,1,2,3,4,5,6,7,8};
// int32_t col2values[ROWS] = {1,2,3,4,5,6,7,8,9};
// fwrite(col1values, sizeof(col1values), 1, file);
// maybe_pad_to_next_8_bytes(file, sizeof(col1values));
// fwrite(col2values, sizeof(col2values), 1, file);
// maybe_pad_to_next_8_bytes(file, sizeof(col2values));
// const int message_body_size = round_up_to_multiple_of_8(sizeof(col1values)) + round_up_to_multiple_of_8(sizeof(col2values));

// ------  OLD CODE FOR ARROW IPC FILE FORMAT (we use stream format only) ------

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


// static int batches_written = 0;
// static int64_t batch_offsets[100];
// static int32_t batch_metaDataLengths[100];
// static int64_t batch_bodyLengths[100];


// arrow_start_file(file);

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
//
// arrow_end_file(file);


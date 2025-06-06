// --- Code using c++ fs apis; doesn't compile on arm-gcc 13+ due to newlib
// changes (should be fixable?) #include "SDFileSystem.h" #include "mbed.h"
// #include <filesystem>
// #include <iostream>
// #include <fstream>
// #include <wchar.h>
//
// namespace fs = std::filesystem;
//
// int main() {
//   SDFileSystem sd{D2, A5, A4, D3, "sd"}; // Mosi, miso, sclk, cs
//
//   printf("Hello World!\n");
//
//   fs::create_directory("/sd/dir2");
//   const fs::path file("/sd/dir2/test.txt");
//
//   std::ofstream out_stream(file);
//   out_stream << "asjdlkajsdklajs" << std::endl;
//
//   printf("Goodbye World!\n");
//
//
//   return 0;
// }

// #include <SDFileSystem.h>
// #include <mbed.h>
//
// #define buflen (1024)
// #define nwrites (1024)
//
// DigitalOut led(LED1);
// Timer t;
// // SDBlockDevice sd(D2, A5, A4, D3, 1000);
// // FATFileSystem fs("sdcard");
// // uint8_t buf[buflen] = {0};
//
//
// int main() {
//     led = 1;
//     printf("Hello world!\n");
//     // fs.mount(&sd);
//
//
//     SDFileSystem sd{D2, A5, A4, D3, "sd"}; // Mosi, miso, sclk, cs
//     // sd.format();
//     mkdir("/sd/testdir", 0777);
//
//     uint8_t buf[buflen] = {0}; // cant be 2048????
//     for (int i =0; i<buflen; i++) {
//         buf[i] = i;
//     }
//
//     int fd = open("/sd/testdir/meow.bin", O_WRONLY | O_CREAT |O_TRUNC);
//
//     if (-1 == fd) {
//         error("Failed to open file\n");
//     }
//
//     // FILE *fp = fopen("/sd/testdir/test.bin", "w");
//
//
//     // if (fp == NULL) error("Could not open file for write\n");
//
//     printf("Beginning write...\n");
//     t.start();
//     for (int i = 0; i< nwrites; i++) {
//     // uint res = fwrite(buf, buflen, 1, fp);
//         int res = write(fd, buf, buflen);
//         // printf("buffer %d written\n", i);
//         if (res == -1) error("Failed to write buffer %d\n", i);
//     // if (res != 1) error("Failed to write %d-th buffer!\n", 0+1);
//     }
//
//
//     // fflush(fp);
//     t.stop();
//     int64_t time = t.elapsed_time().count();
//     printf("Elapsed time was %lldus\n", time);
//     double bitrate = 8.0*(buflen)*(nwrites) / ((long double)
//     time/(1000.0*1000)); printf("Bitrate: %lf Kbit/s\n", bitrate/1000.0);
//
//     close(fd);
//
//     // fclose(fp);
//
//     printf("Goodbye world!\n");
//     led = 0;
// }

#include <SDFileSystem.h>
#include <mbed.h>
#include <nanoarrow/nanoarrow.h>
#include <nanoarrow/nanoarrow.hpp>
#include <nanoarrow/nanoarrow_ipc.h>
#include <nanoarrow/nanoarrow_ipc.hpp>
#include <stdnoreturn.h>

// #include "./nanoarrow_generated_from_dbc.hpp"

namespace na = nanoarrow;

#define ARROW_ERROR_PRINT(EXPR)                                                \
    do {                                                                       \
        const int code = (EXPR);                                               \
        if (code)                                                              \
            printf("Nanoarror error: %s: %s\n", #EXPR, error.message);         \
    } while (0)

#define COLS 200
#define ROWS 10


void error_quit(std::string msg) {
    printf("%s\n", msg.c_str());
    while (1) {
    };
}

// ArrowBufferAllocator fake_alloc = {0};
// ArrowBuffer cols[100] = {
//     {.data = nullptr,
//      .size_bytes = 5,
//      .capacity_bytes = 5,
//      .allocator = fake_alloc},
// };

// #define BUMP_ALLOCATOR_SIZE 65536 // Tune this to match your expected need
// static uint8_t bump_allocator_buffer[BUMP_ALLOCATOR_SIZE];
// static size_t bump_allocator_offset = 0;
// void bump_allocator_reset() { bump_allocator_offset = 0; }
// uint8_t *bump_malloc(int64_t size) {
//     if (bump_allocator_offset + size > BUMP_ALLOCATOR_SIZE) {
//         return NULL;
//     }
//     uint8_t *ptr = &bump_allocator_buffer[bump_allocator_offset];
//     bump_allocator_offset += size;
//     return ptr;
// }
// uint8_t *bump_realloc(struct ArrowBufferAllocator *alloc, uint8_t *ptr,
//                       int64_t old_size, int64_t new_size) {
//     // Simplified: we can't resize, only allocate new
//     (void)old_size;
//     if (ptr == NULL)
//         return bump_malloc(new_size);
//     return NULL;
// }
// void bump_free(struct ArrowBufferAllocator *alloc, uint8_t *ptr, int64_t size) {
//     (void)ptr;
//     (void)size;
// }
// struct ArrowBufferAllocator bump_allocator = {
//     .reallocate = &bump_realloc,
//     .free = &bump_free,
// };

int main(int argc, char *argv[]) {
    printf("Hello world!\n");

    // SDFileSystem sd{D2, A5, A4, D3, "sd"}; // Mosi, miso, sclk, cs // L423KC
    SDFileSystem sd{D11, D12, D13, D10, "sd"}; // Mosi, miso, sclk, cs // F446RE
    if (sd.disk_initialize() != 0)
        error_quit("Failed to initialize SD card!");

    mkdir("/sd/arrow", 0777);

    FILE *file = fopen("/sd/arrow/test.arrow", "w");
    if (file == NULL)
        error_quit("Error opening file!");

    // Prepare for error handling of nanoarrow calls
    ArrowError error;

    // Create an Arrow Schema for the array(s)
    na::UniqueSchema schema_root;
    // // code = ArrowSchemaInitFromType(schema_root.get(),
    // NANOARROW_TYPE_STRUCT);
    ArrowSchemaInit(schema_root.get());
    // ArrowSchemaSetTypeStruct(schema_root.get(), COLS);
    ArrowSchemaSetTypeStruct(schema_root.get(), 100);

    // for (int i = 0; i < COLS; i++) {
    //   ArrowSchema *schema_n = schema_root->children[i];
    //   ArrowSchemaInitFromType(schema_n, NANOARROW_TYPE_INT16);
    //   ArrowSchemaSetName(schema_n, std::to_string(i).c_str());
    // }

    printf("1\n");

    ArrowSchemaSetType(schema_root->children[0], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[1], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[2], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[3], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[4], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[5], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[6], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[7], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[8], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[9], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[10], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[11], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[12], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[13], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[14], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[15], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[16], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[17], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[18], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[19], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[20], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[21], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[22], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[23], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[24], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[25], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[26], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[27], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[28], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[29], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[30], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[31], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[32], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[33], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[34], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[35], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[36], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[37], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[38], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[39], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[40], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[41], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[42], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[43], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[44], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[45], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[46], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[47], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[48], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[49], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[50], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[51], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[52], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[53], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[54], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[55], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[56], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[57], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[58], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[59], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[60], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[61], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[62], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[63], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[64], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[65], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[66], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[67], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[68], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[69], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[70], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[71], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[72], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[73], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[74], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[75], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[76], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[77], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[78], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[79], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[80], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[81], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[82], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[83], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[84], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[85], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[86], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[87], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[88], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[89], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[90], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[91], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[92], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[93], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[94], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[95], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[96], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[97], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[98], NANOARROW_TYPE_INT32);
    ArrowSchemaSetType(schema_root->children[99], NANOARROW_TYPE_INT32);
    ArrowSchemaSetName(schema_root->children[0], "0");
    ArrowSchemaSetName(schema_root->children[1], "1");
    ArrowSchemaSetName(schema_root->children[2], "2");
    ArrowSchemaSetName(schema_root->children[3], "3");
    ArrowSchemaSetName(schema_root->children[4], "4");
    ArrowSchemaSetName(schema_root->children[5], "5");
    ArrowSchemaSetName(schema_root->children[6], "6");
    ArrowSchemaSetName(schema_root->children[7], "7");
    ArrowSchemaSetName(schema_root->children[8], "8");
    ArrowSchemaSetName(schema_root->children[9], "9");
    ArrowSchemaSetName(schema_root->children[10], "10");
    ArrowSchemaSetName(schema_root->children[11], "11");
    ArrowSchemaSetName(schema_root->children[12], "12");
    ArrowSchemaSetName(schema_root->children[13], "13");
    ArrowSchemaSetName(schema_root->children[14], "14");
    ArrowSchemaSetName(schema_root->children[15], "15");
    ArrowSchemaSetName(schema_root->children[16], "16");
    ArrowSchemaSetName(schema_root->children[17], "17");
    ArrowSchemaSetName(schema_root->children[18], "18");
    ArrowSchemaSetName(schema_root->children[19], "19");
    ArrowSchemaSetName(schema_root->children[20], "20");
    ArrowSchemaSetName(schema_root->children[21], "21");
    ArrowSchemaSetName(schema_root->children[22], "22");
    ArrowSchemaSetName(schema_root->children[23], "23");
    ArrowSchemaSetName(schema_root->children[24], "24");
    ArrowSchemaSetName(schema_root->children[25], "25");
    ArrowSchemaSetName(schema_root->children[26], "26");
    ArrowSchemaSetName(schema_root->children[27], "27");
    ArrowSchemaSetName(schema_root->children[28], "28");
    ArrowSchemaSetName(schema_root->children[29], "29");
    ArrowSchemaSetName(schema_root->children[30], "30");
    ArrowSchemaSetName(schema_root->children[31], "31");
    ArrowSchemaSetName(schema_root->children[32], "32");
    ArrowSchemaSetName(schema_root->children[33], "33");
    ArrowSchemaSetName(schema_root->children[34], "34");
    ArrowSchemaSetName(schema_root->children[35], "35");
    ArrowSchemaSetName(schema_root->children[36], "36");
    ArrowSchemaSetName(schema_root->children[37], "37");
    ArrowSchemaSetName(schema_root->children[38], "38");
    ArrowSchemaSetName(schema_root->children[39], "39");
    ArrowSchemaSetName(schema_root->children[40], "40");
    ArrowSchemaSetName(schema_root->children[41], "41");
    ArrowSchemaSetName(schema_root->children[42], "42");
    ArrowSchemaSetName(schema_root->children[43], "43");
    ArrowSchemaSetName(schema_root->children[44], "44");
    ArrowSchemaSetName(schema_root->children[45], "45");
    ArrowSchemaSetName(schema_root->children[46], "46");
    ArrowSchemaSetName(schema_root->children[47], "47");
    ArrowSchemaSetName(schema_root->children[48], "48");
    ArrowSchemaSetName(schema_root->children[49], "49");
    ArrowSchemaSetName(schema_root->children[50], "50");
    ArrowSchemaSetName(schema_root->children[51], "51");
    ArrowSchemaSetName(schema_root->children[52], "52");
    ArrowSchemaSetName(schema_root->children[53], "53");
    ArrowSchemaSetName(schema_root->children[54], "54");
    ArrowSchemaSetName(schema_root->children[55], "55");
    ArrowSchemaSetName(schema_root->children[56], "56");
    ArrowSchemaSetName(schema_root->children[57], "57");
    ArrowSchemaSetName(schema_root->children[58], "58");
    ArrowSchemaSetName(schema_root->children[59], "59");
    ArrowSchemaSetName(schema_root->children[60], "60");
    ArrowSchemaSetName(schema_root->children[61], "61");
    ArrowSchemaSetName(schema_root->children[62], "62");
    ArrowSchemaSetName(schema_root->children[63], "63");
    ArrowSchemaSetName(schema_root->children[64], "64");
    ArrowSchemaSetName(schema_root->children[65], "65");
    ArrowSchemaSetName(schema_root->children[66], "66");
    ArrowSchemaSetName(schema_root->children[67], "67");
    ArrowSchemaSetName(schema_root->children[68], "68");
    ArrowSchemaSetName(schema_root->children[69], "69");
    ArrowSchemaSetName(schema_root->children[70], "70");
    ArrowSchemaSetName(schema_root->children[71], "71");
    ArrowSchemaSetName(schema_root->children[72], "72");
    ArrowSchemaSetName(schema_root->children[73], "73");
    ArrowSchemaSetName(schema_root->children[74], "74");
    ArrowSchemaSetName(schema_root->children[75], "75");
    ArrowSchemaSetName(schema_root->children[76], "76");
    ArrowSchemaSetName(schema_root->children[77], "77");
    ArrowSchemaSetName(schema_root->children[78], "78");
    ArrowSchemaSetName(schema_root->children[79], "79");
    ArrowSchemaSetName(schema_root->children[80], "80");
    ArrowSchemaSetName(schema_root->children[81], "81");
    ArrowSchemaSetName(schema_root->children[82], "82");
    ArrowSchemaSetName(schema_root->children[83], "83");
    ArrowSchemaSetName(schema_root->children[84], "84");
    ArrowSchemaSetName(schema_root->children[85], "85");
    ArrowSchemaSetName(schema_root->children[86], "86");
    ArrowSchemaSetName(schema_root->children[87], "87");
    ArrowSchemaSetName(schema_root->children[88], "88");
    ArrowSchemaSetName(schema_root->children[89], "89");
    ArrowSchemaSetName(schema_root->children[90], "90");
    ArrowSchemaSetName(schema_root->children[91], "91");
    ArrowSchemaSetName(schema_root->children[92], "92");
    ArrowSchemaSetName(schema_root->children[93], "93");
    ArrowSchemaSetName(schema_root->children[94], "94");
    ArrowSchemaSetName(schema_root->children[95], "95");
    ArrowSchemaSetName(schema_root->children[96], "96");
    ArrowSchemaSetName(schema_root->children[97], "97");
    ArrowSchemaSetName(schema_root->children[98], "98");
    ArrowSchemaSetName(schema_root->children[99], "99");

    printf("2\n");

    // Create one Arrow STRUCT array, which is the main array of children.
    // Append one INT16 array to it
    na::UniqueArray array_root;

    printf("2.3\n");

    // NOTE: This function should initialize the type and name of all children
    // arrays!!

    // ARROW_ERROR_PRINT(ArrowArrayInitFromSchema(array_root.get(),
    // schema_root.get(), &error));

    ArrowArrayInitFromType(array_root.get(), NANOARROW_TYPE_STRUCT);

    printf("2.5\n");

    // ArrowArrayAllocateChildren(array_root.get(), COLS);
    ArrowArrayAllocateChildren(array_root.get(), 100);

    printf("3\n");

    // for (int i = 0; i < COLS; i++) {
    //   ArrowArray *array_n = array_root->children[i];
    //   ARROW_ERROR_PRINT(ArrowArrayInitFromSchema(array_n,
    //   schema_root->children[i], &error)); ArrowArrayStartAppending(array_n);
    //   ArrowArrayReserve(array_n, ROWS);
    //   for (int i = 0; i < ROWS; i++) {
    //     ArrowArrayAppendInt(array_n, 12340 + i);
    //   }
    //   ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_n, &error));
    // }
    ArrowArrayInitFromType(array_root->children[0], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[1], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[2], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[3], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[4], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[5], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[6], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[7], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[8], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[9], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[10], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[11], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[12], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[13], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[14], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[15], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[16], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[17], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[18], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[19], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[20], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[21], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[22], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[23], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[24], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[25], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[26], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[27], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[28], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[29], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[30], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[31], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[32], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[33], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[34], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[35], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[36], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[37], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[38], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[39], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[40], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[41], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[42], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[43], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[44], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[45], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[46], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[47], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[48], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[49], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[50], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[51], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[52], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[53], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[54], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[55], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[56], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[57], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[58], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[59], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[60], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[61], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[62], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[63], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[64], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[65], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[66], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[67], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[68], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[69], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[70], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[71], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[72], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[73], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[74], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[75], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[76], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[77], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[78], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[79], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[80], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[81], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[82], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[83], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[84], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[85], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[86], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[87], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[88], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[89], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[90], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[91], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[92], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[93], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[94], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[95], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[96], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[97], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[98], NANOARROW_TYPE_INT32);
    ArrowArrayInitFromType(array_root->children[99], NANOARROW_TYPE_INT32);

    printf("4\n");

    // FIXME: Should only be necessary to call this on the root!!!
    ArrowArrayStartAppending(array_root.get());
    // ArrowArrayStartAppending(array_0);
    // ArrowArrayStartAppending(array_1);
    // ArrowArrayStartAppending(array_2);
    // ArrowArrayStartAppending(array_3);
    // ArrowArrayStartAppending(array_4);
    // ArrowArrayStartAppending(array_5);
    // ArrowArrayStartAppending(array_6);
    // ArrowArrayStartAppending(array_7);
    // ArrowArrayStartAppending(array_8);
    // ArrowArrayStartAppending(array_9);
    // ArrowArrayStartAppending(array_10);
    // ArrowArrayStartAppending(array_11);
    // ArrowArrayStartAppending(array_12);
    // ArrowArrayStartAppending(array_13);
    // ArrowArrayStartAppending(array_14);
    // ArrowArrayStartAppending(array_15);
    // ArrowArrayStartAppending(array_16);
    // ArrowArrayStartAppending(array_17);
    // ArrowArrayStartAppending(array_18);
    // ArrowArrayStartAppending(array_19);
    // ArrowArrayStartAppending(array_20);
    // ArrowArrayStartAppending(array_21);
    // ArrowArrayStartAppending(array_22);
    // ArrowArrayStartAppending(array_23);
    // ArrowArrayStartAppending(array_24);
    // ArrowArrayStartAppending(array_25);
    // ArrowArrayStartAppending(array_26);
    // ArrowArrayStartAppending(array_27);
    // ArrowArrayStartAppending(array_28);
    // ArrowArrayStartAppending(array_29);
    // ArrowArrayStartAppending(array_30);
    // ArrowArrayStartAppending(array_31);
    // ArrowArrayStartAppending(array_32);
    // ArrowArrayStartAppending(array_33);
    // ArrowArrayStartAppending(array_34);
    // ArrowArrayStartAppending(array_35);
    // ArrowArrayStartAppending(array_36);
    // ArrowArrayStartAppending(array_37);
    // ArrowArrayStartAppending(array_38);
    // ArrowArrayStartAppending(array_39);

    printf("5\n");

    // FIXME: Should (maybe?) only be necessary to call this on the root!!!
    ArrowArrayReserve(array_root.get(), 5);
    // ArrowArrayReserve(array_0, 5);
    // ArrowArrayReserve(array_1, 5);
    // ArrowArrayReserve(array_2, 5);
    // ArrowArrayReserve(array_3, 5);
    // ArrowArrayReserve(array_4, 5);
    // ArrowArrayReserve(array_5, 5);
    // ArrowArrayReserve(array_6, 5);
    // ArrowArrayReserve(array_7, 5);
    // ArrowArrayReserve(array_8, 5);
    // ArrowArrayReserve(array_9, 5);
    // ArrowArrayReserve(array_10, 5);
    // ArrowArrayReserve(array_11, 5);
    // ArrowArrayReserve(array_12, 5);
    // ArrowArrayReserve(array_13, 5);
    // ArrowArrayReserve(array_14, 5);
    // ArrowArrayReserve(array_15, 5);
    // ArrowArrayReserve(array_16, 5);
    // ArrowArrayReserve(array_17, 5);
    // ArrowArrayReserve(array_18, 5);
    // ArrowArrayReserve(array_19, 5);
    // ArrowArrayReserve(array_20, 5);
    // ArrowArrayReserve(array_21, 5);
    // ArrowArrayReserve(array_22, 5);
    // ArrowArrayReserve(array_23, 5);
    // ArrowArrayReserve(array_24, 5);
    // ArrowArrayReserve(array_25, 5);
    // ArrowArrayReserve(array_26, 5);
    // ArrowArrayReserve(array_27, 5);
    // ArrowArrayReserve(array_28, 5);
    // ArrowArrayReserve(array_29, 5);
    // ArrowArrayReserve(array_30, 5);
    // ArrowArrayReserve(array_31, 5);
    // ArrowArrayReserve(array_32, 5);
    // ArrowArrayReserve(array_33, 5);
    // ArrowArrayReserve(array_34, 5);
    // ArrowArrayReserve(array_35, 5);
    // ArrowArrayReserve(array_36, 5);
    // ArrowArrayReserve(array_37, 5);
    // ArrowArrayReserve(array_38, 5);
    // ArrowArrayReserve(array_39, 5);

    printf("6\n");

    for (int i = 0; i < 1; i++) {
        ArrowArrayAppendInt(array_root->children[0], 12340 + i);
        ArrowArrayAppendInt(array_root->children[1], 12340 + i);
        ArrowArrayAppendInt(array_root->children[2], 12340 + i);
        ArrowArrayAppendInt(array_root->children[3], 12340 + i);
        ArrowArrayAppendInt(array_root->children[4], 12340 + i);
        ArrowArrayAppendInt(array_root->children[5], 12340 + i);
        ArrowArrayAppendInt(array_root->children[6], 12340 + i);
        ArrowArrayAppendInt(array_root->children[7], 12340 + i);
        ArrowArrayAppendInt(array_root->children[8], 12340 + i);
        ArrowArrayAppendInt(array_root->children[9], 12340 + i);
        ArrowArrayAppendInt(array_root->children[10], 12340 + i);
        ArrowArrayAppendInt(array_root->children[11], 12340 + i);
        ArrowArrayAppendInt(array_root->children[12], 12340 + i);
        ArrowArrayAppendInt(array_root->children[13], 12340 + i);
        ArrowArrayAppendInt(array_root->children[14], 12340 + i);
        ArrowArrayAppendInt(array_root->children[15], 12340 + i);
        ArrowArrayAppendInt(array_root->children[16], 12340 + i);
        ArrowArrayAppendInt(array_root->children[17], 12340 + i);
        ArrowArrayAppendInt(array_root->children[18], 12340 + i);
        ArrowArrayAppendInt(array_root->children[19], 12340 + i);
        ArrowArrayAppendInt(array_root->children[20], 12340 + i);
        ArrowArrayAppendInt(array_root->children[21], 12340 + i);
        ArrowArrayAppendInt(array_root->children[22], 12340 + i);
        ArrowArrayAppendInt(array_root->children[23], 12340 + i);
        ArrowArrayAppendInt(array_root->children[24], 12340 + i);
        ArrowArrayAppendInt(array_root->children[25], 12340 + i);
        ArrowArrayAppendInt(array_root->children[26], 12340 + i);
        ArrowArrayAppendInt(array_root->children[27], 12340 + i);
        ArrowArrayAppendInt(array_root->children[28], 12340 + i);
        ArrowArrayAppendInt(array_root->children[29], 12340 + i);
        ArrowArrayAppendInt(array_root->children[30], 12340 + i);
        ArrowArrayAppendInt(array_root->children[31], 12340 + i);
        ArrowArrayAppendInt(array_root->children[32], 12340 + i);
        ArrowArrayAppendInt(array_root->children[33], 12340 + i);
        ArrowArrayAppendInt(array_root->children[34], 12340 + i);
        ArrowArrayAppendInt(array_root->children[35], 12340 + i);
        ArrowArrayAppendInt(array_root->children[36], 12340 + i);
        ArrowArrayAppendInt(array_root->children[37], 12340 + i);
        ArrowArrayAppendInt(array_root->children[38], 12340 + i);
        ArrowArrayAppendInt(array_root->children[39], 12340 + i);
        ArrowArrayAppendInt(array_root->children[40], 12340 + i);
        ArrowArrayAppendInt(array_root->children[41], 12340 + i);
        ArrowArrayAppendInt(array_root->children[42], 12340 + i);
        ArrowArrayAppendInt(array_root->children[43], 12340 + i);
        ArrowArrayAppendInt(array_root->children[44], 12340 + i);
        ArrowArrayAppendInt(array_root->children[45], 12340 + i);
        ArrowArrayAppendInt(array_root->children[46], 12340 + i);
        ArrowArrayAppendInt(array_root->children[47], 12340 + i);
        ArrowArrayAppendInt(array_root->children[48], 12340 + i);
        ArrowArrayAppendInt(array_root->children[49], 12340 + i);
        ArrowArrayAppendInt(array_root->children[50], 12340 + i);
        ArrowArrayAppendInt(array_root->children[51], 12340 + i);
        ArrowArrayAppendInt(array_root->children[52], 12340 + i);
        ArrowArrayAppendInt(array_root->children[53], 12340 + i);
        ArrowArrayAppendInt(array_root->children[54], 12340 + i);
        ArrowArrayAppendInt(array_root->children[55], 12340 + i);
        ArrowArrayAppendInt(array_root->children[56], 12340 + i);
        ArrowArrayAppendInt(array_root->children[57], 12340 + i);
        ArrowArrayAppendInt(array_root->children[58], 12340 + i);
        ArrowArrayAppendInt(array_root->children[59], 12340 + i);
        ArrowArrayAppendInt(array_root->children[60], 12340 + i);
        ArrowArrayAppendInt(array_root->children[61], 12340 + i);
        ArrowArrayAppendInt(array_root->children[62], 12340 + i);
        ArrowArrayAppendInt(array_root->children[63], 12340 + i);
        ArrowArrayAppendInt(array_root->children[64], 12340 + i);
        ArrowArrayAppendInt(array_root->children[65], 12340 + i);
        ArrowArrayAppendInt(array_root->children[66], 12340 + i);
        ArrowArrayAppendInt(array_root->children[67], 12340 + i);
        ArrowArrayAppendInt(array_root->children[68], 12340 + i);
        ArrowArrayAppendInt(array_root->children[69], 12340 + i);
        ArrowArrayAppendInt(array_root->children[70], 12340 + i);
        ArrowArrayAppendInt(array_root->children[71], 12340 + i);
        ArrowArrayAppendInt(array_root->children[72], 12340 + i);
        ArrowArrayAppendInt(array_root->children[73], 12340 + i);
        ArrowArrayAppendInt(array_root->children[74], 12340 + i);
        ArrowArrayAppendInt(array_root->children[75], 12340 + i);
        ArrowArrayAppendInt(array_root->children[76], 12340 + i);
        ArrowArrayAppendInt(array_root->children[77], 12340 + i);
        ArrowArrayAppendInt(array_root->children[78], 12340 + i);
        ArrowArrayAppendInt(array_root->children[79], 12340 + i);
        ArrowArrayAppendInt(array_root->children[80], 12340 + i);
        ArrowArrayAppendInt(array_root->children[81], 12340 + i);
        ArrowArrayAppendInt(array_root->children[82], 12340 + i);
        ArrowArrayAppendInt(array_root->children[83], 12340 + i);
        ArrowArrayAppendInt(array_root->children[84], 12340 + i);
        ArrowArrayAppendInt(array_root->children[85], 12340 + i);
        ArrowArrayAppendInt(array_root->children[86], 12340 + i);
        ArrowArrayAppendInt(array_root->children[87], 12340 + i);
        ArrowArrayAppendInt(array_root->children[88], 12340 + i);
        ArrowArrayAppendInt(array_root->children[89], 12340 + i);
        ArrowArrayAppendInt(array_root->children[90], 12340 + i);
        ArrowArrayAppendInt(array_root->children[91], 12340 + i);
        ArrowArrayAppendInt(array_root->children[92], 12340 + i);
        ArrowArrayAppendInt(array_root->children[93], 12340 + i);
        ArrowArrayAppendInt(array_root->children[94], 12340 + i);
        ArrowArrayAppendInt(array_root->children[95], 12340 + i);
        ArrowArrayAppendInt(array_root->children[96], 12340 + i);
        ArrowArrayAppendInt(array_root->children[97], 12340 + i);
        ArrowArrayAppendInt(array_root->children[98], 12340 + i);
        ArrowArrayAppendInt(array_root->children[99], 12340 + i);
    }

    printf("7\n");

    // FIXME: Should only be necessary to call this on the root!!!
    ARROW_ERROR_PRINT(ArrowArrayFinishBuilding(
        array_root.get(), NANOARROW_VALIDATION_LEVEL_MINIMAL, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_0, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_1, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_2, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_3, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_4, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_5, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_6, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_7, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_8, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_9, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_10, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_11, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_12, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_13, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_14, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_15, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_16, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_17, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_18, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_19, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_20, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_21, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_22, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_23, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_24, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_25, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_26, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_27, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_28, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_29, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_30, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_31, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_32, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_33, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_34, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_35, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_36, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_37, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_38, &error));
    // ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_39, &error));

    printf("8\n");

    mbed_stats_heap_t heap;
    mbed_stats_stack_t stack;

    mbed_stats_heap_get(&heap);
    printf("Heap: %u/%u, reserved: %u, allocs: %u\n", heap.current_size,
           heap.max_size, heap.reserved_size, heap.alloc_cnt);
    mbed_stats_stack_get(&stack);
    printf("Stack: max used: %u, reserved: %u\n", stack.max_size,
           stack.reserved_size);

    // na::UniqueSchema schema_root = make_nanoarrow_schema();
    //
    // mbed_stats_heap_get(&heap);
    // printf("Heap: %u/%u, reserved: %u, allocs: %u\n", heap.current_size,
    // heap.max_size, heap.reserved_size, heap.alloc_cnt);
    // mbed_stats_stack_get(&stack);
    // printf("Stack: max used: %u, reserved: %u\n", stack.max_size,
    // stack.reserved_size);
    //
    // na::UniqueArray array_root = make_nanoarrow_array(schema_root.get(), 5);
    //
    // mbed_stats_heap_get(&heap);
    // printf("Heap: %u/%u, reserved: %u, allocs: %u\n", heap.current_size,
    // heap.max_size, heap.reserved_size, heap.alloc_cnt);
    // mbed_stats_stack_get(&stack);
    // printf("Stack: max used: %u, reserved: %u\n", stack.max_size,
    // stack.reserved_size);
    //
    // for (int i = 0; i < array_root->n_children; i++) {
    //   ArrowArrayStartAppending(array_root->children[i]);
    //   for (int j = 0; j < 3; j++) {
    //     ArrowArrayAppendInt(array_root->children[i], i + j);
    //   }
    //   ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_root->children[i],
    //   &error));
    // }
    //
    // mbed_stats_heap_get(&heap);
    // printf("Heap: %u/%u, reserved: %u, allocs: %u\n", heap.current_size,
    // heap.max_size, heap.reserved_size, heap.alloc_cnt);
    // mbed_stats_stack_get(&stack);
    // printf("Stack: max used: %u, reserved: %u\n", stack.max_size,
    // stack.reserved_size);

    // Create the main arrow IPC output stream
    na::ipc::UniqueOutputStream out_stream;
    ArrowIpcOutputStreamInitFile(out_stream.get(), file, false);

    printf("9\n");

    // Create a view onto the root array (the record batch)
    na::UniqueArrayView array_view;
    ARROW_ERROR_PRINT(ArrowArrayViewInitFromSchema(array_view.get(),
                                                   schema_root.get(), &error));
    ARROW_ERROR_PRINT(
        ArrowArrayViewSetArray(array_view.get(), array_root.get(), &error));

    printf("10\n");

    mbed_stats_heap_get(&heap);
    printf("Heap: %u/%u, reserved: %u, allocs: %u\n", heap.current_size,
           heap.max_size, heap.reserved_size, heap.alloc_cnt);
    mbed_stats_stack_get(&stack);
    printf("Stack: max used: %u, reserved: %u\n", stack.max_size,
           stack.reserved_size);

    // Create an IPC writer (to write into the file pointer)
    na::ipc::UniqueWriter ipc_writer;
    ArrowIpcWriterInit(ipc_writer.get(), out_stream.get());
    ARROW_ERROR_PRINT(ArrowIpcWriterStartFile(ipc_writer.get(), &error));
    ARROW_ERROR_PRINT(
        ArrowIpcWriterWriteSchema(ipc_writer.get(), schema_root.get(), &error));

    printf("11\n");

    mbed_stats_heap_get(&heap);
    printf("Heap: %u/%u, reserved: %u, allocs: %u\n", heap.current_size,
           heap.max_size, heap.reserved_size, heap.alloc_cnt);
    mbed_stats_stack_get(&stack);
    printf("Stack: max used: %u, reserved: %u\n", stack.max_size,
           stack.reserved_size);

    Timer t;
    t.start();
    // array_view is now a view onto a record batch with COLS columns and ROWS
    // rows. Write it to the stream a bunch of times, and time it
    for (int i = 0; i < 1; i++) {
        ARROW_ERROR_PRINT(ArrowIpcWriterWriteArrayView(
            ipc_writer.get(), array_view.get(), &error));
    }
    printf("Time elapsed: %lldms\n", t.elapsed_time().count() / 1000);

    printf("12\n");

    mbed_stats_heap_get(&heap);
    printf("Heap: %u/%u, reserved: %u, allocs: %u\n", heap.current_size,
           heap.max_size, heap.reserved_size, heap.alloc_cnt);
    mbed_stats_stack_get(&stack);
    printf("Stack: max used: %u, reserved: %u\n", stack.max_size,
           stack.reserved_size);

    ARROW_ERROR_PRINT(ArrowIpcWriterFinalizeFile(ipc_writer.get(), &error));

    printf("13\n");

    fclose(file);

    printf("Goodbye world!\n");

    while (1) {
    }
    return 0;
}

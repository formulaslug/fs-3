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
#include <nanoarrow/nanoarrow.hpp>
#include <nanoarrow/nanoarrow.h>
#include <nanoarrow/nanoarrow_ipc.hpp>
#include <nanoarrow/nanoarrow_ipc.h>

namespace na = nanoarrow;

#define ARROW_ERROR_PRINT(EXPR) \
  do {                                            \
    const int code = (EXPR);                                                            \
    if (code) printf("Nanoarror error: %s: %s\n", #EXPR, error.message);                           \
  } while (0)

int main(int argc, char *argv[]) {
    printf("Hello world!\n");

    SDFileSystem sd{D2, A5, A4, D3, "sd"}; // Mosi, miso, sclk, cs
    // sd.format();
    mkdir("/sd/arrow", 0777);
    FILE *file = fopen("/sd/arrow/test.arrow", "w");
    
    // Prepare for error handling of nanoarrow calls
    ArrowError error;

    // Create an Arrow Schema for the array(s)
    na::UniqueSchema schema_root;
    // code = ArrowSchemaInitFromType(schema_root.get(), NANOARROW_TYPE_STRUCT);
    ArrowSchemaInit(schema_root.get());
    ArrowSchemaSetTypeStruct(schema_root.get(), 2);

    ArrowSchema *schema_mycoolint16 = schema_root->children[0];
    ArrowSchema *schema_somebools = schema_root->children[1];
    ArrowSchemaInitFromType(schema_mycoolint16, NANOARROW_TYPE_INT16);
    ArrowSchemaInitFromType(schema_somebools, NANOARROW_TYPE_BOOL);
    ArrowSchemaSetName(schema_mycoolint16, "MyCoolInt16");
    ArrowSchemaSetName(schema_somebools, "SomeBools");

    // Create one Arrow STRUCT array, which is the main array of children.
    // Append one INT16 array to it
    na::UniqueArray array_root;
    ARROW_ERROR_PRINT(ArrowArrayInitFromSchema(array_root.get(), schema_root.get(), &error));
    ArrowArrayAllocateChildren(array_root.get(), 2);

    ArrowArray *array_mycoolint16 = array_root->children[0];
    ArrowArray *array_somebools = array_root->children[1];
    ARROW_ERROR_PRINT(ArrowArrayInitFromSchema(array_mycoolint16, schema_mycoolint16, &error));
    ARROW_ERROR_PRINT(ArrowArrayInitFromSchema(array_somebools, schema_somebools, &error));
    ArrowArrayStartAppending(array_mycoolint16);
    ArrowArrayStartAppending(array_somebools);
    ArrowArrayReserve(array_mycoolint16, 5);
    ArrowArrayReserve(array_somebools, 5);
    for (int i = 0; i < 1; i++) {
        ArrowArrayAppendInt(array_mycoolint16, 12340 + i);
        ArrowArrayAppendUInt(array_somebools, i%2==0);
    }
    ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_mycoolint16, &error));
    ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_somebools, &error));

    na::ipc::UniqueOutputStream out_stream;
    ArrowIpcOutputStreamInitFile(out_stream.get(), file, false);


    // Create a view onto the int array
    na::UniqueArrayView array_view;
    ARROW_ERROR_PRINT(ArrowArrayViewInitFromSchema(array_view.get(), schema_root.get(), &error));
    ARROW_ERROR_PRINT(ArrowArrayViewSetArray(array_view.get(), array_root.get(), &error));

    na::ipc::UniqueWriter ipc_writer;
    ArrowIpcWriterInit(ipc_writer.get(), out_stream.get());
    ARROW_ERROR_PRINT(ArrowIpcWriterStartFile(ipc_writer.get(), &error));
    ARROW_ERROR_PRINT(ArrowIpcWriterWriteSchema(ipc_writer.get(), schema_root.get(), &error));
    ARROW_ERROR_PRINT(ArrowIpcWriterWriteArrayView(ipc_writer.get(), array_view.get(), &error));
    ARROW_ERROR_PRINT(ArrowIpcWriterWriteArrayView(ipc_writer.get(), array_view.get(), &error));
    ARROW_ERROR_PRINT(ArrowIpcWriterWriteArrayView(ipc_writer.get(), array_view.get(), &error));
    ARROW_ERROR_PRINT(ArrowIpcWriterFinalizeFile(ipc_writer.get(), &error));

    fclose(file);

    printf("Goodbye world!\n");
    return 0;
}

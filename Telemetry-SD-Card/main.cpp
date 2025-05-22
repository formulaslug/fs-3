#include "SDFileSystem.h"
#include "mbed.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <wchar.h>


// namespace fs = std::filesystem;

// int main() {
//   SDFileSystem sd{D2, A5, A4, D3, "sd"}; // Mosi, miso, sclk, cs
//
//   printf("Hello World!\n");
//
//   //fs::create_directory("/sd/dir2");
//   //const fs::path file("/sd/dir2/test.txt");
//
//   // std::ofstream out_stream(file);
//   // out_stream << "asjdlkajsdklajs" << std::endl;
//
//   printf("Goodbye World!\n");
//
//
//   return 0;
// }


// Timer t;
int main() {
    printf("Hellow world!\n");

    mkdir("/sd/tstdir", 0777);

    // const uint8_t BUFF[256] = {0};

    FILE *fp = fopen("/sd/tstdir/sdtest.txt", "w");

    if (fp== NULL) {
        error("Could not open file for write\n");
    }
    // while (1) {
    // t.start();
        // fwrite(BUFF, 8, 256, fp);
    // t.stop();
    // int64_t time = t.elapsed_time().count() / 1000;
    // printf("Elapsed time was %lld\n", time);
    // }
    fclose(fp);

    printf("Goodbye world!\n");
}
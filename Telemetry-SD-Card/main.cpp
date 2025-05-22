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

#include "SDFileSystem.h"
#include <mbed.h>

DigitalOut led(LED1);
Timer t;

int main() {
    led = 1;
    printf("Hello world!\n");

    SDFileSystem sd{D2, A5, A4, D3, "sd"}; // Mosi, miso, sclk, cs

    mkdir("/sd/tstdir", 0777);

    const uint8_t buf[1024] = {0}; // cant be 2048????

    FILE *fp = fopen("/sd/tstdir/sdtest.txt", "w");

    if (fp == NULL) error("Could not open file for write\n"); 

    t.start();

    uint res = fwrite(buf, 1024, 1, fp);
    if (res != 1) error("Failed to write buffer!\n");

    int64_t time = t.elapsed_time().count();
    t.stop();
    printf("Elapsed time was %lldus\n", time);

    fclose(fp);

    printf("Goodbye world!\n");
    led = 0;
}

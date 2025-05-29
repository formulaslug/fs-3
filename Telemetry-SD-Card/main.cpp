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

#include <SDFileSystem.h>
#include <mbed.h>


#define buflen (1024)
#define nwrites (1024)

DigitalOut led(LED1);
Timer t;
// SDBlockDevice sd(D2, A5, A4, D3, 1000);
// FATFileSystem fs("sdcard");
// uint8_t buf[buflen] = {0};


int main() {
    led = 1;
    printf("Hello world!\n");
    // fs.mount(&sd);


    SDFileSystem sd{D2, A5, A4, D3, "sd"}; // Mosi, miso, sclk, cs
    // sd.format();
    mkdir("/sd/testdir", 0777);

    uint8_t buf[buflen] = {0}; // cant be 2048????
    for (int i =0; i<buflen; i++) {
        buf[i] = i;
    }

    int fd = open("/sd/testdir/meow.bin", O_WRONLY | O_CREAT |O_TRUNC);

    if (-1 == fd) {
        error("Failed to open file\n");
    }

    // FILE *fp = fopen("/sd/testdir/test.bin", "w");


    // if (fp == NULL) error("Could not open file for write\n");

    printf("Beginning write...\n");
    t.start();
    for (int i = 0; i< nwrites; i++) {
    // uint res = fwrite(buf, buflen, 1, fp);
        int res = write(fd, buf, buflen);
        // printf("buffer %d written\n", i);
        if (res == -1) error("Failed to write buffer %d\n", i);
    // if (res != 1) error("Failed to write %d-th buffer!\n", 0+1);
    }


    // fflush(fp);
    t.stop();
    int64_t time = t.elapsed_time().count();
    printf("Elapsed time was %lldus\n", time);
    double bitrate = 8.0*(buflen)*(nwrites) / ((long double) time/(1000.0*1000));
    printf("Bitrate: %lf Kbit/s\n", bitrate/1000.0);

    close(fd);

    // fclose(fp);

    printf("Goodbye world!\n");
    led = 0;
}

#include "SDFileSystem.h"
#include "mbed.h"
#include <iostream>

int main() {
  SDFileSystem fs{D2, A5, A4, D3, "sd"}; // Mosi, miso, sclk, cs

  printf("Hello World!\n");   

  mkdir("/sd/mydir", 0777);
  
  FILE *fp = fopen("/sd/mydir/sdtest.txt", "w");
  if(fp == NULL) {
      error("Could not open file for write\n");
  }
  fprintf(fp, "Hello World!");
  fclose(fp); 

  printf("Goodbye World!\n");

  // fs.disk_initialize();
  //
  // fs.
  //
  // printf("File system initialized\n");
  //
  // FileHandle *file = fs.open("test", FA_WRITE); // | FA_CREATE_NEW
  //
  // file->write("Hello_world", 13);
  //
  // printf("Closing File\n");
  // file->close();
  return 0;
}

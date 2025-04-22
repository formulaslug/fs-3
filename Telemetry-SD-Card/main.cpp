#include "SDFileSystem.h"
#include "mbed.h"
#include <iostream>

int main() {
  SDFileSystem fs{D2, A5, A4, D3, "sd"}; // Mosi, miso, sclk, cs
  fs.disk_initialize();
  printf("File system initialized\n");
  const auto file = fs.open("sd", FA_WRITE | FA_CREATE_ALWAYS);
  file->write("Hello_world", 12);
  printf("Closing File\n");
  file->close();
  // main() is expected to loop forever.
  // If main() actually returns the processor will halt
  return 0;
} //
// Created by Goob on 4/21/2025.
//

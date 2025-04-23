#include "SDFileSystem.h"
#include "mbed.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <wchar.h>


namespace fs = std::filesystem;

int main() {
  SDFileSystem sd{D2, A5, A4, D3, "sd"}; // Mosi, miso, sclk, cs

  printf("Hello World!\n");

  fs::create_directory("/sd/dir2");
  const fs::path file("/sd/dir2/test.txt");

  std::ofstream out_stream(file);
  // out_stream << "asjdlkajsdklajs" << std::endl;

  printf("Goodbye World!\n");


  return 0;
}

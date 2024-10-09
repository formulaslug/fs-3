//
// Created by wangd on 10/8/2024.
//
#include "mbed.h"

int main()
{
    while(true) {
        printf("Hello World\n");
        ThisThread::sleep_for(1s);
    }

    // main() is expected to loop forever.
    // If main() actually returns the processor will halt
    return 0;
}
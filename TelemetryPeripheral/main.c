/** 
 * @file main.c
 * @author jack
 * @date 2025-02-22
 * @brief Main function 
 */

#include <avr/io.h>
#include "mcc_generated_files/mcc.h"

int main(){

    // Add your code here and press Ctrl + Shift + B to build
    SYSTEM_Initialize();

    while (true) {
        DELAY_milliseconds(1000);
        IO_PA6_SetHigh();
        DELAY_milliseconds(1000);
        IO_PA6_SetLow();
    }

    return 0;
};

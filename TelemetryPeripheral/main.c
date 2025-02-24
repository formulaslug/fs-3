/** 
 * @file main.c
 * @author jack
 * @date 2025-02-22
 * @brief Main function 
 */

#include "mcc_generated_files/mcc.h"
#include <avr/io.h>

int main(){

    // Add your code here and press Ctrl + Shift + B to build
    SYSTEM_Initialize();

    while (true) {
        DELAY_milliseconds(1000);
    }

    return 0;
};

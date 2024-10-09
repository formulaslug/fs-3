//
// Created by wangd on 10/8/2024.
//
#include "mbed.h"

#include "src/inc/console.h"

int main()
{
    Console console{};
    while(true) {
        console.run();
    }
}
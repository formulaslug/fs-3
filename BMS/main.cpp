
#include "mbed.h"

#include "src/inc/console.h"

int main()
{
    Console console{};
    while(true) {
        console.run();
    }
}
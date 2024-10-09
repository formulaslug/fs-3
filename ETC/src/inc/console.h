//
// Created by wangd on 10/8/2024.
//

#ifndef CONSOLE_H
#define CONSOLE_H

#include <cstdint>
#include "module.h"
#include "mbed.h"

/**
* @brief This class operates a serial connection in a terminal like manner
*
* This class operates a serial connection in a terminal like manner by echoing characters, deleting characters, holding a buffer, and clearing it when enter is pressed
*
* @todo Create a cmd class that has some functions that take the buffer as input and outputs it back to the console class
*/

class Console: public Module {

    static constexpr int32_t MAX_BUFFER_SIZE = 80;
    static constexpr char PROMPT[] = "> ";
    static constexpr char START_UP_MSG[] = "Command Prompt Start\n\r";
    static constexpr char ERROR_SOUND[] = "\a";
    static constexpr char BACKSPACE[] = "\b \b";
    static constexpr char ENTER[] = "\n\r";

    BufferedSerial *serial;
    char cmd_buffer[MAX_BUFFER_SIZE] = {0};
    int32_t num_char_received = 0;

public:


    Console() {
        static BufferedSerial usb(USBTX, USBRX);
        serial = &usb;
        write_startup();
        serial->set_blocking(false);
    }


    explicit Console(BufferedSerial *usb) {
        assert(usb != nullptr && "Pointer to BufferedSerial cannot be nullptr");
        assert(usb->is_blocking() == false && "BufferedSerial cannot be blocking");
        serial = usb;
        write_startup();
    }

    int32_t run();
private:
    void write_startup() {
        serial->write(START_UP_MSG, sizeof(START_UP_MSG));
        serial->write(PROMPT, sizeof(PROMPT));
    };
};

#endif //CONSOLE_H

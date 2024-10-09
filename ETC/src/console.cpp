//
// Created by wangd on 10/8/2024.
//

#include "inc/console.h"

int32_t Console::run() {

    char c[1] = {0};
    if(serial->read(c, sizeof(c)) != -EAGAIN) {
        //Handle return
        if(*c == '\n' || *c == '\r') {
            serial->write(ENTER, sizeof(ENTER));
            cmd_buffer[num_char_received+1] = '\n';
            num_char_received = 0;

            //ADD CODE HERE TO SEND BUFFER DATA TO CMD CLASS
            memset(cmd_buffer, 0, sizeof(cmd_buffer));
            serial->write(PROMPT, sizeof(PROMPT));
            return 0;
        }

        //Handle backspace
        if (*c == '\b' || *c == '\x7f') {
            if (num_char_received > 0) {
                // Overwrite last character with a blank.
                serial->write(BACKSPACE, sizeof(BACKSPACE));
                num_char_received--;
            }
            return 0;
        }

        //Echo handling
        if(num_char_received < MAX_BUFFER_SIZE-1) {
            serial->write(c, sizeof(c));
            cmd_buffer[num_char_received++] = *c;
        } else {
            serial->write(ERROR_SOUND, sizeof(ERROR_SOUND));
        }
    }
    return 0;
}

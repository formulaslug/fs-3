#include "radio.hpp"
#include "DigitalIn.h"
#include "DigitalOut.h"

XBeeRadio::XBeeRadio(SPI& spi, DigitalOut& csPin, DigitalIn& attnPin)
    : spi(spi), cs(csPin), spi_attn(attnPin) {
};

int XBeeRadio::get_temp(void) {

    uint8_t resp_buf[11] = {0};
    send_at_command(TEMPERATURE, {}, 0, resp_buf, sizeof(resp_buf));

    return resp_buf[9]; // todo: verify this number
}

int XBeeRadio::get_at_command(AT_COMMAND at_command, uint8_t at_command_bytes[]) {

    switch (at_command) {

        case TEMPERATURE:
            at_command_bytes[0] = 0x54;
            at_command_bytes[1] = 0x50;
            return 1;
            break;
    }
}

int XBeeRadio::send_at_command(AT_COMMAND at_command, uint8_t parameters[], uint parameters_size, uint8_t resp_buf[], uint resp_buf_size) {

    uint8_t at_command_bytes[] = {};

    get_at_command(at_command, at_command_bytes);

    uint8_t local_at_command[32] = {0};

    local_at_command[0] = 0x7E;
    local_at_command[1] = 0x00; // todo: calc length
    local_at_command[2] = 0x04; // length lsb

    // Start checksum
    local_at_command[3] = 0x08; // frametype
    local_at_command[4] = 0x17; // frameid
    local_at_command[5] = at_command_bytes[0]; // at command byte 1
    local_at_command[6] = at_command_bytes[1]; // at command byte 2

    for (uint i = 0; i < parameters_size; i++) {
        local_at_command[i + 7] = parameters[i];
    }

    local_at_command[parameters_size + 7] = calculate_checksum(local_at_command + 3, 4 + parameters_size);

    cs.write(0);
    spi.write(local_at_command, sizeof(local_at_command), resp_buf, resp_buf_size);
    cs.write(1);

    if (resp_buf[resp_buf_size - 1] != calculate_checksum(resp_buf, resp_buf_size - 1)) {
        return AT_CHECKSUM_INVALID;
    }

    return AT_OKAY;
}


int XBeeRadio::transmit(uint64_t destination, char *payload) {

    uint8_t transmit_command[100] = {0};

    transmit_command[0] = 0x7E;
    transmit_command[1] = 0x00; // length msb
    transmit_command[2] = 0x08; // todo :calculate length
    transmit_command[3] = 0x00; // frametype 0x00 == transmit request
    transmit_command[4] = 0x77; // frameid
    transmit_command[5] = destination; // frameid
    transmit_command[13] = 0xFF; // reserved
    transmit_command[14] = 0xEE; // reserved
    transmit_command[15] = 0x00; // broadcast radius

    return 0;
}

uint8_t XBeeRadio::calculate_checksum(const uint8_t *data, size_t length) {
    uint16_t sum = 0;

    // Sum all bytes
    for (size_t i = 0; i < length; ++i) {
        sum += data[i];
    }

    // Keep only the lowest 8 bits of the sum
    uint8_t low_byte = sum & 0xFF;

    // Subtract from 0xFF to get checksum
    return 0xFF - low_byte;
}

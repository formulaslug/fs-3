#include "radio.hpp"
#include "packetize.hpp"
#include "DigitalIn.h"
#include "DigitalOut.h"
#include <cstdint>



XBeeRadio::XBeeRadio(SPI& spi, DigitalOut& csPin, DigitalIn& attnPin)
    : spi(spi), cs(csPin), spi_attn(attnPin) {
};

int16_t XBeeRadio::get_temp(void) {

    uint8_t resp_buf[20] = {0};
    send_at_command(TEMPERATURE, {}, 0, resp_buf, sizeof(resp_buf));

    printf("RESPONSE: ");
    for (int i = 0; i < 20; i++) {
        printf("%02x ", resp_buf[i]);
    }
    printf("\n");

    return resp_buf[9];
}

int XBeeRadio::set_repeat_transmissions(uint8_t repeat_count) {
    uint8_t resp_buf[15] = {0};

    uint8_t parameters[] = {repeat_count};
    send_at_command(BROADCAST_MULTI_TRANSMITS, parameters, sizeof(parameters), resp_buf, sizeof(resp_buf));

    printf("RESPONSE: ");
    for (int i = 0; i < 15; i++) {
        printf("%02x ", resp_buf[i]);
    }
    printf("\n");

    return 0;
}

int XBeeRadio::baud_rate_set(uint8_t baud_rate) {
    uint8_t resp_buf[15] = {0};

    uint8_t parameters[] = {baud_rate};
    send_at_command(STREAMING_LIMIT, parameters, sizeof(parameters), resp_buf, sizeof(resp_buf));

    printf("RESPONSE: ");
    for (int i = 0; i < 15; i++) {
        printf("%02x ", resp_buf[i]);
    }
    printf("\n");

    return 0;
}

int XBeeRadio::set_streaming_limit(uint8_t streaming_limit) {
    uint8_t resp_buf[15] = {0};

    uint8_t parameters[] = {streaming_limit};
    send_at_command(STREAMING_LIMIT, parameters, sizeof(parameters), resp_buf, sizeof(resp_buf));

    printf("RESPONSE: ");
    for (int i = 0; i < 15; i++) {
        printf("%02x ", resp_buf[i]);
    }
    printf("\n");

    return 0;
}

int XBeeRadio::set_network_hops(uint8_t network_hops) {
    uint8_t resp_buf[15] = {0};

    uint8_t parameters[] = {network_hops};
    send_at_command(NETWORK_HOPS, parameters, sizeof(parameters), resp_buf, sizeof(resp_buf));

    printf("RESPONSE: ");
    for (int i = 0; i < 15; i++) {
        printf("%02x ", resp_buf[i]);
    }
    printf("\n");

    return 0;
}

int XBeeRadio::set_network_identifier(char* network_identifier, int network_identifier_size) {
    uint8_t resp_buf[15] = {0};

    send_at_command(NETWORK_IDENTIFIER, (uint8_t*) network_identifier, network_identifier_size, resp_buf, sizeof(resp_buf));

    printf("RESPONSE: ");
    for (int i = 0; i < 15; i++) {
        printf("%02x ", resp_buf[i]);
    }
    printf("\n");

    return 0;
}

int16_t XBeeRadio::get_max_transmit_size(void) {

    uint8_t resp_buf[11] = {0};
    send_at_command(MAX_TRANSMIT_SIZE, {}, 0, resp_buf, sizeof(resp_buf));

    printf("RESPONSE: ");
    for (int i = 0; i < 11; i++) {
        printf("%02x ", resp_buf[i]);
    }
    printf("\n");
    
    return (resp_buf[8] << 8) | resp_buf[9];
}

int XBeeRadio::get_at_command(AT_COMMAND at_command, uint8_t at_command_bytes[]) {

    switch (at_command) {

        case TEMPERATURE:
            at_command_bytes[0] = 0x54;
            at_command_bytes[1] = 0x50;
            return 1;
            break;
        case DESTINATION_SET_LOW:
            at_command_bytes[0] = 0x44;
            at_command_bytes[1] = 0x54;
            return 1;
            break;
        case MAX_TRANSMIT_SIZE:
            at_command_bytes[0] = 0x4E;
            at_command_bytes[1] = 0x50;
            return 1;
            break;
        case BAUD_RATE_SET:
            at_command_bytes[0] = 'B';
            at_command_bytes[1] = 'R';
            return 1;
            break;
        case BROADCAST_MULTI_TRANSMITS:
            at_command_bytes[0] = 'M';
            at_command_bytes[1] = 'T';
            return 1;
            break;
        case NETWORK_HOPS:
            at_command_bytes[0] = 'N';
            at_command_bytes[1] = 'H';
            return 1;
            break;
        case NETWORK_IDENTIFIER:
            at_command_bytes[0] = 'N';
            at_command_bytes[1] = 'I';
            return 1;
            break;
        case STREAMING_LIMIT:
            at_command_bytes[0] = 'T';
            at_command_bytes[1] = 'T';
            return 1;
            break;
        case TRANSMIT_OPTIONS:
            at_command_bytes[0] = 'T';
            at_command_bytes[1] = 'O';
            return 1;
            break;

        }
}

int XBeeRadio::send_at_command(AT_COMMAND at_command, uint8_t parameters[], uint parameters_size, uint8_t resp_buf[], uint resp_buf_size) {

    uint8_t at_command_bytes[] = {};

    get_at_command(at_command, at_command_bytes);

    uint8_t local_at_command[300] = {0};

    uint16_t packet_length = 7+parameters_size - 3;
    local_at_command[0] = 0x7E;
    local_at_command[1] = (packet_length & 0xFF00) >> 2; // length msb
    local_at_command[2] = packet_length & 0x00FF; // length lsb

    // Start checksum
    local_at_command[3] = 0x08; // frametype
    local_at_command[4] = 0x17; // frameid
    local_at_command[5] = at_command_bytes[0]; // at command byte 1
    local_at_command[6] = at_command_bytes[1]; // at command byte 2

    for (uint i = 0; i < parameters_size; i++) {
        local_at_command[i + 7] = parameters[i];
    }

    local_at_command[parameters_size + 7] = calculate_checksum(local_at_command + 3, 4 + parameters_size);

    printf("SENT: ");
    for (unsigned int i = 0; i < parameters_size+8; i++) {
        printf("%02x ", local_at_command[i]);
    }
    printf("\n");

    cs.write(0);
    spi.write(local_at_command, parameters_size+8, nullptr, 0);
    cs.write(1);

    while (spi_attn.read()) {
    }

    cs.write(0);
    spi.write(nullptr, 0, resp_buf, resp_buf_size);
    cs.write(1);

    if (resp_buf[resp_buf_size - 1] != calculate_checksum(resp_buf, resp_buf_size - 1)) {
        return AT_CHECKSUM_INVALID;
    }

    return AT_OKAY;
}

int XBeeRadio::transmit(uint8_t* payload, int payload_size) {

    Packet packet;
    packetize((uint8_t*) payload, payload_size, &packet);

    for (int i = 0; i < packet.total_segments; i++) {

        std::vector<uint8_t> bytes_to_send = serialize(&packet.segments[i]);

        for (unsigned int n = 0; n < bytes_to_send.size(); n++) {
            // printf("%02x ", bytes_to_send[n]);
        }
        // printf("\n");

        transmit_raw(0x0, bytes_to_send.data(), bytes_to_send.size(), 0x69);

    }

    return 1;
}


int XBeeRadio::transmit_raw(uint64_t destination, uint8_t *payload, int payload_size, uint8_t frameid) {

    uint8_t transmit_command[300] = {0};

    transmit_command[0] = 0x7E;

    uint16_t packet_length = 17+payload_size - 3;
    transmit_command[1] = (packet_length & 0xFF00) >> 2; // length msb
    transmit_command[2] = packet_length & 0x00FF; // length lsb
    transmit_command[3] = 0x10; // frametype 0x10 == transmit request
    transmit_command[4] = frameid; // frameid
    transmit_command[5] = 0x00;
    transmit_command[6] = 0x00;
    transmit_command[7] = 0x00;
    transmit_command[8] = 0x00;
    transmit_command[9] = 0x00;
    transmit_command[10] = 0x00;
    transmit_command[11] = 0xFF;
    transmit_command[12] = 0xFF;
    transmit_command[13] = 0xFF; // RESERVED
    transmit_command[14] = 0xFE; // RESERVED
    transmit_command[15] = 0x00; // BROADCAST RAD
    transmit_command[16] = 0x00; // TRANSMIT OPTIONS
    int n = 0;
    for (n = 0; n < payload_size; n++) {
        transmit_command[17+n] = payload[n]; // rf-data
    }
    transmit_command[17+n] = calculate_checksum(transmit_command + 3, 17+n - 3); // broadcast radius
    // printf("SENT: ");
    // for (int i = 0; i < 18+n; i++) {
    //     printf("%02x ", transmit_command[i]);
    // }
    // printf("\n");

    uint8_t resp_buf[300] = {0};
    cs.write(0);
    spi.write(transmit_command, 18+n, nullptr, 0);
    cs.write(1);
    
    while (spi_attn.read()) {
    }

    cs.write(0);
    spi.write(nullptr, 0, resp_buf, sizeof(resp_buf));
    cs.write(1);

    // WAIT FOR TRANSMIT RESPONSE
    // printf("RESPONSE: ");
    // for (int i = 0; i < 30; i++) {
    //     printf("%02x ", resp_buf[i]);
    // }
    // printf("\n");

    if (resp_buf[4] != frameid) {
        return(NO_RESPONSE);
    }

    return(resp_buf[8]);

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
